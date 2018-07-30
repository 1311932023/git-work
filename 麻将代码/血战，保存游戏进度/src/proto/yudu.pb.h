// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: yudu.proto

#ifndef PROTOBUF_yudu_2eproto__INCLUDED
#define PROTOBUF_yudu_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace proto {
namespace game {
namespace yudu {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_yudu_2eproto();
void protobuf_AssignDesc_yudu_2eproto();
void protobuf_ShutdownFile_yudu_2eproto();

class YuDuGameEnd;

// ===================================================================

class YuDuGameEnd : public ::google::protobuf::Message {
 public:
  YuDuGameEnd();
  virtual ~YuDuGameEnd();

  YuDuGameEnd(const YuDuGameEnd& from);

  inline YuDuGameEnd& operator=(const YuDuGameEnd& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const YuDuGameEnd& default_instance();

  void Swap(YuDuGameEnd* other);

  // implements Message ----------------------------------------------

  YuDuGameEnd* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const YuDuGameEnd& from);
  void MergeFrom(const YuDuGameEnd& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated int32 hufen = 1;
  inline int hufen_size() const;
  inline void clear_hufen();
  static const int kHufenFieldNumber = 1;
  inline ::google::protobuf::int32 hufen(int index) const;
  inline void set_hufen(int index, ::google::protobuf::int32 value);
  inline void add_hufen(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      hufen() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_hufen();

  // repeated int32 shangbaofen = 2;
  inline int shangbaofen_size() const;
  inline void clear_shangbaofen();
  static const int kShangbaofenFieldNumber = 2;
  inline ::google::protobuf::int32 shangbaofen(int index) const;
  inline void set_shangbaofen(int index, ::google::protobuf::int32 value);
  inline void add_shangbaofen(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      shangbaofen() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_shangbaofen();

  // repeated int32 genzhuangfen = 3;
  inline int genzhuangfen_size() const;
  inline void clear_genzhuangfen();
  static const int kGenzhuangfenFieldNumber = 3;
  inline ::google::protobuf::int32 genzhuangfen(int index) const;
  inline void set_genzhuangfen(int index, ::google::protobuf::int32 value);
  inline void add_genzhuangfen(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      genzhuangfen() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_genzhuangfen();

  // repeated int32 zhigangfen = 4;
  inline int zhigangfen_size() const;
  inline void clear_zhigangfen();
  static const int kZhigangfenFieldNumber = 4;
  inline ::google::protobuf::int32 zhigangfen(int index) const;
  inline void set_zhigangfen(int index, ::google::protobuf::int32 value);
  inline void add_zhigangfen(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      zhigangfen() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_zhigangfen();

  // repeated int32 wangangfen = 5;
  inline int wangangfen_size() const;
  inline void clear_wangangfen();
  static const int kWangangfenFieldNumber = 5;
  inline ::google::protobuf::int32 wangangfen(int index) const;
  inline void set_wangangfen(int index, ::google::protobuf::int32 value);
  inline void add_wangangfen(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      wangangfen() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_wangangfen();

  // repeated int32 angangfen = 6;
  inline int angangfen_size() const;
  inline void clear_angangfen();
  static const int kAngangfenFieldNumber = 6;
  inline ::google::protobuf::int32 angangfen(int index) const;
  inline void set_angangfen(int index, ::google::protobuf::int32 value);
  inline void add_angangfen(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      angangfen() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_angangfen();

  // repeated int32 zhigangjing = 7;
  inline int zhigangjing_size() const;
  inline void clear_zhigangjing();
  static const int kZhigangjingFieldNumber = 7;
  inline ::google::protobuf::int32 zhigangjing(int index) const;
  inline void set_zhigangjing(int index, ::google::protobuf::int32 value);
  inline void add_zhigangjing(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      zhigangjing() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_zhigangjing();

  // repeated int32 wangangjing = 8;
  inline int wangangjing_size() const;
  inline void clear_wangangjing();
  static const int kWangangjingFieldNumber = 8;
  inline ::google::protobuf::int32 wangangjing(int index) const;
  inline void set_wangangjing(int index, ::google::protobuf::int32 value);
  inline void add_wangangjing(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      wangangjing() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_wangangjing();

  // repeated int32 angangjing = 9;
  inline int angangjing_size() const;
  inline void clear_angangjing();
  static const int kAngangjingFieldNumber = 9;
  inline ::google::protobuf::int32 angangjing(int index) const;
  inline void set_angangjing(int index, ::google::protobuf::int32 value);
  inline void add_angangjing(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      angangjing() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_angangjing();

  // @@protoc_insertion_point(class_scope:proto.game.yudu.YuDuGameEnd)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > hufen_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > shangbaofen_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > genzhuangfen_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > zhigangfen_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > wangangfen_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > angangfen_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > zhigangjing_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > wangangjing_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > angangjing_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(9 + 31) / 32];

  friend void  protobuf_AddDesc_yudu_2eproto();
  friend void protobuf_AssignDesc_yudu_2eproto();
  friend void protobuf_ShutdownFile_yudu_2eproto();

  void InitAsDefaultInstance();
  static YuDuGameEnd* default_instance_;
};
// ===================================================================


// ===================================================================

// YuDuGameEnd

// repeated int32 hufen = 1;
inline int YuDuGameEnd::hufen_size() const {
  return hufen_.size();
}
inline void YuDuGameEnd::clear_hufen() {
  hufen_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::hufen(int index) const {
  return hufen_.Get(index);
}
inline void YuDuGameEnd::set_hufen(int index, ::google::protobuf::int32 value) {
  hufen_.Set(index, value);
}
inline void YuDuGameEnd::add_hufen(::google::protobuf::int32 value) {
  hufen_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::hufen() const {
  return hufen_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_hufen() {
  return &hufen_;
}

// repeated int32 shangbaofen = 2;
inline int YuDuGameEnd::shangbaofen_size() const {
  return shangbaofen_.size();
}
inline void YuDuGameEnd::clear_shangbaofen() {
  shangbaofen_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::shangbaofen(int index) const {
  return shangbaofen_.Get(index);
}
inline void YuDuGameEnd::set_shangbaofen(int index, ::google::protobuf::int32 value) {
  shangbaofen_.Set(index, value);
}
inline void YuDuGameEnd::add_shangbaofen(::google::protobuf::int32 value) {
  shangbaofen_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::shangbaofen() const {
  return shangbaofen_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_shangbaofen() {
  return &shangbaofen_;
}

// repeated int32 genzhuangfen = 3;
inline int YuDuGameEnd::genzhuangfen_size() const {
  return genzhuangfen_.size();
}
inline void YuDuGameEnd::clear_genzhuangfen() {
  genzhuangfen_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::genzhuangfen(int index) const {
  return genzhuangfen_.Get(index);
}
inline void YuDuGameEnd::set_genzhuangfen(int index, ::google::protobuf::int32 value) {
  genzhuangfen_.Set(index, value);
}
inline void YuDuGameEnd::add_genzhuangfen(::google::protobuf::int32 value) {
  genzhuangfen_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::genzhuangfen() const {
  return genzhuangfen_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_genzhuangfen() {
  return &genzhuangfen_;
}

// repeated int32 zhigangfen = 4;
inline int YuDuGameEnd::zhigangfen_size() const {
  return zhigangfen_.size();
}
inline void YuDuGameEnd::clear_zhigangfen() {
  zhigangfen_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::zhigangfen(int index) const {
  return zhigangfen_.Get(index);
}
inline void YuDuGameEnd::set_zhigangfen(int index, ::google::protobuf::int32 value) {
  zhigangfen_.Set(index, value);
}
inline void YuDuGameEnd::add_zhigangfen(::google::protobuf::int32 value) {
  zhigangfen_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::zhigangfen() const {
  return zhigangfen_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_zhigangfen() {
  return &zhigangfen_;
}

// repeated int32 wangangfen = 5;
inline int YuDuGameEnd::wangangfen_size() const {
  return wangangfen_.size();
}
inline void YuDuGameEnd::clear_wangangfen() {
  wangangfen_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::wangangfen(int index) const {
  return wangangfen_.Get(index);
}
inline void YuDuGameEnd::set_wangangfen(int index, ::google::protobuf::int32 value) {
  wangangfen_.Set(index, value);
}
inline void YuDuGameEnd::add_wangangfen(::google::protobuf::int32 value) {
  wangangfen_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::wangangfen() const {
  return wangangfen_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_wangangfen() {
  return &wangangfen_;
}

// repeated int32 angangfen = 6;
inline int YuDuGameEnd::angangfen_size() const {
  return angangfen_.size();
}
inline void YuDuGameEnd::clear_angangfen() {
  angangfen_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::angangfen(int index) const {
  return angangfen_.Get(index);
}
inline void YuDuGameEnd::set_angangfen(int index, ::google::protobuf::int32 value) {
  angangfen_.Set(index, value);
}
inline void YuDuGameEnd::add_angangfen(::google::protobuf::int32 value) {
  angangfen_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::angangfen() const {
  return angangfen_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_angangfen() {
  return &angangfen_;
}

// repeated int32 zhigangjing = 7;
inline int YuDuGameEnd::zhigangjing_size() const {
  return zhigangjing_.size();
}
inline void YuDuGameEnd::clear_zhigangjing() {
  zhigangjing_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::zhigangjing(int index) const {
  return zhigangjing_.Get(index);
}
inline void YuDuGameEnd::set_zhigangjing(int index, ::google::protobuf::int32 value) {
  zhigangjing_.Set(index, value);
}
inline void YuDuGameEnd::add_zhigangjing(::google::protobuf::int32 value) {
  zhigangjing_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::zhigangjing() const {
  return zhigangjing_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_zhigangjing() {
  return &zhigangjing_;
}

// repeated int32 wangangjing = 8;
inline int YuDuGameEnd::wangangjing_size() const {
  return wangangjing_.size();
}
inline void YuDuGameEnd::clear_wangangjing() {
  wangangjing_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::wangangjing(int index) const {
  return wangangjing_.Get(index);
}
inline void YuDuGameEnd::set_wangangjing(int index, ::google::protobuf::int32 value) {
  wangangjing_.Set(index, value);
}
inline void YuDuGameEnd::add_wangangjing(::google::protobuf::int32 value) {
  wangangjing_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::wangangjing() const {
  return wangangjing_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_wangangjing() {
  return &wangangjing_;
}

// repeated int32 angangjing = 9;
inline int YuDuGameEnd::angangjing_size() const {
  return angangjing_.size();
}
inline void YuDuGameEnd::clear_angangjing() {
  angangjing_.Clear();
}
inline ::google::protobuf::int32 YuDuGameEnd::angangjing(int index) const {
  return angangjing_.Get(index);
}
inline void YuDuGameEnd::set_angangjing(int index, ::google::protobuf::int32 value) {
  angangjing_.Set(index, value);
}
inline void YuDuGameEnd::add_angangjing(::google::protobuf::int32 value) {
  angangjing_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
YuDuGameEnd::angangjing() const {
  return angangjing_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
YuDuGameEnd::mutable_angangjing() {
  return &angangjing_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace yudu
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_yudu_2eproto__INCLUDED
