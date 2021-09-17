#include "sinkfile.h"

#include <fstream>
#include <iostream>

taf::SinkFile::SinkFile(const std::string &fileName) : fileName_(fileName) {}

void taf::SinkFile::log(const std::stringstream &str)
{
    std::ofstream out;
    out.open(fileName_);

    if (out)
        out << str.rdbuf();

    if (out.bad())
        std::cerr << "Writing to file failed!\n";
}