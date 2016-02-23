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

#include <pbart/Serializer.h>

//#include "??????/org_Lmap_pbart_Serializer.h"
#include GENERATED_JNI_SERIALIZER_HEADER

#include "cpp-java-conversion-helpers.h"
using namespace pbartjni;

//#define DUMP_BUFFER_IN_FILES
#ifdef  DUMP_BUFFER_IN_FILES
#  include <fstream>
#  include <sstream>
#  include <iomanip>
static void dump (const pbart::Message* message, const std::string str, const char coding[])
{
    static int count = 0;

    std::stringstream oss;
    oss << std::setfill('_') << std::setw(4) << count++ <<'_'<< coding
        <<"_id"<< std::setfill('-') << std::setw(3) << -message->id()
       <<"_size"<< std::setw(7) << str.size() <<'.';

#     ifdef _DEBUG
    oss <<'D';
#     else
    oss <<'R';
#     endif

#     ifdef NDEBUG
    oss <<'N';
#     else
    oss <<'A';
#     endif

    oss <<".pbart";
    std::ofstream file (oss.str());
    file << str;
}
#else
static void dump (const pbart::Message*, const std::string, const char*)
{
}
#endif


/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_loadDico
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Serializer_native_1loadDico
(JNIEnv *env, jobject /*object*/, jlong handle, jstring filename)
{
    try
    {
        const char* ptr = env->GetStringUTFChars (filename, NULL);
        assert(ptr);

        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        serializer->loadDico (ptr);

        env->ReleaseStringUTFChars (filename, ptr);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_parseDico
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Serializer_native_1parseDico
(JNIEnv *env, jobject /*object*/, jlong handle, jstring xml)
{
    try
    {
        const char* ptr = env->GetStringUTFChars (xml, NULL);
        assert(ptr);

        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        serializer->parseDico (ptr);

        env->ReleaseStringUTFChars (xml, ptr);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Serializer_native_1create
(JNIEnv *env, jobject)
{
    try
    {
        pbart::Serializer* serializer = (pbart::Serializer*) new pbart::Serializer();
        assert(serializer);
        return jlong (serializer);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_Lmap_pbart_Serializer_native_1destroy
(JNIEnv *env, jobject, jlong handle)
{
    try
    {
        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        delete serializer;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_id
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_Lmap_pbart_Serializer_native_1id
(JNIEnv *env, jobject /*object*/, jlong handle, jstring name)
{
    try
    {
        const std::string& str = convert (env, name);
        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        pbart::Id id = serializer->dico()->id(str);
        return id;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_name
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_Lmap_pbart_Serializer_native_1name
(JNIEnv * env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        const std::string& str = serializer->dico()->field(id).name();
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
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_dicoToMsg
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Serializer_native_1dicoToMsg
(JNIEnv * env, jobject /*object*/, jlong handle)
{
    try
    {
        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        pbart::Message*  message = new pbart::Message (serializer->dicoToMsg());
        return (jlong) message;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_message
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Serializer_native_1message
(JNIEnv * env, jobject /*object*/, jlong handle, jint id)
{
    try
    {
        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        pbart::Message*  message;
        if (id)        message = new pbart::Message (serializer->message(id));
        else           message = new pbart::Message (serializer->message());
        return (jlong) message;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_decode
 * Signature: (J[B)J
 */
JNIEXPORT jlong JNICALL Java_org_Lmap_pbart_Serializer_native_1decode
(JNIEnv *env, jobject /*object*/, jlong handle, jbyteArray bytes)
{
    try
    {
        jsize  length = env->GetArrayLength       (bytes);
        jbyte* pbytes = env->GetByteArrayElements (bytes, NULL);
        const char* ptr = reinterpret_cast<char*> (pbytes);
        pbart::Buffer buffer (ptr, length);
        env->ReleaseByteArrayElements (bytes, pbytes, 0);

        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);
        pbart::Message* message = new pbart::Message (serializer->decode(buffer));
        dump (message, buffer, "Decode");

        return jlong (message);
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

/*
 * Class:     org_Lmap_pbart_Serializer
 * Method:    native_encode
 * Signature: (JLorg/Lmap/pbart/Message;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_Lmap_pbart_Serializer_native_1encode
(JNIEnv *env, jobject /*object*/, jlong handle, jobject messageObject)
{
    try
    {
        jclass messageClass = env->GetObjectClass (messageObject);
        jfieldID fid        = env->GetFieldID     (messageClass, "_handle", "J"); // "J" = jlong
        jlong messageHandle = env->GetLongField   (messageObject, fid);

        pbart::Message* message = (pbart::Message*) messageHandle;
        assert(message);

        pbart::Serializer* serializer = (pbart::Serializer*) handle;
        assert(serializer);

        pbart::Buffer buffer = serializer->encode (*message);
        dump (message, buffer, "Encode");

        jbyteArray bytes = env->NewByteArray (buffer.size());
        jbyte* pbytes = env->GetByteArrayElements (bytes, NULL);
        memcpy (pbytes, buffer.c_str(), buffer.size());
        env->SetByteArrayRegion (bytes, 0, buffer.size(), pbytes);
        env->ReleaseByteArrayElements (bytes, pbytes, 0);

        return bytes;
    }
    catch(...)
    {
        rethrow_cpp_exception_as_java_exception(env);
    }
    return 0;
}

