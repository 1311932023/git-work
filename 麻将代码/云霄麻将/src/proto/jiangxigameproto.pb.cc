// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: jiangxigameproto.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "jiangxigameproto.pb.h"

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
namespace jxmahjonggame {

namespace {

const ::google::protobuf::Descriptor* jxgamescene_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  jxgamescene_reflection_ = NULL;
const ::google::protobuf::Descriptor* AckGameStart_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AckGameStart_reflection_ = NULL;
const ::google::protobuf::Descriptor* FanInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  FanInfo_reflection_ = NULL;
const ::google::protobuf::Descriptor* jxmjGameEndInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  jxmjGameEndInfo_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_jiangxigameproto_2eproto() {
  protobuf_AddDesc_jiangxigameproto_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "jiangxigameproto.proto");
  GOOGLE_CHECK(file != NULL);
  jxgamescene_descriptor_ = file->message_type(0);
  static const int jxgamescene_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxgamescene, roomtype_),
  };
  jxgamescene_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      jxgamescene_descriptor_,
      jxgamescene::default_instance_,
      jxgamescene_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxgamescene, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxgamescene, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(jxgamescene));
  AckGameStart_descriptor_ = file->message_type(1);
  static const int AckGameStart_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AckGameStart, bankerid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AckGameStart, quanfeng_),
  };
  AckGameStart_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AckGameStart_descriptor_,
      AckGameStart::default_instance_,
      AckGameStart_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AckGameStart, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AckGameStart, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AckGameStart));
  FanInfo_descriptor_ = file->message_type(2);
  static const int FanInfo_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, chairid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, endtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, specialtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, fantype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, fannum_),
  };
  FanInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      FanInfo_descriptor_,
      FanInfo::default_instance_,
      FanInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(FanInfo));
  jxmjGameEndInfo_descriptor_ = file->message_type(3);
  static const int jxmjGameEndInfo_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxmjGameEndInfo, faninfo_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxmjGameEndInfo, huscore_),
  };
  jxmjGameEndInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      jxmjGameEndInfo_descriptor_,
      jxmjGameEndInfo::default_instance_,
      jxmjGameEndInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxmjGameEndInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(jxmjGameEndInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(jxmjGameEndInfo));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_jiangxigameproto_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    jxgamescene_descriptor_, &jxgamescene::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AckGameStart_descriptor_, &AckGameStart::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    FanInfo_descriptor_, &FanInfo::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    jxmjGameEndInfo_descriptor_, &jxmjGameEndInfo::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_jiangxigameproto_2eproto() {
  delete jxgamescene::default_instance_;
  delete jxgamescene_reflection_;
  delete AckGameStart::default_instance_;
  delete AckGameStart_reflection_;
  delete FanInfo::default_instance_;
  delete FanInfo_reflection_;
  delete jxmjGameEndInfo::default_instance_;
  delete jxmjGameEndInfo_reflection_;
}

void protobuf_AddDesc_jiangxigameproto_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\026jiangxigameproto.proto\022\030proto.game.jxm"
    "ahjonggame\"\037\n\013jxgamescene\022\020\n\010roomType\030\001 "
    "\001(\005\"2\n\014AckGameStart\022\020\n\010bankerid\030\001 \002(\005\022\020\n"
    "\010quanfeng\030\002 \001(\005\"a\n\007FanInfo\022\017\n\007chairID\030\001 "
    "\001(\005\022\017\n\007EndType\030\002 \001(\005\022\023\n\013SpecialType\030\003 \001("
    "\005\022\017\n\007FanType\030\004 \001(\005\022\016\n\006FanNum\030\005 \001(\005\"V\n\017jx"
    "mjGameEndInfo\0222\n\007faninfo\030\001 \003(\0132!.proto.g"
    "ame.jxmahjonggame.FanInfo\022\017\n\007HuScore\030\002 \003"
    "(\005", 322);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "jiangxigameproto.proto", &protobuf_RegisterTypes);
  jxgamescene::default_instance_ = new jxgamescene();
  AckGameStart::default_instance_ = new AckGameStart();
  FanInfo::default_instance_ = new FanInfo();
  jxmjGameEndInfo::default_instance_ = new jxmjGameEndInfo();
  jxgamescene::default_instance_->InitAsDefaultInstance();
  AckGameStart::default_instance_->InitAsDefaultInstance();
  FanInfo::default_instance_->InitAsDefaultInstance();
  jxmjGameEndInfo::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_jiangxigameproto_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_jiangxigameproto_2eproto {
  StaticDescriptorInitializer_jiangxigameproto_2eproto() {
    protobuf_AddDesc_jiangxigameproto_2eproto();
  }
} static_descriptor_initializer_jiangxigameproto_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int jxgamescene::kRoomTypeFieldNumber;
#endif  // !_MSC_VER

jxgamescene::jxgamescene()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void jxgamescene::InitAsDefaultInstance() {
}

jxgamescene::jxgamescene(const jxgamescene& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void jxgamescene::SharedCtor() {
  _cached_size_ = 0;
  roomtype_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

jxgamescene::~jxgamescene() {
  SharedDtor();
}

void jxgamescene::SharedDtor() {
  if (this != default_instance_) {
  }
}

void jxgamescene::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* jxgamescene::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return jxgamescene_descriptor_;
}

const jxgamescene& jxgamescene::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jiangxigameproto_2eproto();
  return *default_instance_;
}

jxgamescene* jxgamescene::default_instance_ = NULL;

jxgamescene* jxgamescene::New() const {
  return new jxgamescene;
}

void jxgamescene::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    roomtype_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool jxgamescene::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 roomType = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &roomtype_)));
          set_has_roomtype();
        } else {
          goto handle_uninterpreted;
        }
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

void jxgamescene::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 roomType = 1;
  if (has_roomtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->roomtype(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* jxgamescene::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 roomType = 1;
  if (has_roomtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->roomtype(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int jxgamescene::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 roomType = 1;
    if (has_roomtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->roomtype());
    }

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

void jxgamescene::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const jxgamescene* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const jxgamescene*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void jxgamescene::MergeFrom(const jxgamescene& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_roomtype()) {
      set_roomtype(from.roomtype());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void jxgamescene::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void jxgamescene::CopyFrom(const jxgamescene& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool jxgamescene::IsInitialized() const {

  return true;
}

void jxgamescene::Swap(jxgamescene* other) {
  if (other != this) {
    std::swap(roomtype_, other->roomtype_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata jxgamescene::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = jxgamescene_descriptor_;
  metadata.reflection = jxgamescene_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int AckGameStart::kBankeridFieldNumber;
const int AckGameStart::kQuanfengFieldNumber;
#endif  // !_MSC_VER

AckGameStart::AckGameStart()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void AckGameStart::InitAsDefaultInstance() {
}

AckGameStart::AckGameStart(const AckGameStart& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void AckGameStart::SharedCtor() {
  _cached_size_ = 0;
  bankerid_ = 0;
  quanfeng_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AckGameStart::~AckGameStart() {
  SharedDtor();
}

void AckGameStart::SharedDtor() {
  if (this != default_instance_) {
  }
}

void AckGameStart::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AckGameStart::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AckGameStart_descriptor_;
}

const AckGameStart& AckGameStart::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jiangxigameproto_2eproto();
  return *default_instance_;
}

AckGameStart* AckGameStart::default_instance_ = NULL;

AckGameStart* AckGameStart::New() const {
  return new AckGameStart;
}

void AckGameStart::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    bankerid_ = 0;
    quanfeng_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool AckGameStart::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 bankerid = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &bankerid_)));
          set_has_bankerid();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_quanfeng;
        break;
      }

      // optional int32 quanfeng = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_quanfeng:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &quanfeng_)));
          set_has_quanfeng();
        } else {
          goto handle_uninterpreted;
        }
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

void AckGameStart::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 bankerid = 1;
  if (has_bankerid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->bankerid(), output);
  }

  // optional int32 quanfeng = 2;
  if (has_quanfeng()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->quanfeng(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* AckGameStart::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 bankerid = 1;
  if (has_bankerid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->bankerid(), target);
  }

  // optional int32 quanfeng = 2;
  if (has_quanfeng()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->quanfeng(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int AckGameStart::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 bankerid = 1;
    if (has_bankerid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->bankerid());
    }

    // optional int32 quanfeng = 2;
    if (has_quanfeng()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->quanfeng());
    }

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

void AckGameStart::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const AckGameStart* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const AckGameStart*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void AckGameStart::MergeFrom(const AckGameStart& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_bankerid()) {
      set_bankerid(from.bankerid());
    }
    if (from.has_quanfeng()) {
      set_quanfeng(from.quanfeng());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void AckGameStart::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AckGameStart::CopyFrom(const AckGameStart& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AckGameStart::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void AckGameStart::Swap(AckGameStart* other) {
  if (other != this) {
    std::swap(bankerid_, other->bankerid_);
    std::swap(quanfeng_, other->quanfeng_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata AckGameStart::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AckGameStart_descriptor_;
  metadata.reflection = AckGameStart_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int FanInfo::kChairIDFieldNumber;
const int FanInfo::kEndTypeFieldNumber;
const int FanInfo::kSpecialTypeFieldNumber;
const int FanInfo::kFanTypeFieldNumber;
const int FanInfo::kFanNumFieldNumber;
#endif  // !_MSC_VER

FanInfo::FanInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void FanInfo::InitAsDefaultInstance() {
}

FanInfo::FanInfo(const FanInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void FanInfo::SharedCtor() {
  _cached_size_ = 0;
  chairid_ = 0;
  endtype_ = 0;
  specialtype_ = 0;
  fantype_ = 0;
  fannum_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

FanInfo::~FanInfo() {
  SharedDtor();
}

void FanInfo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void FanInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* FanInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return FanInfo_descriptor_;
}

const FanInfo& FanInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jiangxigameproto_2eproto();
  return *default_instance_;
}

FanInfo* FanInfo::default_instance_ = NULL;

FanInfo* FanInfo::New() const {
  return new FanInfo;
}

void FanInfo::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    chairid_ = 0;
    endtype_ = 0;
    specialtype_ = 0;
    fantype_ = 0;
    fannum_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool FanInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 chairID = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &chairid_)));
          set_has_chairid();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_EndType;
        break;
      }

      // optional int32 EndType = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_EndType:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &endtype_)));
          set_has_endtype();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_SpecialType;
        break;
      }

      // optional int32 SpecialType = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_SpecialType:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &specialtype_)));
          set_has_specialtype();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_FanType;
        break;
      }

      // optional int32 FanType = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_FanType:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &fantype_)));
          set_has_fantype();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(40)) goto parse_FanNum;
        break;
      }

      // optional int32 FanNum = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_FanNum:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &fannum_)));
          set_has_fannum();
        } else {
          goto handle_uninterpreted;
        }
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

void FanInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 chairID = 1;
  if (has_chairid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->chairid(), output);
  }

  // optional int32 EndType = 2;
  if (has_endtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->endtype(), output);
  }

  // optional int32 SpecialType = 3;
  if (has_specialtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->specialtype(), output);
  }

  // optional int32 FanType = 4;
  if (has_fantype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->fantype(), output);
  }

  // optional int32 FanNum = 5;
  if (has_fannum()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(5, this->fannum(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* FanInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 chairID = 1;
  if (has_chairid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->chairid(), target);
  }

  // optional int32 EndType = 2;
  if (has_endtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->endtype(), target);
  }

  // optional int32 SpecialType = 3;
  if (has_specialtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->specialtype(), target);
  }

  // optional int32 FanType = 4;
  if (has_fantype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->fantype(), target);
  }

  // optional int32 FanNum = 5;
  if (has_fannum()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(5, this->fannum(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int FanInfo::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 chairID = 1;
    if (has_chairid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->chairid());
    }

    // optional int32 EndType = 2;
    if (has_endtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->endtype());
    }

    // optional int32 SpecialType = 3;
    if (has_specialtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->specialtype());
    }

    // optional int32 FanType = 4;
    if (has_fantype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->fantype());
    }

    // optional int32 FanNum = 5;
    if (has_fannum()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->fannum());
    }

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

void FanInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const FanInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const FanInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void FanInfo::MergeFrom(const FanInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_chairid()) {
      set_chairid(from.chairid());
    }
    if (from.has_endtype()) {
      set_endtype(from.endtype());
    }
    if (from.has_specialtype()) {
      set_specialtype(from.specialtype());
    }
    if (from.has_fantype()) {
      set_fantype(from.fantype());
    }
    if (from.has_fannum()) {
      set_fannum(from.fannum());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void FanInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FanInfo::CopyFrom(const FanInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FanInfo::IsInitialized() const {

  return true;
}

void FanInfo::Swap(FanInfo* other) {
  if (other != this) {
    std::swap(chairid_, other->chairid_);
    std::swap(endtype_, other->endtype_);
    std::swap(specialtype_, other->specialtype_);
    std::swap(fantype_, other->fantype_);
    std::swap(fannum_, other->fannum_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata FanInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = FanInfo_descriptor_;
  metadata.reflection = FanInfo_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int jxmjGameEndInfo::kFaninfoFieldNumber;
const int jxmjGameEndInfo::kHuScoreFieldNumber;
#endif  // !_MSC_VER

jxmjGameEndInfo::jxmjGameEndInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void jxmjGameEndInfo::InitAsDefaultInstance() {
}

jxmjGameEndInfo::jxmjGameEndInfo(const jxmjGameEndInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void jxmjGameEndInfo::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

jxmjGameEndInfo::~jxmjGameEndInfo() {
  SharedDtor();
}

void jxmjGameEndInfo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void jxmjGameEndInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* jxmjGameEndInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return jxmjGameEndInfo_descriptor_;
}

const jxmjGameEndInfo& jxmjGameEndInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jiangxigameproto_2eproto();
  return *default_instance_;
}

jxmjGameEndInfo* jxmjGameEndInfo::default_instance_ = NULL;

jxmjGameEndInfo* jxmjGameEndInfo::New() const {
  return new jxmjGameEndInfo;
}

void jxmjGameEndInfo::Clear() {
  faninfo_.Clear();
  huscore_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool jxmjGameEndInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .proto.game.jxmahjonggame.FanInfo faninfo = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_faninfo:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_faninfo()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_faninfo;
        if (input->ExpectTag(16)) goto parse_HuScore;
        break;
      }

      // repeated int32 HuScore = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_HuScore:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 16, input, this->mutable_huscore())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_huscore())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_HuScore;
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

void jxmjGameEndInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .proto.game.jxmahjonggame.FanInfo faninfo = 1;
  for (int i = 0; i < this->faninfo_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->faninfo(i), output);
  }

  // repeated int32 HuScore = 2;
  for (int i = 0; i < this->huscore_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      2, this->huscore(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* jxmjGameEndInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .proto.game.jxmahjonggame.FanInfo faninfo = 1;
  for (int i = 0; i < this->faninfo_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->faninfo(i), target);
  }

  // repeated int32 HuScore = 2;
  for (int i = 0; i < this->huscore_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(2, this->huscore(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int jxmjGameEndInfo::ByteSize() const {
  int total_size = 0;

  // repeated .proto.game.jxmahjonggame.FanInfo faninfo = 1;
  total_size += 1 * this->faninfo_size();
  for (int i = 0; i < this->faninfo_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->faninfo(i));
  }

  // repeated int32 HuScore = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->huscore_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->huscore(i));
    }
    total_size += 1 * this->huscore_size() + data_size;
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

void jxmjGameEndInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const jxmjGameEndInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const jxmjGameEndInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void jxmjGameEndInfo::MergeFrom(const jxmjGameEndInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  faninfo_.MergeFrom(from.faninfo_);
  huscore_.MergeFrom(from.huscore_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void jxmjGameEndInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void jxmjGameEndInfo::CopyFrom(const jxmjGameEndInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool jxmjGameEndInfo::IsInitialized() const {

  return true;
}

void jxmjGameEndInfo::Swap(jxmjGameEndInfo* other) {
  if (other != this) {
    faninfo_.Swap(&other->faninfo_);
    huscore_.Swap(&other->huscore_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata jxmjGameEndInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = jxmjGameEndInfo_descriptor_;
  metadata.reflection = jxmjGameEndInfo_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace jxmahjonggame
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
