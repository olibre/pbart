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
    int    qty;
    double prc;
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




class Variant : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( Variant   );
  CPPUNIT_TEST( Bool            );
  CPPUNIT_TEST( Long            );
  CPPUNIT_TEST( Double          );
  CPPUNIT_TEST( String          );
  CPPUNIT_TEST( Empty           );
  CPPUNIT_TEST( SetFieldValues  );
  CPPUNIT_TEST( GetFieldValues  );
  CPPUNIT_TEST( ParseFields_Msg );
  CPPUNIT_TEST( Miscellaneous   );
  CPPUNIT_TEST( Print           );
  CPPUNIT_TEST(  OutOfRange     );
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

//pbart::Message moMsg;
//pbart::Message eoMsg;
//pbart::Message foMsg;
//pbart::Message coMsg;

  struct noDefaultCtor
  {
    noDefaultCtor (Variant* aggregator) :
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
  //--------------------------------------------------------------------------
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

    //noDC->moMsg = serializer->decode(mo);
    //noDC->eoMsg = serializer->decode(eo);
    //noDC->foMsg = serializer->decode(fo,fo.bidule);
    //noDC->coMsg = serializer->decode(co);
    noDC = new noDefaultCtor (this);
  }

   //--------------------------------------------------------------------------
  void Bool()
  {
    pbart::Variant v;
    v = false; CPPUNIT_ASSERT_EQUAL( v.toBool() , false );
    v = true;  CPPUNIT_ASSERT_EQUAL( v.toBool() , true );
    CPPUNIT_ASSERT_THROW(            v.toString(), ::pbart::TypeException );
  }

  //--------------------------------------------------------------------------
  void Long()
  {
    pbart::Variant v;
    v = 202;  CPPUNIT_ASSERT_EQUAL( (int) v.toLong() , 202 ); //compilation error
    v = 203L; CPPUNIT_ASSERT_EQUAL( (int) v.toLong() , 203 );
    CPPUNIT_ASSERT_THROW(                 v.toString(), ::pbart::TypeException );
  }

  //--------------------------------------------------------------------------
  void Double()
  {
    pbart::Variant v = 2.3;
    CPPUNIT_ASSERT_EQUAL( v.toDouble() , 2.3 );
    CPPUNIT_ASSERT_THROW( v.toString(), ::pbart::TypeException );
  }

  //--------------------------------------------------------------------------
  void String()
  {
    pbart::Variant s = std::string("string");
    pbart::Variant v =             "string";
    CPPUNIT_ASSERT_EQUAL(s.toString(),std::string("string"));
    CPPUNIT_ASSERT_EQUAL(v.toString(),std::string("string"));

    pbart::Type    t = v.type();
    bool         b = v.empty();
    CPPUNIT_ASSERT(  v.type() == pbart::Type::STRING );
    CPPUNIT_ASSERT(! v.empty());
    CPPUNIT_ASSERT_THROW( v.toBool()  , ::pbart::TypeException );
    CPPUNIT_ASSERT_THROW( v.toDouble(), ::pbart::TypeException );
  }

  //--------------------------------------------------------------------------
  void Empty()
  {
    pbart::Variant v; //empty
    int  t = v.type(); //pbart::Type
    bool b = v.empty();
    CPPUNIT_ASSERT(       v.type() == pbart::Type::EMPTY     ); //Caution
    CPPUNIT_ASSERT(       v.empty()                        ); //Caution
    CPPUNIT_ASSERT_THROW( v.toBool(), ::pbart::TypeException );
  }

  //--------------------------------------------------------------------------
  void SetFieldValues()
  {
    //  pbart::Message coMsg = serializer->decode(co);

    vq = 203L;       //LONG
    vp = 2.3;        //DOUBLE
    vi = std::string("GMT");      //STRING
    vr = std::string("because");
    vs = true;

    noDC->coMsg.set(q, vq);
    noDC->coMsg.set(p, vp);

    CPPUNIT_ASSERT_THROW(
    noDC->moMsg.set(p, vp), pbart::IdException );
    noDC->moMsg.set(s, vs);

    noDC->foMsg.set(r, vi);
    noDC->foMsg.set(r, vr);

    // NO EXEPTION => ok
  }

  //--------------------------------------------------------------------------
  void GetFieldValues()
  {
    SetFieldValues();

    int         iq = noDC->coMsg.get(q).toLong   ();
    double      dp = noDC->coMsg.get(p).toDouble ();
    std::string sr = noDC->foMsg.get(r).toString ();

    CPPUNIT_ASSERT_EQUAL( iq , 203       );
    CPPUNIT_ASSERT_EQUAL( dp , 2.3       );
    CPPUNIT_ASSERT_EQUAL( sr , std::string("because") );

    const pbart::Message const_moMsg = noDC->moMsg;
    const pbart::Message const_eoMsg = noDC->eoMsg;
    const pbart::Message const_foMsg = noDC->foMsg;
    const pbart::Message const_coMsg = noDC->coMsg;

    iq = noDC->coMsg.get(q).toLong   ();
    dp = noDC->coMsg.get(p).toDouble ();
    sr = noDC->foMsg.get(r).toString ();

    CPPUNIT_ASSERT_EQUAL( iq , 203       );
    CPPUNIT_ASSERT_EQUAL( dp , 2.3       );
    CPPUNIT_ASSERT_EQUAL( sr , std::string("because") );

    pbart::Id isSent = dico->id("isSent");

    CPPUNIT_ASSERT_EQUAL (noDC->moMsg.get(isSent).toBool(),   true);
    CPPUNIT_ASSERT_EQUAL (const_moMsg.get(isSent).toBool(),   true);
    CPPUNIT_ASSERT_EQUAL (noDC->moMsg.get(r).toString(), std::string("zero"));
    CPPUNIT_ASSERT_EQUAL (const_moMsg.get(r).toString(), std::string("zero"));

    CPPUNIT_ASSERT_THROW (noDC->moMsg.get(i), pbart::IdException);
    CPPUNIT_ASSERT_THROW (const_moMsg.get(i), pbart::IdException);

    CPPUNIT_ASSERT_THROW (noDC->foMsg.get(q), pbart::IdException);
    CPPUNIT_ASSERT_THROW (noDC->foMsg.get(p), pbart::IdException);
    CPPUNIT_ASSERT_THROW (noDC->foMsg.get(i), pbart::IdException);

    CPPUNIT_ASSERT_THROW (const_foMsg.get(q), pbart::IdException);
    CPPUNIT_ASSERT_THROW (const_foMsg.get(p), pbart::IdException);
    CPPUNIT_ASSERT_THROW (const_foMsg.get(i), pbart::IdException);
  }

  //--------------------------------------------------------------------------
  void ParseFields_Msg()
  {
    GetFieldValues();

    std::string str = "super";
    noDC->moMsg.set(r, str);
    int count = 0;
    for (pbart::Message::iterator it = noDC->moMsg.begin(); it != noDC->moMsg.end(); ++it)
    {
      count++;
      if (it.id() == s || it.id() == r)
        CPPUNIT_ASSERT( ! it.empty() );
      else
        CPPUNIT_ASSERT(   it.empty() );
    }
    CPPUNIT_ASSERT_EQUAL( count , 3 );

    count = 0;
    for (pbart::Message::iterator it = noDC->foMsg.begin(); it != noDC->foMsg.end(); ++it)
    {
      count++;
      CPPUNIT_ASSERT( ! it.empty() );
    }
    CPPUNIT_ASSERT_EQUAL( count , 1 );
  }

  //--------------------------------------------------------------------------
  void Miscellaneous()
  {
    pbart::Variant v1;
    pbart::Variant v2 = true;
  //v1.set(v2);
    v1 = v2;
    v1.reset();
  }

  //--------------------------------------------------------------------------
  void Print()
  {
    pbart::Variant vBool = true;
    std::ostringstream oss;
    oss << vBool;
    CPPUNIT_ASSERT_EQUAL( std::string("true"), oss.str() );
  }

  void OutOfRange()
  {
    pbart::Variant seq = pbart::Variant::SeqString();
//  CPPUNIT_ASSERT_THROW( seq.toString(0,std::nothrow), pbart::OutOfRangeException );
    CPPUNIT_ASSERT_THROW( seq.toString(0             ), pbart::OutOfRangeException );
    seq.toSeqString().push_back(std::string());

    CPPUNIT_ASSERT_EQUAL( std::string(), seq.toString(0,std::nothrow) );
    CPPUNIT_ASSERT_EQUAL( std::string(), seq.toString(0             ) );

    std::string expected = "so good";
    seq.toString(0             ) = expected + expected;
    seq.toString(0,std::nothrow) = expected;

    CPPUNIT_ASSERT_EQUAL( expected     , seq.toString(0,std::nothrow) );
    CPPUNIT_ASSERT_EQUAL( expected     , seq.toString(0             ) );
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Variant );
