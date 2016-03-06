package org.Lmap.pbart;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertEquals;
import        org.junit.Test;

//import org.junit.AfterClass;
//import org.junit.BeforeClass;
import java.util.*;
import java.util.Calendar;

public class TestSimple {

    private Serializer serializer;

    @Test
    public void init() throws DicoException {
        serializer = new Serializer();
        serializer.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                + "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                + "<FieldsDef>"
                + "  <FieldDef id='1' name='Qty'    type='long'   deprecated='false' comment='Volume'/>"
                + "  <FieldDef id='3' name='Prc'    type='double' deprecated='false' comment='Price' />"
                + "  <FieldDef id='4' name='isSent' type='bool'   deprecated='false' comment='is on market'/>"
                + "  <FieldDef id='5' name='Reason' type='string' deprecated='false' comment='Reject msg' />"
                + "  <FieldDef id='2' name='TZinfo' type='string' deprecated='true'  comment='always UTC' />"
                + "  <FieldDef id='9' name='Date'   type='DOUBLE' deprecated='true'  comment='always UTC' />"
                + "</FieldsDef>"
                + "<MessagesDef>"
                + "  <MessageDef id='91' name='client.cash.StOrder' deprecated='false' comment='basic order'>"
                + "    <Field name='Prc' default='0.1' required='false' repeated='false' />"
                + "    <Field name='Qty' default='132' required='true'  repeated='false'  />"
                + "  </MessageDef>"
                + "  <MessageDef id='99' name='date' deprecated='false' comment='basic order'>"
                + "    <Field name='Date' default='1970'  required='true'  repeated='false'  />"
                + "  </MessageDef>"
                + "  <MessageDef id='93' name='StEmpty' deprecated='false' comment='for test'>"
                + "    <Field   name='isSent'              default='true'     required='true'  repeated='false' />"
                + "    <Message name='client.cash.StOrder'                    required='true'  repeated='false' />"
                + "    <Field   name='Reason'              default='I decide' required='false' repeated='false' />"
                + "  </MessageDef>" + "</MessagesDef>" + "</Dico>");
    }

    @Test(expected = DicoException.class)
    public void badDico() throws DicoException {
        serializer = new Serializer();
        serializer.parseDico("<?xml version='1.0' encoding='UTF-8' ?>"
                + "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                + "<FieldsDef>"
                + "  <FieldDef id='1' name='Qty' type='long'   deprecated='false' comment='Volume'/>"
                + "  <FieldDef id='1' name='Prc' type='double' deprecated='false' comment='Price' />"
                + "</FieldsDef></Dico>");
    }

    @Test
    public void emptyMessage() throws DicoException {
        init();
        Message msg = serializer.createEmptyMessage(93);
        int id = serializer.getId(msg.getName());
        assertTrue(id == msg.getId());
    }

    @Test
    public void getId() throws DicoException {
        init();
        int m = serializer.getId("client.cash.StOrder");
        int f = serializer.getId("Prc");
        assertTrue(m == 91);
        assertTrue(f == 3);
    }

    @Test
    public void getName() throws DicoException {
        init();
        int f = serializer.getId("Prc");
        String name = serializer.getName(f);
        assertTrue("Prc".equals(name));
    }

    @Test //(expected = IdException.class)
    public void getNameEmpty() throws DicoException {
        init();
        String name = serializer.getName(35468);
        assertTrue("".equals(name));
    }

    @Test(expected = TypeException.class)
    public void badType() throws DicoException {
        init();
        int m = serializer.getId("client.cash.StOrder");
        int f = serializer.getId("Prc");
        Message msg = serializer.createEmptyMessage(m);
        msg.setLong(f, 3);
    }

    @Test
    public void goodType() throws DicoException {
        init();
        int m = serializer.getId("client.cash.StOrder");
        int f = serializer.getId("Prc");
        Message msg = serializer.createEmptyMessage(m);
        Type t = msg.getType(f);
        assertTrue(t == Type.DOUBLE);
        msg.setDouble(f, 3.1415);
        assertTrue(msg.getId() == m);
        assertTrue("client.cash.StOrder".equals(msg.getName()));
    }

    @Test
    public void yaml() throws DicoException {
        init();
        int m = serializer.getId("client.cash.StOrder");
        int f = serializer.getId("Prc");
        Message msg = serializer.createEmptyMessage(m);
        msg.setDouble(f, 3.1415);
        String str = msg.toString();
        //System.out.println(msg.getName());
        //System.out.println(str);
        String reference
                = "client.cash.StOrder:" + "\n"
                + "  id:    91" + "\n"
                + "  Qty:" + "\n"
                + "    id:    1" + "\n"
                + "    type:  LONG" + "\n"
                + "  Prc:" + "\n"
                + "    id:    3" + "\n"
                + "    type:  DOUBLE" + "\n"
                + "    value: 3.1415" + "\n";
        assertTrue(reference.equals(str));
    }

    @Test
    public void nullBuffer() throws DicoException, CodecException {
        init();
        byte[] buffer = null;
        Message msg = serializer.decode(buffer);
        assertTrue(msg.getId() == 0);
    }

    @Test(expected = CodecException.class)
    public void corruptedBuffer() throws DicoException, CodecException {
        init();
        byte[] buffer = new byte[30];
        for (int i = 0; i < 30; ++i) {
            buffer[i] = (byte) i;
        }
        Message msg = serializer.decode(buffer);
        assertEquals(msg.getId(), 0);
    }

    @Test
    public void date() throws DicoException, CodecException {
        init();
        int dateMg = serializer.getId("date");
        int dateFd = serializer.getId("Date");
        Message mA = serializer.createEmptyMessage(dateMg);

        Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
        calendar.clear();

        calendar.set(1970, 0, 1, 0, 0, 0);
        //Date date1970        = calendar.getTime(); //new Date(1970-1900,0,1);
        long date1970_in_ms = calendar.getTimeInMillis(); //date1970.getTime();
        double date1970_in_sec = date1970_in_ms / 1000;

        calendar.set(8099, 0, 1, 0, 0, 0);
        //Date date8099        = calendar.getTime(); //new Date(8099-1900,0,1);
        long date8099_in_ms = calendar.getTimeInMillis(); //date8099.getTime();
        double date8099_in_sec = date8099_in_ms / 1000;

        mA.setDouble(dateFd, date1970_in_sec);
        String str1970A = mA.toString();
        byte[] buf = serializer.encode(mA);
        Message mB = serializer.decode(buf);
        String str1970B = mB.toString();

        mB.setDouble(dateFd, date8099_in_sec);
        String str8099A = mB.toString();
        buf = serializer.encode(mB);
        mA = serializer.decode(buf);
        String str8099B = mA.toString();

        System.out.println("--1970--");
        System.out.println(str1970A);
    //    System.out.println("--1970--");
        //    System.out.println(str1970B);

        System.out.println("--8099--");
        System.out.println(str8099A);
    //    System.out.println("--8099--");
        //    System.out.println(str8099B);

        assertEquals(str1970A, str1970B);
        assertEquals(str8099A, str8099B);

        assertEquals
                ( "date:"                 + "\n"
                + "  id:    99"           + "\n"
                + "  Date:"               + "\n"
                + "    id:    9"          + "\n"
                + "    type:  DOUBLE"     + "\n"
                + "    value: 0"          + "\n", str1970A);

        assertEquals
                ( "date:"                 + "\n"
                + "  id:    99"           + "\n"
                + "  Date:"               + "\n"
                + "    id:    9"          + "\n"
                + "    type:  DOUBLE"     + "\n"
                + "    value: 1.93413e+11"+ "\n", str8099A);
    }

    @Test
    public void getDicoAsMesssage() throws DicoException, CodecException {
        serializer = new Serializer();
        // using an 'empty' Dictionary
        Message msg = serializer.createMessageFromDico();
        String str = msg.toString();
        assertEquals("Dico:" + "\n" + "  id:    1" + "\n", str);

        // using a 'basic' Dictionary
        serializer.parseDico
                ( "<?xml version='1.0' encoding='UTF-8' ?>"
                + "<Dico version='1.0.0' xsd='1.0.0'>"
                + "<FieldsDef>"
                + "  <FieldDef id='1' name='Q' type='long'   deprecated='false' comment='Volume' />"
                + "  <FieldDef id='2' name='P' type='double' deprecated='false' comment='Price'  />"
                + "</FieldsDef>"
                + "<MessagesDef>"
                + "  <MessageDef id='3' name='Msg' deprecated='false' comment='basic'>"
                + "    <Field name='Q' default='3333' required='true'  repeated='false' />"
                + "    <Field name='P' default='3.14' required='false' repeated='false' />"
                + "  </MessageDef>"
                + "</MessagesDef>"
                + "</Dico>");
        msg = serializer.createMessageFromDico();
        str = msg.toString();
        assertEquals
                ( "Dico:"                   + "\n"
                + "  id:    1"              + "\n"
                + "  version:"              + "\n"
                + "    id:    10"           + "\n"
                + "    type:  LONG"         + "\n"
                + "    value: 1"            + "\n"
                + "  Field:"                + "\n"
                + "  - id:"                 + "\n"
                + "      id:    2"          + "\n"
                + "      type:  LONG"       + "\n"
                + "      value: 3"          + "\n"
                + "    name:"               + "\n"
                + "      id:    3"          + "\n"
                + "      type:  STRING"     + "\n"
                + "      value: \"Msg\""    + "\n"
                + "    type:"               + "\n"
                + "      id:    4"          + "\n"
                + "      type:  LONG"       + "\n"
                + "      value: 5"          + "\n"
                + "    deprecated:"         + "\n"
                + "      id:    5"          + "\n"
                + "      type:  BOOL"       + "\n"
                + "      value: false"      + "\n"
                + "    comment:"            + "\n"
                + "      id:    6"          + "\n"
                + "      type:  STRING"     + "\n"
                + "      value: \"basic\""  + "\n"
                + "    Item:"               + "\n"
                + "    - id:"               + "\n"
                + "        id:    2"        + "\n"
                + "        type:  LONG"     + "\n"
                + "        value: 1"        + "\n"
                + "      type:"             + "\n"
                + "        id:    4"        + "\n"
                + "        type:  LONG"     + "\n"
                + "        value: 2"        + "\n"
                + "      required:"         + "\n"
                + "        id:    7"        + "\n"
                + "        type:  BOOL"     + "\n"
                + "        value: true"     + "\n"
                + "      defaultD:"         + "\n"
                + "        id:    8"        + "\n"
                + "        type:  DOUBLE"   + "\n"
                + "        value: 3333"     + "\n"
                + "      defaultS:"         + "\n"
                + "        id:    9"        + "\n"
                + "        type:  STRING"   + "\n"
                + "    - id:"               + "\n"
                + "        id:    2"        + "\n"
                + "        type:  LONG"     + "\n"
                + "        value: 2"        + "\n"
                + "      type:"             + "\n"
                + "        id:    4"        + "\n"
                + "        type:  LONG"     + "\n"
                + "        value: 3"        + "\n"
                + "      required:"         + "\n"
                + "        id:    7"        + "\n"
                + "        type:  BOOL"     + "\n"
                + "        value: false"    + "\n"
                + "      defaultD:"         + "\n"
                + "        id:    8"        + "\n"
                + "        type:  DOUBLE"   + "\n"
                + "        value: 3.14"     + "\n"  // FIXME Actual=3 (LONG)
                + "      defaultS:"         + "\n"
                + "        id:    9"        + "\n"
                + "        type:  STRING"   + "\n"
                + "  - id:"                 + "\n"
                + "      id:    2"          + "\n"
                + "      type:  LONG"       + "\n"
                + "      value: 2"          + "\n"
                + "    name:"               + "\n"
                + "      id:    3"          + "\n"
                + "      type:  STRING"     + "\n"
                + "      value: \"P\""      + "\n"
                + "    type:"               + "\n"
                + "      id:    4"          + "\n"
                + "      type:  LONG"       + "\n"
                + "      value: 3"          + "\n"
                + "    deprecated:"         + "\n"
                + "      id:    5"          + "\n"
                + "      type:  BOOL"       + "\n"
                + "      value: false"      + "\n"
                + "    comment:"            + "\n"
                + "      id:    6"          + "\n"
                + "      type:  STRING"     + "\n"
                + "      value: \"Price\""  + "\n"
                + "    Item:"               + "\n"
                + "  - id:"                 + "\n"
                + "      id:    2"          + "\n"
                + "      type:  LONG"       + "\n"
                + "      value: 1"          + "\n"
                + "    name:"               + "\n"
                + "      id:    3"          + "\n"
                + "      type:  STRING"     + "\n"
                + "      value: \"Q\""      + "\n"
                + "    type:"               + "\n"
                + "      id:    4"          + "\n"
                + "      type:  LONG"       + "\n"
                + "      value: 2"          + "\n"
                + "    deprecated:"         + "\n"
                + "      id:    5"          + "\n"
                + "      type:  BOOL"       + "\n"
                + "      value: false"      + "\n"
                + "    comment:"            + "\n"
                + "      id:    6"          + "\n"
                + "      type:  STRING"     + "\n"
                + "      value: \"Volume\"" + "\n"
                + "    Item:"               + "\n"
                ,
                str);

    }
}
