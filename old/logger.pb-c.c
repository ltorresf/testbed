/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: logger.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "logger.pb-c.h"
void   logger__init
                     (Logger         *message)
{
  static const Logger init_value = LOGGER__INIT;
  *message = init_value;
}
size_t logger__get_packed_size
                     (const Logger *message)
{
  assert(message->base.descriptor == &logger__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t logger__pack
                     (const Logger *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &logger__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t logger__pack_to_buffer
                     (const Logger *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &logger__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Logger *
       logger__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Logger *)
     protobuf_c_message_unpack (&logger__descriptor,
                                allocator, len, data);
}
void   logger__free_unpacked
                     (Logger *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &logger__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   frameparms__init
                     (Frameparms         *message)
{
  static const Frameparms init_value = FRAMEPARMS__INIT;
  *message = init_value;
}
size_t frameparms__get_packed_size
                     (const Frameparms *message)
{
  assert(message->base.descriptor == &frameparms__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t frameparms__pack
                     (const Frameparms *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &frameparms__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t frameparms__pack_to_buffer
                     (const Frameparms *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &frameparms__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Frameparms *
       frameparms__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Frameparms *)
     protobuf_c_message_unpack (&frameparms__descriptor,
                                allocator, len, data);
}
void   frameparms__free_unpacked
                     (Frameparms *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &frameparms__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor logger__field_descriptors[1] =
{
  {
    "frame_param",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Logger, frame_param),
    &frameparms__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned logger__field_indices_by_name[] = {
  0,   /* field[0] = frame_param */
};
static const ProtobufCIntRange logger__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor logger__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "logger",
  "Logger",
  "Logger",
  "",
  sizeof(Logger),
  1,
  logger__field_descriptors,
  logger__field_indices_by_name,
  1,  logger__number_ranges,
  (ProtobufCMessageInit) logger__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor frameparms__field_descriptors[2] =
{
  {
    "N_RB_DL",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Frameparms, n_rb_dl),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "N_RB_UL",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Frameparms, n_rb_ul),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned frameparms__field_indices_by_name[] = {
  0,   /* field[0] = N_RB_DL */
  1,   /* field[1] = N_RB_UL */
};
static const ProtobufCIntRange frameparms__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor frameparms__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Frameparms",
  "Frameparms",
  "Frameparms",
  "",
  sizeof(Frameparms),
  2,
  frameparms__field_descriptors,
  frameparms__field_indices_by_name,
  1,  frameparms__number_ranges,
  (ProtobufCMessageInit) frameparms__init,
  NULL,NULL,NULL    /* reserved[123] */
};
