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

#define COMPILING_CPPUNIT_MESSAGE  "avoid some assert" //oliver
#define COMPILING_CPPUNIT_DICO

#ifndef COMPILING_CPPUNIT_MESSAGE
#error  COMPILING_CPPUNIT_MESSAGE should be defined
#endif

#include "pbart/Serializer.h"
#include "pbart/Message.h"
#include "pbart/Dico.h"

#include <cppunit/extensions/HelperMacros.h>

class Message : public CPPUNIT_NS::TestFixture
{
    //--------------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE( Message            );
    CPPUNIT_TEST( EmptyDefaultMessage      );
    CPPUNIT_TEST( EmptyNameIdZero          );
    CPPUNIT_TEST( EmptyNameField           );
    CPPUNIT_TEST( EmptyNameDecode          );
    CPPUNIT_TEST( EmptyNameMessageDefault  );
    CPPUNIT_TEST( EmptyException           );
    CPPUNIT_TEST( FillEraseItems           );
    CPPUNIT_TEST( AllowedItems             );
    CPPUNIT_TEST(     IdExceptionConst     );
    CPPUNIT_TEST(     IdExceptionMutable   );
    CPPUNIT_TEST( ZeroIdExceptionConst     );
    CPPUNIT_TEST( ZeroIdExceptionMutable   );
    CPPUNIT_TEST( ZeroIdMessageFromNothing );
    CPPUNIT_TEST( ZeroIdMessageFromBuffer  );
    CPPUNIT_TEST( ZeroIdMessageByReference );
    CPPUNIT_TEST( BadInputBuffer           );
    CPPUNIT_TEST( CorruptedBeginning       );
    CPPUNIT_TEST( CorruptedHeader_1        );
    CPPUNIT_TEST( CorruptedHeader_2        );
    CPPUNIT_TEST( CorruptedHeader_3        );
    CPPUNIT_TEST( CorruptedHeader_4        );
    CPPUNIT_TEST( CorruptedHeader_5        );
    CPPUNIT_TEST( CorruptedHeader_6        );
    CPPUNIT_TEST( CorruptedHeader_7        );
    CPPUNIT_TEST( CorruptedHeader_8        );
    CPPUNIT_TEST( CorruptedHeader_9        );
    CPPUNIT_TEST( CorruptedHeader10        );
    CPPUNIT_TEST( CorruptedHeader11        );
    CPPUNIT_TEST( CorruptedHeader12        );
    CPPUNIT_TEST( CorruptedHeader13        );
    CPPUNIT_TEST( CorruptedHeader14        );
    CPPUNIT_TEST( CorruptedHeader15        );
    CPPUNIT_TEST( CorruptedHeader16        );
    CPPUNIT_TEST( CorruptedHeader17        );
    CPPUNIT_TEST( CorruptedHeader18        );
    CPPUNIT_TEST( CorruptedHeader19        );
    CPPUNIT_TEST( CorruptedHeader20        );
    CPPUNIT_TEST( CorruptedBegin           );
    CPPUNIT_TEST( CorruptedMiddle          );
    CPPUNIT_TEST( CorruptedEnd             );
    CPPUNIT_TEST( MixedTwoBuffers          );
    CPPUNIT_TEST( RequestZeroIdDecode      );
    CPPUNIT_TEST( NullTrailingBytes        );
    CPPUNIT_TEST( BufferAppendedToItself   );
    CPPUNIT_TEST_SUITE_END();



    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------



    //--------------------------------------------------------------------------------
    pbart::Serializer  serializer;
    pbart::SharedDico dico;
    //--------------------------------------------------------------------------------


public:
    //--------------------------------------------------------------------------------
    void setUp()
    {
        extern std::string global_directory; // defined in main.cpp
        serializer.loadDico( global_directory + "dico-Perf.xml" );
        dico = serializer.dico();
    }



    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------



    //--------------------------------------------------------------------------------
    void EmptyDefaultMessage()
    {
        pbart::Id      dadId  = dico->id("MsgMsg");
        pbart::Id      sonId  = dico->id("MsgHas");

        pbart::Message dadMsg = serializer.message(dadId);
        pbart::Message sonMsg = dadMsg.get(sonId).toMessage();

        CPPUNIT_ASSERT_EQUAL (sonId, sonMsg.id());
    }

    //--------------------------------------------------------------------------------
    void EmptyNameIdZero()
    {
        pbart::Id id = dico->id("");
        CPPUNIT_ASSERT_EQUAL (id, pbart::Id(0));
    }

    //--------------------------------------------------------------------------------
    void EmptyNameField()
    {
        const pbart::Field& f = dico->field (pbart::Id(0));
        CPPUNIT_ASSERT (f.name().empty());
    }

    //--------------------------------------------------------------------------------
    void EmptyNameDecode()
    {
        pbart::Message m = serializer.message();
        CPPUNIT_ASSERT (m.check() == pbart::Id(-1));
        CPPUNIT_ASSERT (m.name().empty());
        std::ostringstream o; o << m;
    }

    //--------------------------------------------------------------------------------
    void EmptyNameMessageIdZero()
    {
        pbart::Message m = serializer.message (pbart::Id(0));
        CPPUNIT_ASSERT (m.check() == pbart::Id(-1));
        CPPUNIT_ASSERT (m.name().empty());
        std::ostringstream o; o << m;
    }

    //--------------------------------------------------------------------------------
    void EmptyNameMessageDefault()
    {
        pbart::Message m;
        CPPUNIT_ASSERT (m.check() == pbart::Id(-1));
        CPPUNIT_ASSERT (m.name().empty());
        std::ostringstream o; o << m;
        m.id(12345);
        CPPUNIT_ASSERT (m.check()  == pbart::Id( 12345));
        CPPUNIT_ASSERT (m.check(1) == pbart::Id( 1    ));
        CPPUNIT_ASSERT (m.check(0) == pbart::Id(-1    ));
    }

    //--------------------------------------------------------------------------------
    void EmptyException()
    {
        pbart::Id id = dico->id("MsgMsg");
        pbart::Message m = serializer.message (id);
        CPPUNIT_ASSERT (m.check() == pbart::Id(0));

        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            CPPUNIT_ASSERT_EQUAL( dico->field(it.id()).name(),    it.name()       );
            CPPUNIT_ASSERT_EQUAL( false,                          it.deprecated() );
            CPPUNIT_ASSERT_EQUAL( false,                          it.repeated()   );
            CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                     it.check()      );
            CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                     m .check(it.id()) );
            CPPUNIT_ASSERT_EQUAL( true,                           it.empty()      );
            CPPUNIT_ASSERT      (                                 it.id()         );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.type()       );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.typeDico()   );

            CPPUNIT_ASSERT_THROW( *it,        pbart::EmptyException );
            CPPUNIT_ASSERT_THROW( it->type(), pbart::EmptyException );

            m.erase(it);

            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.type()       );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.typeDico()   );

            CPPUNIT_ASSERT_THROW( *it,            pbart::EmptyException );
            CPPUNIT_ASSERT_THROW( it->type(),     pbart::EmptyException );

            // insert it
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE), m.get(it.id()).type() );
        }
        std::ostringstream o; o << m;

        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            CPPUNIT_ASSERT_EQUAL( dico->field(it.id()).name(),    it.name()       );
            CPPUNIT_ASSERT_EQUAL( false,                          it.deprecated() );
            CPPUNIT_ASSERT_EQUAL( false,                          it.repeated()   );
            CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                   it.check()      );
            CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                   m .check(it.id()) );
            CPPUNIT_ASSERT_EQUAL( false,                          it.empty()      );
            CPPUNIT_ASSERT_EQUAL( m.get(it.id()),                *it              );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it->type()      );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.type()       );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.typeDico()   );
            m.erase(it);
        }
        o << m;

        for (pbart::Message::const_iterator it = m.cbegin(); it != m.cend(); ++it)
        {
            CPPUNIT_ASSERT_EQUAL( dico->field(it.id()).name(),    it.name()       );
            CPPUNIT_ASSERT_EQUAL( false,                          it.deprecated() );
            CPPUNIT_ASSERT_EQUAL( false,                          it.repeated()   );
            CPPUNIT_ASSERT_EQUAL( pbart::Id(0),                     it.check()      );
            CPPUNIT_ASSERT_EQUAL( true,                           it.empty()      );
            CPPUNIT_ASSERT      (                                 it.id()         );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.type()       );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it.typeDico()   );

            const pbart::Message& cm = m;
            o << cm;
            CPPUNIT_ASSERT_EQUAL( cm.get(it.id()),               *it              );
            CPPUNIT_ASSERT_EQUAL( pbart::Type(pbart::Type::MESSAGE),  it->type()      );
        }
    }

    //--------------------------------------------------------------------------------
    void FillEraseItems()
    {
        pbart::Id id = dico->id("MsgMsg");
        pbart::Message m = serializer.message (id);
        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            id = it.id();
            //    std::cout <<"\nBefore\n"<< m <<'\n';
            m.set (id, pbart::Variant(serializer.message (id)));
            //    std::cout <<"\nDecode("<< id <<")\n"<< m <<'\n';
            m.set (id, pbart::Message(pbart::SharedDico(dico),id));
            //    std::cout <<"\nMessage("<< id <<")\n"<< m <<'\n';
        }
        std::ostringstream o; o << m;

        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            id = it.id();
            CPPUNIT_ASSERT(      ! it.name().empty()                  );
            CPPUNIT_ASSERT(      ! it.deprecated()                    );
            CPPUNIT_ASSERT(      ! it.repeated()                      );
            CPPUNIT_ASSERT(      ! it.check()                         );
            CPPUNIT_ASSERT(      ! it.empty()                         );
            CPPUNIT_ASSERT(        m.allowed(id)                      );
            CPPUNIT_ASSERT(        it.id() > 0                        );
            CPPUNIT_ASSERT(        it.type()    == pbart::Type::MESSAGE );
            CPPUNIT_ASSERT(        it.typeDico()== pbart::Type::MESSAGE );
            CPPUNIT_ASSERT_EQUAL( *it,   pbart::Variant(serializer.message (id)) );
            CPPUNIT_ASSERT(       *it != pbart::Variant()               );
            CPPUNIT_ASSERT_EQUAL(  it->type(), it.type()              );
            m.erase(it);
            CPPUNIT_ASSERT(        it.empty()                         );
            CPPUNIT_ASSERT(      ! it.check()                         );
            CPPUNIT_ASSERT(        m.allowed(id)                      );
            CPPUNIT_ASSERT_THROW( *it,         pbart::EmptyException    );
            CPPUNIT_ASSERT_THROW(  it->type(), pbart::EmptyException    );
        }
        o << m;
    }

    //--------------------------------------------------------------------------------
    void AllowedItems()
    {
        pbart::Id id = dico->id("SeqAll");
        pbart::Message m = serializer.message (id);
        for (id = pbart::Id(-1000); id != pbart::Id(1000); ++id)
        {
            if (m.allowed(id))
            {
                CPPUNIT_ASSERT(   m.empty     (id)         );
                CPPUNIT_ASSERT(   m.repeated  (id)         );
                CPPUNIT_ASSERT( ! m.required  (id)         );
                CPPUNIT_ASSERT( ! m.deprecated(id)         );
                CPPUNIT_ASSERT( ! m.name      (id).empty() );
                CPPUNIT_ASSERT( ! m.erase     (id)         );
            }
            else
            {
                CPPUNIT_ASSERT      (   m.empty(id)                        );
                CPPUNIT_ASSERT_THROW(   m.type      (id), pbart::IdException );
                CPPUNIT_ASSERT_THROW(   m.repeated  (id), pbart::IdException );
                CPPUNIT_ASSERT_THROW(   m.required  (id), pbart::IdException );
                //CPPUNIT_ASSERT_THROW(   m.deprecated(id), pbart::IdException ); //use dico_->field(id)
                //CPPUNIT_ASSERT_THROW(   m.name      (id), pbart::IdException );
                CPPUNIT_ASSERT      ( ! m.erase     (id) );
            }
        }
        std::ostringstream o; o << m;
    }

    //--------------------------------------------------------------------------------
    void IdExceptionConst()
    {
        pbart::Id id = dico->id("MsgHas");
        const pbart::Message m = serializer.message (id);
        id = dico->id("MsgMsg");
        CPPUNIT_ASSERT_THROW( m.get(id), pbart::IdException );
        std::ostringstream o; o << m;
    }

    //--------------------------------------------------------------------------------
    void IdExceptionMutable()
    {
        pbart::Id id = dico->id("MsgHas");
        pbart::Message m = serializer.message (id);
        id = dico->id("MsgMsg");
        CPPUNIT_ASSERT_THROW( m.get(id), pbart::IdException );
        std::ostringstream o; o << m;
    }
    //--------------------------------------------------------------------------------
    void ZeroIdExceptionConst()
    {
        const pbart::Message m;
        pbart::Id id = dico->id("MsgMsg");
        CPPUNIT_ASSERT_THROW( m.get(id), pbart::IdException );
        std::ostringstream o; o << m;
    }

    //--------------------------------------------------------------------------------
    void ZeroIdExceptionMutable()
    {
        pbart::Message m;
        pbart::Id id = dico->id("MsgMsg");
        CPPUNIT_ASSERT_THROW( m.get(id), pbart::IdException );
        std::ostringstream o; o << m;
    }


    //--------------------------------------------------------------------------------
    void ZeroIdMessageFromNothing()
    {
        pbart::Message message = serializer.message();
        pbart::Message empty;
        std::ostringstream o; o << message; std::string s_message = o.str();
        o.str("");            o << empty;   std::string s_empty   = o.str();

        CPPUNIT_ASSERT_EQUAL(   message.id(), pbart::Id(0) );
        CPPUNIT_ASSERT      (   message !=    empty      );  //different dico
        CPPUNIT_ASSERT_EQUAL( s_message,    s_empty      );

        pbart::Serializer serializerWithoutDico;
        message = serializerWithoutDico.message();
        o.str("");  o << message; s_message = o.str();

        CPPUNIT_ASSERT_EQUAL(   message.id(), pbart::Id(0) );
        CPPUNIT_ASSERT_EQUAL(   message,      empty      ); //no Dico for both
        CPPUNIT_ASSERT_EQUAL( s_message,    s_empty      );
    }

    //--------------------------------------------------------------------------------
    void ZeroIdMessageFromBuffer()
    {
        pbart::Buffer  buffer;
        pbart::Message message = serializer.decode (buffer);
        pbart::Message empty;
        std::ostringstream o; o << message; std::string s_message = o.str();
        o.str("");            o << empty;   std::string s_empty   = o.str();

        CPPUNIT_ASSERT_EQUAL(   message.id(), pbart::Id(0) );
        CPPUNIT_ASSERT      (   message !=    empty      );  //different dico
        CPPUNIT_ASSERT_EQUAL( s_message,    s_empty      );

        pbart::Serializer serializerWithoutDico;
        message = serializerWithoutDico.decode (buffer);
        o.str("");  o << message; s_message = o.str();

        CPPUNIT_ASSERT_EQUAL(   message.id(), pbart::Id(0) );
        CPPUNIT_ASSERT_EQUAL(   message,      empty      ); //no Dico for both
        CPPUNIT_ASSERT_EQUAL( s_message,    s_empty      );
    }

    //--------------------------------------------------------------------------------
    void ZeroIdMessageByReference()
    {
        pbart::Buffer  buffer;
        pbart::Message message;
        pbart::Message empty;

        serializer.decode (buffer, message);

        CPPUNIT_ASSERT_EQUAL( pbart::Id(0),     message.id()   );
        CPPUNIT_ASSERT      ( empty.dico() != message.dico() );
        CPPUNIT_ASSERT      ( empty        != message        );
    }

    //--------------------------------------------------------------------------------
    void BadInputBuffer()
    {
        const pbart::Buffer buffer = "this is not a correct PBART serialized message";
        CPPUNIT_ASSERT_THROW( serializer.decode(buffer), pbart::MsgTypeException );
    }

    //--------------------------------------------------------------------------------
    void CorruptedBeginning()
    {
        pbart::Message m = init("SeqMsg");
        pbart::Buffer buffer = serializer.encode(m);
        CPPUNIT_ASSERT( buffer.size() > 2 );
        buffer[ 0] = '-';
        buffer[ 1] = '6';
        buffer[ 2] = 'c';
        CPPUNIT_ASSERT_THROW( serializer.decode(buffer), pbart::MsgTypeException );
    }

    //--------------------------------------------------------------------------------
    template<typename EXCEPTION>
    void corruptHeader (size_t i)
    {
        pbart::Message m = init("SeqMsg");
        pbart::Buffer buffer = serializer.encode(m);
        CPPUNIT_ASSERT( buffer.size() > 20 );
        if (buffer[i])    buffer[i] = - buffer[i];
        else              buffer[i] = - 128;
        CPPUNIT_ASSERT_THROW( serializer.decode(buffer), EXCEPTION );
    }
    //--------------------------------------------------------------------------------
    void CorruptedHeader_1()  { corruptHeader <pbart::DecodeException>     (1); }  //Header format must be [Version][Checksum1][Checksum2]
    void CorruptedHeader_2()  { corruptHeader <pbart::DecodeException>     (2); }  //Header format must be [Version][Checksum1][Checksum2]
    void CorruptedHeader_3()  { corruptHeader <pbart::DecodeException>     (3); }  //Header format must be [Version][Checksum1][Checksum2]
    void CorruptedHeader_4()  { corruptHeader <pbart::DecodeException>     (4); }  //Header format must be [Version][Checksum1][Checksum2]
    void CorruptedHeader_5()  { corruptHeader <pbart::DecodeException>     (5); }  //Header format must be [Version][Checksum1][Checksum2]
    void CorruptedHeader_6()  { corruptHeader <pbart::ItemException>       (6); }
    void CorruptedHeader_7()  { corruptHeader <pbart::ItemException>       (7); }
    void CorruptedHeader_8()  { corruptHeader <pbart::ItemException>       (8); }
    void CorruptedHeader_9()  { corruptHeader <pbart::ItemException>       (9); }
    void CorruptedHeader10()  { corruptHeader <pbart::DecodeException>    (10); }
    void CorruptedHeader11()  { corruptHeader <pbart::DecodeException>    (11); }  //when count=8 is not zero the bytesUntilLimit=321 must be zero
    void CorruptedHeader12()  { corruptHeader <pbart::ItemException>      (12); }
    void CorruptedHeader13()  { corruptHeader <pbart::ItemException>      (13); }
    void CorruptedHeader14()  { corruptHeader <pbart::DecodeException>    (14); }
    void CorruptedHeader15()  { corruptHeader <pbart::DecodeException>    (15); }  //when count=6 is not zero the bytesUntilLimit=317 must be zero
    void CorruptedHeader16()  { corruptHeader <pbart::ItemException>      (16); }
    void CorruptedHeader17()  { corruptHeader <pbart::DecodeException>    (17); }
    void CorruptedHeader18()  { corruptHeader <pbart::DecodeException>    (18); }  //when count=4 is not zero the bytesUntilLimit=313 must be zero
    void CorruptedHeader19()  { corruptHeader <pbart::DecodeException>    (19); }  //when count=4 is not zero the bytesUntilLimit=313 must be zero
    void CorruptedHeader20()  { corruptHeader <pbart::ItemException>      (20); }

    //--------------------------------------------------------------------------------
    void CorruptedBegin()
    {
        pbart::Message m = init("SeqAll");
        pbart::Buffer buffer = serializer.encode(m);
        CPPUNIT_ASSERT( buffer.size() > 87 );
        buffer[80] = 'a';
        buffer[81] = 'b';
        buffer[82] = 'c';
        buffer[83] = 'd';
        buffer[84] = 'a';
        buffer[85] = 'b';
        buffer[86] = 'c';
        buffer[87] = 'd';
        CPPUNIT_ASSERT_THROW( serializer.decode(buffer), pbart::MsgTypeException );
    }

    //--------------------------------------------------------------------------------
    void CorruptedMiddle()
    {
        pbart::Message m = init("SeqAll");
        pbart::Buffer buffer = serializer.encode(m);
        CPPUNIT_ASSERT( buffer.size() > 287 );
        buffer[280] = 'a';
        buffer[281] = 'b';
        buffer[282] = 'c';
        buffer[283] = 'd';
        buffer[284] = 'a';
        buffer[285] = 'b';
        buffer[286] = 'c';
        buffer[287] = 'd';
        CPPUNIT_ASSERT_THROW( serializer.decode(buffer), pbart::ItemException );
    }

    //--------------------------------------------------------------------------------
    void CorruptedEnd()
    {
        pbart::Message m = init("SeqAll");
        pbart::Buffer buffer = serializer.encode(m);
        buffer[ buffer.size() - 8 + 0 ] = 'a';
        buffer[ buffer.size() - 8 + 1 ] = 'b';
        buffer[ buffer.size() - 8 + 2 ] = 'c';
        buffer[ buffer.size() - 8 + 3 ] = 'd';
        buffer[ buffer.size() - 8 + 4 ] = 'a';
        buffer[ buffer.size() - 8 + 5 ] = 'b';
        buffer[ buffer.size() - 8 + 6 ] = 'c';
        buffer[ buffer.size() - 8 + 7 ] = 'd';
        CPPUNIT_ASSERT_THROW( serializer.decode(buffer), pbart::DecodeException );
    }

    //--------------------------------------------------------------------------------
    void MixedTwoBuffers()
    {
        pbart::Message m1 = init("MsgSeq"); //"SeqAll");
        pbart::Message m2 = init("MsgMsg"); //"MsgAll");
        pbart::Buffer  b1 = serializer.encode(m1);
        pbart::Buffer  b2 = serializer.encode(m2);

        for (size_t i = 1; i < b1.size() && i < b2.size(); i+=2)
        {
            b1[i] = b2[i];
        }

        CPPUNIT_ASSERT_THROW( serializer.decode(b1), pbart::ItemException );
    }

    //--------------------------------------------------------------------------------
    void RequestZeroIdDecode()
    {
        pbart::Message m = init("MsgPrc");
        pbart::Buffer  b = serializer.encode(m);
        CPPUNIT_ASSERT_THROW( serializer.decode(b,pbart::Id(1)), pbart::DecodeException );
    }

    //--------------------------------------------------------------------------------
    void NullTrailingBytes()
    {
        pbart::Message m = init("MsgPrc");
        pbart::Buffer  b = serializer.encode(m);
        char nullBytes[] = {1, 1, 1, 1, 1, 1, 0};
        b += nullBytes;
        b[ b.size() - sizeof(nullBytes) + 0 ] = 0;
        b[ b.size() - sizeof(nullBytes) + 1 ] = 0;
        b[ b.size() - sizeof(nullBytes) + 2 ] = 0;
        b[ b.size() - sizeof(nullBytes) + 3 ] = 0;
        b[ b.size() - sizeof(nullBytes) + 4 ] = 0;
        b[ b.size() - sizeof(nullBytes) + 5 ] = 0;
        b[ b.size() - sizeof(nullBytes) + 6 ] = 0;
        CPPUNIT_ASSERT_THROW( serializer.decode(b), pbart::DecodeException );
    }

    //--------------------------------------------------------------------------------
    void BufferAppendedToItself()
    {
        pbart::Message m = init("MsgPrc");
        pbart::Buffer  b = serializer.encode(m);
        b += b;
        CPPUNIT_ASSERT_THROW( serializer.decode(b), pbart::DecodeException );
    }


    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------



private:
    //--------------------------------------------------------------------------------
    bool                     b;
    pbart::Variant::long_t     l;
    double                   d;
    std::string              s;
    //--------------------------------------------------------------------------------
    const int  *eb, *el, *ed, *es, *em;  //enable bool/long/double/string/message

    //--------------------------------------------------------------------------------
    void reset()
    {
        static const int me = 29;     eb = &me;
        static const int ml = -1;     el = &ml;
        static const int md =  1;     ed = &md;
        static const int ms =  0;     es = &ms;
        static const int mm =  2;     em = &mm;

        b  = true;
        l  = 0;
        d  = 0;

        if (*es >= 0)
        {
            s.resize(*es);
            for (int i=0; i<*es; ++i)   s.at(i) = 'A';
        }
    }

    //--------------------------------------------------------------------------------
    void inc()
    {
        if (! s.empty())
        {
            char c = s.at(0);
            if (c == '~')  c = ' ';
            else           c++;
            for (size_t i=0; i<s.size(); ++i)   s.at(i) = c;
        }

        b  = !b;
        l  = l*3/2 + 1;
        d  = 1.1*d + 9.2;
    }

    //--------------------------------------------------------------------------------
    pbart::Message init (pbart::Id id)
    {
        pbart::Message m = serializer.message (id);

        for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
        {
            pbart::Id item = it.id();

            CPPUNIT_ASSERT(m.allowed(item));

            switch (it.type())
            {
            case pbart::Type::BOOL:        if (*eb>=0) { m.set (item,           b ); inc();  } break;
            case pbart::Type::LONG:        if (*el>=0) { m.set (item,           l ); inc();  } break;
            case pbart::Type::DOUBLE:      if (*ed>=0) { m.set (item,           d ); inc();  } break;
            case pbart::Type::STRING:      if (*es>=0) { m.set (item,           s ); inc();  } break;
            case pbart::Type::MESSAGE:     if (*em>=0) { m.set (item, init(item)  );         } break;

            case pbart::Type::SEQ_BOOL:    if (*eb>=0) { m.set(item,pbart::Variant::SeqBool());    for(int i=0;i<*eb;i++) { m.get(item).toSeqBool   ().push_back(b);  inc();  } } break;
            case pbart::Type::SEQ_LONG:    if (*el>=0) { m.set(item,pbart::Variant::SeqLong());    for(int i=0;i<*el;i++) { m.get(item).toSeqLong   ().push_back(l);  inc();  } } break;
            case pbart::Type::SEQ_DOUBLE:  if (*ed>=0) { m.set(item,pbart::Variant::SeqDouble());  for(int i=0;i<*ed;i++) { m.get(item).toSeqDouble ().push_back(d);  inc();  } } break;
            case pbart::Type::SEQ_STRING:  if (*es>=0) { m.set(item,pbart::Variant::SeqString());  for(int i=0;i<*es;i++) { m.get(item).toSeqString ().push_back(s);  inc();  } } break;
            case pbart::Type::SEQ_MESSAGE: if (*em>=0) { m.set(item,pbart::Variant::SeqMessage()); for(int i=0;i<*em;i++) { m.get(item).toSeqMessage().push_back(init(item)); } } break;

            default:
                CPPUNIT_ASSERT (0 && "Unexpected it.type()");
            }
        }

        return m;
    }

    //--------------------------------------------------------------------------------
    void check (const pbart::Message& m)
    {
        for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            if (! it.empty())
                CPPUNIT_ASSERT_EQUAL (it.type(), it->type());

            switch (it.type())
            {
            case pbart::Type::BOOL:        if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          b ,  it->toBool   ()); inc(); } break;
            case pbart::Type::LONG:        if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          l ,  it->toLong   ()); inc(); } break;
            case pbart::Type::DOUBLE:      if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          d ,  it->toDouble ()); inc(); } break;
            case pbart::Type::STRING:      if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          s ,  it->toString ()); inc(); } break;
            case pbart::Type::MESSAGE:     if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(init(it.id()), it->toMessage());        } break;

            case pbart::Type::SEQ_BOOL:    if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(size_t(*eb),it->toSeqBool()   .size()); for(int i=0;i<*eb;i++) { CPPUNIT_ASSERT_EQUAL(it->toBool   (i),b    ); inc(); } } break;
            case pbart::Type::SEQ_LONG:    if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(size_t(*el),it->toSeqLong()   .size()); for(int i=0;i<*el;i++) { CPPUNIT_ASSERT_EQUAL(it->toLong   (i),l    ); inc(); } } break;
            case pbart::Type::SEQ_DOUBLE:  if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(size_t(*ed),it->toSeqDouble() .size()); for(int i=0;i<*ed;i++) { CPPUNIT_ASSERT_EQUAL(it->toDouble (i),d    ); inc(); } } break;
            case pbart::Type::SEQ_STRING:  if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(size_t(*es),it->toSeqString() .size()); for(int i=0;i<*es;i++) { CPPUNIT_ASSERT_EQUAL(it->toString (i),s    ); inc(); } } break;
            case pbart::Type::SEQ_MESSAGE: if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(size_t(*em),it->toSeqMessage().size()); for(int i=0;i<*em;i++) { CPPUNIT_ASSERT_EQUAL(it->toMessage(i),init(it.id()));} } break;

            default:
                CPPUNIT_ASSERT (0 && "Unexpected it.type()");
            }
        }
    }

    //--------------------------------------------------------------------------------
    pbart::Message init (const char name[])
    {
        reset();
        pbart::Id id = dico->id(name);
        return init (id);
    }

    //--------------------------------------------------------------------------------
    pbart::Message process (pbart::Id id)
    {
        reset();
        pbart::Message m1 = init   (id);
        pbart::Buffer  pb = serializer.encode (m1);
        pbart::Message m2 = serializer.decode (pb);

        std::ostringstream oss;
        oss << m1 << m2;

        CPPUNIT_ASSERT_EQUAL (m1, m2);
        reset();
        check (m1);

        return m1;
    }

    //--------------------------------------------------------------------------------
    pbart::Message process (const char name[])
    {
        pbart::Id id = dico->id(name);
        return process (id);
    }

    //--------------------------------------------------------------------------------
    void Booleans()                                   {  process ("MsgHas");  }
    void Longs()                                      {  process ("MsgQty");  }
    void Doubles()                                    {  process ("MsgPrc");  }
    void Strings()                                    {  process ("MsgStr");  }
    void Sequences_Of_Bool()                          {  process ("SeqHas");  }
    void Sequences_Of_Long()                          {  process ("SeqQty");  }
    void Sequences_Of_Double()                        {  process ("SeqPrc");  }
    void Sequences_Of_String()                        {  process ("SeqStr");  }
    void Messages_Of_SingleTypes()                    {  process ("MsgMsg");  }
    void Messages_Of_SequencesOfSingleBasicTypes()    {  process ("MsgSeq");  }
    void Sequences_Of_MessagesOfSingleBasicTypes()    {  process ("SeqMsg");  }
    void Sequences_Of_MessagesOfBasicTypeSequences()  {  process ("SeqSeq");  }
    void SingleTypes_Of_AboveFieldsAndMessages()      {  process ("MsgAll");  }
    void Sequences_Of_AboveFieldsAndMessages()        {  process ("SeqAll");  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Message );

