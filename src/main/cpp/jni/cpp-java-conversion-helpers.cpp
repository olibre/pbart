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

#include <pbart/Exception.h>

#include "cpp-java-conversion-helpers.h"

namespace pbartjni
{

// ---------------------------------------------------------------------------
// Convert from Java (jstring) to C (char*) and then to C++ (std::string)
std::string convert (JNIEnv *env, jstring jstr)
{
    const char* ptr = env->GetStringUTFChars (jstr, NULL);   assert(ptr);
    jsize       len = env->GetStringUTFLength(jstr);
    std::string str (ptr, len);
    env->ReleaseStringUTFChars (jstr, ptr);
    return str;
}

// ---------------------------------------------------------------------------
// Convert C++ exception to Java exception
void rethrow_cpp_exception_as_java_exception (JNIEnv *env)
{
    try
    {
        throw;
    }
    catch (const pbart::DicoException& e)
    {
        jclass jc = env->FindClass("org/Lmap/pbart/DicoException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const pbart::IdException& e)
    {
        jclass jc = env->FindClass("org/Lmap/pbart/IdException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const pbart::TypeException& e)
    {
        jclass jc = env->FindClass("org/Lmap/pbart/TypeException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const pbart::OutOfRangeException& e)
    {
        jclass jc = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const pbart::CodecException& e)
    {
        jclass jc = env->FindClass("org/Lmap/pbart/CodecException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const pbart::Exception& e)
    {
        jclass jc = env->FindClass("org/Lmap/pbart/RuntimeException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const std::out_of_range& e)
    {
        jclass jc = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const std::bad_alloc& e)
    {
        // OOM exception
        jclass jc = env->FindClass("java/lang/OutOfMemoryError");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const std::ios_base::failure& e)
    {
        // IO exception
        jclass jc = env->FindClass("java/io/IOException");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (const std::exception& e)
    {
        // This exception may derive from std::exception => Not identified
        jclass jc = env->FindClass("java/lang/Error");
        if(jc) env->ThrowNew (jc, e.what());
        // if null => NoClassDefFoundError already thrown
    }
    catch (...)
    {
        // Oops I missed identifying this exception!
        //      jclass jc = env->FindClass("java/lang/NoClassDefFoundError");
        jclass jc = env->FindClass("java/lang/Error");
        if(jc) env->ThrowNew (jc, "Unidentified exception => "
                                  "rethrow_cpp_exception_as_java_exception() "
                                  "may require some completion..." );
        // if null => NoClassDefFoundError already thrown
    }
}

} // namespace pbartjni
