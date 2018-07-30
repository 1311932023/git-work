// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: xuezhan.proto

#ifndef PROTOBUF_xuezhan_2eproto__INCLUDED
#define PROTOBUF_xuezhan_2eproto__INCLUDED

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
namespace xuezhan {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_xuezhan_2eproto();
void protobuf_AssignDesc_xuezhan_2eproto();
void protobuf_ShutdownFile_xuezhan_2eproto();

class XueZhanHuanPai;
class XueZhanHuanPaiAck;
class XueZhanGameSence;

// ===================================================================

class XueZhanHuanPai : public ::google::protobuf::Message {
 public:
  XueZhanHuanPai();
  virtual ~XueZhanHuanPai();

  XueZhanHuanPai(const XueZhanHuanPai& from);

  inline XueZhanHuanPai& operator=(const XueZhanHuanPai& from) {
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
  static const XueZhanHuanPai& default_instance();

  void Swap(XueZhanHuanPai* other);

  // implements Message ----------------------------------------------

  XueZhanHuanPai* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const XueZhanHuanPai& from);
  void MergeFrom(const XueZhanHuanPai& from);
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

  // repeated int32 cards = 2;
  inline int cards_size() const;
  inline void clear_cards();
  static const int kCardsFieldNumber = 2;
  inline ::google::protobuf::int32 cards(int index) const;
  inline void set_cards(int index, ::google::protobuf::int32 value);
  inline void add_cards(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      cards() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_cards();

  // @@protoc_insertion_point(class_scope:proto.game.xuezhan.XueZhanHuanPai)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > cards_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_xuezhan_2eproto();
  friend void protobuf_AssignDesc_xuezhan_2eproto();
  friend void protobuf_ShutdownFile_xuezhan_2eproto();

  void InitAsDefaultInstance();
  static XueZhanHuanPai* default_instance_;
};
// -------------------------------------------------------------------

class XueZhanHuanPaiAck : public ::google::protobuf::Message {
 public:
  XueZhanHuanPaiAck();
  virtual ~XueZhanHuanPaiAck();

  XueZhanHuanPaiAck(const XueZhanHuanPaiAck& from);

  inline XueZhanHuanPaiAck& operator=(const XueZhanHuanPaiAck& from) {
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
  static const XueZhanHuanPaiAck& default_instance();

  void Swap(XueZhanHuanPaiAck* other);

  // implements Message ----------------------------------------------

  XueZhanHuanPaiAck* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const XueZhanHuanPaiAck& from);
  void MergeFrom(const XueZhanHuanPaiAck& from);
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

  // repeated int32 my_cards = 1;
  inline int my_cards_size() const;
  inline void clear_my_cards();
  static const int kMyCardsFieldNumber = 1;
  inline ::google::protobuf::int32 my_cards(int index) const;
  inline void set_my_cards(int index, ::google::protobuf::int32 value);
  inline void add_my_cards(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      my_cards() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_my_cards();

  // repeated int32 other_cards = 2;
  inline int other_cards_size() const;
  inline void clear_other_cards();
  static const int kOtherCardsFieldNumber = 2;
  inline ::google::protobuf::int32 other_cards(int index) const;
  inline void set_other_cards(int index, ::google::protobuf::int32 value);
  inline void add_other_cards(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      other_cards() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_other_cards();

  // @@protoc_insertion_point(class_scope:proto.game.xuezhan.XueZhanHuanPaiAck)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > my_cards_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > other_cards_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_xuezhan_2eproto();
  friend void protobuf_AssignDesc_xuezhan_2eproto();
  friend void protobuf_ShutdownFile_xuezhan_2eproto();

  void InitAsDefaultInstance();
  static XueZhanHuanPaiAck* default_instance_;
};
// -------------------------------------------------------------------

class XueZhanGameSence : public ::google::protobuf::Message {
 public:
  XueZhanGameSence();
  virtual ~XueZhanGameSence();

  XueZhanGameSence(const XueZhanGameSence& from);

  inline XueZhanGameSence& operator=(const XueZhanGameSence& from) {
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
  static const XueZhanGameSence& default_instance();

  void Swap(XueZhanGameSence* other);

  // implements Message ----------------------------------------------

  XueZhanGameSence* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const XueZhanGameSence& from);
  void MergeFrom(const XueZhanGameSence& from);
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

  // required int32 isHuanPai = 1;
  inline bool has_ishuanpai() const;
  inline void clear_ishuanpai();
  static const int kIsHuanPaiFieldNumber = 1;
  inline ::google::protobuf::int32 ishuanpai() const;
  inline void set_ishuanpai(::google::protobuf::int32 value);

  // required int32 isDingQue = 2;
  inline bool has_isdingque() const;
  inline void clear_isdingque();
  static const int kIsDingQueFieldNumber = 2;
  inline ::google::protobuf::int32 isdingque() const;
  inline void set_isdingque(::google::protobuf::int32 value);

  // repeated int32 DingQueCardType = 3;
  inline int dingquecardtype_size() const;
  inline void clear_dingquecardtype();
  static const int kDingQueCardTypeFieldNumber = 3;
  inline ::google::protobuf::int32 dingquecardtype(int index) const;
  inline void set_dingquecardtype(int index, ::google::protobuf::int32 value);
  inline void add_dingquecardtype(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      dingquecardtype() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_dingquecardtype();

  // @@protoc_insertion_point(class_scope:proto.game.xuezhan.XueZhanGameSence)
 private:
  inline void set_has_ishuanpai();
  inline void clear_has_ishuanpai();
  inline void set_has_isdingque();
  inline void clear_has_isdingque();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 ishuanpai_;
  ::google::protobuf::int32 isdingque_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > dingquecardtype_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_xuezhan_2eproto();
  friend void protobuf_AssignDesc_xuezhan_2eproto();
  friend void protobuf_ShutdownFile_xuezhan_2eproto();

  void InitAsDefaultInstance();
  static XueZhanGameSence* default_instance_;
};
// ===================================================================


// ===================================================================

// XueZhanHuanPai

// repeated int32 cards = 2;
inline int XueZhanHuanPai::cards_size() const {
  return cards_.size();
}
inline void XueZhanHuanPai::clear_cards() {
  cards_.Clear();
}
inline ::google::protobuf::int32 XueZhanHuanPai::cards(int index) const {
  return cards_.Get(index);
}
inline void XueZhanHuanPai::set_cards(int index, ::google::protobuf::int32 value) {
  cards_.Set(index, value);
}
inline void XueZhanHuanPai::add_cards(::google::protobuf::int32 value) {
  cards_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
XueZhanHuanPai::cards() const {
  return cards_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
XueZhanHuanPai::mutable_cards() {
  return &cards_;
}

// -------------------------------------------------------------------

// XueZhanHuanPaiAck

// repeated int32 my_cards = 1;
inline int XueZhanHuanPaiAck::my_cards_size() const {
  return my_cards_.size();
}
inline void XueZhanHuanPaiAck::clear_my_cards() {
  my_cards_.Clear();
}
inline ::google::protobuf::int32 XueZhanHuanPaiAck::my_cards(int index) const {
  return my_cards_.Get(index);
}
inline void XueZhanHuanPaiAck::set_my_cards(int index, ::google::protobuf::int32 value) {
  my_cards_.Set(index, value);
}
inline void XueZhanHuanPaiAck::add_my_cards(::google::protobuf::int32 value) {
  my_cards_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
XueZhanHuanPaiAck::my_cards() const {
  return my_cards_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
XueZhanHuanPaiAck::mutable_my_cards() {
  return &my_cards_;
}

// repeated int32 other_cards = 2;
inline int XueZhanHuanPaiAck::other_cards_size() const {
  return other_cards_.size();
}
inline void XueZhanHuanPaiAck::clear_other_cards() {
  other_cards_.Clear();
}
inline ::google::protobuf::int32 XueZhanHuanPaiAck::other_cards(int index) const {
  return other_cards_.Get(index);
}
inline void XueZhanHuanPaiAck::set_other_cards(int index, ::google::protobuf::int32 value) {
  other_cards_.Set(index, value);
}
inline void XueZhanHuanPaiAck::add_other_cards(::google::protobuf::int32 value) {
  other_cards_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
XueZhanHuanPaiAck::other_cards() const {
  return other_cards_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
XueZhanHuanPaiAck::mutable_other_cards() {
  return &other_cards_;
}

// -------------------------------------------------------------------

// XueZhanGameSence

// required int32 isHuanPai = 1;
inline bool XueZhanGameSence::has_ishuanpai() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void XueZhanGameSence::set_has_ishuanpai() {
  _has_bits_[0] |= 0x00000001u;
}
inline void XueZhanGameSence::clear_has_ishuanpai() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void XueZhanGameSence::clear_ishuanpai() {
  ishuanpai_ = 0;
  clear_has_ishuanpai();
}
inline ::google::protobuf::int32 XueZhanGameSence::ishuanpai() const {
  return ishuanpai_;
}
inline void XueZhanGameSence::set_ishuanpai(::google::protobuf::int32 value) {
  set_has_ishuanpai();
  ishuanpai_ = value;
}

// required int32 isDingQue = 2;
inline bool XueZhanGameSence::has_isdingque() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void XueZhanGameSence::set_has_isdingque() {
  _has_bits_[0] |= 0x00000002u;
}
inline void XueZhanGameSence::clear_has_isdingque() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void XueZhanGameSence::clear_isdingque() {
  isdingque_ = 0;
  clear_has_isdingque();
}
inline ::google::protobuf::int32 XueZhanGameSence::isdingque() const {
  return isdingque_;
}
inline void XueZhanGameSence::set_isdingque(::google::protobuf::int32 value) {
  set_has_isdingque();
  isdingque_ = value;
}

// repeated int32 DingQueCardType = 3;
inline int XueZhanGameSence::dingquecardtype_size() const {
  return dingquecardtype_.size();
}
inline void XueZhanGameSence::clear_dingquecardtype() {
  dingquecardtype_.Clear();
}
inline ::google::protobuf::int32 XueZhanGameSence::dingquecardtype(int index) const {
  return dingquecardtype_.Get(index);
}
inline void XueZhanGameSence::set_dingquecardtype(int index, ::google::protobuf::int32 value) {
  dingquecardtype_.Set(index, value);
}
inline void XueZhanGameSence::add_dingquecardtype(::google::protobuf::int32 value) {
  dingquecardtype_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
XueZhanGameSence::dingquecardtype() const {
  return dingquecardtype_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
XueZhanGameSence::mutable_dingquecardtype() {
  return &dingquecardtype_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace xuezhan
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_xuezhan_2eproto__INCLUDED
