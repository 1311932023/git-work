// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ningdegameproto.proto

#ifndef PROTOBUF_ningdegameproto_2eproto__INCLUDED
#define PROTOBUF_ningdegameproto_2eproto__INCLUDED

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
namespace ningdemj {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_ningdegameproto_2eproto();
void protobuf_AssignDesc_ningdegameproto_2eproto();
void protobuf_ShutdownFile_ningdegameproto_2eproto();

class NingeGameEnd;

// ===================================================================

class NingeGameEnd : public ::google::protobuf::Message {
 public:
  NingeGameEnd();
  virtual ~NingeGameEnd();

  NingeGameEnd(const NingeGameEnd& from);

  inline NingeGameEnd& operator=(const NingeGameEnd& from) {
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
  static const NingeGameEnd& default_instance();

  void Swap(NingeGameEnd* other);

  // implements Message ----------------------------------------------

  NingeGameEnd* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const NingeGameEnd& from);
  void MergeFrom(const NingeGameEnd& from);
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

  // repeated int32 jinshu = 1;
  inline int jinshu_size() const;
  inline void clear_jinshu();
  static const int kJinshuFieldNumber = 1;
  inline ::google::protobuf::int32 jinshu(int index) const;
  inline void set_jinshu(int index, ::google::protobuf::int32 value);
  inline void add_jinshu(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      jinshu() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_jinshu();

  // repeated int32 gangTimes = 2;
  inline int gangtimes_size() const;
  inline void clear_gangtimes();
  static const int kGangTimesFieldNumber = 2;
  inline ::google::protobuf::int32 gangtimes(int index) const;
  inline void set_gangtimes(int index, ::google::protobuf::int32 value);
  inline void add_gangtimes(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      gangtimes() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_gangtimes();

  // @@protoc_insertion_point(class_scope:proto.game.ningdemj.NingeGameEnd)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > jinshu_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > gangtimes_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_ningdegameproto_2eproto();
  friend void protobuf_AssignDesc_ningdegameproto_2eproto();
  friend void protobuf_ShutdownFile_ningdegameproto_2eproto();

  void InitAsDefaultInstance();
  static NingeGameEnd* default_instance_;
};
// ===================================================================


// ===================================================================

// NingeGameEnd

// repeated int32 jinshu = 1;
inline int NingeGameEnd::jinshu_size() const {
  return jinshu_.size();
}
inline void NingeGameEnd::clear_jinshu() {
  jinshu_.Clear();
}
inline ::google::protobuf::int32 NingeGameEnd::jinshu(int index) const {
  return jinshu_.Get(index);
}
inline void NingeGameEnd::set_jinshu(int index, ::google::protobuf::int32 value) {
  jinshu_.Set(index, value);
}
inline void NingeGameEnd::add_jinshu(::google::protobuf::int32 value) {
  jinshu_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
NingeGameEnd::jinshu() const {
  return jinshu_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
NingeGameEnd::mutable_jinshu() {
  return &jinshu_;
}

// repeated int32 gangTimes = 2;
inline int NingeGameEnd::gangtimes_size() const {
  return gangtimes_.size();
}
inline void NingeGameEnd::clear_gangtimes() {
  gangtimes_.Clear();
}
inline ::google::protobuf::int32 NingeGameEnd::gangtimes(int index) const {
  return gangtimes_.Get(index);
}
inline void NingeGameEnd::set_gangtimes(int index, ::google::protobuf::int32 value) {
  gangtimes_.Set(index, value);
}
inline void NingeGameEnd::add_gangtimes(::google::protobuf::int32 value) {
  gangtimes_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
NingeGameEnd::gangtimes() const {
  return gangtimes_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
NingeGameEnd::mutable_gangtimes() {
  return &gangtimes_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace ningdemj
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ningdegameproto_2eproto__INCLUDED
