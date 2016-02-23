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

//#define DUMP_BUFFER_IN_FILES
//#define DUMP_MESSAGES_IN_YAML
//#define CHECK_ALL_POSSIBILITIES


#ifdef _MSC_VER
//#include "WinNT.h"    //LARGE_INTEGER
//#include "WinBase.h"  //QueryPerformanceCounter
#  include <windows.h>
#else
#  include <time.h>
#endif

#include "pbart/Serializer.h"
#include "pbart/Message.h"
#include "pbart/Dico.h"

#include <cppunit/extensions/HelperMacros.h>

#ifdef  DUMP_BUFFER_IN_FILES
#  include <fstream>
#  include <sstream>
#  include <iomanip>
#endif

class Perf : public CPPUNIT_NS::TestFixture
{
  //--------------------------------------------------------------------------
  static const int LOOPS = 1;

  //--------------------------------------------------------------------------
  CPPUNIT_TEST_SUITE( Perf );
  CPPUNIT_TEST( ResetYamlFile                             );
  CPPUNIT_TEST( Booleans                                  );
  CPPUNIT_TEST( Longs                                     );
  CPPUNIT_TEST( Doubles                                   );
  CPPUNIT_TEST( Strings                                   );
  CPPUNIT_TEST( Sequences_Of_Bool                         );
  CPPUNIT_TEST( Sequences_Of_Long                         );
  CPPUNIT_TEST( Sequences_Of_Double                       );
  CPPUNIT_TEST( Sequences_Of_String                       );
  CPPUNIT_TEST( Messages_Of_SingleTypes                   );
  CPPUNIT_TEST( Messages_Of_SequencesOfSingleBasicTypes   );
  CPPUNIT_TEST( Sequences_Of_MessagesOfSingleBasicTypes   );
  CPPUNIT_TEST( Sequences_Of_MessagesOfBasicTypeSequences );
  CPPUNIT_TEST( SingleTypes_Of_AboveFieldsAndMessages     );
  CPPUNIT_TEST( Sequences_Of_AboveFieldsAndMessages       );
  CPPUNIT_TEST_SUITE_END();

  //--------------------------------------------------------------------------
  bool                     b;
  pbart::Variant::long_t   l;
  double                   d;
  std::string              s;
  //--------------------------------------------------------------------------
  const ssize_t  *eb, *el, *ed, *es, *em;  //enable bool/long/double/string/message
  //--------------------------------------------------------------------------
  double      ratio;
  int         loops;
  //--------------------------------------------------------------------------
  int         nm;     //number of messages
  int         ne;     //number of empty fields
  int         nb;     //number of fields
  //--------------------------------------------------------------------------
  std::ofstream     file;
  pbart::Serializer serializer;
  pbart::SharedDico dico;
  //--------------------------------------------------------------------------


public:

  //--------------------------------------------------------------------------
  void setUp()
  {
#ifdef _MSC_VER
    LARGE_INTEGER freq;
    QueryPerformanceFrequency (&freq);
    ratio = (double) freq.QuadPart / 1000;
#else
    ratio = 1000;
#endif

#ifdef  DUMP_MESSAGES_IN_YAML
    file.open("messages.yaml", std::ios_base::app);
#endif

    extern std::string global_directory; // defined in main.cpp
    serializer.loadDico( global_directory + "dico-Perf.xml" );
    dico = serializer.dico();
  }

  //--------------------------------------------------------------------------
  void ResetYamlFile()
  {
#ifdef  DUMP_MESSAGES_IN_YAML
    file.close();
    file.open("messages.yaml", std::ios_base::trunc);
#endif
  }

  //--------------------------------------------------------------------------
  void tearDown()
  {
    file.close();
  }

  //--------------------------------------------------------------------------
  void reset()
  {
    b  = true;
    l  = 0;
    d  = 0;

    if (*es >= 0)
    {
      s.resize(*es);
      for (int i=0; i<*es; ++i)   s.at(i) = 'A';
    }

    nb = nm = ne = 0;
  }

  //--------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------
  void check_classic (const pbart::Message& m)
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

  //--------------------------------------------------------------------------
  void check_recursive (const pbart::Message& m)
  {
    for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
    {
      if (! it.empty())
        CPPUNIT_ASSERT_EQUAL( it.type() , it->type() );

      switch (it.type())
      {
        case pbart::Type::BOOL:        CPPUNIT_ASSERT_EQUAL(it.empty(),*eb<0); if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          b ,it->toBool  ()); inc(); } break;
        case pbart::Type::LONG:        CPPUNIT_ASSERT_EQUAL(it.empty(),*el<0); if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          l ,it->toLong  ()); inc(); } break;
        case pbart::Type::DOUBLE:      CPPUNIT_ASSERT_EQUAL(it.empty(),*ed<0); if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          d ,it->toDouble()); inc(); } break;
        case pbart::Type::STRING:      CPPUNIT_ASSERT_EQUAL(it.empty(),*es<0); if(!it.empty()) { CPPUNIT_ASSERT_EQUAL(          s ,it->toString()); inc(); } break;
        case pbart::Type::MESSAGE:     CPPUNIT_ASSERT_EQUAL(it.empty(),*em<0); if(!it.empty()) { check_recursive (it->toMessage());                        } break;

        case pbart::Type::SEQ_BOOL:    CPPUNIT_ASSERT_EQUAL(it.empty(),*eb<0); if(!it.empty()) { CPPUNIT_ASSERT(size_t(*eb)==it->toSeqBool   ().size()); for(int i=0;i<*eb;i++) { CPPUNIT_ASSERT_EQUAL(b,it->toBool  (i)); inc(); } } break;
        case pbart::Type::SEQ_LONG:    CPPUNIT_ASSERT_EQUAL(it.empty(),*el<0); if(!it.empty()) { CPPUNIT_ASSERT(size_t(*el)==it->toSeqLong   ().size()); for(int i=0;i<*el;i++) { CPPUNIT_ASSERT_EQUAL(l,it->toLong  (i)); inc(); } } break;
        case pbart::Type::SEQ_DOUBLE:  CPPUNIT_ASSERT_EQUAL(it.empty(),*ed<0); if(!it.empty()) { CPPUNIT_ASSERT(size_t(*ed)==it->toSeqDouble ().size()); for(int i=0;i<*ed;i++) { CPPUNIT_ASSERT_EQUAL(d,it->toDouble(i)); inc(); } } break;
        case pbart::Type::SEQ_STRING:  CPPUNIT_ASSERT_EQUAL(it.empty(),*es<0); if(!it.empty()) { CPPUNIT_ASSERT(size_t(*es)==it->toSeqString ().size()); for(int i=0;i<*es;i++) { CPPUNIT_ASSERT_EQUAL(s,it->toString(i)); inc(); } } break;
        case pbart::Type::SEQ_MESSAGE: CPPUNIT_ASSERT_EQUAL(it.empty(),*em<0); if(!it.empty()) { CPPUNIT_ASSERT(size_t(*em)==it->toSeqMessage().size()); for(int i=0;i<*em;i++) { check_recursive (it->toMessage(i));             } } break;

        default:
          CPPUNIT_ASSERT (0 && "Unexpected it.type()");
      }
    }
  }

  //--------------------------------------------------------------------------
  void check_allowed (const pbart::Message& m)
  {
    for (pbart::Id id = pbart::Id(-1); id != pbart::Id(1000); ++id)
    {
      if (! m.allowed(id))
        continue;

      bool empty = m.empty(id);
      if (! empty)
      {
        CPPUNIT_ASSERT_EQUAL( m.type(id) , m.get(id).type() );
      }

      switch (m.type(id))
      {
        case pbart::Type::BOOL:        CPPUNIT_ASSERT_EQUAL(empty,*eb<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(          b ,m.get(id).toBool  ()); inc(); } break;
        case pbart::Type::LONG:        CPPUNIT_ASSERT_EQUAL(empty,*el<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(          l ,m.get(id).toLong  ()); inc(); } break;
        case pbart::Type::DOUBLE:      CPPUNIT_ASSERT_EQUAL(empty,*ed<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(          d ,m.get(id).toDouble()); inc(); } break;
        case pbart::Type::STRING:      CPPUNIT_ASSERT_EQUAL(empty,*es<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(          s ,m.get(id).toString()); inc(); } break;
        case pbart::Type::MESSAGE:     CPPUNIT_ASSERT_EQUAL(empty,*em<0); if(!empty) { check_allowed       (             m.get(id).toMessage());       } break;

        case pbart::Type::SEQ_BOOL:    CPPUNIT_ASSERT_EQUAL(empty,*eb<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(size_t(*eb),m.get(id).toSeqBool   ().size()); for(int i=0;i<*eb;i++) { CPPUNIT_ASSERT_EQUAL(b,m.get(id).toBool  (i)); inc(); } } break;
        case pbart::Type::SEQ_LONG:    CPPUNIT_ASSERT_EQUAL(empty,*el<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(size_t(*el),m.get(id).toSeqLong   ().size()); for(int i=0;i<*el;i++) { CPPUNIT_ASSERT_EQUAL(l,m.get(id).toLong  (i)); inc(); } } break;
        case pbart::Type::SEQ_DOUBLE:  CPPUNIT_ASSERT_EQUAL(empty,*ed<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(size_t(*ed),m.get(id).toSeqDouble ().size()); for(int i=0;i<*ed;i++) { CPPUNIT_ASSERT_EQUAL(d,m.get(id).toDouble(i)); inc(); } } break;
        case pbart::Type::SEQ_STRING:  CPPUNIT_ASSERT_EQUAL(empty,*es<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(size_t(*es),m.get(id).toSeqString ().size()); for(int i=0;i<*es;i++) { CPPUNIT_ASSERT_EQUAL(s,m.get(id).toString(i)); inc(); } } break;
        case pbart::Type::SEQ_MESSAGE: CPPUNIT_ASSERT_EQUAL(empty,*em<0); if(!empty) { CPPUNIT_ASSERT_EQUAL(size_t(*em),m.get(id).toSeqMessage().size()); for(int i=0;i<*em;i++) { check_allowed (        m.get(id).toMessage(i));       } } break;

        default:
          CPPUNIT_ASSERT (0 && "Unexpected it.type()");
      }
    }
  }

//----------------------------------------------------------------------------
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
#   define CHK_EMPTY(ex) CPPUNIT_ASSERT_EQUAL(it.empty(),*ex<0)
#else
#   define CHK_EMPTY(ex) CPPUNIT_ASSERT_EQUAL(it.empty(),*ex<=0)
#endif
//----------------------------------------------------------------------------
  void count (const pbart::Message& m)
  {
    nm++;

    for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
    {
      pbart::Type t = it.type();
      if (! it.empty())
      {
        CPPUNIT_ASSERT_EQUAL (it.type(), it->type());
      }

      switch (it.type())
      {
        case pbart::Type::BOOL:    CPPUNIT_ASSERT_EQUAL(it.empty(),*eb<0); if(!it.empty()) nb++; else ne++; break;
        case pbart::Type::LONG:    CPPUNIT_ASSERT_EQUAL(it.empty(),*el<0); if(!it.empty()) nb++; else ne++; break;
        case pbart::Type::DOUBLE:  CPPUNIT_ASSERT_EQUAL(it.empty(),*ed<0); if(!it.empty()) nb++; else ne++; break;
        case pbart::Type::STRING:  CPPUNIT_ASSERT_EQUAL(it.empty(),*es<0); if(!it.empty()) nb++; else ne++; break;

        case pbart::Type::MESSAGE:
          CPPUNIT_ASSERT_EQUAL (it.empty(), (*em<0));
          if (it.empty())  ne++;
          else count (it->toMessage());
          break;

        case pbart::Type::SEQ_BOOL:    CHK_EMPTY(eb); if(it.empty()) ne++; else nb+=it->toSeqBool  ().size(); break;
        case pbart::Type::SEQ_LONG:    CHK_EMPTY(el); if(it.empty()) ne++; else nb+=it->toSeqLong  ().size(); break;
        case pbart::Type::SEQ_DOUBLE:  CHK_EMPTY(ed); if(it.empty()) ne++; else nb+=it->toSeqDouble().size(); break;
        case pbart::Type::SEQ_STRING:  CHK_EMPTY(es); if(it.empty()) ne++; else nb+=it->toSeqString().size(); break;
        case pbart::Type::SEQ_MESSAGE: CHK_EMPTY(em);
          if (it.empty())
            ne++;
          else
            for (size_t i=0; i < it->toSeqMessage().size(); i++)
              count (it->toMessage(i));
          break;

        default:
          CPPUNIT_ASSERT (0 && "Unexpected it.type()");
      }
    }
  }

  //--------------------------------------------------------------------------
  void erase (pbart::Message& m)
  {
    int count = 0;
    for (pbart::Message::iterator it = m.begin(); it != m.end(); ++it)
    {
      m.erase(it);
      count++;
    }
  }

  //--------------------------------------------------------------------------
  unsigned long long ticks()
  {
#ifdef _MSC_VER
    LARGE_INTEGER time;
    QueryPerformanceCounter (&time);
    return time.QuadPart;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC,&ts) != 0)   std::cerr << "ERROR in clock_gettime()\n";
    return 1000*1000*1000 * ts.tv_sec + ts.tv_nsec;
#endif
  }

  //--------------------------------------------------------------------------
  pbart::Message Init (std::ostream& oss, pbart::Id id)
  {
    reset();
    pbart::Message m = init(id);

    count(m); // set variables: 'nb' 'nm' 'ne' (output)

    loops = LOOPS / (nb+nm+ne)  +  1;
    assert(loops >= 1);

    oss << m.name() <<" Loops="<< loops <<"\t" "Fields="<< nb <<"\t" "Msg="<< nm <<"\t" "Empty="<< ne;

    if (*eb>=0 || *el>=0 || *ed>=0 || *es>=0 || *em>=0)
    {
      oss <<"\t" "Enable";
      if (*eb>=0)   {   oss <<"-Bool";     if(*eb>0) oss<<'['<< *eb <<']';   }
      if (*el>=0)   {   oss <<"-Long";     if(*el>0) oss<<'['<< *el <<']';   }
      if (*ed>=0)   {   oss <<"-Double";   if(*ed>0) oss<<'['<< *ed <<']';   }
      if (*es>=0)   {   oss <<"-Str";      if(*es>0) oss<<'['<< *es <<']';   }
      if (*em>=0)   {   oss <<"-Msg";      if(*em>0) oss<<'['<< *em <<']';   }
    }

    if (*eb<0 || *el<0 || *ed<0 || *es<0 || *em<0)
    {
      oss <<"\t" "Disable";
      if (*eb<0)    {   oss <<"-Bool";     }
      if (*el<0)    {   oss <<"-Long";     }
      if (*ed<0)    {   oss <<"-Double";   }
      if (*es<0)    {   oss <<"-Str";      }
      if (*em<0)    {   oss <<"-Msg";      }
    }

    oss   <<'\n';

    return m;
  }

  //--------------------------------------------------------------------------
#ifndef  DUMP_BUFFER_IN_FILES
  void dump (const pbart::Message*        , const std::string    , const char*)
  {
  }
#else
  void dump (const pbart::Message* message, const std::string str, const char coding[])
  {
    static int count = 0;

    std::stringstream oss;
    oss << std::setw(4) << std::setfill('_')  << count++ <<'_'<< coding
        << std::setw(7) << message->name()
        <<"_id"<< std::setfill('-') << std::setw(3) << -message->id()
        <<"_size"<< std::setw(7) << str.size() <<'.'
        << (*eb>=0 ? (*eb==0 ? 'b' : 'B') : '-')
        << (*el>=0 ? (*el==0 ? 'l' : 'L') : '-')
        << (*ed>=0 ? (*ed==0 ? 'd' : 'D') : '-')
        << (*es>=0 ? (*es==0 ? 's' : 'S') : '-')
        << (*em>=0 ? (*em==0 ? 'm' : 'M') : '-') <<'.';

#  ifdef _DEBUG
    oss <<'D';
#  else
    oss <<'R';
#  endif

#  ifdef NDEBUG
    oss <<'N';
#  else
    oss <<'A';
#  endif

    oss <<".pbart";
    std::ofstream file (oss.str().c_str());
    file << str;
  }
#endif

  //--------------------------------------------------------------------------
  void timings (std::ostream& oss, double us)
  {
    CPPUNIT_ASSERT (nm > 0);
    if (us < 100*1000)  oss << us                 <<" us"             "\t";
    else                oss << us/1000/1000       <<" sec"            "\t";
    oss          << us * 1000 / loops             <<" ns per loop"    "\t";
    if(nm-1) oss << us * 1000 / loops / (   nm-1) <<" ns per message" "\t";
    if(nb)   oss << us * 1000 / loops / (nb+nm-1) <<" ns per field";
    oss <<'\n';
  }

  //--------------------------------------------------------------------------
  pbart::Buffer Encode (std::ostream& oss, const pbart::Message& m)
  {
    pbart::Buffer buffer;

    unsigned long long start = ticks();

    for (int i = 0; i < loops; ++i)
    {
      buffer.clear();
      serializer.encode (m, buffer);
    }

    unsigned long long stop  = ticks();
    unsigned long long after = ticks();
    unsigned long long overhead = (after - stop);
    unsigned long long elapsed  = (stop  - start) - overhead;
    double us = double(elapsed) / ratio;

    oss << m.name() <<" Encode = ";
    timings(oss,us);
    dump ( &m, buffer, "Encode" );

    return buffer;
  }

  //--------------------------------------------------------------------------
  pbart::Message Decode (std::ostream& oss, const pbart::Buffer& buffer)
  {
    pbart::Message m = serializer.message();

    dump (&m, buffer, "Decode"); //id = 0

    unsigned long long start = ticks();

    for (int i = 0; i < loops; ++i)
    {
      serializer.decode (buffer, m);
    }

    unsigned long long stop = ticks();
    unsigned long long after = ticks();
    unsigned long long overhead = (after - stop);
    unsigned long long elapsed  = (stop  - start) - overhead;
    double us = double(elapsed) / ratio;

    oss << m.name() <<" Decode = ";
    timings(oss,us);
    //dump(&m, buffer, "Decode"); //id is correct

    return m;
  }

  //--------------------------------------------------------------------------
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
  void verify (const pbart::Message& m)
  {
    reset();   check_allowed   (m);
    reset();   check_classic   (m);
    reset();   check_recursive (m);
  }
#else
  void verify (const pbart::Message&)
  {
  }
#endif

  //--------------------------------------------------------------------------
  bool duplicated (const pbart::Message& m)
  {
    static size_t const SIZE  = 33;
    static size_t       index = 0;
    static pbart::Message old [SIZE];

    for (size_t i=0; i<SIZE; i++)
      if (m == old[i])
        return true; //avoid doing again the same test

    old[index] = m;
    if (++index == SIZE)
      index = 0;

    return false;
  }

  //--------------------------------------------------------------------------
  template <typename OS>
  void bench (OS& os, pbart::Id id)
  {
    std::ostringstream osI, osE, osD;

    const  pbart::Message  m1 = Init (osI, id);

//#ifdef CHECK_ALL_POSSIBILITIES
    if (duplicated(m1))
      return; //avoid doing again the same test
//#endif

    os <<"---"  "\n" "#"<< osI.str();
    std::cout <<'\n'    << osI.str();

    const  pbart::Buffer  pb = Encode (osE, m1);
    std::cout <<      osE.str();
    os        <<'#'<< osE.str();

    const  pbart::Message m2 = Decode (osD, pb);
    std::cout <<      osD.str();
    os        <<'#'<< osD.str();

    std::cout << "Protocol buffer = "<< pb.size() <<" bytes" "\n";
    os        <<"#Protocol buffer = "<< pb.size() <<" bytes" "\n";
    os        << m1;


    pbart::Message        ma = pbart::Message(m1);
    pbart::Message const& mb = pbart::Message(m1);
    CPPUNIT_ASSERT_EQUAL (ma, mb);

#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    const pbart::Message& m3 = pbart::Message(m1);
#else
    pbart::Message        m3 = pbart::Message(m1);
    m3.trim();
#endif

    CPPUNIT_ASSERT_EQUAL (m3, m2);
    verify (m3);

    pbart::Message m4 = m3;
    erase (m4);
    pbart::Buffer  b3 = serializer.encode (m4);
    pbart::Message m5 = serializer.decode (b3);
    CPPUNIT_ASSERT_EQUAL( m5.id(), m1.id() );
    CPPUNIT_ASSERT_EQUAL( m5.size(), 0 );
    // TODO check m4 == m3
  }

  //--------------------------------------------------------------------------
  void bench (const char name[])
  {
    pbart::Id id = dico->id(name);

#define END( SEQ_XXX)  ((SEQ_XXX) + sizeof(SEQ_XXX)/sizeof(*SEQ_XXX))


    const ssize_t maxEB      = 35;
    const ssize_t SEQ_BOOL[] = { 1, -1, 0, 4, maxEB };  // in range [4 .. 32] => booleans are stored as bits (bitset)
    const ssize_t SEQ_LONG[] = { 1, -1, 0           };
    const ssize_t SEQ_DBL [] = { 1, -1, 0           };
    const ssize_t SEQ_STR [] = { 1, -1, 0           };
    const ssize_t SEQ_MSG [] = { 1, -1, 0           };


    for         (eb=SEQ_BOOL; eb!=END(SEQ_BOOL); ++eb)
#ifdef CHECK_ALL_POSSIBILITIES
      for       (el=SEQ_LONG; el!=END(SEQ_LONG); ++el)
        for     (ed=SEQ_DBL;  ed!=END(SEQ_DBL);  ++ed)
          for   (es=SEQ_STR;  es!=END(SEQ_STR);  ++es)
            for (em=SEQ_MSG;  em!=END(SEQ_MSG);  ++em)
            {
#else
            {
              if (*eb == maxEB)
                el = ed = es = em = SEQ_BOOL;
              else
                el = ed = es = em = eb;
#endif
              bench (file, id);
            }
  }

  //--------------------------------------------------------------------------
  void Booleans()                                   {  bench ("MsgHas");  }
  void Longs()                                      {  bench ("MsgQty");  }
  void Doubles()                                    {  bench ("MsgPrc");  }
  void Strings()                                    {  bench ("MsgStr");  }
  void Sequences_Of_Bool()                          {  bench ("SeqHas");  }
  void Sequences_Of_Long()                          {  bench ("SeqQty");  }
  void Sequences_Of_Double()                        {  bench ("SeqPrc");  }
  void Sequences_Of_String()                        {  bench ("SeqStr");  }
  void Messages_Of_SingleTypes()                    {  bench ("MsgMsg");  }
  void Messages_Of_SequencesOfSingleBasicTypes()    {  bench ("MsgSeq");  }
  void Sequences_Of_MessagesOfSingleBasicTypes()    {  bench ("SeqMsg");  }
  void Sequences_Of_MessagesOfBasicTypeSequences()  {  bench ("SeqSeq");  }
  void SingleTypes_Of_AboveFieldsAndMessages()      {  bench ("MsgAll");  }
  void Sequences_Of_AboveFieldsAndMessages()        {  bench ("SeqAll");  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Perf );
