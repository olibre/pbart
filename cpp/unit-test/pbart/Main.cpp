/*
 *   This file is from CppUnit example money/MoneyApp.cpp
 *   http://cgit.freedesktop.org/libreoffice/cppunit/tree/examples/money/MoneyApp.cpp?id=cppunit-1.13.2
 *
 *   Therefore this file is licensed under the LGPL v2
 *   https://www.gnu.org/licenses/lgpl-2.0.html
 *
 *   Autors are Baptiste Lepilleur and Steve M. Robbins
 */


#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <sys/stat.h> // stat()


std::string global_directory;


template <size_t SIZE>
std::string stop_if_dir_exist (const char * const (&path) [SIZE])
{
    struct stat info;

    for( size_t i = 0; i < SIZE; ++i )
    {
        int ret = stat (path[i], &info);
        bool exist = (ret == 0);
        if( exist && (info.st_mode & S_IFDIR) )
            return path[i];
    }

    return std::string();
}


int main (int argc, char *argv[])
{
    // First argument is the directory to find the dico.xml
    if (argc > 1)
    {
        global_directory = argv[1];
    }
    else
    {
        const char* dirs[] = {  "../../../../cpp/unit-test/data/"
                                ,  "../../../cpp/unit-test/data/"
                                ,     "../../cpp/unit-test/data/"
                                ,        "../cpp/unit-test/data/"
                                ,  "../pbart/cpp/unit-test/data/"
                                ,     "pbart/cpp/unit-test/data/"
                                ,           "cpp/unit-test/data/"
                                ,               "unit-test/data/"
                                ,                         "data/"
                             };

        // Stop when a directory exist and set variable global_directory
        global_directory = stop_if_dir_exist (dirs);
    }

    std::cout <<"Path to search dictionary: "<< global_directory <<'\n';

    // Get the top level suite from the registry
    CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CPPUNIT_NS::TextUi::TestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &runner.result(),
                                                            CPPUNIT_NS::stdCOut() ) );
    // Run the test.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}
