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
    
    // cria o dicionario de linhas para o preprocessamento, para a passagem de macros e o dicionario composto dos dois
    // o indice representa a linha atual, e o valor no indice é a linha original
    std::vector<int> lineDictPre;
    std::vector<int> lineDictMcr;
    std::vector<int> lineDict;
    
    // passagem de pre processamento
    if (operation == "-p" || operation == "-m" || operation == "-o")
        preProcessFile (inFileName, preFileName, lineDictPre, instrList, dirList);
    
    // passagem de macros
    if (operation == "-m" || operation == "-o")
        expandMacros (preFileName, mcrFileName, lineDictMcr, lineDictPre, instrList, dirList);
    
    // faz o dicionario "composto"
    for (int i = 0; i < lineDictMcr.size(); ++i) {
        lineDict.push_back(lineDictPre[lineDictMcr[i]-1]);
        // std::cout << ".mcr line: " << i+1 << ", .pre line: " << lineDictMcr[i] << ", .asm line: " << lineDictPre[lineDictMcr[i]-1] << "\n";
    }
        
    // passagem normal
    if (operation == "-o")
        assembleCode (mcrFileName, outFileName, lineDict, instrList, dirList);
    
    return 0;
    
}