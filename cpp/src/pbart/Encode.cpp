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

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif

#include <string>

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#include "google/protobuf/wire_format_lite_inl.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h" //StringOutputStream
//#include "google/protobuf/stubs/common.h"  // required for internal serialization

#include "pbart/Codec.h"



namespace pbart
{
  //----------------------------------------
  // if buffer is not empty => concatenate
  void Codec::encode (const Message& message, Buffer& buffer)
  {
    assert(message.dico());

    if (! buffer.empty()) //there is already a message in the buffer
    {
      buffer += '\0'; //null character => flag to indicate the stop of the previous message
    }

    gp::io::StringOutputStream str (&buffer);
    gp::io::CodedOutputStream output (&str);

    // write Msg id + header (version + checksum)
    gpi::WireFormatLite::WriteBytes (message.id(), message.dico()->header(), &output);

    // write the rest of the message
    Codec::encode (message, output);
  }

  //--------------------------------------------------------------------------
  inline void Codec::encodeString (Id id, const std::string& str, gp::io::CodedOutputStream& output)
  {
#ifdef USE_STRICT_BOOST_UTF8_STRING_FORMAT
    gpi::WireFormatLite::WriteString (id, str, &output); //UTF-8 specific
#else
    gpi::WireFormatLite::WriteBytes  (id, str, &output); //on v2.4.1, WriteString() and WriteBytes() do the same
#endif
  }

  //--------------------------------------------------------------------------
  inline void Codec::encodeMessage (Id id, const Message& m, gp::io::CodedOutputStream& output)
  {
#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
    gpi::WireFormatLite::WriteInt32 (id, m.size(), &output);
#else
    gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);
    int bytes = Codec::length(m);
    output.WriteVarint32 (bytes);
#endif
    encode (m, output);
  }

  //--------------------------------------------------------------------------
  inline int Codec::lengthSeqBool (const Variant::SeqBool& v)
  {
#ifdef USE_BOOST_DYNAMIC_BITSET
    if (4 <= v.size() && v.size() <= 32)
      return 1 + 4; // 1 byte (store the number of relevant bits) + 4 bytes (store 32 bits)
#endif
    return v.size();
  }

  //--------------------------------------------------------------------------
  inline void Codec::encodeSeqBool (Id id, const Variant::SeqBool& v, gp::io::CodedOutputStream& output)
  {

#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    if (v.size() == 0)  //do not encode empty sequence
    {
      return;
    }
#endif

    gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);

#ifdef USE_BOOST_DYNAMIC_BITSET
    if (4 <= v.size() && v.size() <= 32)
    {
      output.WriteVarint32 (1 + 4);              // 1 byte + 4 bytes
      output.WriteVarint32 (v.size());           // -> 1 byte  => store the number of relevant bits
      output.WriteLittleEndian32 (v.to_ulong()); // -> 4 bytes => store 32 bits
    }
    else
#endif
    {
      // encode using the [packed=true] format
      output.WriteVarint32  (v.size());
      for (size_t i = 0; i < v.size(); ++i)
        gpi::WireFormatLite::WriteBoolNoTag (v[i], &output); // -> output.WriteVarint32 (bool ? 1 : 0)
    }
  }

  //--------------------------------------------------------------------------
  inline int Codec::lengthSeqLong (const Variant::SeqLong& v)
  {
    int length = 0; //buffer size required to store the whole array
    for (size_t i = 0; i < v.size(); ++i)
      length += gpi::WireFormatLite::Int64Size(v[i]);
    return length;
  }

  //--------------------------------------------------------------------------
  // encode using the [packed=true] format
  inline void Codec::encodeSeqLong (Id id, const Variant::SeqLong& v, gp::io::CodedOutputStream& output)
  {
    int length = lengthSeqLong (v); //buffer size required to store the whole array

#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    if (length == 0)  //do not encode empty sequence
    {
      return;
    }
#endif

    gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);
    output.WriteVarint32 (length);

    for (size_t i = 0; i < v.size(); ++i)
      gpi::WireFormatLite::WriteInt64NoTag (v[i], &output); // -> output.WriteVarint64SignExtended()
  }

  //--------------------------------------------------------------------------
  inline int Codec::lengthSeqDouble (const Variant::SeqDouble& v)
  {
    return 8 * v.size(); //buffer size required to store the whole array
  }

  //--------------------------------------------------------------------------
  // encode using the [packed=true] format
  inline void Codec::encodeSeqDouble (Id id, const Variant::SeqDouble& v, gp::io::CodedOutputStream& output)
  {
    int length = lengthSeqDouble (v); //buffer size required to store the whole array

#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    if (length == 0)  //do not encode empty sequence
    {
      return;
    }
#endif

    gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);
    output.WriteVarint32 (length);

    for (size_t i = 0; i < v.size(); ++i)
      gpi::WireFormatLite::WriteDoubleNoTag (v[i], &output); // -> output.WriteLittleEndian64()
  }

  //--------------------------------------------------------------------------
  inline int Codec::lengthSeqString (const Variant::SeqString& v, int tagSize)
  {
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    int length = 0; //buffer size required to store the whole array
#else
    int length = tagSize * v.size();
#endif
    for (size_t i = 0; i < v.size(); ++i)
      length += gpi::WireFormatLite::BytesSize (v[i]); //in v2.4.1 -> same body as StringSize()
    return length;
  }

  //--------------------------------------------------------------------------
  inline void Codec::encodeSeqString (Id id, const Variant::SeqString& v, gp::io::CodedOutputStream& output)
  {
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);

    int length = lengthSeqString (v); //buffer size required to store the whole array
    output.WriteVarint32 (length);

    for (size_t i = 0; i < v.size(); ++i)
    {
      output.WriteVarint32(v[i].size());
      output.WriteString  (v[i]);  //in v2.4.1 -> calls WriteRaw()
    }
#else
    for (size_t i = 0; i < v.size(); ++i)  //do not encode empty sequence
    {
      gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);
      output.WriteVarint32(v[i].size());
      output.WriteString  (v[i]);  //in v2.4.1 -> calls WriteRaw()
    }
#endif
  }

  //--------------------------------------------------------------------------
  inline int Codec::lengthSeqMessage (const Variant::SeqMessage& v, int tagSize)
  {
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    int length = 0; //buffer size required to store the whole array
#else
    int length = tagSize * v.size();
#endif
    for (size_t i = 0; i < v.size(); ++i)
    {
#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
      length += gpi::WireFormatLite::Int32Size (v[i].size()); //number of fields within the message
      length += Codec::length (v[i]);
#else
      int bytes = Codec::length(v[i]);
      length += gpi::WireFormatLite::Int32Size (bytes) + bytes;
#endif
    }
    return length;
  }

  //--------------------------------------------------------------------------
  inline void Codec::encodeSeqMessage (Id id, const Variant::SeqMessage& v, gp::io::CodedOutputStream& output)
  {
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);

    int bytes = lengthSeqMessage (v); //buffer size required to store the whole array
    output.WriteVarint32 (bytes);

    for (size_t i = 0; i < v.size(); ++i)
    {
      const Message& m = v[i];
#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
      output.WriteVarint32 (m.size()); //number of fields within the message
#else
      int bytes = Codec::length(m);
      output.WriteVarint32 (bytes);    //number of bytes of the serialized message
#endif
      encode (m, output);
    }
#else
    for (size_t i = 0; i < v.size(); ++i)  //do not encode empty sequence
    {
      const Message& m = v[i];
      gpi::WireFormatLite::WriteTag (id, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, &output);
#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
      output.WriteVarint32 (m.size()); //number of fields within the message
#else
      int bytes = Codec::length(m);
      output.WriteVarint32 (bytes);    //number of bytes of the serialized message
#endif
      encode (m, output);
    }
#endif
  }

  //--------------------------------------------------------------------------
  // calculate number of bytes required to serialize the message
  int Codec::calculateLength (Message const& msg)
  {
    int total = 0;

    for (ValuesById::const_iterator it  = msg.values_.begin();
                                    it != msg.values_.end();
                                  ++it)
    {
      assert (it->second.type() == msg.type(it->first));
      switch (it->second.type())
      {
        case Type::BOOL:
          total += gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_BOOL);
          total += 1;
          break;

        case Type::LONG:
          total += gpi::WireFormatLite::TagSize   (it->first, gpi::WireFormatLite::TYPE_INT32);
          total += gpi::WireFormatLite::Int64Size (it->second.toLong(std::nothrow));
          break;

        case Type::DOUBLE:
          total += gpi::WireFormatLite::TagSize   (it->first, gpi::WireFormatLite::TYPE_DOUBLE);
          total += 8;
          break;

        case Type::STRING:
          total += gpi::WireFormatLite::TagSize   (it->first, gpi::WireFormatLite::TYPE_STRING);
          total += gpi::WireFormatLite::StringSize(it->second.toString(std::nothrow));
          break;

        case Type::MESSAGE:
#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
          {
            total += gpi::WireFormatLite::TagSize   (it->first, gpi::WireFormatLite::TYPE_INT32);
            total += gpi::WireFormatLite::Int32Size (it->second.toMessage(std::nothrow).size());
            total += size (it->second.toMessage(std::nothrow));
          }
#else
          {
            total += gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
            int length = Codec::length (it->second.toMessage(std::nothrow));
            total += gpi::WireFormatLite::Int32Size (length) + length;
          }
#endif
          break;

        case Type::SEQ_BOOL:
          {
            int length = lengthSeqBool (it->second.toSeqBool(std::nothrow));
#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
            if (length)  //do not encode empty sequence
#endif
            {
              total += gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
              total     += gpi::WireFormatLite::Int32Size (length) + length;
            }
          }
          break;

        case Type::SEQ_LONG:
          {
            int length = lengthSeqLong (it->second.toSeqLong(std::nothrow));
#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
            if (length)  //do not encode empty sequence
#endif
            {
              total += gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
              total += gpi::WireFormatLite::Int32Size (length) + length;
            }
          }
          break;

        case Type::SEQ_DOUBLE:
          {
            int length = lengthSeqDouble (it->second.toSeqDouble(std::nothrow));
#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
            if (length)  //do not encode empty sequence
#endif
            {
              total += gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
              total += gpi::WireFormatLite::Int32Size (length) + length;
            }
          }
          break;

        case Type::SEQ_STRING:
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
          {
            total += gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
            int length = lengthSeqString (it->second.toSeqString(std::nothrow));
            total += gpi::WireFormatLite::Int32Size (length) + length;
          }
#else
          {
            int tagSize = gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
            int length = lengthSeqString (it->second.toSeqString(std::nothrow), tagSize);
            total += length;
          }
#endif
          break;

        case Type::SEQ_MESSAGE:
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
          {
            total += gpi::WireFormatLite::TagSize   (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
            int length = lengthSeqMessage (it->second.toSeqMessage(std::nothrow));
            total += gpi::WireFormatLite::Int32Size (length) + length;
          }
#else
          {
            int tagSize = gpi::WireFormatLite::TagSize (it->first, gpi::WireFormatLite::TYPE_MESSAGE);
            int length = lengthSeqMessage (it->second.toSeqMessage(std::nothrow), tagSize);
            total += length;
          }
#endif

          break;

        default:
          assert(0&&"Unexpected type from it->second.type()");
        case Type::EMPTY:
          break; //ignore

      } //switch case
    }   //for

    //assert(!total == !msg.size());  //Message may contain empty sequences
    return total;
  }

  //--------------------------------------------------------------------------
  void Codec::encode (Message const& msg, gp::io::CodedOutputStream& output)
  {
    for (Message::const_iterator it  = msg.cbegin();
                                 it != msg.cend();
                               ++it)
    {
      if (it.empty())
      {
        continue;
      }

      const Variant& v = *it;

      // If types are not consistant => throw Exception
      // type of Variant must be same as type read from Dico
      // internal Variant type may be changed if user do `msg.get = Variant(Type::SEQ_MESSAGE);`
      if (v.type() != it.type())
      {
        throw MsgTypeException (msg, it.id());
      }

      switch (v.type())
      {
        case Type::BOOL:
          gpi::WireFormatLite::WriteBool   (it.id(), v.toBool(std::nothrow), &output);
          break;

        case Type::LONG:
          gpi::WireFormatLite::WriteInt64  (it.id(), v.toLong(std::nothrow), &output);
          break;

        case Type::DOUBLE:
          gpi::WireFormatLite::WriteDouble (it.id(), v.toDouble(std::nothrow), &output);
          break;

        case Type::STRING:
          encodeString (it.id(), v.toString(std::nothrow), output);
          break;

        case Type::MESSAGE:
          encodeMessage (it.id(), v.toMessage(std::nothrow), output);
          break;

        case Type::SEQ_BOOL:
          encodeSeqBool (it.id(), v.toSeqBool(std::nothrow), output);
          break;

        case Type::SEQ_LONG:
          encodeSeqLong (it.id(), v.toSeqLong(std::nothrow), output);
          break;

        case Type::SEQ_DOUBLE:
          encodeSeqDouble (it.id(), v.toSeqDouble(std::nothrow), output);
          break;

        case Type::SEQ_STRING:
          encodeSeqString (it.id(), v.toSeqString(std::nothrow), output);
          break;

        default:
          assert(0&&"Unexpected type from variant.type()");
          //this CANNOT occur: for optimization continue with next case

        case Type::SEQ_MESSAGE:
          encodeSeqMessage (it.id(), v.toSeqMessage(std::nothrow), output);
          break;

      } //switch case
    }   //for
  }


} // namespace pbart
