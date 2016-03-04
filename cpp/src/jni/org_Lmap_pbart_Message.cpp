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

#include <pbart/Message.h>

//#include "??????/org_Lmap_pbart_Message.h"
#include GENERATED_JNI_MESSAGE_HEADER

#include "cpp-java-conversion-helpers.h"
using namespace pbartjni;

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1destroy
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        delete message;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_clone
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Message_native_1clone
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        pbart::Message* clone = new pbart::Message (*message);
        return jlong (clone);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_toString
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_Lmap_pbart_Message_native_1toString
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        std::ostringstream oss;
        oss << *message;
        std::string const& str = oss.str(); // str() returns a copy (an unnamed temporary rvalue /*object*/) but its lifetime is extended to the lifetime of the const reference
        jstring jstr = env->NewStringUTF (str.c_str());
        return jstr;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_check
 * Signature: (J)Z
 */
JNIEXPORT jint JNICALL Java_org_Lmap_pbart_Message_native_1check__J
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);
        return message->check();
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_check
 * Signature: (JI)Z
 */
JNIEXPORT jint JNICALL Java_org_Lmap_pbart_Message_native_1check__JI
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->check(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_size
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1size
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->size();
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getId
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_Lmap_pbart_Message_native_1getId
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->id();
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getName
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_Lmap_pbart_Message_native_1getName__J
(JNIEnv *env, jobject /*object*/, jlong handle)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const std::string& name = message->name();
        jstring jstr = env->NewStringUTF (name.c_str());
        return jstr;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getName
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_Lmap_pbart_Message_native_1getName__JI
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const std::string& name = message->name(id);
        jstring jstr = env->NewStringUTF (name.c_str());
        return jstr;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_isAllowed
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1isAllowed
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->allowed(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_isEmpty
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1isEmpty
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->empty(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_erase
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1erase
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->erase(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getType
 * Signature: (JIZ)I
 */
JNIEXPORT jint JNICALL Java_org_Lmap_pbart_Message_native_1getType
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jboolean sequence)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return sequence ? message->type(id) : message->typeDico(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_isRepeated
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1isRepeated
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->repeated(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_isRequired
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1isRequired
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->required(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_isDeprecated
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1isDeprecated
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->deprecated(id);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getSeqSize
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_org_Lmap_pbart_Message_native_1getSeqSize
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        pbart::Type type = message->type(id);

        switch (type)
        {
        case pbart::Type::SEQ_BOOL:    return message->get(id).toSeqBool   ().size();
        case pbart::Type::SEQ_LONG:    return message->get(id).toSeqLong   ().size();
        case pbart::Type::SEQ_DOUBLE:  return message->get(id).toSeqDouble ().size();
        case pbart::Type::SEQ_STRING:  return message->get(id).toSeqString ().size();
        case pbart::Type::SEQ_MESSAGE: return message->get(id).toSeqMessage().size();

        case pbart::Type::EMPTY:
        case pbart::Type::BOOL:
        case pbart::Type::LONG:
        case pbart::Type::DOUBLE:
        case pbart::Type::STRING:
        case pbart::Type::MESSAGE:
        default:
            return 0;
        }
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getBool
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1getBool
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->get(id).toBool();
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getLong
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Message_native_1getLong
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->get(id).toLong();
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getDouble
 * Signature: (JI)D
 */
JNIEXPORT jdouble JNICALL Java_org_Lmap_pbart_Message_native_1getDouble
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->get(id).toDouble();
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getString
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_Lmap_pbart_Message_native_1getString
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const std::string& str = message->get(id).toString();
        jstring jstr = env->NewStringUTF (str.c_str());
        return jstr;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_cloneMessage
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Message_native_1cloneMessage
(JNIEnv *env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const pbart::Message& inner = message->get(id).toMessage();
        pbart::Message* clone = new pbart::Message (inner);
        return jlong (clone);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getSeqBool
 * Signature: (JII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_Lmap_pbart_Message_native_1getSeqBool
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->get(id).toBool(index);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getSeqLong
 * Signature: (JII)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Message_native_1getSeqLong
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->get(id).toLong(index);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getSeqDouble
 * Signature: (JII)D
 */
JNIEXPORT jdouble JNICALL Java_org_Lmap_pbart_Message_native_1getSeqDouble
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        return message->get(id).toDouble(index);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_getSeqString
 * Signature: (JII)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_Lmap_pbart_Message_native_1getSeqString
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const std::string& str = message->get(id).toString(index);
        jstring jstr = env->NewStringUTF (str.c_str());
        return jstr;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_cloneSeqMessage
 * Signature: (JII)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Message_native_1cloneSeqMessage
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index)
{
    try
    {
        const pbart::Message* message = (const pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const pbart::Message& inner = message->get(id).toMessage(index);
        pbart::Message* clone = new pbart::Message (inner);
        return jlong (clone);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setBool
 * Signature: (JIZ)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setBool
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jboolean v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        message->set (id, bool(v));
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setLong
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setLong
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jlong v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        message->set (id, long(v));
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setDouble
 * Signature: (JID)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setDouble
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jdouble v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        message->set (id, double(v));
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setString
 * Signature: (JILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setString
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jstring v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const std::string& str = convert (env, v);
        message->set (id, str);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);

    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setMessage
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setMessage
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jlong handle2)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        pbart::Message* inner   = (pbart::Message*) handle2;
        assert(message);  assert(message->check()!=-2);
        assert(inner);
        message->set (id, *inner);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setSeqBool
 * Signature: (JIIZ)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setSeqBool
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index, jboolean v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);

        if (message->empty(id))
            message->set (id, pbart::Type(pbart::Type::SEQ_BOOL));
        pbart::Variant::SeqBool& vector = message->get(id).toSeqBool();

        size_t pos = index;
        if (pos >= vector.size())
            vector.resize (pos + 1);
        vector[pos] = v;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setSeqLong
 * Signature: (JIIJ)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setSeqLong
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index, jlong v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);

        if (message->empty(id))
            message->set (id, pbart::Type(pbart::Type::SEQ_LONG));
        pbart::Variant::SeqLong& vector = message->get(id).toSeqLong();

        size_t pos = index;
        if (pos >= vector.size())
            vector.resize (pos + 1);
        vector[pos] = v;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setSeqDouble
 * Signature: (JIID)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setSeqDouble
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index, jdouble v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);

        if (message->empty(id))
            message->set (id, pbart::Type(pbart::Type::SEQ_DOUBLE));
        pbart::Variant::SeqDouble& vector = message->get(id).toSeqDouble();

        size_t pos = index;
        if (pos >= vector.size())
            vector.resize (pos + 1);
        vector[pos] = v;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setSeqString
 * Signature: (JIILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setSeqString
(JNIEnv *env, jobject /*object*/, jlong handle, jint id, jint index, jstring v)
{
    try
    {
        pbart::Message* message = (pbart::Message*) handle;
        assert(message);  assert(message->check()!=-2);
        const std::string& str = convert (env, v);

        if (message->empty(id))
            message->set (id, pbart::Type(pbart::Type::SEQ_STRING));
        pbart::Variant::SeqString& vector = message->get(id).toSeqString();

        size_t pos = index;
        if (pos >= vector.size())
            vector.resize (pos + 1);
        vector[pos] = str;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}


#include <sstream>


/*
 * Class:     org_Lmap_pbart_Message
 * Method:    native_setSeqMessage
 * Signature: (JIIJ)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Message_native_1setSeqMessage
(JNIEnv *env, jobject /*object*/, jlong handleMainMsg, jint id, jint index, jlong handleSubMsg)  // handleSubMsg is the sub-message pointer
{
    try
    {
        pbart::Message* message = (pbart::Message*) handleMainMsg;
        pbart::Message* inner   = (pbart::Message*) handleSubMsg;
        assert(message);  assert(message->check()!=-2);
        assert(inner);

        if (inner->id() != pbart::Id(id))
        {
            jclass jc = env->FindClass("java/lang/Error");
            if (jc)
            {
                std::stringstream oss;
                oss <<"In message id="<< message->id() <<" name="<< message->name()
                   <<" the 'Sequence of messages' id="<< id <<" name="<< message->name(id)
                  <<" cannot accept the message in parameter (name="<< inner->id() <<") "
                                                                                     "because it uses a bad id="<< inner->id();
                env->ThrowNew (jc, oss.str().c_str());
            }
        }

        if (message->empty(id))
            message->set (id, pbart::Type(pbart::Type::SEQ_MESSAGE));
        pbart::Variant::SeqMessage& vector = message->get(id).toSeqMessage();

        size_t pos = index;
        if (pos < vector.size())
        {
            vector[pos] = *inner;
        }
        else if (pos == vector.size())
        {
            vector.push_back (*inner);
        }
        else
        {
            jclass jc = env->FindClass("java/lang/Error");
            if (jc)
            {
                std::stringstream oss;
                oss <<"In message id="<< message->id() <<" name="<< message->name()
                   <<" the 'Sequence of messages' id="<< id <<" name="<< message->name(id)
                  <<" cannot be set at position="<< pos <<" because size="<< message->get(id).toSeqBool().size()
                 <<". Please set messages consecutively to avoid invalid holes within the array.";
                env->ThrowNew (jc, oss.str().c_str());
            }
        }
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}






