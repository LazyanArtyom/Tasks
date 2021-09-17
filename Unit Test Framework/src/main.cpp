#include "testframework.h"
#include "sink/sinkfile.h"
#include "sink/sinkconsole.h"

#include <memory>
#include <iostream>

int main()
{
    taf::TestFramework test;
    test.addSink(std::make_unique<taf::SinkConsole>());
    test.addSink(std::make_unique<taf::SinkFile>("result.log"));
 
    test.addTest("TestCase1", []{
        return true;
    });

    test.addTest("TestCase2", []{
        return true;
    });

    test.addTest("TestCase3", []{
        return false;
    });
    
    test.addTest("TestCase4", []{
        abort();
        return true;
    }); 
   
    test.addTest("TestCase5", []{
        abort();
        return false;
    });

    test.runTests();

    return 0;
}