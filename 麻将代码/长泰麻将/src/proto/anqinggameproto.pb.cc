// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: anqinggameproto.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "anqinggameproto.pb.h"

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
namespace anqingmj {

namespace {

const ::google::protobuf::Descriptor* AnQingGameEnd_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AnQingGameEnd_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_anqinggameproto_2eproto() {
  protobuf_AddDesc_anqinggameproto_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "anqinggameproto.proto");
  GOOGLE_CHECK(file != NULL);
  AnQingGameEnd_descriptor_ = file->message_type(0);
  static const int AnQingGameEnd_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AnQingGameEnd, buhuanum_),
  };
  AnQingGameEnd_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AnQingGameEnd_descriptor_,
      AnQingGameEnd::default_instance_,
      AnQingGameEnd_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AnQingGameEnd, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AnQingGameEnd, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AnQingGameEnd));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_anqinggameproto_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AnQingGameEnd_descriptor_, &AnQingGameEnd::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_anqinggameproto_2eproto() {
  delete AnQingGameEnd::default_instance_;
  delete AnQingGameEnd_reflection_;
}

void protobuf_AddDesc_anqinggameproto_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\025anqinggameproto.proto\022\023proto.game.anqi"
    "ngmj\"!\n\rAnQingGameEnd\022\020\n\010BuHuaNum\030\001 \003(\005", 79);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "anqinggameproto.proto", &protobuf_RegisterTypes);
  AnQingGameEnd::default_instance_ = new AnQingGameEnd();
  AnQingGameEnd::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_anqinggameproto_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_anqinggameproto_2eproto {
  StaticDescriptorInitializer_anqinggameproto_2eproto() {
    protobuf_AddDesc_anqinggameproto_2eproto();
  }
} static_descriptor_initializer_anqinggameproto_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int AnQingGameEnd::kBuHuaNumFieldNumber;
#endif  // !_MSC_VER

AnQingGameEnd::AnQingGameEnd()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void AnQingGameEnd::InitAsDefaultInstance() {
}

AnQingGameEnd::AnQingGameEnd(const AnQingGameEnd& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void AnQingGameEnd::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AnQingGameEnd::~AnQingGameEnd() {
  SharedDtor();
}

void AnQingGameEnd::SharedDtor() {
  if (this != default_instance_) {
  }
}

void AnQingGameEnd::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AnQingGameEnd::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AnQingGameEnd_descriptor_;
}

const AnQingGameEnd& AnQingGameEnd::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_anqinggameproto_2eproto();
  return *default_instance_;
}

AnQingGameEnd* AnQingGameEnd::default_instance_ = NULL;

AnQingGameEnd* AnQingGameEnd::New() const {
  return new AnQingGameEnd;
}

void AnQingGameEnd::Clear() {
  buhuanum_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool AnQingGameEnd::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated int32 BuHuaNum = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_BuHuaNum:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 8, input, this->mutable_buhuanum())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_buhuanum())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(8)) goto parse_BuHuaNum;
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

void AnQingGameEnd::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated int32 BuHuaNum = 1;
  for (int i = 0; i < this->buhuanum_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      1, this->buhuanum(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* AnQingGameEnd::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated int32 BuHuaNum = 1;
  for (int i = 0; i < this->buhuanum_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(1, this->buhuanum(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int AnQingGameEnd::ByteSize() const {
  int total_size = 0;

  // repeated int32 BuHuaNum = 1;
  {
    int data_size = 0;
    for (int i = 0; i < this->buhuanum_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->buhuanum(i));
    }
    total_size += 1 * this->buhuanum_size() + data_size;
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

void AnQingGameEnd::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const AnQingGameEnd* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const AnQingGameEnd*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void AnQingGameEnd::MergeFrom(const AnQingGameEnd& from) {
  GOOGLE_CHECK_NE(&from, this);
  buhuanum_.MergeFrom(from.buhuanum_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void AnQingGameEnd::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AnQingGameEnd::CopyFrom(const AnQingGameEnd& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AnQingGameEnd::IsInitialized() const {

  return true;
}

void AnQingGameEnd::Swap(AnQingGameEnd* other) {
  if (other != this) {
    buhuanum_.Swap(&other->buhuanum_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata AnQingGameEnd::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AnQingGameEnd_descriptor_;
  metadata.reflection = AnQingGameEnd_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace anqingmj
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
