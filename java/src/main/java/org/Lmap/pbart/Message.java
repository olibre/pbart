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
public class Message {

    final private long _handle; // '_handle' = pointer to pbart::Message in C++ (JNI)

    // ----------------------------------------------------------------
    protected Message(long handle) {
        _handle = handle;
    }

    @Override
    protected Object clone() throws CloneNotSupportedException {
        long handle;
        handle = native_clone(_handle);
        return new Message(handle);
    }

    @Override
    protected void finalize() throws Throwable {
        native_destroy(_handle);
        super.finalize();
    }

    @Override
    public String toString() {
        return native_toString(_handle);
    }

    // ----------------------------------------------------------------
    public int check() {
        return native_check(_handle);
    }

    public int check(int id) throws IdException {
        return native_check(_handle, id);
    }

    // ----------------------------------------------------------------
    public boolean size() throws IdException {
        return native_size(_handle);
    }

    public int getId() throws IdException {
        return native_getId(_handle);
    }

    public String getName() throws IdException {
        return native_getName(_handle);
    }

    // ----------------------------------------------------------------
    public String getName(int id) throws IdException {
        return native_getName(_handle, id);
    }

    public boolean isAllowed(int id) throws IdException {
        return native_isAllowed(_handle, id);
    }

    public boolean isEmpty(int id) throws IdException {
        return native_isEmpty(_handle, id);
    }

    public boolean erase(int id) throws IdException {
        return native_erase(_handle, id);
    }

    // ----------------------------------------------------------------
    // Example for a field of type="STRING"
    // if repeated="false" and parameter sequence=false => return STRING
    // if repeated="false" and parameter sequence=true => return STRING
    // if repeated="true" and parameter sequence=false => return STRING
    // if repeated="true" and parameter sequence=true => return SEQ_STRING
    public Type getType(int id, boolean sequence) {
        int type = native_getType(_handle, id, sequence);
        return Type.getFrom(type);
    }

    // by default sequence=true
    public Type getType(int id) throws IdException {
        return getType(id, true);
    }

    // ----------------------------------------------------------------
    public boolean isDeprecated(int id) throws IdException {
        return native_isDeprecated(_handle, id);
    }

    public boolean isRequired(int id) throws IdException {
        return native_isRequired(_handle, id);
    }

    public boolean isRepeated(int id) throws IdException {
        return native_isRepeated(_handle, id);
    }

    // ----------------------------------------------------------------
    public int getSeqSize(int id) throws IdException {
        return native_getSeqSize(_handle, id);
    }

    // ----------------------------------------------------------------
    public boolean getBool(int id) throws IdException {
        return native_getBool(_handle, id);
    }

    public boolean getBoolean(int id) throws IdException {
        return getBool(id);
    }

    public long getLong(int id) throws IdException {
        return native_getLong(_handle, id);
    }

    public double getDouble(int id) throws IdException {
        return native_getDouble(_handle, id);
    }

    public String getString(int id) throws IdException {
        return native_getString(_handle, id);
    }

    public Message cloneMessage(int id) throws IdException {
        long handle = native_cloneMessage(_handle, id);
        return new Message(handle);
    }

    // ----------------------------------------------------------------
    public boolean getBool(int id, int index) throws IdException {
        return native_getSeqBool(_handle, id, index);
    }

    public boolean getBoolean(int id, int index) throws IdException {
        return getBool(id, index);
    }

    public long getLong(int id, int index) throws IdException {
        return native_getSeqLong(_handle, id, index);
    }

    public double getDouble(int id, int index) throws IdException {
        return native_getSeqDouble(_handle, id, index);
    }

    public String getString(int id, int index) throws IdException {
        return native_getSeqString(_handle, id, index);
    }

    public Message cloneMessage(int id, int index) throws IdException {
        long handle = native_cloneSeqMessage(_handle, id, index);
        return new Message(handle);
    }

    // ----------------------------------------------------------------
    public void setBool(int id, boolean v) throws IdException, TypeException {
        native_setBool(_handle, id, v);
    }

    public void setBoolean(int id, boolean v) throws IdException, TypeException {
        setBool(id, v);
    }

    public void setLong(int id, long v) throws IdException, TypeException {
        native_setLong(_handle, id, v);
    }

    public void setDouble(int id, double v) throws IdException, TypeException {
        native_setDouble(_handle, id, v);
    }

    public void setString(int id, String v) throws IdException, TypeException {
        native_setString(_handle, id, v);
    }

    public void setMessage(int id, Message v) throws IdException, TypeException {
        long messageHandle = v._handle;
        native_setMessage(_handle, id, messageHandle);
    }

    // ----------------------------------------------------------------
    public void setBool(int id, int index, boolean v) throws IdException, TypeException {
        native_setSeqBool(_handle, id, index, v);
    }

    public void setBoolean(int id, int index, boolean v) throws IdException, TypeException {
        setBool(id, index, v);
    }

    public void setLong(int id, int index, long v) throws IdException, TypeException {
        native_setSeqLong(_handle, id, index, v);
    }

    public void setDouble(int id, int index, double v) throws IdException, TypeException {
        native_setSeqDouble(_handle, id, index, v);
    }

    public void setString(int id, int index, String v) throws IdException, TypeException {
        native_setSeqString(_handle, id, index, v);
    }

    public void setMessage(int id, int index, Message v) throws IdException, TypeException {
        long messageHandle = v._handle;
        native_setSeqMessage(_handle, id, index, messageHandle);
    }

    // ----------------------------------------------------------------
    private native void native_destroy(long handle);
    private native long    native_clone          (long handle) throws CloneNotSupportedException;
    private native String  native_toString       (long handle);
    private native boolean native_size           (long handle);
    private native int     native_getId          (long handle);
    private native String  native_getName        (long handle);
    private native int     native_check          (long handle);
    private native String  native_getName        (long handle, int id);
    private native int     native_check          (long handle, int id);  //returns the ID of the bad field, else returns 'zero' meaning OK
    private native boolean native_isAllowed      (long handle, int id);
    private native boolean native_isEmpty        (long handle, int id);
    private native boolean native_erase          (long handle, int id);
    private native boolean native_isRepeated     (long handle, int id);
    private native boolean native_isRequired     (long handle, int id);
    private native boolean native_isDeprecated   (long handle, int id);
    private native int     native_getType        (long handle, int id, boolean s);

    private native boolean native_getBool        (long handle, int id)                   throws IdException, TypeException;
    private native long    native_getLong        (long handle, int id)                   throws IdException, TypeException;
    private native double  native_getDouble      (long handle, int id)                   throws IdException, TypeException;
    private native String  native_getString      (long handle, int id)                   throws IdException, TypeException;
    private native long    native_cloneMessage   (long handle, int id)                   throws IdException, TypeException;

    private native int     native_getSeqSize     (long handle, int id)                   throws IdException;
    private native boolean native_getSeqBool     (long handle, int id, int i)            throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native long    native_getSeqLong     (long handle, int id, int i)            throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native double  native_getSeqDouble   (long handle, int id, int i)            throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native String  native_getSeqString   (long handle, int id, int i)            throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native long    native_cloneSeqMessage(long handle, int id, int i)            throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;

    private native void    native_setBool        (long handle, int id, boolean v)        throws IdException, TypeException;
    private native void    native_setLong        (long handle, int id, long    v)        throws IdException, TypeException;
    private native void    native_setDouble      (long handle, int id, double  v)        throws IdException, TypeException;
    private native void    native_setString      (long handle, int id, String  v)        throws IdException, TypeException;
    private native void    native_setMessage     (long handle, int id, long message)     throws IdException, TypeException;

    private native void    native_setSeqBool     (long handle, int id, int i, boolean v) throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native void    native_setSeqLong     (long handle, int id, int i, long    v) throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native void    native_setSeqDouble   (long handle, int id, int i, double  v) throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native void    native_setSeqString   (long handle, int id, int i, String  v) throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
    private native void    native_setSeqMessage  (long handle, int id, int i, long h)    throws IdException, TypeException, java.lang.ArrayIndexOutOfBoundsException;
}
