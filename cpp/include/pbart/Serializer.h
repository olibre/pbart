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

#ifndef PBART_SERIALIZER_H_
#define PBART_SERIALIZER_H_

#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

#include "pbart/Dico.h"
#include "pbart/Message.h"

#include "pbart/Exception.h"    // required by inline code
#include "pbart/Codec.h"        // required by inline code


namespace pbart
{
  class PBART_EXPORT Serializer
  {
    public:
      // Initialize Dico -----------------------------------------------------
      /* decode Buffer => decode binary serialized dico, returns the rest undecoded
      template <typename CHAR_T, typename SIZE_T>
      int  bufferToDico (const CHAR_T  buf[], SIZE_T, bool concatenated=true);
      int  bufferToDico (const Buffer& buff,          bool concatenated=true);*/
      void msgToDico    (const Message&     msg);  // decode Message    => replace current Dico  = inverse of dicoToMsg()
      void loadDico     (const std::string& file); // decode XML file   => replace current Dico
      void parseDico    (const char*        xml);  // decode XML string => replace current Dico

      // Get dictionary ------------------------------------------------------
      const SharedDico& dico() const;      // get current Dico as a 'shared_prt(const Dico*)'

      // Message
      Message  dicoToMsg()   const;        // get current Dico as a Message = inverse of msgToDico()  [Dico of the Message is a standard Dico describing the Dico structure itself]
      Message  message  ()   const;        // get an empty Message (using the current Dico of the Serializer, Serializer may use later another Dico)
      Message  message  (Id) const;        // get an empty Message (difference with above function: this one checks the id validity in debug mode)

      // Decode --------------------------------------------------------------
      template <typename CHAR_T, typename SIZE_T>
      int      decode (const CHAR_T  buf[], SIZE_T, Message&, bool concatenated=true, bool dico=true);
      int      decode (const Buffer& buf,           Message&, bool concatenated=true, bool dico=true);
      Message  decode (const Buffer& buf, Id id=0)   const;  //when id=0 => extract id from buffer header
      Id       id     (const Buffer& buf)            const;

      // Encode --------------------------------------------------------------
      template <typename CHAR_T, typename SIZE_T, typename POS_T>
      void     encode (const Message&, CHAR_T preallocated[], SIZE_T size=0, POS_T pos=0) const;
      void     encode (const Message&, Buffer& buffer, bool concatenate=true)             const;
      Buffer   encode (const Message&)                                                    const;
      size_t   size   (const Message&)                                                    const;

      static void pointers (Dico* ptr, const SharedDico& dic); //not private to because required by Dico Test
    private:
      const std::string& header() const;
      static const SharedDico& dicoOfDico();
      static void addItem  (const Variant::SeqMessage::const_iterator jt, Field& field);
      static void addField (const Variant::SeqMessage::const_iterator it, Dico* ptr);

      // the current dico of the Serializer ----------------------------------
      SharedDico dico_;

      // static data used by dicoOfDico() ------------------------------------
      static       Id idId_;
      static       Id nameId_;
      static       Id typeId_;
      static       Id deprecatedId_;
      static       Id commentId_;
      static       Id requiredId_;
      static       Id defaultDId_;
      static       Id defaultSId_;
      static       Id versionId_;
      static       Id itemId_;
      static       Id fieldId_;
      static const Id dicoId_ = Id(1);

      friend class Codec;
  };

}  // namespace pbart


namespace std
{
  template<> inline
  void swap (pbart::Dico& d1, pbart::Dico& d2)
  {
    d1.swap(d2);
  }
}


// INLINE


namespace pbart
{
  //--------------------------------------------------------------------------
  inline const SharedDico& Serializer::dico() const
  {
    return dico_;
  }

  //--------------------------------------------------------------------------
  // produce an empty message having id=0
  inline Message Serializer::message() const
  {
    return Message (dico());
  }

  //--------------------------------------------------------------------------
  // produce an empty message
  // in debug mode: check validity of the 'id'
  inline Message Serializer::message (Id id) const
  {
    assert (id > ID_MIN || id==0);
    assert (id < ID_MAX);
    return Message (dico(), id);
  }

  //--------------------------------------------------------------------------
  template <typename CHAR_T, typename SIZE_T>
  int Serializer::decode (const CHAR_T ptr[], SIZE_T size, Message& msg, bool concatenated, bool dico)
  {
    //assert( (concatenated || !dico) && "Cannot handle dico and following messages without concatenated messages => concatenated=0 && dico=1 is not acceptable");
    if (!concatenated && dico)
    {
      dico = false;
    }

    if (msg.dico())
    {
      msg.clear(); //empty message
    }
    else
    {
      msg = message(msg.id());
    }

    if (!size)
    {
      return 0;
    }

    int rest = Codec::decode (msg, ptr, size, concatenated);

    // if auto-dico disabled or message is not a dico => stop here
    if (!dico || msg.id()!=dicoId_ || msg.dico()!=dicoOfDico())
    {
      return rest;
    }

    // else apply the detected dico
    msgToDico (msg);
    msg = message(); //reset dico
    if (rest)
    {
      // decode next message
      // recursive call insteaof Codec::decode() because successive Dico can happen
      return decode (ptr+size-rest, rest, msg, true, true); //concatenated=true dico=true
    }
    else
    {
      return 0; //rest=0
    }
  }

  //--------------------------------------------------------------------------
  inline Message Serializer::decode (const Buffer& buffer, Id requestedId) const
  {
    Message msg = requestedId ? message(requestedId) : message();

    if (! buffer.empty())
    {
      const unsigned char* ptr; //const gp::uint8*
      int size;
      Codec::bufferToPtr (buffer, ptr, size);
      Codec::decode (msg, ptr, size, false);
    }

    return msg;
  }

  //--------------------------------------------------------------------------
  inline int Serializer::decode (const Buffer& buffer, Message& msg, bool concatenated, bool dico)
  {
    const char*  ptr; //const gp::uint8*
    int          size;
    Codec::bufferToPtr (buffer, ptr, size);
    return decode (ptr, size, msg, concatenated, dico);
  }

  //--------------------------------------------------------------------------
  inline const std::string& Serializer::header() const
  {
    if (dico())
    {
      return dico()->header();
    }

    static const std::string nodico("NoDico");
    return nodico;
  }


  //--------------------------------------------------------------------------
  inline Buffer Serializer::encode (const Message& msg) const
  {
    Buffer buffer;
    if (msg.id())
    {
      Codec::encode (msg, buffer);
    }
    return buffer;
  }

  //--------------------------------------------------------------------------
  inline void Serializer::encode (const Message& msg, Buffer& buffer, bool concatenate) const
  {
    // if buffer not empty but no concatenation => first clear the buffer
    if (!concatenate) // && !buffer.empty())
    {
      buffer.clear();
    }

    if (msg.id())
    {
      Codec::encode (msg, buffer);
    }
  }

  //--------------------------------------------------------------------------
  template <typename CHAR_T, typename SIZE_T, typename POS_T>
  inline void Serializer::encode (const Message& msg, CHAR_T preallocated[], SIZE_T size, POS_T pos) const
  {
    if (msg.id())
    {
      Codec::encode (msg, preallocated, size);
    }
  }

  //--------------------------------------------------------------------------
  inline Id Serializer::id (const Buffer& buffer) const
  {
    return Codec::id (buffer, header());
  }


} // namespace pbart


#endif  // PBART_SERIALIZER_H_
