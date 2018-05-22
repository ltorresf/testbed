#include "ezC2X/sensor/gps/GpsdSensor.h"

#ifdef EZC2X_LIBGPS

#include <boost/cast.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ezC2X/core/position/Wgs84Position.h"
#include "ezC2X/core/time/ITimeProvider.h"
#include "ezC2X/core/property/PropertyMapper.h"

#include "ezC2X/sensor/MountingPositionProperty.h"
#include "ezC2X/core/Log.h"

#include "gps.h"

using ezC2X::GpsdSensor;

GpsdSensor::GpsdSensor()
    : host_("localhost"), port_(DEFAULT_GPSD_PORT), gpsData_(new gps_data_t), isStopping_(false),
      mountingPositionHasChanged_(false)
{
    initCapabilities();
    if (!doStart())
    {
        throw ConnectionError();
    }
}

GpsdSensor::GpsdSensor(std::string host)
    : host_(host), port_(DEFAULT_GPSD_PORT), gpsData_(new gps_data_t), isStopping_(false),
      mountingPositionHasChanged_(false)
{
    initCapabilities();
    if (!doStart())
    {
        throw ConnectionError();
    }
}

GpsdSensor::GpsdSensor(std::string host, int port)
    : host_(host), port_(boost::lexical_cast<std::string>(port)), gpsData_(new gps_data_t), isStopping_(false),
      mountingPositionHasChanged_(false)
{
    initCapabilities();
    if (!doStart())
    {
        throw ConnectionError();
    }
}

GpsdSensor::~GpsdSensor()
{
    doStop();
}

void
GpsdSensor::initCapabilities()
{
    capabilities_.hasPosition = true;
    capabilities_.hasElevation = true;
    capabilities_.hasFixType = true;
    capabilities_.hasSpeed = true;
    capabilities_.hasHeading = true;
    capabilities_.hasNumSatellites = true;
    capabilities_.hasTimestamp = true;
    capabilities_.hasHdop = true;
    capabilities_.hasLatitudeDeviation = true;
    capabilities_.hasLongitudeDeviation = true;
    capabilities_.hasMountingPosition = false;

#if GPSD_API_MAJOR_VERSION >= 5
    capabilities_.hasErrorEllipse = true;
#endif
}

GpsdSensor*
GpsdSensor::create(boost::property_tree::ptree const& properties)
{
    std::string host = "localhost";
    int port = boost::lexical_cast<int>(DEFAULT_GPSD_PORT);
    PropertyMapper mapper;
    boost::optional<MountingPosition> pos;
    mapper.addProperty("Host", &host, false);
    mapper.addProperty("Port", &port, false);
    mapper.addProperty("MountingPosition", makeProperty(&pos), false);

    if(!mapper.configure(properties))
    {
        EZLOG_ERROR << "No valid properties supplied for GpsdSensor.";
        return 0;
    }
    std::unique_ptr<GpsdSensor> mySensor;
    try
    {
        mySensor.reset(new GpsdSensor(host, port));
    }
    catch (ConnectionError &e)
    {
        // will already be logged by doStart
        return 0;
    }
    if(pos)
    {
        mySensor->setMountingPosition(*pos);
    }
    return mySensor.release();
}

void
GpsdSensor::setMountingPosition(MountingPosition pos)
{
    mountPos_ = pos;
    mountingPositionHasChanged_ = true;
    capabilities_.hasMountingPosition = true;
}

ezC2X::IGpsSensor::Capabilities const&
GpsdSensor::getCapabilities() const
{
    return capabilities_;
}

bool
GpsdSensor::doStart()
{
    assert(!readThread_);
    errno = 0;

    EZLOG_VERBOSE << "Connecting to gpsd @ " << host_ << ":" << port_;

    // open the device
#if GPSD_API_MAJOR_VERSION >= 5
    if (0 != gps_open(host_.c_str(), port_.c_str(), gpsData_.get()))
    {
#else
        if (0 != gps_open_r(host_.c_str(), port_.c_str(), gpsData_.get()))
        {
#endif
        EZLOG_ERROR << "Failed to open connection to gpsd. Reason: " << gps_errstr(errno);
        return false;
    }

    // enable streaming of data
    gps_stream(gpsData_.get(), WATCH_ENABLE | WATCH_JSON, NULL);

    // start the read thread
    readThread_.reset(new boost::thread(&GpsdSensor::doRead, this));

    return true;
}

void
GpsdSensor::doStop()
{
    if (readThread_)
    {
        isStopping_ = true;

        readThread_->join();
        readThread_.reset();

        // clean up the mess and close the device
        gps_stream(gpsData_.get(), WATCH_DISABLE, NULL);
        gps_close(gpsData_.get());
    }
}

void
GpsdSensor::doRead()
{

    errno = 0;

    // main loop
    while (!isStopping_)
    {
#if GPSD_API_MAJOR_VERSION >= 5
        if (gps_waiting(gpsData_.get(), 1000 * 1000))
        { // timeout every second
#else
        if (gps_waiting(gpsData_.get()))
        { // no timeout, hopefully we do not get stuck
#endif
            errno = 0;
            if (-1 != gps_read(gpsData_.get()))
            {
                // process the data
                processGpsData(*gpsData_);
                gpsData_->set = 0; // clear flags (not sure if this is necessary)
            }
            else
            {
                EZLOG_ERROR << "Failed to read gps data. Reason: " << gps_errstr(errno);
            }
        }
    }

}

void
GpsdSensor::processGpsData(gps_data_t const& data)
{
    EZLOG_TRACE << "Processing gps data.";

    Time now;
    if (boost::shared_ptr<ITimeProvider> tp = timeProvider_.get())
    {
        now = tp->now();
    }
    else
    {
        EZLOG_WARN << "GpsdSensor is missing a time provider, ignoring data read from gpsd";
        return;
    }

    if (STATUS_NO_FIX == data.status)
    {
        EZLOG_WARN << "No GPS fix available.";
        if (data.set & STATUS_SET)
        {
            // update fix type
            std::unique_ptr<Writer> writer = getWriter();
            using ezC2X::gps::FixType;
            writer->setFixType(TimedValue<FixType::e>(FixType::Unknown, now));
        }
        return;
    }

    onGpsd_(data);

    // update sensor
    std::unique_ptr<Writer> writer = getWriter();
    if (data.set & LATLON_SET)
    {

        if ((!std::isnan(data.fix.latitude)) && (!std::isnan(data.fix.longitude)))
        {
            using ezC2X::Wgs84Position;
            writer->setPosition(TimedValue<Wgs84Position>(Wgs84Position(data.fix.latitude, data.fix.longitude), now));
            writer->setLongitudeDeviation(TimedValue<float>(data.fix.epx, now));
            writer->setLatitudeDeviation(TimedValue<float>(data.fix.epy, now));
        }
    }

    if (data.set & ALTITUDE_SET)
    {
        if (MODE_3D == data.fix.mode)
        {
            writer->setElevation(TimedValue<float>(data.fix.altitude, now));
        }
    }

    if (data.set & SPEED_SET)
    {
        writer->setSpeed(TimedValue<float>(data.fix.speed, now));
    }

    if (data.set & TRACK_SET)
    {
        writer->setHeading(TimedValue<float>(data.fix.track, now));
    }

    if (data.set & MODE_SET)
    {
        using ezC2X::gps::FixType;
        if (STATUS_FIX == data.status)
        {
            if (MODE_2D == data.fix.mode)
            {
                writer->setFixType(TimedValue<FixType::e>(FixType::Fix2D, now));
            }
            else if (MODE_3D == data.fix.mode)
            {
                writer->setFixType(TimedValue<FixType::e>(FixType::Fix3D, now));
            }
            else
            {
                EZLOG_WARN << "Unexpected gps fix mode: " << data.fix.mode;
                writer->setFixType(TimedValue<FixType::e>(FixType::Unknown, now));
            }
        }
/*
 * Status type STATUS_DGPS_FIX was removed from the public API with version 3.14 of libgps.
 * However, we keep it around for older versions of the library even though it
 * is not clear if status 2 was ever provided by the public data structures of the library.
 */
#ifdef STATUS_DGPS_FIX
        else if (STATUS_DGPS_FIX == data.status)
        {
            if (MODE_2D == data.fix.mode)
            {
                writer->setFixType(TimedValue<FixType::e>(FixType::Fix2DwithDGPS, now));
            }
            else if (MODE_3D == data.fix.mode)
            {
                writer->setFixType(TimedValue<FixType::e>(FixType::Fix3DwithDGPS, now));
            }
            else
            {
                EZLOG_WARN << "Unexpected dgps fix mode: " << data.fix.mode;
                writer->setFixType(TimedValue<FixType::e>(FixType::Unknown, now));
            }
        }
#endif // ifdef STATUS_DGPS_FIX
        else
        {
            EZLOG_WARN << "Unexpected gps status: " << data.status;
            writer->setFixType(TimedValue<FixType::e>(FixType::Unknown, now));
        }
    }

    if (data.set & SATELLITE_SET)
    {
        writer->setNumSatellites(TimedValue<int>(data.satellites_used, now));
    }

    if (data.set & TIME_SET)
    {
        writer->setTimestamp(TimedValue<boost::uint64_t>(boost::numeric_cast<boost::uint64_t>(data.fix.time * 1000.f), now));
    }

    if (data.set & DOP_SET)
    {
        writer->setHdop(TimedValue<float>(data.dop.hdop, now));
    }

#if GPSD_API_MAJOR_VERSION >= 5
    if (data.set & GST_SET)
    {
        using ezC2X::gps::ErrorEllipse;
        ErrorEllipse errEll;
        errEll.majorAxisLength = data.gst.smajor_deviation;
        errEll.minorAxisLength = data.gst.sminor_deviation;
        errEll.majorAxisAngle = data.gst.smajor_orientation;
        writer->setErrorEllipse(TimedValue<ErrorEllipse>(errEll, now));
    }
#endif

    /*
     * [review]
     * See comment in the ARS308LrrRadar implementation.
     * [/review]
     */
    if(mountingPositionHasChanged_)
    {
        writer->setMountingPosition(TimedValue<MountingPosition>(mountPos_, now));
        mountingPositionHasChanged_ = false;
    }
}

boost::signals2::connection
GpsdSensor::subscribeOnGpsdUpdate(OnGpsd::slot_type const& handler)
{
    return onGpsd_.connect(handler);
}

void
GpsdSensor::setSensorManager(boost::optional<ezC2X::SensorManager&>)
{
}

#endif // EZC2X_LIBGPS
