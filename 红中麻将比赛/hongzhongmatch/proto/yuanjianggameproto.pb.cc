// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: yuanjianggameproto.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "yuanjianggameproto.pb.h"

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
namespace yuanjiangmj {

namespace {

const ::google::protobuf::Descriptor* yjgamescene_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  yjgamescene_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_yuanjianggameproto_2eproto() {
  protobuf_AddDesc_yuanjianggameproto_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "yuanjianggameproto.proto");
  GOOGLE_CHECK(file != NULL);
  yjgamescene_descriptor_ = file->message_type(0);
  static const int yjgamescene_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(yjgamescene, optype2_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(yjgamescene, baotingflag_),
  };
  yjgamescene_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      yjgamescene_descriptor_,
      yjgamescene::default_instance_,
      yjgamescene_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(yjgamescene, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(yjgamescene, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(yjgamescene));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_yuanjianggameproto_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    yjgamescene_descriptor_, &yjgamescene::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_yuanjianggameproto_2eproto() {
  delete yjgamescene::default_instance_;
  delete yjgamescene_reflection_;
}

void protobuf_AddDesc_yuanjianggameproto_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030yuanjianggameproto.proto\022\026proto.game.y"
    "uanjiangmj\"3\n\013yjgamescene\022\017\n\007OpType2\030\001 \001"
    "(\005\022\023\n\013BaoTingFlag\030\002 \003(\005", 103);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "yuanjianggameproto.proto", &protobuf_RegisterTypes);
  yjgamescene::default_instance_ = new yjgamescene();
  yjgamescene::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_yuanjianggameproto_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_yuanjianggameproto_2eproto {
  StaticDescriptorInitializer_yuanjianggameproto_2eproto() {
    protobuf_AddDesc_yuanjianggameproto_2eproto();
  }
} static_descriptor_initializer_yuanjianggameproto_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int yjgamescene::kOpType2FieldNumber;
const int yjgamescene::kBaoTingFlagFieldNumber;
#endif  // !_MSC_VER

yjgamescene::yjgamescene()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void yjgamescene::InitAsDefaultInstance() {
}

yjgamescene::yjgamescene(const yjgamescene& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void yjgamescene::SharedCtor() {
  _cached_size_ = 0;
  optype2_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

yjgamescene::~yjgamescene() {
  SharedDtor();
}

void yjgamescene::SharedDtor() {
  if (this != default_instance_) {
  }
}

void yjgamescene::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* yjgamescene::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return yjgamescene_descriptor_;
}

const yjgamescene& yjgamescene::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_yuanjianggameproto_2eproto();
  return *default_instance_;
}

yjgamescene* yjgamescene::default_instance_ = NULL;

yjgamescene* yjgamescene::New() const {
  return new yjgamescene;
}

void yjgamescene::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    optype2_ = 0;
  }
  baotingflag_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool yjgamescene::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 OpType2 = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &optype2_)));
          set_has_optype2();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_BaoTingFlag;
        break;
      }

      // repeated int32 BaoTingFlag = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_BaoTingFlag:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 16, input, this->mutable_baotingflag())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_baotingflag())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_BaoTingFlag;
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

void yjgamescene::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 OpType2 = 1;
  if (has_optype2()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->optype2(), output);
  }

  // repeated int32 BaoTingFlag = 2;
  for (int i = 0; i < this->baotingflag_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      2, this->baotingflag(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* yjgamescene::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 OpType2 = 1;
  if (has_optype2()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->optype2(), target);
  }

  // repeated int32 BaoTingFlag = 2;
  for (int i = 0; i < this->baotingflag_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(2, this->baotingflag(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int yjgamescene::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 OpType2 = 1;
    if (has_optype2()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->optype2());
    }

  }
  // repeated int32 BaoTingFlag = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->baotingflag_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->baotingflag(i));
    }
    total_size += 1 * this->baotingflag_size() + data_size;
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

void yjgamescene::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const yjgamescene* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const yjgamescene*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void yjgamescene::MergeFrom(const yjgamescene& from) {
  GOOGLE_CHECK_NE(&from, this);
  baotingflag_.MergeFrom(from.baotingflag_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_optype2()) {
      set_optype2(from.optype2());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void yjgamescene::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void yjgamescene::CopyFrom(const yjgamescene& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool yjgamescene::IsInitialized() const {

  return true;
}

void yjgamescene::Swap(yjgamescene* other) {
  if (other != this) {
    std::swap(optype2_, other->optype2_);
    baotingflag_.Swap(&other->baotingflag_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata yjgamescene::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = yjgamescene_descriptor_;
  metadata.reflection = yjgamescene_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace yuanjiangmj
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
