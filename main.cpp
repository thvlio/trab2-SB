#include "types.h"
#include "functions.h"

// compilar com
// g++ -std=c++11 main.cpp -o main.out

// rodar com
// ./main.out -x xxx.asm yyy.o

int main (int argc, char *argv[]) {
    
    if (errorCheck(argc, argv) == -1)
        return 0;
    
    // coloca os argumentos em strings
    std::string operation ( *(argv+1) ),
        inFileName ( *(argv+2) ),
        outFileName ( *(argv+3) );
        
    std::string preFileName (o2pre(outFileName)),
        mcrFileName (o2mcr(outFileName));
    
    // passagem de pre processamento
    if (operation == "-p" || operation == "-m" || operation == "-o")
        preProcessFile (inFileName, preFileName);
        
    // passagem de macros
    if (operation == "-m" || operation == "-o")
        expandMacros (preFileName, mcrFileName);
    
    // passagem normal
    if (operation == "-o")
        assembleCode (mcrFileName, outFileName);
    
    return 0;
    
}