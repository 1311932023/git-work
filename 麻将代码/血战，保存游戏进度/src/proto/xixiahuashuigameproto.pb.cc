// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: xixiahuashuigameproto.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "xixiahuashuigameproto.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace proto {
namespace game {
namespace xixiahuashuimj {

namespace {

const ::google::protobuf::Descriptor* XiXiaHuaShuiGameEnd_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  XiXiaHuaShuiGameEnd_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_xixiahuashuigameproto_2eproto() {
  protobuf_AddDesc_xixiahuashuigameproto_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "xixiahuashuigameproto.proto");
  GOOGLE_CHECK(file != NULL);
  XiXiaHuaShuiGameEnd_descriptor_ = file->message_type(0);
  static const int XiXiaHuaShuiGameEnd_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(XiXiaHuaShuiGameEnd, huangzhuang_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(XiXiaHuaShuiGameEnd, genzhuang_),
  };
  XiXiaHuaShuiGameEnd_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      XiXiaHuaShuiGameEnd_descriptor_,
      XiXiaHuaShuiGameEnd::default_instance_,
      XiXiaHuaShuiGameEnd_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(XiXiaHuaShuiGameEnd, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(XiXiaHuaShuiGameEnd, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(XiXiaHuaShuiGameEnd));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_xixiahuashuigameproto_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    XiXiaHuaShuiGameEnd_descriptor_, &XiXiaHuaShuiGameEnd::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_xixiahuashuigameproto_2eproto() {
  delete XiXiaHuaShuiGameEnd::default_instance_;
  delete XiXiaHuaShuiGameEnd_reflection_;
}

void protobuf_AddDesc_xixiahuashuigameproto_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\033xixiahuashuigameproto.proto\022\031proto.gam"
    "e.xixiahuashuimj\"=\n\023XiXiaHuaShuiGameEnd\022"
    "\023\n\013huangzhuang\030\001 \003(\005\022\021\n\tgenzhuang\030\002 \003(\005", 119);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "xixiahuashuigameproto.proto", &protobuf_RegisterTypes);
  XiXiaHuaShuiGameEnd::default_instance_ = new XiXiaHuaShuiGameEnd();
  XiXiaHuaShuiGameEnd::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_xixiahuashuigameproto_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_xixiahuashuigameproto_2eproto {
  StaticDescriptorInitializer_xixiahuashuigameproto_2eproto() {
    protobuf_AddDesc_xixiahuashuigameproto_2eproto();
  }
} static_descriptor_initializer_xixiahuashuigameproto_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int XiXiaHuaShuiGameEnd::kHuangzhuangFieldNumber;
const int XiXiaHuaShuiGameEnd::kGenzhuangFieldNumber;
#endif  // !_MSC_VER

XiXiaHuaShuiGameEnd::XiXiaHuaShuiGameEnd()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void XiXiaHuaShuiGameEnd::InitAsDefaultInstance() {
}

XiXiaHuaShuiGameEnd::XiXiaHuaShuiGameEnd(const XiXiaHuaShuiGameEnd& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void XiXiaHuaShuiGameEnd::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

XiXiaHuaShuiGameEnd::~XiXiaHuaShuiGameEnd() {
  SharedDtor();
}

void XiXiaHuaShuiGameEnd::SharedDtor() {
  if (this != default_instance_) {
  }
}

void XiXiaHuaShuiGameEnd::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* XiXiaHuaShuiGameEnd::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return XiXiaHuaShuiGameEnd_descriptor_;
}

const XiXiaHuaShuiGameEnd& XiXiaHuaShuiGameEnd::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_xixiahuashuigameproto_2eproto();
  return *default_instance_;
}

XiXiaHuaShuiGameEnd* XiXiaHuaShuiGameEnd::default_instance_ = NULL;

XiXiaHuaShuiGameEnd* XiXiaHuaShuiGameEnd::New() const {
  return new XiXiaHuaShuiGameEnd;
}

void XiXiaHuaShuiGameEnd::Clear() {
  huangzhuang_.Clear();
  genzhuang_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool XiXiaHuaShuiGameEnd::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated int32 huangzhuang = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_huangzhuang:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 8, input, this->mutable_huangzhuang())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_huangzhuang())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(8)) goto parse_huangzhuang;
        if (input->ExpectTag(16)) goto parse_genzhuang;
        break;
      }

      // repeated int32 genzhuang = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_genzhuang:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 16, input, this->mutable_genzhuang())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_genzhuang())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_genzhuang;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void XiXiaHuaShuiGameEnd::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated int32 huangzhuang = 1;
  for (int i = 0; i < this->huangzhuang_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      1, this->huangzhuang(i), output);
  }

  // repeated int32 genzhuang = 2;
  for (int i = 0; i < this->genzhuang_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      2, this->genzhuang(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* XiXiaHuaShuiGameEnd::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated int32 huangzhuang = 1;
  for (int i = 0; i < this->huangzhuang_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(1, this->huangzhuang(i), target);
  }

  // repeated int32 genzhuang = 2;
  for (int i = 0; i < this->genzhuang_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(2, this->genzhuang(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int XiXiaHuaShuiGameEnd::ByteSize() const {
  int total_size = 0;

  // repeated int32 huangzhuang = 1;
  {
    int data_size = 0;
    for (int i = 0; i < this->huangzhuang_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->huangzhuang(i));
    }
    total_size += 1 * this->huangzhuang_size() + data_size;
  }

  // repeated int32 genzhuang = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->genzhuang_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->genzhuang(i));
    }
    total_size += 1 * this->genzhuang_size() + data_size;
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void XiXiaHuaShuiGameEnd::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const XiXiaHuaShuiGameEnd* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const XiXiaHuaShuiGameEnd*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void XiXiaHuaShuiGameEnd::MergeFrom(const XiXiaHuaShuiGameEnd& from) {
  GOOGLE_CHECK_NE(&from, this);
  huangzhuang_.MergeFrom(from.huangzhuang_);
  genzhuang_.MergeFrom(from.genzhuang_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void XiXiaHuaShuiGameEnd::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void XiXiaHuaShuiGameEnd::CopyFrom(const XiXiaHuaShuiGameEnd& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool XiXiaHuaShuiGameEnd::IsInitialized() const {

  return true;
}

void XiXiaHuaShuiGameEnd::Swap(XiXiaHuaShuiGameEnd* other) {
  if (other != this) {
    huangzhuang_.Swap(&other->huangzhuang_);
    genzhuang_.Swap(&other->genzhuang_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata XiXiaHuaShuiGameEnd::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = XiXiaHuaShuiGameEnd_descriptor_;
  metadata.reflection = XiXiaHuaShuiGameEnd_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace xixiahuashuimj
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
