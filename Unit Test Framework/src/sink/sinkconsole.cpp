#include "sinkconsole.h"

#include <iostream>


void taf::SinkConsole::log(const std::stringstream& str)
{
    std::cout << str.str();
}     