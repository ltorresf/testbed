/*!
 * @file GpsdSensor.h
 * @brief Implementation of a GPS sensor using gpsd and libgps.
 * @author Karsten Roscher
 *
 * Copyright (c) 2016 Fraunhofer Institute for Embedded Systems and Communication Technologies ESK. All rights reserved.
 */
#ifndef EZC2X_SENSOR_GPS_GPSD_SENSOR_H
#define EZC2X_SENSOR_GPS_GPSD_SENSOR_H

#include "ezC2X/config.h"

#ifdef EZC2X_LIBGPS

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/signals2.hpp>

#include "ezC2X/core/object/ObjectHandle.h"
#include "ezC2X/sensor/base/GpsSensor.h"
#include "ezC2X/sensor/AbstractSensorBase.h"

//! Forward declaration of the gps data structure (@see gps.h)
struct gps_data_t;

namespace ezC2X
{

class ITimeProvider;

/*!
 * @brief Implementation of a GPS sensor using gpsd and libgps.
 *
 * @ingroup gps
 */
class GpsdSensor : public ezC2X::sensor_base::GpsSensor,
    public ezC2X::AbstractSensorBase,
    public boost::noncopyable
{

public:
    //! Exception indicating an error with the connection to the gpsd.
    class ConnectionError : public ezC2X::Exception
    {
    };

    /*!
     * @brief Create a new gps sensor connecting to gpsd at localhost and the default port.
     *
     * @throw ConnectionError if connecting to gpsd failed
     *
     * @note The default port is taken from @c gps.h - DEFAULT_GPSD_PORT.
     */
    GpsdSensor();

    /*!
     * @brief Create a new gps sensor connecting to gpsd at the provided host and the default port.
     *
     * @param host String representation of the host to connect to.
     *
     * @throw ConnectionError if connecting to gpsd failed
     *
     * @note The default port is taken from @c gps.h - DEFAULT_GPSD_PORT.
     */
    explicit GpsdSensor(std::string host);

    /*!
     * @brief Create a new gps sensor connecting to gpsd at the provided host and port.
     *
     * @param host String representation of the host to connect to.
     * @param port Port to connect to.
     *
     * @throw ConnectionError if connecting to gpsd failed
     */
    GpsdSensor(std::string host, int port);

    //! Destructor.
    virtual ~GpsdSensor();

    /*!
     * @brief Create a new gps sensor with the provided properties.
     *
     * Properties:
     *
     * - @b Host: name of the host to connect to (@e localhost will be used if missing)
     * - @b Port: port to connect to (@e DEFAULT_GPSD_PORT from @c gps.g will be used if missing
     * - @b MountingPosition: mounting position of the sensor, optional
     *
     * @param properties Properties for the sensor.
     *
     * @return Pointer to the created GpsdSensor if successful, null pointer if not
     */
    static GpsdSensor*
    create(boost::property_tree::ptree const& properties);

    //! Provide the capabilities of this sensor implementation.
    virtual IGpsSensor::Capabilities const&
    getCapabilities() const;

    //! Signal for received updates from GPSD
    typedef boost::signals2::signal<void(gps_data_t const&)> OnGpsd;

    /*!
     * @brief Subscribes a handler for received GPSD updates.
     *
     * @param handler A handler
     *
     * @return Connection
     */
    virtual boost::signals2::connection
    subscribeOnGpsdUpdate(OnGpsd::slot_type const& handler);

    /*
     * ISensor interface
     */

    virtual void
    setSensorManager(boost::optional<SensorManager&> manager);

    /*!
     * @brief Set the mounting position of the sensor.
     * @param pos The mounting position to be set.
     */
    void
    setMountingPosition(MountingPosition pos);

private:

    //! Initialize capabilities.
    void
    initCapabilities();

    /*!
     * @brief Connect to the gpsd and start reading values.
     *
     * @return false if connection failed
     */
    bool
    doStart();

    //! Stop reading values and close connection to gpsd.
    void
    doStop();

    //! Read values and process them (executed by the read thread).
    void
    doRead();

    //! Process read gps data and update internal sensor values accordingly.
    void
    processGpsData(gps_data_t const& data);

    //! The capabilities of this sensor.
    IGpsSensor::Capabilities capabilities_;

    //! Read thread.
    boost::scoped_ptr<boost::thread> readThread_;

    //! gpsd hostname
    std::string host_;

    //! gpsd port
    std::string port_;

    //! GPS data structure.
    boost::scoped_ptr<gps_data_t> gpsData_;

    //! Flag signaling the read thread to stop.
    mutable bool isStopping_;

    //! Mounting position
    ezC2X::MountingPosition mountPos_;

    //! Value to indicate a mounting position update
    bool mountingPositionHasChanged_;

    //! Signal for received GPSD
    GpsdSensor::OnGpsd onGpsd_;
};

}  // namespace ezC2X

#endif // EZC2X_LIBGPS
#endif // EZC2X_SENSOR_GPS_GPSD_SENSOR_H
