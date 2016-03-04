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
package org.Lmap.pbart;

/**
 * @author olibre (olibre@Lmap.org)
 *
 */
public class Serializer {

    final private long _handle; // '_handle' = pointer to pbart::Serializer in C++ (JNI)

    // ----------------------------------------------------------------
    static {
        System.loadLibrary("pbart-jni");
    }

    public Serializer() {
        _handle = native_create();
    }

    // Is there a good reason to clone the Serializer?
    @Override
    protected Object clone() throws CloneNotSupportedException {
        throw new CloneNotSupportedException();
    }

    @Override
    protected void finalize() throws Throwable {
        native_destroy(_handle);
        super.finalize();
    }

    // ----------------------------------------------------------------
    public void loadDico(String filename)
            throws DicoException {
        native_loadDico(_handle, filename);
    }

    public void parseDico(String xml)
            throws DicoException {
        native_parseDico(_handle, xml);
    }

    // ----------------------------------------------------------------
    public Message createEmptyMessage(int id) {
        long handle = native_message(_handle, id);
        return new Message(handle);
    }

    public Message createEmptyMessage() {
        long handle = native_message(_handle, 0);
        return new Message(handle);
    }

    public Message createMessageFromDico() throws DicoException, CodecException {
        long handle = native_dicoToMsg(_handle);
        return new Message(handle);
    }

    // ----------------------------------------------------------------
    public int getId(String name) {
        int id = native_id(_handle, name);
        return id;
    }

    public String getName(int id) {
        String name = native_name(_handle, id);
        return name;
    }

    // ----------------------------------------------------------------
    public Message decode(byte[] buffer) throws CodecException {
        if (buffer == null) {
            return createEmptyMessage();
        } else {
            long handle = native_decode(_handle, buffer);
            return new Message(handle);
        }
    }

    public byte[] encode(Message message) throws CodecException {
        return native_encode(_handle, message);
    }

    // ----------------------------------------------------------------
    private native long   native_create();
    private native void   native_destroy  (long handle);
    private native void   native_loadDico (long handle, String s)        throws DicoException;
    private native void   native_parseDico(long handle, String s)        throws DicoException;
    private native int    native_id       (long handle, String name);
    private native String native_name     (long handle, int id);
    private native long   native_dicoToMsg(long handle)                  throws DicoException, CodecException;
    private native long   native_message  (long handle, int id);
    private native long   native_decode   (long handle, byte[] stream)   throws CodecException;
    private native byte[] native_encode   (long handle, Message message) throws CodecException;
}
