// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: henangameproto.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "henangameproto.pb.h"

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
namespace henanmj {

namespace {

const ::google::protobuf::Descriptor* tagGameEnd_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  tagGameEnd_reflection_ = NULL;
const ::google::protobuf::Descriptor* AddFanInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AddFanInfo_reflection_ = NULL;
const ::google::protobuf::Descriptor* FanInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  FanInfo_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_henangameproto_2eproto() {
  protobuf_AddDesc_henangameproto_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "henangameproto.proto");
  GOOGLE_CHECK(file != NULL);
  tagGameEnd_descriptor_ = file->message_type(0);
  static const int tagGameEnd_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagGameEnd, faninfo_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagGameEnd, piaonum_),
  };
  tagGameEnd_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      tagGameEnd_descriptor_,
      tagGameEnd::default_instance_,
      tagGameEnd_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagGameEnd, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagGameEnd, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(tagGameEnd));
  AddFanInfo_descriptor_ = file->message_type(1);
  static const int AddFanInfo_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddFanInfo, addtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddFanInfo, addnum_),
  };
  AddFanInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AddFanInfo_descriptor_,
      AddFanInfo::default_instance_,
      AddFanInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddFanInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddFanInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AddFanInfo));
  FanInfo_descriptor_ = file->message_type(2);
  static const int FanInfo_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, chairid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, specialtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, fannum_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FanInfo, addfan_),
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
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_henangameproto_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    tagGameEnd_descriptor_, &tagGameEnd::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AddFanInfo_descriptor_, &AddFanInfo::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    FanInfo_descriptor_, &FanInfo::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_henangameproto_2eproto() {
  delete tagGameEnd::default_instance_;
  delete tagGameEnd_reflection_;
  delete AddFanInfo::default_instance_;
  delete AddFanInfo_reflection_;
  delete FanInfo::default_instance_;
  delete FanInfo_reflection_;
}

void protobuf_AddDesc_henangameproto_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\024henangameproto.proto\022\022proto.game.henan"
    "mj\"K\n\ntagGameEnd\022,\n\007faninfo\030\001 \003(\0132\033.prot"
    "o.game.henanmj.FanInfo\022\017\n\007piaoNum\030\002 \003(\005\""
    "-\n\nAddFanInfo\022\017\n\007AddType\030\001 \001(\005\022\016\n\006AddNum"
    "\030\002 \001(\005\"o\n\007FanInfo\022\017\n\007chairID\030\001 \002(\005\022\023\n\013Sp"
    "ecialType\030\002 \002(\005\022\016\n\006FanNum\030\003 \002(\005\022.\n\006addfa"
    "n\030\004 \003(\0132\036.proto.game.henanmj.AddFanInfo", 279);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "henangameproto.proto", &protobuf_RegisterTypes);
  tagGameEnd::default_instance_ = new tagGameEnd();
  AddFanInfo::default_instance_ = new AddFanInfo();
  FanInfo::default_instance_ = new FanInfo();
  tagGameEnd::default_instance_->InitAsDefaultInstance();
  AddFanInfo::default_instance_->InitAsDefaultInstance();
  FanInfo::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_henangameproto_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_henangameproto_2eproto {
  StaticDescriptorInitializer_henangameproto_2eproto() {
    protobuf_AddDesc_henangameproto_2eproto();
  }
} static_descriptor_initializer_henangameproto_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int tagGameEnd::kFaninfoFieldNumber;
const int tagGameEnd::kPiaoNumFieldNumber;
#endif  // !_MSC_VER

tagGameEnd::tagGameEnd()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void tagGameEnd::InitAsDefaultInstance() {
}

tagGameEnd::tagGameEnd(const tagGameEnd& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void tagGameEnd::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

tagGameEnd::~tagGameEnd() {
  SharedDtor();
}

void tagGameEnd::SharedDtor() {
  if (this != default_instance_) {
  }
}

void tagGameEnd::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* tagGameEnd::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return tagGameEnd_descriptor_;
}

const tagGameEnd& tagGameEnd::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_henangameproto_2eproto();
  return *default_instance_;
}

tagGameEnd* tagGameEnd::default_instance_ = NULL;

tagGameEnd* tagGameEnd::New() const {
  return new tagGameEnd;
}

void tagGameEnd::Clear() {
  faninfo_.Clear();
  piaonum_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool tagGameEnd::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .proto.game.henanmj.FanInfo faninfo = 1;
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
        if (input->ExpectTag(16)) goto parse_piaoNum;
        break;
      }

      // repeated int32 piaoNum = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_piaoNum:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 16, input, this->mutable_piaonum())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_piaonum())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_piaoNum;
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

void tagGameEnd::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .proto.game.henanmj.FanInfo faninfo = 1;
  for (int i = 0; i < this->faninfo_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->faninfo(i), output);
  }

  // repeated int32 piaoNum = 2;
  for (int i = 0; i < this->piaonum_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      2, this->piaonum(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* tagGameEnd::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .proto.game.henanmj.FanInfo faninfo = 1;
  for (int i = 0; i < this->faninfo_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->faninfo(i), target);
  }

  // repeated int32 piaoNum = 2;
  for (int i = 0; i < this->piaonum_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(2, this->piaonum(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int tagGameEnd::ByteSize() const {
  int total_size = 0;

  // repeated .proto.game.henanmj.FanInfo faninfo = 1;
  total_size += 1 * this->faninfo_size();
  for (int i = 0; i < this->faninfo_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->faninfo(i));
  }

  // repeated int32 piaoNum = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->piaonum_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->piaonum(i));
    }
    total_size += 1 * this->piaonum_size() + data_size;
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

void tagGameEnd::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const tagGameEnd* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const tagGameEnd*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void tagGameEnd::MergeFrom(const tagGameEnd& from) {
  GOOGLE_CHECK_NE(&from, this);
  faninfo_.MergeFrom(from.faninfo_);
  piaonum_.MergeFrom(from.piaonum_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void tagGameEnd::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void tagGameEnd::CopyFrom(const tagGameEnd& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool tagGameEnd::IsInitialized() const {

  for (int i = 0; i < faninfo_size(); i++) {
    if (!this->faninfo(i).IsInitialized()) return false;
  }
  return true;
}

void tagGameEnd::Swap(tagGameEnd* other) {
  if (other != this) {
    faninfo_.Swap(&other->faninfo_);
    piaonum_.Swap(&other->piaonum_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata tagGameEnd::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = tagGameEnd_descriptor_;
  metadata.reflection = tagGameEnd_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int AddFanInfo::kAddTypeFieldNumber;
const int AddFanInfo::kAddNumFieldNumber;
#endif  // !_MSC_VER

AddFanInfo::AddFanInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void AddFanInfo::InitAsDefaultInstance() {
}

AddFanInfo::AddFanInfo(const AddFanInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void AddFanInfo::SharedCtor() {
  _cached_size_ = 0;
  addtype_ = 0;
  addnum_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AddFanInfo::~AddFanInfo() {
  SharedDtor();
}

void AddFanInfo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void AddFanInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AddFanInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AddFanInfo_descriptor_;
}

const AddFanInfo& AddFanInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_henangameproto_2eproto();
  return *default_instance_;
}

AddFanInfo* AddFanInfo::default_instance_ = NULL;

AddFanInfo* AddFanInfo::New() const {
  return new AddFanInfo;
}

void AddFanInfo::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    addtype_ = 0;
    addnum_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool AddFanInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 AddType = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &addtype_)));
          set_has_addtype();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_AddNum;
        break;
      }

      // optional int32 AddNum = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_AddNum:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &addnum_)));
          set_has_addnum();
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

void AddFanInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 AddType = 1;
  if (has_addtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->addtype(), output);
  }

  // optional int32 AddNum = 2;
  if (has_addnum()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->addnum(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* AddFanInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 AddType = 1;
  if (has_addtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->addtype(), target);
  }

  // optional int32 AddNum = 2;
  if (has_addnum()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->addnum(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int AddFanInfo::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 AddType = 1;
    if (has_addtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->addtype());
    }

    // optional int32 AddNum = 2;
    if (has_addnum()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->addnum());
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

void AddFanInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const AddFanInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const AddFanInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void AddFanInfo::MergeFrom(const AddFanInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_addtype()) {
      set_addtype(from.addtype());
    }
    if (from.has_addnum()) {
      set_addnum(from.addnum());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void AddFanInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AddFanInfo::CopyFrom(const AddFanInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AddFanInfo::IsInitialized() const {

  return true;
}

void AddFanInfo::Swap(AddFanInfo* other) {
  if (other != this) {
    std::swap(addtype_, other->addtype_);
    std::swap(addnum_, other->addnum_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata AddFanInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AddFanInfo_descriptor_;
  metadata.reflection = AddFanInfo_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int FanInfo::kChairIDFieldNumber;
const int FanInfo::kSpecialTypeFieldNumber;
const int FanInfo::kFanNumFieldNumber;
const int FanInfo::kAddfanFieldNumber;
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
  specialtype_ = 0;
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
  if (default_instance_ == NULL) protobuf_AddDesc_henangameproto_2eproto();
  return *default_instance_;
}

FanInfo* FanInfo::default_instance_ = NULL;

FanInfo* FanInfo::New() const {
  return new FanInfo;
}

void FanInfo::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    chairid_ = 0;
    specialtype_ = 0;
    fannum_ = 0;
  }
  addfan_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool FanInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 chairID = 1;
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
        if (input->ExpectTag(16)) goto parse_SpecialType;
        break;
      }

      // required int32 SpecialType = 2;
      case 2: {
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
        if (input->ExpectTag(24)) goto parse_FanNum;
        break;
      }

      // required int32 FanNum = 3;
      case 3: {
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
        if (input->ExpectTag(34)) goto parse_addfan;
        break;
      }

      // repeated .proto.game.henanmj.AddFanInfo addfan = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_addfan:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_addfan()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_addfan;
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
  // required int32 chairID = 1;
  if (has_chairid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->chairid(), output);
  }

  // required int32 SpecialType = 2;
  if (has_specialtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->specialtype(), output);
  }

  // required int32 FanNum = 3;
  if (has_fannum()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->fannum(), output);
  }

  // repeated .proto.game.henanmj.AddFanInfo addfan = 4;
  for (int i = 0; i < this->addfan_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, this->addfan(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* FanInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 chairID = 1;
  if (has_chairid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->chairid(), target);
  }

  // required int32 SpecialType = 2;
  if (has_specialtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->specialtype(), target);
  }

  // required int32 FanNum = 3;
  if (has_fannum()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->fannum(), target);
  }

  // repeated .proto.game.henanmj.AddFanInfo addfan = 4;
  for (int i = 0; i < this->addfan_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        4, this->addfan(i), target);
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
    // required int32 chairID = 1;
    if (has_chairid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->chairid());
    }

    // required int32 SpecialType = 2;
    if (has_specialtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->specialtype());
    }

    // required int32 FanNum = 3;
    if (has_fannum()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->fannum());
    }

  }
  // repeated .proto.game.henanmj.AddFanInfo addfan = 4;
  total_size += 1 * this->addfan_size();
  for (int i = 0; i < this->addfan_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->addfan(i));
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
  addfan_.MergeFrom(from.addfan_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_chairid()) {
      set_chairid(from.chairid());
    }
    if (from.has_specialtype()) {
      set_specialtype(from.specialtype());
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
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;

  return true;
}

void FanInfo::Swap(FanInfo* other) {
  if (other != this) {
    std::swap(chairid_, other->chairid_);
    std::swap(specialtype_, other->specialtype_);
    std::swap(fannum_, other->fannum_);
    addfan_.Swap(&other->addfan_);
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


// @@protoc_insertion_point(namespace_scope)

}  // namespace henanmj
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
