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

#define COMPILING_CPPUNIT_CODEC

#include "pbart/Serializer.h"
#include "pbart/Message.h"
#include "pbart/Dico.h"

#include <cppunit/extensions/HelperMacros.h>

namespace market
{
  struct StOrder
  {
    pbart::Buffer dynMsg;
  };
}

struct StEmpty
{
  pbart::Buffer dynMsg;
};

namespace client
{
  namespace cash
  {
    struct StOrder
    {
      pbart::Buffer dynMsg;
    };
  }
}

namespace forceV3
{
  struct StOrder
  {
    pbart::Buffer bidule;
  };
}

namespace not_mapped
{
  struct StOrder
  {
    pbart::Buffer dynMsg;
  };
}




class Codec : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( Codec    );
  CPPUNIT_TEST( EncodeField    );
  CPPUNIT_TEST( DecodeField    );
  CPPUNIT_TEST( EncodeMsg      );
  CPPUNIT_TEST( DecodeMsg      );
  CPPUNIT_TEST( DecodeTwice    );
  CPPUNIT_TEST( DecodeButBadId );
  CPPUNIT_TEST( CheckMessage   );
  CPPUNIT_TEST( ConcatenateMessages );
  CPPUNIT_TEST( DicoAndMessages     );
  CPPUNIT_TEST( AutoDecodedDico     );
  CPPUNIT_TEST_SUITE_END();

  market::StOrder        mo;
  StEmpty                eo;
  forceV3::StOrder       fo;
  client::cash::StOrder  co;
  not_mapped  ::StOrder  no;

  pbart::Serializer* serializer;
  pbart::SharedDico dico;

  pbart::Id m;
  pbart::Id e;
  pbart::Id f;
  pbart::Id c;

  pbart::Id q;
  pbart::Id p;
  pbart::Id i;
  pbart::Id r;
  pbart::Id s;

  pbart::Variant vq;
  pbart::Variant vp;
  pbart::Variant vi;
  pbart::Variant vr;
  pbart::Variant vs;

  struct noDefaultCtor
  {
    noDefaultCtor (Codec* aggregator) :
      moMsg (aggregator->serializer->message(94)),//aggregator->mo.dynMsg)),
      eoMsg (aggregator->serializer->message(93)),//aggregator->eo.dynMsg)),
      foMsg (aggregator->serializer->message(92)),//aggregator->fo.bidule)),
      coMsg (aggregator->serializer->message(91)) //aggregator->co.dynMsg))
      {}

    pbart::Message moMsg;
    pbart::Message eoMsg;
    pbart::Message foMsg;
    pbart::Message coMsg;
  };
  noDefaultCtor* noDC;

public:

  //--------------------------------------------------------------------------------
  void setUp()
  {
    serializer = new pbart::Serializer();
    extern std::string global_directory; // defined in main.cpp
    serializer->loadDico( global_directory + "dico.xml" );

    dico = serializer->dico();
    q = dico->id("Qty");
    p = dico->id("Prc");
    i = dico->id("TZinfo");
    r = dico->id("Reason");
    s = dico->id("isSent");

    vq = 203L;       //LONG
    vp = 2.3;        //DOUBLE
    vi = std::string("GMT");      //STRING
    vr = std::string("because");
    vs = true;

    noDC = new noDefaultCtor (this);

    noDC->coMsg.set (q, vq);
    noDC->coMsg.set (p, vp);

    noDC->moMsg.set (s, vs);
    noDC->moMsg.set (noDC->coMsg.id(), noDC->coMsg);

    noDC->foMsg.set (r, vi);
    noDC->foMsg.set (r, vr);

    pbart::Variant vv = noDC->foMsg.get (r);
    const std::string& sr = vr.toString();
    const std::string& sv = vv.toString();
    CPPUNIT_ASSERT_EQUAL( sr , sv );
  }

  //--------------------------------------------------------------------------------
  void tearDown()
  {
    delete serializer;
  }

  //--------------------------------------------------------------------------------
  void EncodeField()
  {
    mo.dynMsg = serializer->encode (noDC->moMsg);
    eo.dynMsg = serializer->encode (noDC->eoMsg);
    fo.bidule = serializer->encode (noDC->foMsg);
    co.dynMsg = serializer->encode (noDC->coMsg);

    size_t size_m = mo.dynMsg.size();
    size_t size_e = eo.dynMsg.size();
    size_t size_f = fo.bidule.size();
    size_t size_c = co.dynMsg.size();

    CPPUNIT_ASSERT( size_m );
    CPPUNIT_ASSERT( size_e );
    CPPUNIT_ASSERT( size_f );
    CPPUNIT_ASSERT( size_c );
  }

  //--------------------------------------------------------------------------------
  void DecodeField()
  {
    EncodeField();

    pbart::Message moMsg = serializer->decode(mo.dynMsg);
    pbart::Message eoMsg = serializer->decode(eo.dynMsg);
    pbart::Message foMsg = serializer->decode(fo.bidule);
    pbart::Message coMsg = serializer->decode(co.dynMsg);

    pbart::Message moMsg_bis = serializer->decode(mo.dynMsg,(pbart::Id)94);
    pbart::Message eoMsg_bis = serializer->decode(eo.dynMsg,(pbart::Id)93);
    pbart::Message foMsg_bis = serializer->decode(fo.bidule,(pbart::Id)92);
    pbart::Message coMsg_bis = serializer->decode(co.dynMsg,(pbart::Id)91);

    CPPUNIT_ASSERT_EQUAL( moMsg , moMsg_bis );
    CPPUNIT_ASSERT_EQUAL( eoMsg , eoMsg_bis );
    CPPUNIT_ASSERT_EQUAL( foMsg , foMsg_bis );
    CPPUNIT_ASSERT_EQUAL( coMsg , coMsg_bis );


    moMsg.clear();
    moMsg = serializer->decode (mo.dynMsg);

    eoMsg.clear();
    eoMsg = serializer->decode (eo.dynMsg);

    foMsg.clear();
    foMsg = serializer->decode (fo.bidule);

    coMsg.clear();
    coMsg = serializer->decode (co.dynMsg);


    moMsg_bis.clear();
    moMsg_bis = serializer->decode (mo.dynMsg);

    eoMsg_bis.clear();
    eoMsg_bis = serializer->decode (eo.dynMsg);

    foMsg_bis.clear();
    foMsg_bis = serializer->decode (fo.bidule);

    coMsg_bis.clear();
    coMsg_bis = serializer->decode (co.dynMsg);

    CPPUNIT_ASSERT_EQUAL( moMsg , moMsg_bis );
    CPPUNIT_ASSERT_EQUAL( eoMsg , eoMsg_bis );
    CPPUNIT_ASSERT_EQUAL( foMsg , foMsg_bis );
    CPPUNIT_ASSERT_EQUAL( coMsg , coMsg_bis );
  }

  void EncodeMsg()
  {
    noDC->foMsg.set ( dico->id("Reason"), std::string("only one single attribute"));
    fo.bidule = serializer->encode (noDC->foMsg);
    fo.bidule = serializer->encode (noDC->foMsg);  //encode twice

    noDC->coMsg.set ( dico->id("Prc"), 46.03);
    noDC->coMsg.set ( dico->id("Qty"), 500);

    noDC->moMsg.set ( dico->id("isSent"), true);
    noDC->moMsg.set ( dico->id("client.cash.StOrder"), noDC->coMsg);
    noDC->moMsg.set ( dico->id("Reason"), "buy shares");

    co.dynMsg = serializer->encode (noDC->coMsg);
    mo.dynMsg = serializer->encode (noDC->moMsg);

    size_t size_f = fo.bidule.size();
    size_t size_c = co.dynMsg.size();
    size_t size_m = mo.dynMsg.size();

    CPPUNIT_ASSERT( size_f );
    CPPUNIT_ASSERT( size_c );
    CPPUNIT_ASSERT( size_m );
  }

  void DecodeMsg()
  {
    EncodeMsg();

    pbart::Message moMsg = serializer->decode(mo.dynMsg);
    pbart::Message foMsg = serializer->decode(fo.bidule);
    pbart::Message coMsg = serializer->decode(co.dynMsg);

    pbart::Message moMsg_bis = serializer->decode(mo.dynMsg,(pbart::Id)94);
    pbart::Message foMsg_bis = serializer->decode(fo.bidule,(pbart::Id)92);
    pbart::Message coMsg_bis = serializer->decode(co.dynMsg,(pbart::Id)91);

    CPPUNIT_ASSERT_EQUAL( moMsg , moMsg_bis );
    CPPUNIT_ASSERT_EQUAL( foMsg , foMsg_bis );
    CPPUNIT_ASSERT_EQUAL( coMsg , coMsg_bis );


    moMsg.clear();
    moMsg = serializer->decode (mo.dynMsg);

    foMsg.clear();
    foMsg = serializer->decode (fo.bidule);

    coMsg.clear();
    coMsg = serializer->decode (co.dynMsg);

    moMsg_bis.clear();
    moMsg_bis = serializer->decode (mo.dynMsg);

    foMsg_bis.clear();
    foMsg_bis = serializer->decode (fo.bidule);

    coMsg_bis.clear();
    coMsg_bis = serializer->decode (co.dynMsg);

    CPPUNIT_ASSERT_EQUAL( moMsg , moMsg_bis );
    CPPUNIT_ASSERT_EQUAL( foMsg , foMsg_bis );
    CPPUNIT_ASSERT_EQUAL( coMsg , coMsg_bis );


    CPPUNIT_ASSERT_EQUAL( noDC->foMsg.get(dico->id("Reason")).toString() , std::string("only one single attribute") );
    CPPUNIT_ASSERT_EQUAL( noDC->coMsg.get(dico->id("Prc")).toDouble()    , 46.03 );
    CPPUNIT_ASSERT_EQUAL( noDC->coMsg.get(dico->id("Qty")).toLong()      , pbart::Variant::long_t(500) );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("isSent")).toBool()   , true  );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("client.cash.StOrder")).toMessage().get(dico->id("Prc")).toDouble() , 46.03 );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("client.cash.StOrder")).toMessage().get(dico->id("Qty")).toLong()   , pbart::Variant::long_t(500) );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("Reason")).toString() , std::string() + "buy shares" );

    CPPUNIT_ASSERT_EQUAL(       foMsg.get(dico->id("Reason")).toString() , std::string("only one single attribute") );
    CPPUNIT_ASSERT_EQUAL(       coMsg.get(dico->id("Prc")).toDouble()    , 46.03 );
    CPPUNIT_ASSERT_EQUAL(       coMsg.get(dico->id("Qty")).toLong()      , pbart::Variant::long_t(500) );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("isSent")).toBool()   , true  );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("client.cash.StOrder")).toMessage().get(dico->id("Prc")).toDouble() , 46.03 );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("client.cash.StOrder")).toMessage().get(dico->id("Qty")).toLong()   , pbart::Variant::long_t(500) );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("Reason")).toString() , std::string() + "buy shares" );
  }


  void DecodeTwice()
  {
    // common buffer
    pbart::Buffer buffer;
    pbart::Message moMsg_DecodeTwice = serializer->message(94);

    // set many items + encode
    {
      pbart::Message moMsg_FirstEncode = serializer->message(94);
      pbart::Message coMsg_FirstEncode = serializer->message(91);
      coMsg_FirstEncode.set ( dico->id("Qty"),    111);
      moMsg_FirstEncode.set ( dico->id("client.cash.StOrder"), coMsg_FirstEncode);
      moMsg_FirstEncode.set ( dico->id("isSent"), true);
      moMsg_FirstEncode.set ( dico->id("Reason"), "many");
      buffer = serializer->encode (moMsg_FirstEncode);
    }

    // decode and check (First)
    moMsg_DecodeTwice = serializer->decode (buffer);
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().empty(dico->id("Prc"))          , true );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().empty(dico->id("Qty"))          , false);
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().get  (dico->id("Qty")).toLong() , pbart::Variant::long_t(111) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("isSent"))            , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("isSent")).toBool()   , true  );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("Reason"))            , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toString() , std::string("many"));

    // set the required items only + encode
    {
      pbart::Message moMsg_SecondEncode = serializer->message(94);
      pbart::Message coMsg_SecondEncode = serializer->message(91);
      coMsg_SecondEncode.set ( dico->id("Qty"),    22);
      moMsg_SecondEncode.set ( dico->id("isSent"), false);
      moMsg_SecondEncode.set ( dico->id("client.cash.StOrder"), coMsg_SecondEncode);
      buffer = serializer->encode (moMsg_SecondEncode);
    }

    // decode and check (Second)
    moMsg_DecodeTwice = serializer->decode (buffer);
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().empty(dico->id("Prc"))          , true );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().empty(dico->id("Qty"))          , false);
//  CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().get  (dico->id("Qty"))          , pbart::Variant        (22) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toMessage().get  (dico->id("Qty")).toLong() , pbart::Variant::long_t(22) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("isSent"))            , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("isSent")).toBool()   , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("Reason"))            , true  );
  }

  // -------------------------------------------------------------------------
  void DecodeButBadId()
  {
    EncodeField();
    CPPUNIT_ASSERT_THROW( serializer->decode(mo.dynMsg,13), pbart::DecodeException );
  }

  // -------------------------------------------------------------------------
  void CheckMessage()
  {
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->moMsg.check() );  //return 'zero' means OK
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->eoMsg.check() );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->foMsg.check() );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->coMsg.check() );

    CPPUNIT_ASSERT_EQUAL( pbart::Id(-1), noDC->coMsg.check(0) );

                    noDC->moMsg.get(dico->id("client.cash.StOrder")).toMessage().erase(dico->id("Qty"));
    CPPUNIT_ASSERT( noDC->moMsg.get(dico->id("client.cash.StOrder")).toMessage().empty(dico->id("Qty")) );
    CPPUNIT_ASSERT_EQUAL( dico->id("client.cash.StOrder"),  noDC->moMsg.check()                                );
    CPPUNIT_ASSERT_EQUAL( dico->id("Qty"),                  noDC->moMsg.check(dico->id("client.cash.StOrder")) );


    for (pbart::Message::iterator it = noDC->moMsg.begin(); it != noDC->moMsg.end(); ++it)
    {
      if (it.id() == dico->id("client.cash.StOrder"))
      {
        CPPUNIT_ASSERT_EQUAL( dico->id("Qty"), it.check() );
      }
      else
      {
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), it.check() );
      }
    }

    noDC->moMsg.get(dico->id("client.cash.StOrder")) = true;
    CPPUNIT_ASSERT_EQUAL( dico->id("client.cash.StOrder"),  noDC->moMsg.check()                                );
    CPPUNIT_ASSERT_EQUAL( dico->id("client.cash.StOrder"),  noDC->moMsg.check(dico->id("client.cash.StOrder")) );

    for (pbart::Message::iterator it = noDC->moMsg.begin(); it != noDC->moMsg.end(); ++it)
    {
      if (it.id() == dico->id("client.cash.StOrder"))
      {
        CPPUNIT_ASSERT_EQUAL( it.id(), it.check() );
      }
      else
      {
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), it.check() );
      }
    }


    // empty messages
    pbart::Message moMsg = serializer->message(94);
    pbart::Message eoMsg = serializer->message(93);
    pbart::Message foMsg = serializer->message(92);
    pbart::Message coMsg = serializer->message(91);

    CPPUNIT_ASSERT_EQUAL( dico->id("isSent"), moMsg.check() );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),         eoMsg.check() ); // Dico does not define any field for this message
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),         foMsg.check() ); // The single field is optional
    CPPUNIT_ASSERT_EQUAL( dico->id("Qty"),    coMsg.check() );

    CPPUNIT_ASSERT_EQUAL( dico->id("isSent"),              moMsg.check(dico->id("isSent"))              );
    CPPUNIT_ASSERT_EQUAL( dico->id("client.cash.StOrder"), moMsg.check(dico->id("client.cash.StOrder")) );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                      moMsg.check(dico->id("Reason"))              );

    CPPUNIT_ASSERT_EQUAL( dico->id("isSent"),              eoMsg.check(dico->id("isSent"))              );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                      foMsg.check(dico->id("Reason"))              );
  }

  // -------------------------------------------------------------------------
  void ConcatenateMessages()
  {
    pbart::Message sub  = serializer->message(91);
    sub.set ( dico->id("Qty"),    111);

    pbart::Buffer buffer;
    serializer->encode (sub, buffer);

    pbart::Message main = serializer->message(94);
    main.set( dico->id("client.cash.StOrder"), sub);
    main.set( dico->id("isSent"), true);
    main.set( dico->id("Reason"), "many");

    //buffer += '\0'; //null character
    serializer->encode (main, buffer); //concatenate

    pbart::Message msg;
    int rest = serializer->decode (buffer, msg, true, false); //concatenate
    CPPUNIT_ASSERT_EQUAL( sub, msg );

    //msg = pbart::Message();
    msg.id(0); //reset
    const char* ptr = buffer.c_str() + buffer.length() - rest;
    int rest2 = serializer->decode (ptr, rest, msg, false); //end => no concatenate
    CPPUNIT_ASSERT_EQUAL( 0, rest2 );
    CPPUNIT_ASSERT_EQUAL( main, msg );
  }


  // -------------------------------------------------------------------------
  void DicoAndMessages()
  {
    pbart::Buffer buffer;

    // encode DicoOfDico + two messages

    pbart::Message dod = serializer->dicoToMsg();
    serializer->encode (dod, buffer);

    pbart::Message sub  = serializer->message(91);
    sub.set ( dico->id("Qty"),    111);
    serializer->encode (sub, buffer);

    pbart::Message main = serializer->message(94);
    main.set( dico->id("client.cash.StOrder"), sub);
    main.set( dico->id("isSent"), true);
    main.set( dico->id("Reason"), "many");
    serializer->encode (main, buffer); //concatenate

    // using another Serializer, decode these DicoOfDico + messages

    pbart::Serializer newcomer;
    pbart::Message m1, m2, m3;
    int r1 = newcomer.decode (buffer, m1, true, false);
    CPPUNIT_ASSERT( r1 > 0 );
    CPPUNIT_ASSERT_EQUAL( dod, m1 );

    newcomer.msgToDico (m1);

    const char* ptr = buffer.c_str() + buffer.length() - r1;
    int r2 = newcomer.decode (ptr, r1, m2, true, false); //concatenate
    CPPUNIT_ASSERT( 0 < r2 );
    CPPUNIT_ASSERT(     r2 < r1 );
    CPPUNIT_ASSERT_EQUAL( sub, m2 );

    ptr += r1 - r2;
    int r3 = newcomer.decode (ptr, r2, m3, false); //end => no concatenate
    CPPUNIT_ASSERT_EQUAL( 0, r3 );
    CPPUNIT_ASSERT_EQUAL( main, m3 );
  }

  // -------------------------------------------------------------------------
  void AutoDecodedDico()
  {
    pbart::Buffer buffer;

    // encode DicoOfDico + two messages

    pbart::Message dod = serializer->dicoToMsg();
    serializer->encode (dod, buffer);

    pbart::Message sub  = serializer->message(91);
    sub.set( dico->id("Qty"), 111 );
    serializer->encode (sub, buffer);

    pbart::Message main = serializer->message(94);
    main.set( dico->id("client.cash.StOrder"), sub);
    main.set( dico->id("isSent"),  true  );
    main.set( dico->id("Reason"), "many" );
    serializer->encode (main, buffer); //concatenate

    // using another Serializer, decode these DicoOfDico + messages

    pbart::Serializer newcomer;
    pbart::Message m2, m3;
    int r2 = newcomer.decode (buffer, m2, true, true);
    CPPUNIT_ASSERT( 0 < r2 );
    CPPUNIT_ASSERT_EQUAL( sub, m2 );

    const char* ptr = buffer.c_str() + buffer.length() - r2;
    int r3 = newcomer.decode (ptr, r2, m3, false); //end => no concatenate
    CPPUNIT_ASSERT_EQUAL( 0, r3 );
    CPPUNIT_ASSERT_EQUAL( main, m3 );
  }





}; //class Codec

CPPUNIT_TEST_SUITE_REGISTRATION( Codec );
