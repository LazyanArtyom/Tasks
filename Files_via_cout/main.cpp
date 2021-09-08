#include <iostream>
#include <fstream>
#include <cstring>

int main()
{
    std::ofstream outFile("out.txt");

    if (outFile.fail()) // check failbit  
    {
        std::cerr << std::strerror(errno);
        return 1;
    }

    std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(outFile.rdbuf()); //redirect std::cout to out.txt!

    std::cout << "Text1\n";  //output to the file out.txt


    std::cout.rdbuf(coutbuf); //reset to standard output again

    outFile.close();
    if (outFile.fail()) 
    {
        std::cerr << std::strerror(errno);
        return 1;
    }

    return 0;
}
