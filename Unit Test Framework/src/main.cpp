#include "testframework.h"

#include <memory>
#include <iostream>

int main()
{
    taf::TestFramework test(true);
 
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