#include "types.h"
#include "common.h"
#include "pre.h"
#include "mcr.h"
#include "asm.h"

// compilar com
// g++ -std=c++11 main.cpp -o main.out
// ou entao com CTRL SHIFT B no VSCODE

// rodar com
// ./main.out -x xxx.asm yyy.o

int main (int argc, char *argv[]) {
    
    // nomes dos arquivos contendo as tabelas de intrucoes e diretivas
    std::string instrFileName ("tabInstr.txt"),
        dirFileName ("tabDir.txt");
    
    // checa se houveram erros
    if (errorCheck(argc, argv, instrFileName, dirFileName) == -1)
        return 0;
            
    // constroi a lista de instrucoes e de diretivas
    std::vector<Instr> instrList = getInstrList (instrFileName);
    std::vector<Dir> dirList = getDirList (dirFileName);
    
    // coloca os argumentos em strings
    std::string operation ( *(argv+1) ),
        inFileName ( *(argv+2) ),
        outFileName ( *(argv+3) );
    
    // cria os nomes dos arquivos com as extensoes '.pre' e '.mcr'
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