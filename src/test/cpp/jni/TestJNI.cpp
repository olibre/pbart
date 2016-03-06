#include GENERATED_JNI_SERIALIZER_HEADER
#include GENERATED_JNI_MESSAGE_HEADER
#include <pbart/Types.h>

#include <cppunit/extensions/HelperMacros.h>

#ifdef _MSC_VER
#  include <windows.h>
#endif


class Perf : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( Perf );
    /*
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
*/
    CPPUNIT_TEST_SUITE_END();

    jlong    serializer;

    jint has[10];
    jint qty[10];
    jint prc[10];
    jint str[10];

public:

    //--------------------------------------------------------------------------------
    char    s[1024];
    bool    b;
    long    l;
    double  d;
    size_t  strlen;
    size_t  tablen;
    size_t  min;
    size_t  max;
    double  ratio;
    int     loops;

    JNINativeInterface_ functions;
    JNIEnv_             env;

    //--------------------------------------------------------------------------------
    void setUp()
    {
        loops  = 5*1000;
        tablen = 5; //table length = size of sequence = number of repeated elements
        min    = 2;
        max    = 40; //less than 1024

#ifdef _MSC_VER
        LARGE_INTEGER freq;
        QueryPerformanceFrequency (&freq);
        ratio = (double) freq.QuadPart / 1000;
#else
        ratio = 1000;
#endif

        env.functions = &functions;

        serializer = Java_org_Lmap_pbart_Serializer_native_1create (&env, NULL);

        jstring jstr = 0; //"../../../../../../../cpp/resources/dico-Perf.xml" = env->NewStringUTF (name.c_str());
        Java_org_Lmap_pbart_Serializer_native_1loadDico (&env, NULL, serializer, jstr);

        for (int i=0; i<10; i++)   has[i] = Java_org_Lmap_pbart_Serializer_native_1id(&env, NULL, serializer, cat("Has",i) );
        for (int i=0; i<10; i++)   qty[i] = Java_org_Lmap_pbart_Serializer_native_1id(&env, NULL, serializer, cat("Qty",i) );
        for (int i=0; i<10; i++)   prc[i] = Java_org_Lmap_pbart_Serializer_native_1id(&env, NULL, serializer, cat("Prc",i) );
        for (int i=0; i<10; i++)   str[i] = Java_org_Lmap_pbart_Serializer_native_1id(&env, NULL, serializer, cat("Str",i) );
    }

    //--------------------------------------------------------------------------------
    void tearDown()
    {
        Java_org_Lmap_pbart_Serializer_native_1destroy (&env, NULL, serializer);
    }

    //--------------------------------------------------------------------------------
    jstring cat (const std::string& str, int i)
    {
        char digit = '0' + char(i);
        char nb[] = { digit, '\0' }; // conversion: digit to null terminated string
        return jstring();
    }

    //--------------------------------------------------------------------------------
    void reset()
    {
        b    = true;
        l    = 1;
        d    = 2.0;

        for (int i=0; i<strlen-1; ++i)
            s[i] = 'A';
        s[strlen-1] = 0;
    }

    //--------------------------------------------------------------------------------
    jlong init (jint id)
    {
        jlong m = Java_org_Lmap_pbart_Serializer_native_1message (&env, NULL, serializer, id);

        for (jint item=-100; item<100; ++item)
        {
            if (! Java_org_Lmap_pbart_Message_native_1isEmpty (&env, NULL, serializer, id))
#ifndef DO_NOT_COMPILE
                ;
#else
                switch (it.typeSeq())
                {
                case pbart::Type::BOOL:     m.set (item, b);  b=!b;    break;
                case pbart::Type::LONG:     m.set (item, l);  l++;     break;
                case pbart::Type::DOUBLE:   m.set (item, d);  d++;     break;
                case pbart::Type::STRING:   m.set (item, s);  s[0]++;  break;
                case pbart::Type::MESSAGE:  m.set (item, init(item));  break;

                case pbart::Type::SEQ_BOOL:    m.set(item,pbart::Variant::SeqBool());    for(int i=0;i<tablen;i++) { m.get(item).toSeqBool   ().push_back(b);  b=!b;   } break;
                case pbart::Type::SEQ_LONG:    m.set(item,pbart::Variant::SeqLong());    for(int i=0;i<tablen;i++) { m.get(item).toSeqLong   ().push_back(i);  l++;    } break;
                case pbart::Type::SEQ_DOUBLE:  m.set(item,pbart::Variant::SeqDouble());  for(int i=0;i<tablen;i++) { m.get(item).toSeqDouble ().push_back(d);  d++;    } break;
                case pbart::Type::SEQ_STRING:  m.set(item,pbart::Variant::SeqString());  for(int i=0;i<tablen;i++) { m.get(item).toSeqString ().push_back(s);  s[0]++; } break;
                case pbart::Type::SEQ_MESSAGE: m.set(item,pbart::Variant::SeqMessage()); for(int i=0;i<tablen;i++) { m.get(item).toSeqMessage().push_back(init(item)); } break;
                }
#endif
        }

        return m;
    }

    //--------------------------------------------------------------------------------
    void check (jlong m)
    {
#ifdef DO_NOT_COMPILE
        for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            switch (it->typeSeq())
            {
            case pbart::Type::BOOL:    CPPUNIT_ASSERT_EQUAL( it->toBool()    , b            ); b=!b;   break;
            case pbart::Type::LONG:    CPPUNIT_ASSERT_EQUAL( it->toLong()    , l            ); l++;    break;
            case pbart::Type::DOUBLE:  CPPUNIT_ASSERT_EQUAL( it->toDouble()  , d            ); d++;    break;
            case pbart::Type::STRING:  CPPUNIT_ASSERT_EQUAL( it->toString()  , s            ); s[0]++; break;
            case pbart::Type::MESSAGE: CPPUNIT_ASSERT_EQUAL( it->toMessage() , init(it.id()));         break;

            case pbart::Type::SEQ_BOOL:    for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toBool   (i) , b    ); b=!b;   } break;
            case pbart::Type::SEQ_LONG:    for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toLong   (i) , i    ); l++;    } break;
            case pbart::Type::SEQ_DOUBLE:  for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toDouble (i) , d    ); d++;    } break;
            case pbart::Type::SEQ_STRING:  for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toString (i) , s    ); s[0]++; } break;
            case pbart::Type::SEQ_MESSAGE: for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toMessage(i) , init(it.id())); } break;
            }
        }
#endif
    }

    //--------------------------------------------------------------------------------
    void recursive (jlong m)
    {
#ifdef DO_NOT_COMPILE
        for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            switch (it->typeSeq())
            {
            case pbart::Type::BOOL:    CPPUNIT_ASSERT_EQUAL( it->toBool()    , b ); b=!b;   break;
            case pbart::Type::LONG:    CPPUNIT_ASSERT_EQUAL( it->toLong()    , l ); l++;    break;
            case pbart::Type::DOUBLE:  CPPUNIT_ASSERT_EQUAL( it->toDouble()  , d ); d++;    break;
            case pbart::Type::STRING:  CPPUNIT_ASSERT_EQUAL( it->toString()  , s ); s[0]++; break;
            case pbart::Type::MESSAGE: recursive (it->toMessage());                    break;

            case pbart::Type::SEQ_BOOL:    for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toBool   (i) , b ); b=!b;   } break;
            case pbart::Type::SEQ_LONG:    for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toLong   (i) , i ); l++;    } break;
            case pbart::Type::SEQ_DOUBLE:  for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toDouble (i) , d ); d++;    } break;
            case pbart::Type::SEQ_STRING:  for(int i=0;i<tablen;i++) { CPPUNIT_ASSERT_EQUAL( it->toString (i) , s ); s[0]++; } break;
            case pbart::Type::SEQ_MESSAGE: for(int i=0;i<tablen;i++) { recursive (it->toMessage(i));                    } break;
            }
        }
#endif
    }

    //--------------------------------------------------------------------------------
    int count (jlong m)
    {
        int nb = 0;

#ifdef DO_NOT_COMPILE
        for (pbart::Message::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            switch (it->typeSeq())
            {
            case pbart::Type::BOOL:
            case pbart::Type::LONG:
            case pbart::Type::DOUBLE:
            case pbart::Type::STRING:
                nb++;
                break;

            case pbart::Type::MESSAGE:
                nb += count (it->toMessage());
                break;

            case pbart::Type::SEQ_BOOL:    nb += it->toSeqBool  ().size(); break;
            case pbart::Type::SEQ_LONG:    nb += it->toSeqLong  ().size(); break;
            case pbart::Type::SEQ_DOUBLE:  nb += it->toSeqDouble().size(); break;
            case pbart::Type::SEQ_STRING:  nb += it->toSeqString().size(); break;

            case pbart::Type::SEQ_MESSAGE:
                for (size_t i=0; i < it->toSeqMessage().size(); i++)
                    nb += count (it->toMessage(i));
                break;
            }
        }
#endif

        return nb;
    }

    //--------------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------------
    jlong Init (jint id)
    {
        reset();
        jlong msg = init (id);

        int nb = count (msg);

        jstring jstr = Java_org_Lmap_pbart_Message_native_1getName__J (&env, NULL, msg);
        std::string name; //TODO convert jstr to std::string

        std::cout <<'\n'<< name <<" Loops="<< loops <<"\tAttributes="<< nb <<"\tChars="<< strlen <<" \tRepeated="<< tablen<<'\n';

        return msg;
    }

    //--------------------------------------------------------------------------------
    std::string Encode (jlong m)
    {
        std::string dynMsg;

        unsigned long long start = ticks();

#ifdef DO_NOT_COMPILE
        for (int i = 0; i < loops; ++i)
            m.encode (dynMsg);
#endif

        unsigned long long stop  = ticks();
        unsigned long long after = ticks();
        unsigned long long overhead = (after - stop);
        unsigned long long elapsed  = (stop  - start) - overhead;
        double us = double(elapsed) / ratio;

        int nb = count (m);

        std::string name; //TODO ' = m.name()'

        std::cout << name <<
                     " Encode = "<< us <<" us"                         "\t"
                                         "("<< us * 1000 / loops     <<" ns per message) " "\t"
                                                                       "("<< us * 1000 / loops /nb <<" ns per field)"    "\n";

        return dynMsg;
    }

    //--------------------------------------------------------------------------------
    jlong Decode (const std::string& dynMsg)
    {
        jlong m = Java_org_Lmap_pbart_Serializer_native_1create (&env, NULL); //serializer->message();

        unsigned long long start = ticks();

#ifdef DO_NOT_COMPILE
        for (int i = 0; i < loops; ++i)
            m.decode (dynMsg);
#endif

        unsigned long long stop = ticks();
        unsigned long long after = ticks();
        unsigned long long overhead = (after - stop);
        unsigned long long elapsed  = (stop  - start) - overhead;
        double us = double(elapsed) / ratio;

        int nb = count (m);

        std::string name; //TODO ' = m.name()'

        std::cout << name <<
                     " Decode = "<< us <<" us"                         "\t"
                                         "("<< us * 1000 / loops     <<" ns per message) " "\t"
                                                                       "("<< us * 1000 / loops /nb <<" ns per field)"    "\n";

        return m;
    }

    //--------------------------------------------------------------------------------
    void verify (jlong m)
    {
        reset();
        check (m);

        reset();
        recursive (m);
    }

    //--------------------------------------------------------------------------------
    void bench (jint id, size_t len)
    {
        strlen = len;

        const jlong        m1   =  Init   (id);
        const std::string  buf  =  Encode (m1);
        const jlong        m2   =  Decode (buf);

        verify (m1);
        verify (m2);
    }

    //--------------------------------------------------------------------------------
    void bench (const char name[])
    {
        jint id;
#ifdef DO_NOT_COMPILE
        id = dico->id(name);
#endif
        bench (id, min);
        bench (id, max);
    }

    //--------------------------------------------------------------------------------
    void Booleans()                                    {  bench ("MsgHas");  }
    void Longs()                                       {  bench ("MsgQty");  }
    void Doubles()                                     {  bench ("MsgPrc");  }
    void Strings()                                     {  bench ("MsgStr");  }
    void Sequences_Of_Bool()                           {  bench ("SeqHas");  }
    void Sequences_Of_Long()                           {  bench ("SeqQty");  }
    void Sequences_Of_Double()                         {  bench ("SeqPrc");  }
    void Sequences_Of_String()                         {  bench ("SeqStr");  }
    void Messages_Of_SingleTypes()                     {  bench ("MsgMsg");  }
    void Messages_Of_SequencesOfSingleBasicTypes()     {  bench ("MsgSeq");  }
    void Sequences_Of_MessagesOfSingleBasicTypes()     {  bench ("SeqMsg");  }
    void Sequences_Of_MessagesOfBasicTypeSequences()   {  bench ("SeqSeq");  }
    void SingleTypes_Of_AboveFieldsAndMessages()       {  bench ("MsgAll");  }
    void Sequences_Of_AboveFieldsAndMessages()         {  bench ("SeqAll");  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Perf );
