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

#define COMPILING_CPPUNIT_DICO //oliver

#include "pbart/Serializer.h"
#include "pbart/Message.h"
#include "pbart/Dico.h"

#include <cppunit/extensions/HelperMacros.h>
#include <ostream>


extern std::string global_directory; // defined in main.cpp

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




class Dico : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( Dico                     );
    // Check Dico basics --------------------------
    CPPUNIT_TEST( Init                           );
    CPPUNIT_TEST( FieldDescription               );
    CPPUNIT_TEST( DefaultValue                   );
    CPPUNIT_TEST( Require                        );
    CPPUNIT_TEST( Require_QtyInMarketStOrder     );
    CPPUNIT_TEST( RequireMsg                     );
    CPPUNIT_TEST( ParseFields                    );
    CPPUNIT_TEST( ParseMsg                       );
    CPPUNIT_TEST( RecursiveCheckItemType_const   );
    CPPUNIT_TEST( RecursiveCheckItemType_mutable );
    // Errors in XML ------------------------------
    CPPUNIT_TEST( BadFilename                    );
    CPPUNIT_TEST( EmptyXmlString                 );
    CPPUNIT_TEST( WrongXmlString                 );
    CPPUNIT_TEST( BadXmlString                   );
    CPPUNIT_TEST( FileButNoDico                  );
    CPPUNIT_TEST( BadFieldId                     );
    CPPUNIT_TEST( BadType                        );
    CPPUNIT_TEST( BadMessageType                 );
    CPPUNIT_TEST( BadMsgId                       );
    CPPUNIT_TEST( BadItemTag                     );
    CPPUNIT_TEST( DuplicatedFieldId              );
    CPPUNIT_TEST( DuplicatedFieldName            );
    CPPUNIT_TEST( DuplicatedMsgId                );
    CPPUNIT_TEST( DuplicatedMsgName              );
    CPPUNIT_TEST( DuplicatedMsgItem              );
    CPPUNIT_TEST( MissingFieldDefinition         );
    CPPUNIT_TEST( MissingMessageDefinition       );
    CPPUNIT_TEST( FieldIdTooLarge                );
    CPPUNIT_TEST( MessageIdTooLarge              );
    CPPUNIT_TEST( StringDefinesNothing           );
    CPPUNIT_TEST( FileDefinesNothing             );
    CPPUNIT_TEST( Miscellaneous                  );
    // YAML ---------------------------------------
    CPPUNIT_TEST( PrintYaml                      );
    // Dico to/from Msg ---------------------------
    CPPUNIT_TEST( SeveralDicos                   );
    CPPUNIT_TEST( NoDico2Msg                     );
    CPPUNIT_TEST( EmptyDico2Msg                  );
    CPPUNIT_TEST( BasicDico2Msg                  );
    CPPUNIT_TEST( ComplexDico2Msg                );
    CPPUNIT_TEST( DicoOfDico2Msg                 );
    CPPUNIT_TEST( FillDicoProperties             );
    // --------------------------------------------
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
    void Init()
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        pbart::Message m = serializer.message(94);
    }

    // -------------------------------------------------------------------------
    void BadFilename()
    {
        pbart::Serializer serializer;
        CPPUNIT_ASSERT_THROW( serializer.loadDico( "path/this_file_does_not_exist.xml" ), pbart::DicoException );
    }

    // -------------------------------------------------------------------------
    void EmptyXmlString()
    {
        pbart::Serializer serializer;
        CPPUNIT_ASSERT_THROW( serializer.parseDico( "" ), pbart::DicoException );
    }

    // -------------------------------------------------------------------------
    void WrongXmlString()
    {
        pbart::Serializer serializer;
        CPPUNIT_ASSERT_THROW( serializer.parseDico( "very>  \00 wrong \01\02\00 string "), pbart::DicoException );
    }

    // -------------------------------------------------------------------------
    void BadXmlString()
    {
        pbart::Serializer serializer;
        CPPUNIT_ASSERT_THROW( serializer.parseDico( "<Dico>Bad XML Data</ZZ>" ), pbart::DicoException );
    }

    // -------------------------------------------------------------------------
    void FileButNoDico()
    {
        pbart::Serializer serializer;
        CPPUNIT_ASSERT_THROW( serializer.loadDico( "../../cpp/resources/this-is-not-a-dico.xsd" ), pbart::DicoException );
    }

    // -------------------------------------------------------------------------
    void BadFieldId()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico (global_directory + "dico-BadFieldId.xml");
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("negative id");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void BadType()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico (global_directory + "dico-BadType.xml");
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("bad type");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void BadMessageType()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-BadMessageType.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("type MESSAGE");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void BadMsgId()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-BadMsgId.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("negative id");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    // Within a MessageDef, a sub-item can be declared using tags 'FIELD' or 'MESSAGE'
    void BadItemTag()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-BadItemTag.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Unexpected parameter");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void DuplicatedFieldId()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-DuplicatedFieldId.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Duplicated id");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void DuplicatedFieldName()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-DuplicatedFieldName.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Duplicated name");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void DuplicatedMsgId()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-DuplicatedMsgId.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Duplicated id");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void DuplicatedMsgName()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-DuplicatedMsgName.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Duplicated name");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void DuplicatedMsgItem()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-DuplicatedMsgItem.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Duplicated item");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void MissingFieldDefinition()
    {
        pbart::Serializer serializer;
        // field declared in a message but missing in the fields definition section
        try
        {
            serializer.loadDico( global_directory + "dico-MissingFieldDefinition.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Missing definition");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void MissingMessageDefinition()
    {
        pbart::Serializer serializer;
        // SubMessage declared in a message but missing in the messages definition section
        try
        {
            serializer.loadDico( global_directory + "dico-MissingMessageDefinition.xml" );
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Missing definition");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void FieldIdTooLarge()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.parseDico(
                        "<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                        "  <FieldsDef>"
                        "      <FieldDef id='1'      name='prc'    type='double' deprecated='false' comment='Price'  />"
                        "      <FieldDef id='2'      name='qty'    type='long'   deprecated='false' comment='Volume' />"
                        "      <FieldDef id='3'      name='side'   type='bool'   deprecated='false' comment='bid/ask'/>"
                        "      <FieldDef id='555000' name='client' type='string' deprecated='false' comment='Name'   />"
                        "  </FieldsDef>"
                        "  <MessagesDef>"
                        "      <MessageDef id='6' name='Order' deprecated='false' comment='basic order'>"
                        "          <Field name='prc' default='0' required='true'  repeated='false' />"
                        "          <Field name='qty' default='1' required='false' repeated='false' />"
                        "      </MessageDef>"
                        "      <MessageDef id='7' name='MassQuote' deprecated='false' comment='sub-Msg'>"
                        "          <Message name='Order'                required='true'  repeated='true'  />"
                        "          <Field   name='client' default='abc' required='false' repeated='false' />"
                        "      </MessageDef>"
                        "  </MessagesDef>"
                        "</Dico>" );

            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("cannot have id=555000 greater than");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void MessageIdTooLarge()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.parseDico(
                        "<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                        "  <FieldsDef>"
                        "      <FieldDef id='1' name='prc'    type='double' deprecated='false' comment='Price'  />"
                        "      <FieldDef id='2' name='qty'    type='long'   deprecated='false' comment='Volume' />"
                        "      <FieldDef id='3' name='side'   type='bool'   deprecated='false' comment='bid/ask'/>"
                        "      <FieldDef id='4' name='client' type='string' deprecated='false' comment='Name'   />"
                        "  </FieldsDef>"
                        "  <MessagesDef>"
                        "      <MessageDef id='6' name='Order' deprecated='false' comment='basic order'>"
                        "          <Field name='prc' default='0' required='true'  repeated='false' />"
                        "          <Field name='qty' default='1' required='false' repeated='false' />"
                        "      </MessageDef>"
                        "      <MessageDef id='77000' name='MassQuote' deprecated='false' comment='sub-Msg'>"
                        "          <Message name='Order'                required='true'  repeated='true'  />"
                        "          <Field   name='client' default='abc' required='false' repeated='false' />"
                        "      </MessageDef>"
                        "  </MessagesDef>"
                        "</Dico>" );

            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("cannot have id=77000 greater than");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void Miscellaneous()
    {
        pbart::Serializer serializer;

        CPPUNIT_ASSERT_NO_THROW( serializer.parseDico(
                                     "<?xml version='1.0' encoding='UTF-8' ?>       <Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                     "<FieldsDef><FieldDef id='1' name='n' type='bool'  deprecated='false' comment=''/></FieldsDef> </Dico>") );

        CPPUNIT_ASSERT_NO_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                      "<FieldsDef><FieldDef id='1' name='n' type='bool'  deprecated='true'  comment=''/></FieldsDef> </Dico>") );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1'          type='bool'  deprecated='true'  comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='u' name='n' type='bool'  deprecated='false' comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1' name=''  type='bool'  deprecated='false' comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1' name='n' type=''      deprecated='false' comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1' name='n' type='empty' deprecated='false' comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1' name='n' type='bool'  deprecated='f'     comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1' name='n' type='bool'                     comment=''/></FieldsDef> </Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_NO_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                      "<FieldsDef><FieldDef id='1' name='n' type='bool'  deprecated='true'            /></FieldsDef> </Dico>") );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef id='1' name='n' type='bool'  deprecated='true'  comment=''/></FieldsDef>        "), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico                                      >"
                                                   "<FieldsDef><FieldDef id='1' name='n' type='bool'  deprecated='true'  comment=''/></FieldsDef> </Dico>"), pbart::DicoException );


        CPPUNIT_ASSERT_NO_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                      "<FieldsDef><FieldDef     id='1' name='f' type='bool'   deprecated='true' comment=''/></FieldsDef>"
                                                      "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                      "<Field                          name='f' default='0' required='true' repeated='false' /></MessageDef></MessagesDef></Dico>") );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='bool'   deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f' default='A' required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );
        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='bool'   deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f'             required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='long'   deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f' default='A' required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );
        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='long'   deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f'             required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='double' deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f' default='A' required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );
        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='double' deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f'             required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );

        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='string' deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f' default='A'                                    /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );
        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='string' deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f'             required='true' repeated='false' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );
        CPPUNIT_ASSERT_THROW( serializer.parseDico("<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                                   "<FieldsDef><FieldDef     id='1' name='f' type='string' deprecated='true' comment=''/></FieldsDef>"
                                                   "<MessagesDef><MessageDef id='6' name='m'               deprecated='true' comment=''>"
                                                   "<Field                          name='f' default='A' required='ABCD' repeated='EFGHI' /></MessageDef></MessagesDef></Dico>"), pbart::DicoException );
    }


    // -------------------------------------------------------------------------
    void StringDefinesNothing()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.parseDico ("<?xml version='1.0' encoding='UTF-8' ?>"
                                  "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                  "  <FieldsDef>   </FieldsDef>"
                                  "  <MessagesDef> </MessagesDef>"
                                  "  <FieldsDef/>"
                                  "  <MessagesDef/>"
                                  "</Dico>");
            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Dictionary is empty");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }

    // -------------------------------------------------------------------------
    void FileDefinesNothing()
    {
        pbart::Serializer serializer;
        try
        {
            serializer.loadDico( global_directory + "dico-ValidButNothingDefined.xml");

            CPPUNIT_ASSERT( 0 && "must throw" );
        }
        catch (pbart::DicoException e)
        {
            std::string w = e.what();
            std::string::size_type p = w.find("Dictionary is empty => Verify file");
            CPPUNIT_ASSERT_MESSAGE( w, p != std::string::npos );
        }
        catch (...)
        {
            CPPUNIT_ASSERT( 0 && "must throw DicoException" );
        }
    }


    // -------------------------------------------------------------------------
    void FieldDescription()
    {
        {
            pbart::Serializer serializer;
            serializer.loadDico (global_directory + "dico.xml");
            dico = serializer.dico();
        }

        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), dico->id("DoNotExist") );
        q =  dico->id("Qty");
        i =  dico->id("TZinfo");
        p =  dico->id("Prc");
        s =  dico->id("isSent");
        r =  dico->id("Reason");

        CPPUNIT_ASSERT_EQUAL( (pbart::Id) 1, q );
        CPPUNIT_ASSERT_EQUAL( (pbart::Id) 2, i );
        CPPUNIT_ASSERT_EQUAL( (pbart::Id) 3, p );
        CPPUNIT_ASSERT_EQUAL( (pbart::Id) 4, s );
        CPPUNIT_ASSERT_EQUAL( (pbart::Id) 5, r );

        CPPUNIT_ASSERT_EQUAL( std::string("Qty"),     dico->field(q)           .name() );
        CPPUNIT_ASSERT_EQUAL( std::string("TZinfo"),  dico->field(i)           .name() );
        CPPUNIT_ASSERT_EQUAL( std::string(),          dico->field(pbart::Id(-22)).name() );
        CPPUNIT_ASSERT_EQUAL( std::string(),          dico->field(pbart::Id(  7)).name() );
        CPPUNIT_ASSERT_EQUAL( pbart::Type(),            dico->field(pbart::Id(-22)).type() );
        CPPUNIT_ASSERT_EQUAL( pbart::Type(),            dico->field(pbart::Id(  7)).type() );

        CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::LONG  ), dico->field(q).type()          );
        CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::STRING), dico->field(i).type()          );
        CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::DOUBLE), dico->field(p).type()          );
        CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::BOOL  ), dico->field(s).type()          );
        CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::EMPTY ), dico->field(pbart::Id(0)).type() );
        //CPPUNIT_ASSERT_THROW( dico->field((pbart::Id) 0), pbart::IdException );

        CPPUNIT_ASSERT_EQUAL( false, dico->field(q).deprecated() );
        CPPUNIT_ASSERT_EQUAL( true,  dico->field(i).deprecated() );
        CPPUNIT_ASSERT_EQUAL( false, dico->field(p).deprecated() );
        CPPUNIT_ASSERT_EQUAL( true,  dico->field((pbart::Id) 65465468).deprecated() );
        //CPPUNIT_ASSERT_THROW( dico->field((pbart::Id) 65465468).deprecated(), pbart::IdException );
    }

    // -------------------------------------------------------------------------
    void DefaultValue()
    {
        pbart::Serializer serializer;

        CPPUNIT_ASSERT_NO_THROW( serializer.parseDico(
                                     "<?xml version='1.0' encoding='UTF-8' ?>"
                                     "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                     "  <FieldsDef>"
                                     "      <FieldDef id='1' name='prc'    type='double' deprecated='false' comment='Price'  />"
                                     "      <FieldDef id='2' name='qty'    type='long'   deprecated='false' comment='Volume' />"
                                     "      <FieldDef id='3' name='side'   type='bool'   deprecated='false' comment='bid/ask'/>"
                                     "      <FieldDef id='4' name='client' type='string' deprecated='false' comment='Name'   />"
                                     "  </FieldsDef>"
                                     "  <MessagesDef>"
                                     "      <MessageDef id='6' name='Order' deprecated='false' comment='basic order'>"
                                     "          <Field name='prc' default='0.1' required='true'  repeated='false' />"
                                     "          <Field name='qty' default='1'   required='false' repeated='false' />"
                                     "      </MessageDef>"
                                     "      <MessageDef id='7' name='MassQuote' deprecated='false' comment='sub-Msg'>"
                                     "          <Message name='Order'                required='true'  repeated='true'  />"
                                     "          <Field   name='client' default='abc' required='false' repeated='false' />"
                                     "      </MessageDef>"
                                     "  </MessagesDef>"
                                     "</Dico>" ) );

        pbart::Message msg = serializer.dicoToMsg();
        pbart::Id i = msg.id("Field");
        std::cout <<"\n###################################\n"
                 << msg[i].type()
                 <<"\n###################################\n";


    }

    // -------------------------------------------------------------------------
    void Require()
    {
        FieldDescription();

        CPPUNIT_ASSERT_EQUAL( dico->field(91).item(p).required(), false );
        CPPUNIT_ASSERT_EQUAL( dico->field(91).item(q).required(), true  );
        CPPUNIT_ASSERT_THROW( dico->field(91).item(i).required(), pbart::IdException);
        CPPUNIT_ASSERT_THROW( dico->field(91).item(r).required(), pbart::IdException);

        CPPUNIT_ASSERT_THROW( dico->field(93).item(p).required(), pbart::IdException);
        CPPUNIT_ASSERT_THROW( dico->field(93).item(r).required(), pbart::IdException);

        CPPUNIT_ASSERT_THROW( dico->field(92).item(q).required(), pbart::IdException);
        CPPUNIT_ASSERT_THROW( dico->field(92).item(p).required(), pbart::IdException);
        CPPUNIT_ASSERT_THROW( dico->field(92).item(i).required(), pbart::IdException);
        CPPUNIT_ASSERT_EQUAL( dico->field(92).item(r).required(), false );

        CPPUNIT_ASSERT_EQUAL( dico->field(94).item(s).required(), true  );
        CPPUNIT_ASSERT_EQUAL( dico->field(94).item(r).required(), false );
        CPPUNIT_ASSERT_THROW( dico->field(94).item(p).required(), pbart::IdException);
    }

    // -------------------------------------------------------------------------
    void Require_QtyInMarketStOrder()
    {
        FieldDescription();

        CPPUNIT_ASSERT_THROW( dico->field(94).item(q).required(), pbart::IdException);
    }

    // -------------------------------------------------------------------------
    void RequireMsg()
    {
        FieldDescription();

        c = dico->id("client.cash.StOrder");
        f = dico->id("forceV3.StOrder");
        e = dico->id("StEmpty");
        m = dico->id("market.StOrder");

        CPPUNIT_ASSERT_EQUAL( c, (pbart::Id) 91 );
        CPPUNIT_ASSERT_EQUAL( f, (pbart::Id) 92 );
        CPPUNIT_ASSERT_EQUAL( e, (pbart::Id) 93 );
        CPPUNIT_ASSERT_EQUAL( m, (pbart::Id) 94 );
    }

    // -------------------------------------------------------------------------
    void ParseFields()
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        dico = serializer.dico();

        pbart::Message m = serializer.message(94); //mo.dynMsg);

        size_t count = 0;
        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            pbart::Id id = it.id();
            CPPUNIT_ASSERT( id != 0 );

            const pbart::Field& fc = dico->field(id);
            CPPUNIT_ASSERT_EQUAL( id , dico->id(fc.name()) );

            if (fc.type() != pbart::Type::MESSAGE)
            {
                count++;

                CPPUNIT_ASSERT_EQUAL( fc.name()       , dico->field(id).name()       );
                CPPUNIT_ASSERT_EQUAL( fc.type()       , dico->field(id).type()       );
                CPPUNIT_ASSERT_EQUAL( fc.deprecated() , dico->field(id).deprecated() );

                CPPUNIT_ASSERT_EQUAL( fc.name()       , it.name()        );
                CPPUNIT_ASSERT_EQUAL( fc.type()       , it.type()        );
                CPPUNIT_ASSERT_EQUAL( fc.deprecated() , it.deprecated()  );

                CPPUNIT_ASSERT_EQUAL( it.required()   , dico->field(m.id()).item(id).required() );
            }

            //  CPPUNIT_ASSERT( it.check() );
            CPPUNIT_ASSERT_EQUAL( it.empty() , true );
        }

        CPPUNIT_ASSERT_EQUAL( count , size_t(2) );
    }

    //--------------------------------------------------------------------------------
    void ParseMsg()
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        dico = serializer.dico();

        pbart::Message m = serializer.message(94); //mo.dynMsg);

        size_t count = 0;
        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            pbart::Id id = it.id();
            CPPUNIT_ASSERT( id != 0 );

            const pbart::Field& md = dico->field(id);
            CPPUNIT_ASSERT_EQUAL( id , dico->id(md.name()) );

            if (md.type() == pbart::Type::MESSAGE)
            {
                count++;

                CPPUNIT_ASSERT      ( it.type()      == pbart::Type::MESSAGE );
                CPPUNIT_ASSERT_EQUAL( it.required()   , dico->field(m.id()).item(id).required() );

                CPPUNIT_ASSERT_EQUAL( md.name()       , dico->field(id).name() );
                CPPUNIT_ASSERT_EQUAL( md.name()       , it.name()          );

                CPPUNIT_ASSERT_EQUAL( md.deprecated() , dico->field(id).deprecated());
                CPPUNIT_ASSERT_EQUAL( md.deprecated() , it.deprecated()  );
            }

            //  CPPUNIT_ASSERT( it.check() );
            CPPUNIT_ASSERT_EQUAL( it.empty() , true );
        }

        CPPUNIT_ASSERT_EQUAL( count , size_t(1) );
    }

    //--------------------------------------------------------------------------------
    void RecursiveCheckItemType_mutable (pbart::Message& m)
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        dico = serializer.dico();

        const pbart::Field& def_from_dico = dico->field( m.id() );

        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            pbart::Id id = it.id();
            CPPUNIT_ASSERT( id != 0 );

            pbart::ItemsById::const_iterator item_from_dico = def_from_dico.items().find (id);
            CPPUNIT_ASSERT( item_from_dico != def_from_dico.items().end());

            pbart::Type  type_from_dico = item_from_dico->second.type();

            if (! it.empty())
            {
                pbart::Type type_from_variant = it->type();
                CPPUNIT_ASSERT_EQUAL( type_from_variant , type_from_dico );
            }

            if (type_from_dico == pbart::Type::MESSAGE)
            {
                pbart::Message m2 = serializer.message(id);
                RecursiveCheckItemType_const (m2);
            }
            else
            {
                CPPUNIT_ASSERT (type_from_dico != pbart::Type::MESSAGE);
                CPPUNIT_ASSERT (type_from_dico != pbart::Type::EMPTY);
            }
        }
    }

    //--------------------------------------------------------------------------------
    void RecursiveCheckItemType_const (const pbart::Message& m)
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        dico = serializer.dico();

        const pbart::Field& def_from_dico = dico->field( m.id() );

        for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            pbart::Id id = it.id();
            CPPUNIT_ASSERT( id != 0 );

            pbart::ItemsById::const_iterator item_from_dico = def_from_dico.items().find (id);
            CPPUNIT_ASSERT( item_from_dico != def_from_dico.items().end());

            pbart::Type  type_from_dico = item_from_dico->second.type();

            if (! it.empty())
            {
                pbart::Type type_from_variant = it->type();
                CPPUNIT_ASSERT_EQUAL( type_from_variant , type_from_dico );
            }

            if (type_from_dico == pbart::Type::MESSAGE)
            {
                pbart::Message m2 = serializer.message(id);
                RecursiveCheckItemType_const (m2);
            }
            else
            {
                CPPUNIT_ASSERT (type_from_dico != pbart::Type::MESSAGE);
                CPPUNIT_ASSERT (type_from_dico != pbart::Type::EMPTY);
            }
        }
    }

    //--------------------------------------------------------------------------------
    void RecursiveCheckItemType_const()
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        dico = serializer.dico();

        {
            const pbart::Message cmo = serializer.message(94); //mo.dynMsg);
            const pbart::Message cfo = serializer.message(92); //mo.dynMsg);
            const pbart::Message ceo = serializer.message(93); //eo.dynMsg);
            const pbart::Message cco = serializer.message(91); //co.dynMsg);

            RecursiveCheckItemType_const (cmo);
            RecursiveCheckItemType_const (cfo);
            RecursiveCheckItemType_const (ceo);
            RecursiveCheckItemType_const (cco);
        }

        {
            pbart::Message mfo = serializer.message(92);
            mfo.set ( dico->id("Reason"), std::string("only one single attribute"));
            const pbart::Message cfo = mfo;
            RecursiveCheckItemType_const (cfo);

            pbart::Message mco = serializer.message(91);
            mco.set ( dico->id("Prc"), 46.03);
            mco.set ( dico->id("Qty"), 500);
            const pbart::Message cco = mco;
            RecursiveCheckItemType_const (cco);

            pbart::Message mmo = serializer.message(94);
            mmo.set ( dico->id("isSent"), true);
            mmo.set ( dico->id("client.cash.StOrder"), mco); //TODO: &mco -> &cco
            mmo.set ( dico->id("Reason"), "buy shares");
            const pbart::Message cmo = mmo;
            RecursiveCheckItemType_const (cmo);
        }
    }

    //--------------------------------------------------------------------------------
    void RecursiveCheckItemType_mutable()
    {
        pbart::Serializer serializer;
        serializer.loadDico (global_directory + "dico.xml");
        dico = serializer.dico();

        pbart::Message mmo = serializer.message(94); //mo.dynMsg);
        pbart::Message mfo = serializer.message(92); //mo.dynMsg);
        pbart::Message meo = serializer.message(93); //eo.dynMsg);
        pbart::Message mco = serializer.message(91); //co.dynMsg);

        RecursiveCheckItemType_mutable (mmo);
        RecursiveCheckItemType_mutable (mfo);
        RecursiveCheckItemType_mutable (meo);
        RecursiveCheckItemType_mutable (mco);

        mco.set ( dico->id("Prc"), 46.03);
        mco.set ( dico->id("Qty"), 500);

        mmo.set ( dico->id("isSent"), true);
        mmo.set ( dico->id("client.cash.StOrder"), mco);
        mmo.set ( dico->id("Reason"), "buy shares");

        mfo.set ( dico->id("Reason"), std::string("only one single attribute"));

        RecursiveCheckItemType_mutable (mmo);
        RecursiveCheckItemType_mutable (mfo);
        RecursiveCheckItemType_mutable (meo);
        RecursiveCheckItemType_mutable (mco);
    }

    //--------------------------------------------------------------------------------
    void PrintYaml()
    {
        pbart::Serializer serializer;

        serializer.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                             "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                             "<FieldsDef>"
                             "  <FieldDef id='1' name='Qty'    type='long'   deprecated='false' comment='Volume'      />"
                             "  <FieldDef id='3' name='Prc'    type='double' deprecated='false' comment='Price'       />"
                             "  <FieldDef id='4' name='isSent' type='bool'   deprecated='false' comment='is on market'/>"
                             "  <FieldDef id='5' name='Reason' type='string' deprecated='false' comment='Reject msg'  />"
                             "  <FieldDef id='2' name='TZinfo' type='string' deprecated='true'  comment='always UTC'  />"
                             "</FieldsDef>"
                             "<MessagesDef>"
                             "  <MessageDef id='91' name='client.cash.StOrder' deprecated='false' comment='basic order'>"
                             "    <Field name='Prc'  default='3.14' required='false' repeated='false' />"
                             "    <Field name='Qty'  default='3333' required='true'  repeated='false'  />"
                             "  </MessageDef>"
                             "  <MessageDef id='93' name='StEmpty' deprecated='false' comment='for test'>"
                             "    <Field   name='isSent'              default='true'   required='true'  repeated='false' />"
                             "    <Message name='client.cash.StOrder' default=''       required='true'  repeated='false' />"
                             "    <Field   name='Reason'              default='blabla' required='false' repeated='false' />"
                             "  </MessageDef>"
                             "</MessagesDef>"
                             "</Dico>");

        pbart::Id m;
        m = serializer.dico()->id("client.cash.StOrder");
        pbart::Id f = serializer.dico()->id("Prc");
        pbart::Message msg = serializer.message(m);
        msg.set(f, 3.1415);
        std::ostringstream oss;
        oss << msg;
        std::cout <<'\n'<< oss.str().c_str() << std::endl;
        std::cout <<'\n'<< msg << std::endl;

        const std::string& yaml = oss.str();
        CPPUNIT_ASSERT_EQUAL( std::string() + "client.cash.StOrder:"  "\n"
                                              "  id:    91"           "\n"
                                              "  Qty:"                "\n"
                                              "    id:    1"          "\n"
                                              "    type:  LONG"       "\n"
                                              "  Prc:"                "\n"
                                              "    id:    3"          "\n"
                                              "    type:  DOUBLE"     "\n"
                                              "    value: 3.1415"     "\n", yaml);
    }


    //--------------------------------------------------------------------------------
    void SeveralDicos()
    {
        pbart::Message m1, m2, m3, m4, m5;

        // Message 1
        {
            pbart::Serializer s;
            s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0'>"
                        "<FieldsDef>"
                        "  <FieldDef id='1' name='Qty' type='long'   deprecated='false' comment='Volume' />"
                        "  <FieldDef id='2' name='Prc' type='double' deprecated='false' comment='Price'  />"
                        "</FieldsDef>"
                        "<MessagesDef>"
                        "  <MessageDef id='3' name='Msg' deprecated='false' comment='basic order'>"
                        "    <Field name='Qty' default='3333' required='true'  repeated='false' />"
                        "    <Field name='Prc' default='3.14' required='false' repeated='false' />"
                        "  </MessageDef>"
                        "</MessagesDef>"
                        "</Dico>");
            m1 = s.message(s.dico()->id("Msg"));
            m1.set (       s.dico()->id("Qty"), 42);
            m1.set (       s.dico()->id("Prc"), 3.1415);
        }
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m1.check() );

        // Message 2
        {
            pbart::Serializer s;
            s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0'>"
                        "<FieldsDef>"
                        "  <FieldDef id='1' name='Q' type='long'   deprecated='false' comment='Volume' />"
                        "  <FieldDef id='2' name='P' type='double' deprecated='false' comment='Price'  />"
                        "</FieldsDef>"
                        "<MessagesDef>"
                        "  <MessageDef id='3' name='M2' deprecated='false' comment='basic order'>"
                        "    <Field name='Q' default='3333' required='true'  repeated='false' />"
                        "    <Field name='P' default='3.14' required='false' repeated='false' />"
                        "  </MessageDef>"
                        "</MessagesDef>"
                        "</Dico>");
            m2 = s.message(s.dico()->id("M2"));
            m2.set (       s.dico()->id("Q"), 42);
            m2.set (       s.dico()->id("P"), 3.1415);
        }
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m2.check() );

        // Message 3
        {
            pbart::Serializer s;
            s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0'>"
                        "<FieldsDef>"
                        "  <FieldDef id='1' name='B' type='bool'   deprecated='false' comment='Volume' />"
                        "  <FieldDef id='2' name='S' type='string' deprecated='false' comment='Price'  />"
                        "</FieldsDef>"
                        "<MessagesDef>"
                        "  <MessageDef id='3' name='M3' deprecated='false' comment='basic order'>"
                        "    <Field name='B' default='true' required='true'  repeated='false' />"
                        "    <Field name='S' default='rien' required='false' repeated='false' />"
                        "  </MessageDef>"
                        "</MessagesDef>"
                        "</Dico>");
            m3 = s.message(s.dico()->id("M3"));
            m3.set (       s.dico()->id("B"), true);
            m3.set (       s.dico()->id("S"), "this is not a string");
        }
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m3.check() );

        // Message 4
        {
            pbart::Serializer s;
            s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0'>"
                        "<FieldsDef>"
                        "  <FieldDef id='1' name='B' type='bool'   deprecated='false' comment='Volume' />"
                        "  <FieldDef id='2' name='S' type='string' deprecated='false' comment='Price'  />"
                        "</FieldsDef>"
                        "<MessagesDef>"
                        "  <MessageDef id='3' name='M4' deprecated='false' comment='basic order'>"
                        "    <Field name='B' required='true'  repeated='true' />"
                        "    <Field name='S' required='false' repeated='true' />"
                        "  </MessageDef>"
                        "</MessagesDef>"
                        "</Dico>");
            m4 = s.message(s.dico()->id("M4"));
            m4.get (       s.dico()->id("B")).toSeqBool()  .push_back(true);
            m4.get (       s.dico()->id("S")).toSeqString().push_back(":)");
        }
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m4.check() );

        // Message 5 = same as Message 1
        {
            pbart::Serializer s;
            s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                        "<Dico version='1.0.0' xsd='1.0.0'>"
                        "<FieldsDef>"
                        "  <FieldDef id='1' name='Qty' type='long'   deprecated='false' comment='Volume' />"
                        "  <FieldDef id='2' name='Prc' type='double' deprecated='false' comment='Price'  />"
                        "</FieldsDef>"
                        "<MessagesDef>"
                        "  <MessageDef id='3' name='Msg' deprecated='false' comment='basic order'>"
                        "    <Field name='Qty' default='3333' required='true'  repeated='false' />"
                        "    <Field name='Prc' default='3.14' required='false' repeated='false' />"
                        "  </MessageDef>"
                        "</MessagesDef>"
                        "</Dico>");
            m5 = s.message(s.dico()->id("Msg"));
            m5.set (       s.dico()->id("Qty"), 42);
            m5.set (       s.dico()->id("Prc"), 3.1415);
        }
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m1.check() );

        CPPUNIT_ASSERT( m1 != m2 );    CPPUNIT_ASSERT(m1.dico() != m2.dico());     CPPUNIT_ASSERT(*m1.dico() != *m2.dico());
        CPPUNIT_ASSERT( m1 != m3 );    CPPUNIT_ASSERT(m1.dico() != m3.dico());     CPPUNIT_ASSERT(*m1.dico() != *m3.dico());
        CPPUNIT_ASSERT( m1 != m4 );    CPPUNIT_ASSERT(m1.dico() != m4.dico());     CPPUNIT_ASSERT(*m1.dico() != *m4.dico());
        CPPUNIT_ASSERT( m1 == m5 );    CPPUNIT_ASSERT(m1.dico() != m5.dico());     CPPUNIT_ASSERT(*m1.dico() == *m5.dico());

        CPPUNIT_ASSERT( m2 != m3 );    CPPUNIT_ASSERT(m2.dico() != m3.dico());     CPPUNIT_ASSERT(*m2.dico() != *m3.dico());
        CPPUNIT_ASSERT( m2 != m4 );    CPPUNIT_ASSERT(m2.dico() != m4.dico());     CPPUNIT_ASSERT(*m2.dico() != *m4.dico());
        CPPUNIT_ASSERT( m2 != m5 );    CPPUNIT_ASSERT(m2.dico() != m5.dico());     CPPUNIT_ASSERT(*m2.dico() != *m5.dico());

        CPPUNIT_ASSERT( m3 != m4 );    CPPUNIT_ASSERT(m3.dico() != m4.dico());     CPPUNIT_ASSERT(*m3.dico() != *m2.dico());
        CPPUNIT_ASSERT( m3 != m5 );    CPPUNIT_ASSERT(m3.dico() != m5.dico());     CPPUNIT_ASSERT(*m3.dico() != *m2.dico());

        CPPUNIT_ASSERT( m4 != m5 );    CPPUNIT_ASSERT(m1.dico() != m2.dico());     CPPUNIT_ASSERT(*m1.dico() != *m2.dico());

        std::ostringstream oss;
        oss << m1 << m2 << m3 << m4 << m5;

        CPPUNIT_ASSERT_EQUAL( std::string("Msg:"                                 "\n"
                                          "  id:    3"                           "\n"
                                          "  Qty:"                               "\n"
                                          "    id:    1"                         "\n"
                                          "    type:  LONG"                      "\n"
                                          "    value: 42"                        "\n"
                                          "  Prc:"                               "\n"
                                          "    id:    2"                         "\n"
                                          "    type:  DOUBLE"                    "\n"
                                          "    value: 3.1415"                    "\n"
                                          "M2:"                                  "\n"
                                          "  id:    3"                           "\n"
                                          "  Q:"                                 "\n"
                                          "    id:    1"                         "\n"
                                          "    type:  LONG"                      "\n"
                                          "    value: 42"                        "\n"
                                          "  P:"                                 "\n"
                                          "    id:    2"                         "\n"
                                          "    type:  DOUBLE"                    "\n"
                                          "    value: 3.1415"                    "\n"
                                          "M3:"                                  "\n"
                                          "  id:    3"                           "\n"
                                          "  B:"                                 "\n"
                                          "    id:    1"                         "\n"
                                          "    type:  BOOL"                      "\n"
                                          "    value: true"                      "\n"
                                          "  S:"                                 "\n"
                                          "    id:    2"                         "\n"
                                          "    type:  STRING"                    "\n"
                                          "    value: \"this is not a string\""  "\n"
                                          "M4:"                                  "\n"
                                          "  id:    3"                           "\n"
                                          "  B:"                                 "\n"
                                          "    id:    1"                         "\n"
                                          "    type:  SEQ_BOOL"                  "\n"
                                          "    value: [1]"                       "\n"
                                          "  S:"                                 "\n"
                                          "    id:    2"                         "\n"
                                          "    type:  SEQ_STRING"                "\n"
                                          "    value: [\":)\"]"                  "\n"
                                          "Msg:"                                 "\n"
                                          "  id:    3"                           "\n"
                                          "  Qty:"                               "\n"
                                          "    id:    1"                         "\n"
                                          "    type:  LONG"                      "\n"
                                          "    value: 42"                        "\n"
                                          "  Prc:"                               "\n"
                                          "    id:    2"                         "\n"
                                          "    type:  DOUBLE"                    "\n"
                                          "    value: 3.1415"                    "\n"), oss.str() );
    }

    //--------------------------------------------------------------------------------
    void NoDico2Msg()
    {
        pbart::Serializer s;

        pbart::Message m1 = s.dicoToMsg();
        std::ostringstream oss;
        oss << m1;
        CPPUNIT_ASSERT_EQUAL( std::string("Dico:" "\n" "  id:    1" "\n"), oss.str() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m1.check() );

        pbart::SharedDico d0 = s.dico();
        CPPUNIT_ASSERT(!d0);

        s.msgToDico(m1);
        pbart::SharedDico d1 = s.dico();
        CPPUNIT_ASSERT( d1 );
        CPPUNIT_ASSERT( d1 != d0 );

        pbart::Message m2 = s.dicoToMsg();
        oss.str(std::string()); //reset content
        oss << m2;
        CPPUNIT_ASSERT_EQUAL( std::string("Dico:" "\n" "  id:    1" "\n"), oss.str() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m2.check() );

        s.msgToDico(m2);
        pbart::SharedDico d2 = s.dico();
        CPPUNIT_ASSERT(  m1 ==  m2 );
        CPPUNIT_ASSERT(  d1 !=  d2 );
        CPPUNIT_ASSERT( *d1 == *d2 );

        pbart::Message m3 = s.dicoToMsg();
        oss.str(std::string()); //reset content
        oss << m3;
        CPPUNIT_ASSERT_EQUAL( std::string("Dico:" "\n" "  id:    1" "\n"), oss.str() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m3.check() );

        s.msgToDico(m3);
        pbart::SharedDico d3 = s.dico();
        CPPUNIT_ASSERT(  m3 ==  m2 );
        CPPUNIT_ASSERT( *d3 == *d2 );

        CPPUNIT_ASSERT(  m3.dico() ==  m2.dico() );
        CPPUNIT_ASSERT( *m3.dico() == *m2.dico() );
    }

    //--------------------------------------------------------------------------------
    void EmptyDico2Msg()
    {
        pbart::Serializer s;

        // s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"); //Exceptiopn
        pbart::Message m = s.dicoToMsg();
        pbart::Dico* ptr;
        pbart::SharedDico  emptyDico( ptr = new pbart::Dico );
        *ptr = *m.dico().get();
        pbart::Message mm(emptyDico, 1);
        CPPUNIT_ASSERT_EQUAL( m, mm );
        s.msgToDico(mm);

        CPPUNIT_ASSERT_EQUAL( pbart::Id(0),  m.check() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), mm.check() );

        std::ostringstream oss;
        oss << m;
        CPPUNIT_ASSERT_EQUAL( std::string("Dico:" "\n" "  id:    1" "\n"), oss.str() );

        pbart::SharedDico d1 = s.dico();
        s.msgToDico(m);
        pbart::SharedDico d2 = s.dico();

        CPPUNIT_ASSERT(  d1 !=  d2 );
        CPPUNIT_ASSERT( *d1 == *d2 );

        pbart::Message msg = s.message();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(-1), msg.check() );
        CPPUNIT_ASSERT(  msg.dico() !=  d1 );
        CPPUNIT_ASSERT( *msg.dico() == *d1 );
        CPPUNIT_ASSERT(  msg.dico() ==  d2 );
        CPPUNIT_ASSERT(  msg.dico() ==  s.dico() );
        CPPUNIT_ASSERT( *msg.dico() == *s.dico() );

        pbart::Message d2m = s.dicoToMsg();
        CPPUNIT_ASSERT(  m.dico() ==  d2m.dico() );
        CPPUNIT_ASSERT( *m.dico() == *d2m.dico() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), d2m.check() );
    }


    //--------------------------------------------------------------------------------
    void BasicDico2Msg()
    {
        pbart::Serializer s;

        s.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                    "<Dico version='1.0.0' xsd='1.0.0'>"
                    "<FieldsDef>"
                    "  <FieldDef id='1' name='Q' type='long'   deprecated='false' comment='Volume' />"
                    "  <FieldDef id='2' name='P' type='double' deprecated='false' comment='Price'  />"
                    "</FieldsDef>"
                    "<MessagesDef>"
                    "  <MessageDef id='3' name='Msg' deprecated='false' comment='basic'>"
                    "    <Field name='Q' default='3333' required='true'  repeated='false' />"
                    "    <Field name='P' default='3.14' required='false' repeated='false' />"
                    "  </MessageDef>"
                    "</MessagesDef>"
                    "</Dico>");
        pbart::SharedDico d1 = s.dico();
        pbart::Message    m1 = s.dicoToMsg();
        CPPUNIT_ASSERT_EQUAL( 2, (int) d1->field(3).items().size() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m1.check() );

        s.msgToDico(m1);
        pbart::SharedDico d2 = s.dico();
        pbart::Message    m2 = s.dicoToMsg();
        CPPUNIT_ASSERT_EQUAL( 2, (int) d2->field(3).items().size() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m2.check() );

        std::ostringstream oss;
        oss << m1;
        CPPUNIT_ASSERT_EQUAL( basicDicoYaml_, oss.str() );

        oss.str("");
        oss << m2;
        CPPUNIT_ASSERT_EQUAL( basicDicoYaml_, oss.str() );

        CPPUNIT_ASSERT(  d1 !=  d2 );
        CPPUNIT_ASSERT( *d1 == *d2 );
        CPPUNIT_ASSERT(  m1 ==  m2 );
    }

    //--------------------------------------------------------------------------------
    void ComplexDico2Msg()
    {
        pbart::Serializer s;

        s.loadDico( global_directory + "dico-Perf.xml" );
        pbart::SharedDico d1 = s.dico();
        pbart::Message    m1 = s.dicoToMsg();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m1.check() );

        s.msgToDico(m1);
        pbart::SharedDico d2 = s.dico();
        pbart::Message    m2 = s.dicoToMsg();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m2.check() );

        std::ostringstream os1;     os1 << m1;
        std::ostringstream os2;     os2 << m2;
        CPPUNIT_ASSERT_EQUAL( os1.str(), os2.str() );

        CPPUNIT_ASSERT(  d1 !=  d2 );
        CPPUNIT_ASSERT( *d1 == *d2 );
        CPPUNIT_ASSERT(  m1 ==  m2 );
    }

    //--------------------------------------------------------------------------------
    void DicoOfDico2Msg()
    {
        pbart::Serializer s;

        pbart::Message m1 = s.dicoToMsg();
        pbart::SharedDico dod1 = m1.dico();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m1.check() );

        s.loadDico( global_directory + "dico-Perf.xml" );
        pbart::Message    m2 = s.dicoToMsg();
        pbart::SharedDico dod2 = m2.dico();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m2.check() );

        s.msgToDico(m1);
        pbart::Message    m3 = s.dicoToMsg();
        pbart::SharedDico dod3 = m3.dico();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m3.check() );

        CPPUNIT_ASSERT( dod1 == dod2 );
        CPPUNIT_ASSERT( dod3 == dod2 );
        CPPUNIT_ASSERT( m1 == m3 );
        CPPUNIT_ASSERT( m1 != m2 );
        CPPUNIT_ASSERT( m3 != m2 );

        std::ostringstream o1;     o1 << m1;
        std::ostringstream o2;     o2 << m2;
        std::ostringstream o3;     o3 << m3;

        const std::string& s1 = o1.str();
        const std::string& s2 = o2.str();
        const std::string& s3 = o3.str();

        CPPUNIT_ASSERT_EQUAL( s1, s3 );
        CPPUNIT_ASSERT      ( s1 != s2 );
        CPPUNIT_ASSERT      ( s3 != s2 );
    }

    void FillDicoProperties()
    {
        pbart::Serializer s;
        pbart::Message msgFromEmptyDico = s.dicoToMsg();
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), msgFromEmptyDico.check() );

        // add 'field' and 'message' into Dico representing by a message
        // then set this message as a new Dico for the Serializer

        pbart::Id     versionID = msgFromEmptyDico.dico()->id("version");
        pbart::Id       fieldID = msgFromEmptyDico.dico()->id("Field");
        pbart::Id          idID = msgFromEmptyDico.dico()->id("id");
        pbart::Id        nameID = msgFromEmptyDico.dico()->id("name");
        pbart::Id        typeID = msgFromEmptyDico.dico()->id("type");
        pbart::Id  deprecatedID = msgFromEmptyDico.dico()->id("deprecated");
        pbart::Id     commentID = msgFromEmptyDico.dico()->id("comment");
        pbart::Id        itemID = msgFromEmptyDico.dico()->id("Item");
        pbart::Id    requiredID = msgFromEmptyDico.dico()->id("required");
        pbart::Id    defaultSID = msgFromEmptyDico.dico()->id("defaultS");

        msgFromEmptyDico.set( versionID, 2 );

        msgFromEmptyDico.get( fieldID ).toSeqMessage().push_back( pbart::Message( msgFromEmptyDico.dico(), fieldID ) );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set(   idID, 5                       );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( nameID, "fifth"                 );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( typeID, (int) pbart::Type::STRING );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( deprecatedID, false             );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( commentID, "commentFive"        );

        msgFromEmptyDico.get( fieldID ).toSeqMessage().push_back( pbart::Message( msgFromEmptyDico.dico(), fieldID ) );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set(   idID, 6                        );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( nameID, "sixth"                  );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( typeID, (int) pbart::Type::MESSAGE );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( deprecatedID, false              );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().set( commentID, "comment5"            );

        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().get( itemID ).toSeqMessage().push_back( pbart::Message( msgFromEmptyDico.dico(), itemID ) );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().get( itemID ).toSeqMessage().back().set(       idID, 5                             );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().get( itemID ).toSeqMessage().back().set( requiredID, true                          );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().get( itemID ).toSeqMessage().back().set(     typeID, (int) pbart::Type::SEQ_STRING   );
        msgFromEmptyDico.get( fieldID ).toSeqMessage().back().get( itemID ).toSeqMessage().back().set( defaultSID, "defaultStringValue"          );

        s.msgToDico(msgFromEmptyDico);

        // Use this dynamically set Dico to fill a new Message based on it

        pbart::Message m = s.message( s.dico()->id("sixth") );
        CPPUNIT_ASSERT_EQUAL( m.dico()->id("fifth"), m.check() ); //missing required field

        m.get( m.dico()->id("fifth") ).toSeqString().push_back("The first  string");
        m.get( m.dico()->id("fifth") ).toSeqString().push_back("The second string");
        m.get( m.dico()->id("fifth") ).toSeqString().push_back("The third  string");
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), m.check() ); //fixed missing required field

        CPPUNIT_ASSERT_THROW( s.msgToDico(m), pbart::DicoException ); //throw "Cannot extract Dictionary from message 'sixth' (id=6) because it is based on a Dictionary which is not reconized as a DicoOfDico"

        pbart::Message extractedDicoAsMsg = s.dicoToMsg();
        pbart::Message oneField           = extractedDicoAsMsg.get(fieldID).toSeqMessage().front();

        CPPUNIT_ASSERT_EQUAL( pbart::Id(0), extractedDicoAsMsg.check() );
        CPPUNIT_ASSERT_EQUAL( pbart::Id(0),           oneField.check() );

        CPPUNIT_ASSERT_THROW( s.msgToDico(oneField), pbart::DicoException );//throw "Cannot extract Dictionary from message 'Field' because its id=13 must be 1 "

        s.msgToDico (msgFromEmptyDico);
    }

};

const std::string Dico::emptyDicoYaml_  =   "Dico:"                     "\n"
                                            "  id:    1"                "\n"
                                            "  Field:"                  "\n"
                                            "  - id:"                   "\n"
                                            "      id:    2"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 0"            "\n"
                                            "    name:"                 "\n"
                                            "      id:    3"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"\""         "\n"
                                            "    type:"                 "\n"
                                            "      id:    4"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 0"            "\n"
                                            "    deprecated:"           "\n"
                                            "      id:    5"            "\n"
                                            "      type:  BOOL"         "\n"
                                            "      value: false"        "\n"
                                            "    comment:"              "\n"
                                            "      id:    6"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"\""         "\n"
                                            "    Item:"                 "\n";

const std::string Dico::basicDicoYaml_  =   "Dico:"                     "\n"
                                            "  id:    1"                "\n"
                                            "  version:"                "\n"
                                            "    id:    10"             "\n"
                                            "    type:  LONG"           "\n"
                                            "    value: 1"              "\n"
                                            "  Field:"                  "\n"
                                            "  - id:"                   "\n"
                                            "      id:    2"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 3"            "\n"
                                            "    name:"                 "\n"
                                            "      id:    3"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"Msg\""      "\n"
                                            "    type:"                 "\n"
                                            "      id:    4"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 5"            "\n"
                                            "    deprecated:"           "\n"
                                            "      id:    5"            "\n"
                                            "      type:  BOOL"         "\n"
                                            "      value: false"        "\n"
                                            "    comment:"              "\n"
                                            "      id:    6"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"basic\""    "\n"
                                            "    Item:"                 "\n"
                                            "    - id:"                 "\n"
                                            "        id:    2"          "\n"
                                            "        type:  LONG"       "\n"
                                            "        value: 1"          "\n"
                                            "      type:"               "\n"
                                            "        id:    4"          "\n"
                                            "        type:  LONG"       "\n"
                                            "        value: 2"          "\n"
                                            "      required:"           "\n"
                                            "        id:    7"          "\n"
                                            "        type:  BOOL"       "\n"
                                            "        value: true"       "\n"
                                            "      defaultD:"           "\n"
                                            "        id:    8"          "\n"
                                            "        type:  DOUBLE"     "\n"
                                            "        value: 3333"       "\n"
                                            "      defaultS:"           "\n"
                                            "        id:    9"          "\n"
                                            "        type:  STRING"     "\n"
                                            "    - id:"                 "\n"
                                            "        id:    2"          "\n"
                                            "        type:  LONG"       "\n"
                                            "        value: 2"          "\n"
                                            "      type:"               "\n"
                                            "        id:    4"          "\n"
                                            "        type:  LONG"       "\n"
                                            "        value: 3"          "\n"
                                            "      required:"           "\n"
                                            "        id:    7"          "\n"
                                            "        type:  BOOL"       "\n"
                                            "        value: false"      "\n"
                                            "      defaultD:"           "\n"
                                            "        id:    8"          "\n"
                                            "        type:  DOUBLE"     "\n"
                                            "        value: 3.14"       "\n"
                                            "      defaultS:"           "\n"
                                            "        id:    9"          "\n"
                                            "        type:  STRING"     "\n"
                                            "  - id:"                   "\n"
                                            "      id:    2"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 2"            "\n"
                                            "    name:"                 "\n"
                                            "      id:    3"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"P\""        "\n"
                                            "    type:"                 "\n"
                                            "      id:    4"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 3"            "\n"
                                            "    deprecated:"           "\n"
                                            "      id:    5"            "\n"
                                            "      type:  BOOL"         "\n"
                                            "      value: false"        "\n"
                                            "    comment:"              "\n"
                                            "      id:    6"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"Price\""    "\n"
                                            "    Item:"                 "\n"
                                            "  - id:"                   "\n"
                                            "      id:    2"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 1"            "\n"
                                            "    name:"                 "\n"
                                            "      id:    3"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"Q\""        "\n"
                                            "    type:"                 "\n"
                                            "      id:    4"            "\n"
                                            "      type:  LONG"         "\n"
                                            "      value: 2"            "\n"
                                            "    deprecated:"           "\n"
                                            "      id:    5"            "\n"
                                            "      type:  BOOL"         "\n"
                                            "      value: false"        "\n"
                                            "    comment:"              "\n"
                                            "      id:    6"            "\n"
                                            "      type:  STRING"       "\n"
                                            "      value: \"Volume\""   "\n"
                                            "    Item:"                 "\n";




CPPUNIT_TEST_SUITE_REGISTRATION( Dico );
