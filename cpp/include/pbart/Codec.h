/*
 *  Copyright (c) 2014-2016 olibre (olibre@Lmap.org)
 *
 *  This file is part of the library pbart (Protocol Buffers At RunTime).
 *
 *  pbart is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License (AGPL)
 *  version 3 or (at your option) any later version.
 * 
 *  You should have received a copy of the GNU Affero General Public License 
 *  along with pbart (maybe in file LICENSE).
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PBART_CODEC_H_
#define PBART_CODEC_H_

#include <string>

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif

//#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/wire_format.h"

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#include "pbart/Message.h"

namespace gp  = ::google::protobuf;
namespace gpi = ::google::protobuf::internal;

namespace pbart
{
  class PBART_EXPORT Codec
  {
  public:
    typedef gp::io::CodedInputStream InputStream;

  // Entry point
  public:
    template <typename CHAR_T, typename SIZE_T>
    static void encode (Message const&, const CHAR_T buffer[], SIZE_T, int rest);
    static void encode (Message const&, Buffer& buffer);
    static int  length (Message const&);

    template <typename CHAR_T, typename SIZE_T>
    static int  decode (Message&, const CHAR_T  buffer[], SIZE_T, bool concatenated);
    static int  decode (Message&, const Buffer& buffer          , bool concatenated);

    template <typename CHAR>
    static void bufferToPtr (const Buffer&, const CHAR*& ptr, int& size);
    static Id   id          (const Buffer&, const std::string& header);

  // size() private functions
  private:
    static int  lengthSeqBool    (const Variant::SeqBool&);
    static int  lengthSeqLong    (const Variant::SeqLong&);
    static int  lengthSeqDouble  (const Variant::SeqDouble&);
    static int  lengthSeqString  (const Variant::SeqString& , int tagSize=0);
    static int  lengthSeqMessage (const Variant::SeqMessage&, int tagSize=0);
    static int  calculateLength (Message const&);

  // encode() private functions
  private:
    template <typename CHAR_T, typename SIZE_T>
    static void encode          (    const Message&, const CHAR_T buffer[], SIZE_T);
    static void encode          (    const Message&            , gp::io::CodedOutputStream&);
    static void encodeMessage   (Id, const Message&            , gp::io::CodedOutputStream&);
    static void encodeString    (Id, const std::string&        , gp::io::CodedOutputStream&);
    static void encodeSeqBool   (Id, const Variant::SeqBool&   , gp::io::CodedOutputStream&);
    static void encodeSeqLong   (Id, const Variant::SeqLong&   , gp::io::CodedOutputStream&);
    static void encodeSeqDouble (Id, const Variant::SeqDouble& , gp::io::CodedOutputStream&);
    static void encodeSeqString (Id, const Variant::SeqString& , gp::io::CodedOutputStream&);
    static void encodeSeqMessage(Id, const Variant::SeqMessage&, gp::io::CodedOutputStream&);

  // decode() private functions
  private:
    static Id           id       (InputStream&, const std::string& header);
    static std::string  header   (InputStream&, gp::uint32, Id);

    static void decode (Message&, InputStream&); //NullTerminatedBuffer
    static int  decode (Message&, InputStream&, bool concatenated);
    static void decode (Message&, InputStream&, gp::uint32 tag);
    static void decode (Message&, InputStream&, int);
//#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
//  static void decode (Message&, InputStream&, int  count);
//#else
//  static void decode (Message&, InputStream&, int  bytes);
//#endif

    template <gpi::WireFormatLite::FieldType fieldType, typename VectorType, typename ValueType>
    static void decodeSeqAny    (Message&, Id, int, InputStream&);
    static void decodeSeqBool   (Message&, Id, int, InputStream&);
    static void decodeSeqLong   (Message&, Id, int, InputStream&);
    static void decodeSeqDouble (Message&, Id, int, InputStream&, Type);

    static void decodeSeqBool   (Message&, Id, gp::uint32, InputStream&);
    static void decodeSeqLong   (Message&, Id, gp::uint32, InputStream&);
    static void decodeSeqDouble (Message&, Id, gp::uint32, InputStream&, Type);
    static void decodeSeqStr    (Message&, Id, gp::uint32, InputStream&);
    static void decodeSeqMsg    (Message&, Id, gp::uint32, InputStream&);

    static void decodeBool      (Message&, Id, gp::uint32, InputStream&);
    static void decodeLong      (Message&, Id, gp::uint32, InputStream&);
    static void decodeDouble    (Message&, Id, gp::uint32, InputStream&, Type);
    static void decodeStr       (Message&, Id, gp::uint32, InputStream&);
    static void decodeMsg       (Message&, Id, gp::uint32, InputStream&);
    static void decodeMsg       (Message&, Id, gp::uint32, InputStream&, Message&);
  };


  //----------------------------------------
  inline int Codec::length (Message const& msg)
  {
    if (msg.bytes_ < 0)
    {
      msg.bytes_ = calculateLength (msg);
    }

    assert( msg.bytes_ >= 0 );
  //assert(!msg.bytes_ == !msg.size());   //Message may contain empty sequences

    return msg.bytes_;
  }

  //--------------------------------------------------------------------------
  /*
  template <typename CHAR, typename SIZE_T>
  inline void Codec::encode (Message const& msg, const CHAR buffer[], SIZE_T, int rest)
  {
  }
  */

  //--------------------------------------------------------------------------
  template <typename CHAR>
  inline void Codec::bufferToPtr (const Buffer& buffer, const CHAR*& ptr, int& size)
  {
    const char* cstr = buffer.data();
    ptr  = reinterpret_cast<const CHAR*>(cstr);
    size = buffer.size();
  }

  //--------------------------------------------------------------------------
  class Serializer;
  inline int Codec::decode (Message& msg, InputStream& input, bool concatenated)
  {
    decode (msg, input);

    int rest = input.BytesUntilLimit();
    if (rest & !concatenated) // after decoding the message, there is still data but it was not concatenated
    {                         // => there is something wrong
      throw DecodeException (msg.id(), input.BytesUntilLimit());
    }

    return rest;
  }

  //--------------------------------------------------------------------------
  // this is called by public Serializer::decode()
  template <typename CHAR_T, typename SIZE_T>
  inline int Codec::decode (Message& msg, const CHAR_T buf[], SIZE_T size, bool concatenated)
  {
    const gp::uint8* ui8ptr = (const gp::uint8*) buf;
    Codec::InputStream input (ui8ptr, size);
    return decode (msg, input, concatenated);
  }

  //--------------------------------------------------------------------------
  // this is called by public Serializer::decode()
  inline int Codec::decode (Message& msg, const Buffer& buffer, bool concatenated)
  {
    const gp::uint8*  ptr; //const uint8_t*  ptr;
    int               size;
    Codec::bufferToPtr (buffer, ptr, size);
    Codec::InputStream input   (ptr, size);
    return decode (msg, input, concatenated);
  }

  //--------------------------------------------------------------------------
  // this is called by public Serializer::decode()
  inline Id Codec::id (const Buffer& buffer, const std::string& header)
  {
    const gp::uint8*  ptr; //const uint8_t*  ptr;
    int               size;
    Codec::bufferToPtr (buffer, ptr, size);
    Codec::InputStream input   (ptr, size);
    return id (input, header);
  }

} // namespace pbart

#endif  // PBART_CODEC_H_
