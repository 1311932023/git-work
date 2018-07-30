// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: yudu.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "yudu.pb.h"

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
namespace yudu {

namespace {

const ::google::protobuf::Descriptor* YuDuGameEnd_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  YuDuGameEnd_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_yudu_2eproto() {
  protobuf_AddDesc_yudu_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "yudu.proto");
  GOOGLE_CHECK(file != NULL);
  YuDuGameEnd_descriptor_ = file->message_type(0);
  static const int YuDuGameEnd_offsets_[9] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, hufen_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, shangbaofen_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, genzhuangfen_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, zhigangfen_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, wangangfen_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, angangfen_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, zhigangjing_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, wangangjing_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, angangjing_),
  };
  YuDuGameEnd_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      YuDuGameEnd_descriptor_,
      YuDuGameEnd::default_instance_,
      YuDuGameEnd_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(YuDuGameEnd, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(YuDuGameEnd));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_yudu_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    YuDuGameEnd_descriptor_, &YuDuGameEnd::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_yudu_2eproto() {
  delete YuDuGameEnd::default_instance_;
  delete YuDuGameEnd_reflection_;
}

void protobuf_AddDesc_yudu_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\nyudu.proto\022\017proto.game.yudu\"\300\001\n\013YuDuGa"
    "meEnd\022\r\n\005hufen\030\001 \003(\005\022\023\n\013shangbaofen\030\002 \003("
    "\005\022\024\n\014genzhuangfen\030\003 \003(\005\022\022\n\nzhigangfen\030\004 "
    "\003(\005\022\022\n\nwangangfen\030\005 \003(\005\022\021\n\tangangfen\030\006 \003"
    "(\005\022\023\n\013zhigangjing\030\007 \003(\005\022\023\n\013wangangjing\030\010"
    " \003(\005\022\022\n\nangangjing\030\t \003(\005", 224);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "yudu.proto", &protobuf_RegisterTypes);
  YuDuGameEnd::default_instance_ = new YuDuGameEnd();
  YuDuGameEnd::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_yudu_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_yudu_2eproto {
  StaticDescriptorInitializer_yudu_2eproto() {
    protobuf_AddDesc_yudu_2eproto();
  }
} static_descriptor_initializer_yudu_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int YuDuGameEnd::kHufenFieldNumber;
const int YuDuGameEnd::kShangbaofenFieldNumber;
const int YuDuGameEnd::kGenzhuangfenFieldNumber;
const int YuDuGameEnd::kZhigangfenFieldNumber;
const int YuDuGameEnd::kWangangfenFieldNumber;
const int YuDuGameEnd::kAngangfenFieldNumber;
const int YuDuGameEnd::kZhigangjingFieldNumber;
const int YuDuGameEnd::kWangangjingFieldNumber;
const int YuDuGameEnd::kAngangjingFieldNumber;
#endif  // !_MSC_VER

YuDuGameEnd::YuDuGameEnd()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void YuDuGameEnd::InitAsDefaultInstance() {
}

YuDuGameEnd::YuDuGameEnd(const YuDuGameEnd& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void YuDuGameEnd::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

YuDuGameEnd::~YuDuGameEnd() {
  SharedDtor();
}

void YuDuGameEnd::SharedDtor() {
  if (this != default_instance_) {
  }
}

void YuDuGameEnd::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* YuDuGameEnd::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return YuDuGameEnd_descriptor_;
}

const YuDuGameEnd& YuDuGameEnd::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_yudu_2eproto();
  return *default_instance_;
}

YuDuGameEnd* YuDuGameEnd::default_instance_ = NULL;

YuDuGameEnd* YuDuGameEnd::New() const {
  return new YuDuGameEnd;
}

void YuDuGameEnd::Clear() {
  hufen_.Clear();
  shangbaofen_.Clear();
  genzhuangfen_.Clear();
  zhigangfen_.Clear();
  wangangfen_.Clear();
  angangfen_.Clear();
  zhigangjing_.Clear();
  wangangjing_.Clear();
  angangjing_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool YuDuGameEnd::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated int32 hufen = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_hufen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 8, input, this->mutable_hufen())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_hufen())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(8)) goto parse_hufen;
        if (input->ExpectTag(16)) goto parse_shangbaofen;
        break;
      }

      // repeated int32 shangbaofen = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_shangbaofen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 16, input, this->mutable_shangbaofen())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_shangbaofen())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_shangbaofen;
        if (input->ExpectTag(24)) goto parse_genzhuangfen;
        break;
      }

      // repeated int32 genzhuangfen = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_genzhuangfen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 24, input, this->mutable_genzhuangfen())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_genzhuangfen())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_genzhuangfen;
        if (input->ExpectTag(32)) goto parse_zhigangfen;
        break;
      }

      // repeated int32 zhigangfen = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_zhigangfen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 32, input, this->mutable_zhigangfen())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_zhigangfen())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_zhigangfen;
        if (input->ExpectTag(40)) goto parse_wangangfen;
        break;
      }

      // repeated int32 wangangfen = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_wangangfen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 40, input, this->mutable_wangangfen())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_wangangfen())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(40)) goto parse_wangangfen;
        if (input->ExpectTag(48)) goto parse_angangfen;
        break;
      }

      // repeated int32 angangfen = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_angangfen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 48, input, this->mutable_angangfen())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_angangfen())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(48)) goto parse_angangfen;
        if (input->ExpectTag(56)) goto parse_zhigangjing;
        break;
      }

      // repeated int32 zhigangjing = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_zhigangjing:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 56, input, this->mutable_zhigangjing())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_zhigangjing())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(56)) goto parse_zhigangjing;
        if (input->ExpectTag(64)) goto parse_wangangjing;
        break;
      }

      // repeated int32 wangangjing = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_wangangjing:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 64, input, this->mutable_wangangjing())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_wangangjing())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(64)) goto parse_wangangjing;
        if (input->ExpectTag(72)) goto parse_angangjing;
        break;
      }

      // repeated int32 angangjing = 9;
      case 9: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_angangjing:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 72, input, this->mutable_angangjing())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_angangjing())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(72)) goto parse_angangjing;
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

void YuDuGameEnd::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated int32 hufen = 1;
  for (int i = 0; i < this->hufen_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      1, this->hufen(i), output);
  }

  // repeated int32 shangbaofen = 2;
  for (int i = 0; i < this->shangbaofen_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      2, this->shangbaofen(i), output);
  }

  // repeated int32 genzhuangfen = 3;
  for (int i = 0; i < this->genzhuangfen_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      3, this->genzhuangfen(i), output);
  }

  // repeated int32 zhigangfen = 4;
  for (int i = 0; i < this->zhigangfen_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      4, this->zhigangfen(i), output);
  }

  // repeated int32 wangangfen = 5;
  for (int i = 0; i < this->wangangfen_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      5, this->wangangfen(i), output);
  }

  // repeated int32 angangfen = 6;
  for (int i = 0; i < this->angangfen_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      6, this->angangfen(i), output);
  }

  // repeated int32 zhigangjing = 7;
  for (int i = 0; i < this->zhigangjing_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      7, this->zhigangjing(i), output);
  }

  // repeated int32 wangangjing = 8;
  for (int i = 0; i < this->wangangjing_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      8, this->wangangjing(i), output);
  }

  // repeated int32 angangjing = 9;
  for (int i = 0; i < this->angangjing_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      9, this->angangjing(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* YuDuGameEnd::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated int32 hufen = 1;
  for (int i = 0; i < this->hufen_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(1, this->hufen(i), target);
  }

  // repeated int32 shangbaofen = 2;
  for (int i = 0; i < this->shangbaofen_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(2, this->shangbaofen(i), target);
  }

  // repeated int32 genzhuangfen = 3;
  for (int i = 0; i < this->genzhuangfen_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(3, this->genzhuangfen(i), target);
  }

  // repeated int32 zhigangfen = 4;
  for (int i = 0; i < this->zhigangfen_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(4, this->zhigangfen(i), target);
  }

  // repeated int32 wangangfen = 5;
  for (int i = 0; i < this->wangangfen_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(5, this->wangangfen(i), target);
  }

  // repeated int32 angangfen = 6;
  for (int i = 0; i < this->angangfen_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(6, this->angangfen(i), target);
  }

  // repeated int32 zhigangjing = 7;
  for (int i = 0; i < this->zhigangjing_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(7, this->zhigangjing(i), target);
  }

  // repeated int32 wangangjing = 8;
  for (int i = 0; i < this->wangangjing_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(8, this->wangangjing(i), target);
  }

  // repeated int32 angangjing = 9;
  for (int i = 0; i < this->angangjing_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(9, this->angangjing(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int YuDuGameEnd::ByteSize() const {
  int total_size = 0;

  // repeated int32 hufen = 1;
  {
    int data_size = 0;
    for (int i = 0; i < this->hufen_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->hufen(i));
    }
    total_size += 1 * this->hufen_size() + data_size;
  }

  // repeated int32 shangbaofen = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->shangbaofen_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->shangbaofen(i));
    }
    total_size += 1 * this->shangbaofen_size() + data_size;
  }

  // repeated int32 genzhuangfen = 3;
  {
    int data_size = 0;
    for (int i = 0; i < this->genzhuangfen_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->genzhuangfen(i));
    }
    total_size += 1 * this->genzhuangfen_size() + data_size;
  }

  // repeated int32 zhigangfen = 4;
  {
    int data_size = 0;
    for (int i = 0; i < this->zhigangfen_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->zhigangfen(i));
    }
    total_size += 1 * this->zhigangfen_size() + data_size;
  }

  // repeated int32 wangangfen = 5;
  {
    int data_size = 0;
    for (int i = 0; i < this->wangangfen_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->wangangfen(i));
    }
    total_size += 1 * this->wangangfen_size() + data_size;
  }

  // repeated int32 angangfen = 6;
  {
    int data_size = 0;
    for (int i = 0; i < this->angangfen_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->angangfen(i));
    }
    total_size += 1 * this->angangfen_size() + data_size;
  }

  // repeated int32 zhigangjing = 7;
  {
    int data_size = 0;
    for (int i = 0; i < this->zhigangjing_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->zhigangjing(i));
    }
    total_size += 1 * this->zhigangjing_size() + data_size;
  }

  // repeated int32 wangangjing = 8;
  {
    int data_size = 0;
    for (int i = 0; i < this->wangangjing_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->wangangjing(i));
    }
    total_size += 1 * this->wangangjing_size() + data_size;
  }

  // repeated int32 angangjing = 9;
  {
    int data_size = 0;
    for (int i = 0; i < this->angangjing_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->angangjing(i));
    }
    total_size += 1 * this->angangjing_size() + data_size;
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

void YuDuGameEnd::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const YuDuGameEnd* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const YuDuGameEnd*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void YuDuGameEnd::MergeFrom(const YuDuGameEnd& from) {
  GOOGLE_CHECK_NE(&from, this);
  hufen_.MergeFrom(from.hufen_);
  shangbaofen_.MergeFrom(from.shangbaofen_);
  genzhuangfen_.MergeFrom(from.genzhuangfen_);
  zhigangfen_.MergeFrom(from.zhigangfen_);
  wangangfen_.MergeFrom(from.wangangfen_);
  angangfen_.MergeFrom(from.angangfen_);
  zhigangjing_.MergeFrom(from.zhigangjing_);
  wangangjing_.MergeFrom(from.wangangjing_);
  angangjing_.MergeFrom(from.angangjing_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void YuDuGameEnd::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void YuDuGameEnd::CopyFrom(const YuDuGameEnd& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool YuDuGameEnd::IsInitialized() const {

  return true;
}

void YuDuGameEnd::Swap(YuDuGameEnd* other) {
  if (other != this) {
    hufen_.Swap(&other->hufen_);
    shangbaofen_.Swap(&other->shangbaofen_);
    genzhuangfen_.Swap(&other->genzhuangfen_);
    zhigangfen_.Swap(&other->zhigangfen_);
    wangangfen_.Swap(&other->wangangfen_);
    angangfen_.Swap(&other->angangfen_);
    zhigangjing_.Swap(&other->zhigangjing_);
    wangangjing_.Swap(&other->wangangjing_);
    angangjing_.Swap(&other->angangjing_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata YuDuGameEnd::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = YuDuGameEnd_descriptor_;
  metadata.reflection = YuDuGameEnd_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace yudu
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
