package org.Lmap.pbart;

import static org.junit.Assert.*;
import        org.junit.Test;
import        java.util.Date;


public class TestPerf {

    private int      loops   = 1;
    private int      tablen  = 5;
    private int      minlen  = 2;
    private int      maxlen  = 40;
    private boolean  b       = true;
    private long     l       = 1;
    private Date     t       = new Date(0);
    private double   d       = 2;
    private String   s;
    private static int count = 0; //used in filename (of protobuf dump)

    private void reset (int len) {
        b = true;
        l = 1;
        t = new Date(0);
          d = 2.0;
          s = "";
          for (int i=0; i<len; i++)  s+='A';
    }

    private void inc() {
        t.setTime( t.getTime() + 365*24*3600*1000 + 1 );
        if (!s.isEmpty()) {
            char c = s.charAt(0);
            if (c == '~')
                c = ' ';
            else
                c++;
            int len = s.length();
            s = "";
            for (int i = 0; i < len; i++)
                s += c;
        }
    }

    private Serializer  serializer;
    @Test  public void Booleans()                                  throws DicoException, CodecException { bench("MsgHas"); }
    @Test  public void Longs()                                     throws DicoException, CodecException { bench("MsgQty"); }
    @Test  public void Doubles()                                   throws DicoException, CodecException { bench("MsgPrc"); }
    @Test  public void Strings()                                   throws DicoException, CodecException { bench("MsgStr"); }
    @Test  public void Sequences_Of_Bool()                         throws DicoException, CodecException { bench("SeqHas"); }
    @Test  public void Sequences_Of_Long()                         throws DicoException, CodecException { bench("SeqQty"); }
    @Test  public void Sequences_Of_Double()                       throws DicoException, CodecException { bench("SeqPrc"); }
    @Test  public void Sequences_Of_String()                       throws DicoException, CodecException { bench("SeqStr"); }
    @Test  public void Messages_Of_SingleTypes()                   throws DicoException, CodecException { bench("MsgMsg"); }
    @Test  public void Messages_Of_SequencesOfSingleBasicTypes()   throws DicoException, CodecException { bench("MsgSeq"); }
    @Test  public void Sequences_Of_MessagesOfSingleBasicTypes()   throws DicoException, CodecException { bench("SeqMsg"); }
    @Test  public void Sequences_Of_MessagesOfBasicTypeSequences() throws DicoException, CodecException { bench("SeqSeq"); }
    @Test  public void SingleTypes_Of_AboveFieldsAndMessages()     throws DicoException, CodecException { bench("MsgAll"); }
    @Test  public void Sequences_Of_AboveFieldsAndMessages()       throws DicoException, CodecException { bench("SeqAll"); }

    public void bench (String name) throws DicoException, CodecException {
        serializer = new Serializer();
        serializer.loadDico("../cpp/unit-test/data/dico-Perf.xml");
        int id = serializer.getId (name);
        bench (id, minlen);
        bench (id, maxlen);
    }

    public void bench (int id, int len) throws CodecException {
        reset (len);
        Message m1 = init (id);
        byte[]  pb = encode (m1);
        Message m2 = decode (pb);

        reset (len);
        verify (m1);
        reset (len);
        verify (m2);
    }

    public Message init (int id) {
        Message msg = serializer.createEmptyMessage(id);
        assertEquals(0,msg.check());
        for (int item=-1; item<1000; item++) {
            if (msg.isAllowed(item)) {
                assertTrue(msg.isEmpty(item));
                assertEquals(0,msg.check(item));
                switch (msg.getType(item))
                {
                case BOOL:                                    msg.setBool      (item,b);   b=!b;     break;
                case LONG:                                    msg.setLong      (item,l);   l++;      break;
                case DOUBLE:                                  msg.setDouble    (item,d);   d++;      break;
                case STRING:                                  msg.setString    (item,s);   inc();    break;
                case MESSAGE:                                 msg.setMessage   (item,init(item));    break;
                case SEQ_BOOL:    for(int i=0;i<tablen;i++) { msg.setBool   (item,i,b); b=!b;   } break;
                case SEQ_LONG:    for(int i=0;i<tablen;i++) { msg.setLong   (item,i,l); l++;    } break;
                case SEQ_DOUBLE:  for(int i=0;i<tablen;i++) { msg.setDouble (item,i,d); d++;    } break;
                case SEQ_STRING:  for(int i=0;i<tablen;i++) { msg.setString (item,i,s); inc();  } break;
                case SEQ_MESSAGE: for(int i=0;i<tablen;i++) { msg.setMessage(item,i,init(item));} break;
                case EMPTY: fail("Type cannot be EMPTY");  break;
                default:    fail("Unexpected type value"); break;
                }
            }
        }
        return msg;
    }

    public void verify (Message msg) {
        assertEquals(0,msg.check());
        for (int id=-1; id<1000; id++) {
            if (msg.isAllowed(id)) {
                assertFalse(msg.isEmpty(id));
                assertEquals(0,msg.check(id));
                switch (msg.getType(id))
                {
                case BOOL:                                    assertEquals(b,msg.getBool     (id)  ); b=!b;     break;
                case LONG:                                    assertEquals(l,msg.getLong     (id)  ); l++;      break;
                case DOUBLE:                                  assertEquals(d,msg.getDouble   (id),0); d++;      break;
                case STRING:                                  assertEquals(s,msg.getString   (id)  ); inc();    break;
                case MESSAGE:                                 verify(msg.cloneMessage(id));                     break;
                case SEQ_BOOL:    for(int i=0;i<tablen;i++) { assertEquals(b,msg.getBool  (id,i)  ); b=!b; } break;
                case SEQ_LONG:    for(int i=0;i<tablen;i++) { assertEquals(l,msg.getLong  (id,i)  ); l++;  } break;
                case SEQ_DOUBLE:  for(int i=0;i<tablen;i++) { assertEquals(d,msg.getDouble(id,i),0); d++;  } break;
                case SEQ_STRING:  for(int i=0;i<tablen;i++) { assertEquals(s,msg.getString(id,i)  ); inc();} break;
                case SEQ_MESSAGE: for(int i=0;i<tablen;i++) { verify(msg.cloneMessage   (id,i)    );       } break;
                case EMPTY: fail("Type cannot be EMPTY");  break;
                default:    fail("Unexpected type value"); break;
                }
            }
        }
    }

    public byte[] encode (Message msg) throws CodecException {
        System.out.print(count++);
        System.out.print('\t');
        System.out.println(msg.getId());

        byte[] buffer = null;
        for (int i = 0; i < loops; ++i)
            buffer = serializer.encode (msg);
        return  buffer;
    }

    public Message decode (byte[] buffer) throws CodecException {
        count++;
        Message msg = serializer.createEmptyMessage();
        for (int i = 0; i < loops; ++i)
            msg = serializer.decode (buffer);
        return  msg;
    }
}
