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




class Repeated : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( Repeated );
  CPPUNIT_TEST( EncodeField    );
  CPPUNIT_TEST( DecodeField    );
  CPPUNIT_TEST( EncodeMsg      );
  CPPUNIT_TEST( DecodeMsg      );
  CPPUNIT_TEST( DecodeTwice    );
  CPPUNIT_TEST( CheckMessage   );
  CPPUNIT_TEST_SUITE_END();

  market::StOrder        mo;
  StEmpty                eo;
  forceV3::StOrder       fo;
  client::cash::StOrder  co;
  not_mapped  ::StOrder  no;

  pbart::Serializer*    serializer;
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
    noDefaultCtor (Repeated* aggregator) :
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
  void setUp()
  {
    serializer = new pbart::Serializer();
    extern std::string global_directory; // defined in main.cpp
    serializer->loadDico( global_directory + "dico-Repeated.xml" );

    dico = serializer->dico();
    q = dico->id("Qty");
    p = dico->id("Prc");
    i = dico->id("TZinfo");
    r = dico->id("Reason");
    s = dico->id("isSent");

    //SEQ_LONG
    std::vector<pbart::Variant::long_t> vecLong;
    vecLong.push_back (12);
    vecLong.push_back (12L);
    vecLong.push_back (12);
    vecLong.push_back (1024);
    vecLong.push_back (-1024);
    vecLong.push_back (0);
    vq = vecLong;

    //SEQ_DOUBLE
    vp = pbart::Variant::SeqDouble(); // no element !

    //SEQ_STRING
    vi = pbart::Variant::SeqString();
    vi.toSeqString().push_back("GMT");
    std::string str = vi.toSeqString()[0];

    vr = vi;
    vr.toSeqString().push_back(std::string("because"));

    //SEQ_BOOL
    vs = pbart::Variant::SeqBool();
    vs.toSeqBool().push_back(0);
    vs.toSeqBool().push_back(1);
    vs.toSeqBool().push_back(0);
    vs.toSeqBool().push_back(1);
    vs.toSeqBool().push_back(0);
    vs.toSeqBool().push_back(1);
    vs.toSeqBool().push_back(0);
    vs.toSeqBool().push_back(1);
    vs.toSeqBool().push_back(0);
    vs.toSeqBool().push_back(1);
    vs.toSeqBool().push_back(0);
    vs.toSeqBool().push_back(1);
    vs.toSeqBool().push_back(0);

    noDC = new noDefaultCtor (this);

    noDC->coMsg.set (q, vq);
    noDC->coMsg.set (p, vp);

    noDC->moMsg.set (s, vs);
    CPPUNIT_ASSERT_THROW (noDC->moMsg.set (noDC->coMsg.id(), noDC->coMsg), pbart::TypeException);
    pbart::Variant::SeqMessage seqm;
    seqm.push_back  (noDC->coMsg);
    noDC->moMsg.set (noDC->coMsg.id(), seqm);

    noDC->foMsg.set (r, vi);
    noDC->foMsg.set (r, vr);
  }

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

  void checkEncode (const pbart::Message& moMsg)
  {
    pbart::Id               seq_id    =  dico->id("client.cash.StOrder");
    bool                  seq_empty =  moMsg.empty (seq_id);
    CPPUNIT_ASSERT_EQUAL( seq_empty , false );

    pbart::Type             seq_type  =  moMsg.type (seq_id);
    CPPUNIT_ASSERT      ( seq_type        == pbart::Type::SEQ_MESSAGE );

    const pbart::Variant::SeqMessage& seq      =  moMsg.get(seq_id).toSeqMessage();
    size_t                          seq_size =  seq.size();
    CPPUNIT_ASSERT_EQUAL(           seq_size , size_t(10) );

    const pbart::Message&   msg       =  seq.at(0);
    pbart::Id               prc_id    =  dico->id  ("Prc");
    bool                  prc_empty =  msg.empty (prc_id);
    CPPUNIT_ASSERT_EQUAL( prc_empty , false );

    pbart::Type             prc_type  =  msg.type (prc_id);
    CPPUNIT_ASSERT      ( prc_type.type() == pbart::Type::SEQ_DOUBLE );

    const pbart::Variant::SeqDouble& prc      =  msg.get(prc_id).toSeqDouble();
    size_t                         prc_size =  prc.size();
    CPPUNIT_ASSERT_EQUAL(          prc_size , size_t(5) );

    double                p =  prc.at(0);
    CPPUNIT_ASSERT_EQUAL( p , double(0) );

    // Last of the last
    const pbart::Message& last_msg =  seq.at(9);
    const pbart::Variant::SeqDouble&  last_prc =  last_msg.get(prc_id).toSeqDouble();
    p   =  last_prc.at(4);
    CPPUNIT_ASSERT_EQUAL( p , 3.1415 );
  }

  void EncodeMsg()
  {
    pbart::Variant::SeqString vecString;
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");
    vecString.push_back("only one single attribute");

    noDC->foMsg.set (dico->id("Reason"), vecString);
    fo.bidule = serializer->encode (noDC->foMsg);
    fo.bidule = serializer->encode (noDC->foMsg);  //encode twice

    pbart::Variant::SeqDouble vecDouble;
    vecDouble.push_back (0);
    vecDouble.push_back (1);
    vecDouble.push_back (1.1);          // 5 prices
    vecDouble.push_back (1.1F);
    vecDouble.push_back (3.1415);
    noDC->coMsg.set (dico->id("Prc"), vecDouble);

    pbart::Variant::SeqLong vecLong;
    vecLong.push_back(120);
    vecLong.push_back(250);
    vecLong.push_back(1000);
    vecLong.push_back(100000);
    vecLong.push_back(10000000);
    vecLong.push_back(100000000);
    vecLong.push_back(1000000000);
    noDC->coMsg.set (dico->id("Qty"), vecLong);

    pbart::Variant::SeqBool vecBool;
    vecBool.push_back(true);
    vecBool.push_back(false);
    noDC->moMsg.set (dico->id("isSent"), vecBool);

    pbart::Variant::SeqMessage vecMessage;
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);   // 10 messages
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    vecMessage.push_back(noDC->coMsg);
    noDC->moMsg.set (dico->id("client.cash.StOrder"), vecMessage);

    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    vecString.push_back(std::string());
    noDC->moMsg.set (dico->id("Reason"), vecString);

    checkEncode (noDC->moMsg);

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
    checkEncode (noDC->moMsg);

    pbart::Message moMsg = serializer->decode(mo.dynMsg);
    checkEncode (moMsg);
    pbart::Message foMsg = serializer->decode(fo.bidule);
    checkEncode (moMsg);
    pbart::Message coMsg = serializer->decode(co.dynMsg);
    checkEncode (moMsg);

    pbart::Message moMsg_bis = serializer->decode(mo.dynMsg,(pbart::Id)94);
    checkEncode (moMsg);
    checkEncode (moMsg_bis);
    pbart::Message foMsg_bis = serializer->decode(fo.bidule,(pbart::Id)92);
    checkEncode (moMsg);
    checkEncode (moMsg_bis);
    pbart::Message coMsg_bis = serializer->decode(co.dynMsg,(pbart::Id)91);
    checkEncode (moMsg);
    checkEncode (moMsg_bis);

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


    CPPUNIT_ASSERT_EQUAL( moMsg.empty  (dico->id("client.cash.StOrder")) , false );
    CPPUNIT_ASSERT      ( moMsg.type   (dico->id("client.cash.StOrder")).type() == pbart::Type::SEQ_MESSAGE );
    CPPUNIT_ASSERT_EQUAL( moMsg.get    (dico->id("client.cash.StOrder")).toSeqMessage().size() , size_t(10) );
    CPPUNIT_ASSERT_EQUAL( moMsg.get    (dico->id("client.cash.StOrder")).toSeqMessage().at(0).empty  (dico->id("Prc")) , false );
    CPPUNIT_ASSERT      ( moMsg.get    (dico->id("client.cash.StOrder")).toSeqMessage().at(0).type(dico->id("Prc")) == pbart::Type::SEQ_DOUBLE );
    CPPUNIT_ASSERT_EQUAL( moMsg.get    (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get    (dico->id("Prc")).toSeqDouble().size() , size_t(5) );
    CPPUNIT_ASSERT_EQUAL( moMsg.get    (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get    (dico->id("Prc")).toSeqDouble().at(0)  , 0.0000 );
    CPPUNIT_ASSERT_EQUAL( moMsg.get    (dico->id("client.cash.StOrder")).toSeqMessage().at(9).get    (dico->id("Prc")).toSeqDouble().at(4)  , 3.1415 );

    CPPUNIT_ASSERT_EQUAL( noDC->foMsg.get(dico->id("Reason")).toSeqString().at(0) , std::string("only one single attribute") );
    CPPUNIT_ASSERT_EQUAL( noDC->coMsg.get(dico->id("Prc"))   .toSeqDouble().at(2) , 1.1 );
    CPPUNIT_ASSERT_EQUAL( noDC->coMsg.get(dico->id("Qty"))   .toSeqLong()  .at(6) , pbart::Variant::long_t(1000000000) );
    CPPUNIT_ASSERT      ( noDC->moMsg.get(dico->id("isSent")).toSeqBool()[1] ==  false );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage().size() , size_t(10) );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage()[0].get(dico->id("Prc")).toSeqDouble().at(4) , 3.1415 );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage()[0].get(dico->id("Qty")).toSeqLong()  .at(0) , pbart::Variant::long_t(120) );
    CPPUNIT_ASSERT_EQUAL( noDC->moMsg.get(dico->id("Reason")).toSeqString().at(0) , std::string() + "only one single attribute" );

    CPPUNIT_ASSERT_EQUAL(       foMsg.get(dico->id("Reason")).toSeqString().at(0) , std::string("only one single attribute") );
    CPPUNIT_ASSERT_EQUAL(       coMsg.get(dico->id("Prc")).toSeqDouble().at(0)    , 0.0 );
    CPPUNIT_ASSERT_EQUAL(       coMsg.get(dico->id("Qty")).toSeqLong().at(0)      , pbart::Variant::long_t(120) );
    CPPUNIT_ASSERT      (       moMsg.get(dico->id("isSent")).toSeqBool()[0]     == true );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage().size() , size_t(10) );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage()[0].get(dico->id("Prc")).toSeqDouble().at(0) , 0.0 );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage()[0].get(dico->id("Qty")).toSeqLong()  .at(0) , pbart::Variant::long_t(120) );
    CPPUNIT_ASSERT_EQUAL(       moMsg.get(dico->id("Reason")).toSeqString().at(0) , std::string() + "only one single attribute" );
  }


  void DecodeTwice()
  {
    // common buffer
    pbart::Buffer buffer;
    pbart::Message moMsg_DecodeTwice = serializer->message(94);

    // set many items + encode
    {
      pbart::Message coMsg_FirstEncode = serializer->message(91);
      pbart::Variant::SeqLong seqL;
      seqL.push_back(1);  seqL.push_back(2);  seqL.push_back(3);
      seqL.push_back(4);  seqL.push_back(5);  seqL.push_back(6);
      seqL.push_back(7);  seqL.push_back(8);  seqL.push_back(9);
      coMsg_FirstEncode.set ( dico->id("Qty"),    seqL);

      pbart::Message moMsg_FirstEncode = serializer->message(94);
      moMsg_FirstEncode.set ( dico->id("client.cash.StOrder"), pbart::Variant::SeqMessage());
      pbart::Variant::SeqMessage& seqM = moMsg_FirstEncode.get( dico->id("client.cash.StOrder") ).toSeqMessage();
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);
      seqM.push_back (coMsg_FirstEncode);

      moMsg_FirstEncode.set ( dico->id("isSent"), pbart::Variant::SeqBool());
      pbart::Variant::SeqBool& seqB = moMsg_FirstEncode.get( dico->id("isSent") ).toSeqBool();
      seqB.push_back(true);  seqB.push_back(false);
      seqB.push_back(true);  seqB.push_back(false);

      pbart::Variant::SeqString seqS;
      seqS.push_back("1"); seqS.push_back("2"); seqS.push_back("3");
      seqS.push_back("4"); seqS.push_back("5"); seqS.push_back("6");
      seqS.push_back("7"); seqS.push_back("8"); seqS.push_back("9");
      moMsg_FirstEncode.set ( dico->id("Reason"), seqS);

      buffer = serializer->encode (moMsg_FirstEncode);
    }

    // decode and check (First)
    moMsg_DecodeTwice = serializer->decode (buffer);
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).empty(dico->id("Prc")) , true );

    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).empty(dico->id("Qty")) , false);
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong().size() , size_t(9) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[0] , pbart::Variant::long_t(1) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[1] , pbart::Variant::long_t(2) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[2] , pbart::Variant::long_t(3) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[3] , pbart::Variant::long_t(4) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[4] , pbart::Variant::long_t(5) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[5] , pbart::Variant::long_t(6) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[6] , pbart::Variant::long_t(7) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[7] , pbart::Variant::long_t(8) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong()[8] , pbart::Variant::long_t(9) );

    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("isSent")) , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool().size() , size_t(4) );
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[0] == true );
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[1] == false);
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[2] == true );
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[3] == false);

    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("Reason")) , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().size() , size_t(9) );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(0) , std::string("1") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(1) , std::string("2") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(2) , std::string("3") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(3) , std::string("4") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(4) , std::string("5") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(5) , std::string("6") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(6) , std::string("7") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(7) , std::string("8") );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("Reason")).toSeqString().at(8) , std::string("9") );

    // set the required items only + encode
    {
      pbart::Message moMsg_SecondEncode = serializer->message(94);
      pbart::Message coMsg_SecondEncode = serializer->message(91);
      coMsg_SecondEncode.set ( dico->id("Qty"),    pbart::Variant::SeqLong());
      moMsg_SecondEncode.set ( dico->id("isSent"), pbart::Variant::SeqBool());
      pbart::Variant::SeqBool& seqB = moMsg_SecondEncode.get( dico->id("isSent") ).toSeqBool();
      seqB.push_back(false);  seqB.push_back(true);  seqB.push_back(false);
      pbart::Variant::SeqMessage seq;
      seq.push_back (coMsg_SecondEncode);
      moMsg_SecondEncode.set ( dico->id("client.cash.StOrder"), seq);
      buffer = serializer->encode (moMsg_SecondEncode);
    }

    // decode and check (Second)
    moMsg_DecodeTwice = serializer->decode (buffer);
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).empty(dico->id("Prc"))                    , true  );
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).empty(dico->id("Qty"))                    , false );
#else
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).empty(dico->id("Qty"))                    , true  );
#endif
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("client.cash.StOrder")).toSeqMessage().at(0).get  (dico->id("Qty")).toSeqLong().size() , size_t(0) );

    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("isSent"))                    , false );
    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool().size() , size_t(3) );
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[0]    == false );
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[1]    == true  );
    CPPUNIT_ASSERT      (moMsg_DecodeTwice.get  (dico->id("isSent")).toSeqBool()[2]    == false );

    CPPUNIT_ASSERT_EQUAL(moMsg_DecodeTwice.empty(dico->id("Reason")) , true );
  }

  // -------------------------------------------------------------------------
  void CheckMessage()
  {
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->moMsg.check() );  //return 'zero' means OK
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->eoMsg.check() );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->foMsg.check() );
    CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  noDC->coMsg.check() );

    CPPUNIT_ASSERT_EQUAL( pbart::Id(-1), noDC->coMsg.check(0) );


    std::cout <<"\n" "Before removal of required item:"<< noDC->moMsg <<'\n';

                    noDC->moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage()[0].erase(dico->id("Qty"));
    CPPUNIT_ASSERT( noDC->moMsg.get(dico->id("client.cash.StOrder")).toSeqMessage()[0].empty(dico->id("Qty"))  );

    std::cout <<"\n" "After removal of required Qty table:"<< noDC->moMsg <<'\n';

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
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Repeated );
