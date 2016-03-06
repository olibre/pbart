/*
 *  Copyright (c) 2014-2016 olibre (olibre@Lmap.org)
 *
 *  This file is part of the program Pbart - Protocol Buffers at Runtime.
 *
 *  Pbart is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License (AGPL)
 *  version 3 or (at your option) any later version.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Pbart (maybe in file LICENSE).
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#define COMPILING_CPPUNIT_DICO //oliver

#include "pbart/Serializer.h"
#include "pbart/Message.h"
#include "pbart/Dico.h"

#include <cppunit/extensions/HelperMacros.h>
#include <ostream>

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




class DicoAsMessage : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicoAsMessage            );
    CPPUNIT_TEST(       getDicoAsMesssage        );
    CPPUNIT_TEST_SUITE_END();

    market::StOrder        mo;
    StEmpty                eo;
    forceV3::StOrder       fo;
    client::cash::StOrder  co;
    not_mapped  ::StOrder  no;

    pbart::SharedDico dico;

    pbart::Id m;
    pbart::Id e;
    pbart::Id f;
    pbart::Id c;

    pbart::Id q;
    pbart::Id i;
    pbart::Id p;
    pbart::Id s;
    pbart::Id r;

    pbart::Variant vq;
    pbart::Variant vi;
    pbart::Variant vp;
    pbart::Variant vs;
    pbart::Variant vr;

    static const std::string emptyDicoYaml_, basicDicoYaml_;

public:

    //--------------------------------------------------------------------------------
    void setUp()
    {
    }

    //--------------------------------------------------------------------------------
    void tearDown()
    {
    }

    // -------------------------------------------------------------------------
    void getDicoAsMesssage()
    {
        pbart::Serializer serializer;

        // using an 'empty' Dictionary
        pbart::Message msg = serializer.dicoToMsg();
        std::ostringstream oss;
        oss << msg;
        CPPUNIT_ASSERT_EQUAL(std::string("Dico:"  "\n"  "  id:    1"  "\n"), oss.str());

        // using a 'basic' Dictionary
        serializer.parseDico(
                    "\n"  "<?xml version='1.0' encoding='UTF-8' ?>"
                    "\n"  "<Dico version='1.0.0' xsd='1.0.0'>"
                    "\n"  "<FieldsDef>"
                    "\n"  "  <FieldDef id='1' name='Q' type='long'   deprecated='false' comment='Volume' />"
                    "\n"  "  <FieldDef id='2' name='P' type='double' deprecated='false' comment='Price'  />"
                    "\n"  "</FieldsDef>"
                    "\n"  "<MessagesDef>"
                    "\n"  "  <MessageDef id='3' name='Msg' deprecated='false' comment='basic'>"
                    "\n"  "    <Field name='Q' default='3333' required='true'  repeated='false' />"
                    "\n"  "    <Field name='P' default='3.14' required='false' repeated='false' />"
                    "\n"  "  </MessageDef>"
                    "\n"  "</MessagesDef>"
                    "\n"  "</Dico>");
        msg = serializer.dicoToMsg();
        oss.str(""); //reset content
        oss << msg;
        CPPUNIT_ASSERT_EQUAL(
                    std::string() +
                    "Dico:"   "\n"
                    "  id:    1"   "\n"
                    "  version:"   "\n"
                    "    id:    10"   "\n"
                    "    type:  LONG"   "\n"
                    "    value: 1"   "\n"
                    "  Field:"   "\n"
                    "  - id:"   "\n"
                    "      id:    2"   "\n"
                    "      type:  LONG"   "\n"
                    "      value: 3"   "\n"
                    "    name:"   "\n"
                    "      id:    3"   "\n"
                    "      type:  STRING"   "\n"
                    "      value: \"Msg\""   "\n"
                    "    type:"   "\n"
                    "      id:    4"   "\n"
                    "      type:  LONG"   "\n"
                    "      value: 6"   "\n"
                    "    deprecated:"   "\n"
                    "      id:    5"   "\n"
                    "      type:  BOOL"   "\n"
                    "      value: false"   "\n"
                    "    comment:"   "\n"
                    "      id:    6"   "\n"
                    "      type:  STRING"   "\n"
                    "      value: \"basic\""   "\n"
                    "    Item:"   "\n"
                    "    - id:"   "\n"
                    "        id:    2"   "\n"
                    "        type:  LONG"   "\n"
                    "        value: 1"   "\n"
                    "      type:"   "\n"
                    "        id:    4"   "\n"
                    "        type:  LONG"   "\n"
                    "        value: 2"   "\n"
                    "      required:"   "\n"
                    "        id:    7"   "\n"
                    "        type:  BOOL"   "\n"
                    "        value: true"   "\n"
                    "      defaultD:"   "\n"
                    "        id:    8"   "\n"
                    "        type:  DOUBLE"   "\n"
                    "        value: 3333"   "\n"
                    "      defaultS:"   "\n"
                    "        id:    9"   "\n"
                    "        type:  STRING"   "\n"
                    "    - id:"   "\n"
                    "        id:    2"   "\n"
                    "        type:  LONG"   "\n"
                    "        value: 2"   "\n"
                    "      type:"   "\n"
                    "        id:    4"   "\n"
                    "        type:  LONG"   "\n"
                    "        value: 3"   "\n"
                    "      required:"   "\n"
                    "        id:    7"   "\n"
                    "        type:  BOOL"   "\n"
                    "        value: false"   "\n"
                    "      defaultD:"   "\n"
                    "        id:    8"   "\n"
                    "        type:  DOUBLE"   "\n"
                    "        value: 3.14"   "\n"
                    "      defaultS:"   "\n"
                    "        id:    9"   "\n"
                    "        type:  STRING"   "\n"
                    "  - id:"   "\n"
                    "      id:    2"   "\n"
                    "      type:  LONG"   "\n"
                    "      value: 2"   "\n"
                    "    name:"   "\n"
                    "      id:    3"   "\n"
                    "      type:  STRING"   "\n"
                    "      value: \"P\""   "\n"
                    "    type:"   "\n"
                    "      id:    4"   "\n"
                    "      type:  LONG"   "\n"
                    "      value: 3"   "\n"
                    "    deprecated:"   "\n"
                    "      id:    5"   "\n"
                    "      type:  BOOL"   "\n"
                    "      value: false"   "\n"
                    "    comment:"   "\n"
                    "      id:    6"   "\n"
                    "      type:  STRING"   "\n"
                    "      value: \"Price\""   "\n"
                    "    Item:"   "\n"
                    "  - id:"   "\n"
                    "      id:    2"   "\n"
                    "      type:  LONG"   "\n"
                    "      value: 1"   "\n"
                    "    name:"   "\n"
                    "      id:    3"   "\n"
                    "      type:  STRING"   "\n"
                    "      value: \"Q\""   "\n"
                    "    type:"   "\n"
                    "      id:    4"   "\n"
                    "      type:  LONG"   "\n"
                    "      value: 2"   "\n"
                    "    deprecated:"   "\n"
                    "      id:    5"   "\n"
                    "      type:  BOOL"   "\n"
                    "      value: false"   "\n"
                    "    comment:"   "\n"
                    "      id:    6"   "\n"
                    "      type:  STRING"   "\n"
                    "      value: \"Volume\""   "\n"
                    "    Item:"   "\n"
                    ,
                    oss.str());
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION( DicoAsMessage );
