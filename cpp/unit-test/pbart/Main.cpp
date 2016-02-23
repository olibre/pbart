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

std::string global_directory;

int main (int argc, char *argv[])
{
    // First argument is the directory to find the dico.xml
    if (argc > 1)
        global_directory = argv[1];

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
