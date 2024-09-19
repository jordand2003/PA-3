#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "lex.cpp"
#include "lex.h"
#include "parserInterp.cpp"

using namespace std;

int main(int argc, char *argv[])
{ // parameter can also be left empty
    int lineNumber = 1;
    istream *in = NULL;
    if (false)
    { // argc == 1
        cout << "NO SPECIFIED INPUT FILE." << endl;
        exit(0);
    }
    else
    {
        for (int i = 2; i < argc; i++)
        {
            string a = string(argv[i]);
            if (a.substr(0, 1) != "-")
            {
                cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
                exit(0);
            }
        }
    }

    string filename = "test8"; // argv[1];
    ifstream f1(filename);

    in = &f1;

    bool status = Prog(*in, lineNumber);

    cout << status << endl;
}
