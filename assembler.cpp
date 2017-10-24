#include "include/assembler/types.h"
#include "include/assembler/common.h"
#include "include/assembler/pre.h"
#include "include/assembler/mcr.h"
#include "include/assembler/asm.h"

// compilar com
// g++ -std=c++11 -Wall assembler.cpp -o assembler.out
// ou entao com CTRL SHIFT B no VSCODE

// rodar com
// ./assembler.out -w xxx.asm yyy.asm zzz.asm

int main (int argc, char *argv[]) {
    
    // nomes dos arquivos contendo as tabelas de intrucoes e diretivas
    std::string instrFileName ("tabl/tabInstr.txt"),
        dirFileName ("tabl/tabDir.txt");
    
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
    
    // lista de erros a serem mostrados no final da execução
    std::vector<Error> errorList;
    
    // passagem de pre processamento
    if (operation == "-p" || operation == "-m" || operation == "-o")
        preProcessFile (inFileName, preFileName, lineDictPre, instrList, dirList, errorList);
    
    // passagem de macros
    if (operation == "-m" || operation == "-o")
        expandMacros (preFileName, mcrFileName, lineDictMcr, lineDictPre, instrList, dirList, errorList);
    
    // faz o dicionario "composto"
    for (unsigned int i = 0; i < lineDictMcr.size(); ++i)
        lineDict.push_back(lineDictPre[lineDictMcr[i]-1]);
    
    // passagem normal
    if (operation == "-o")
        assembleCode (mcrFileName, outFileName, lineDict, instrList, dirList, errorList);
    
    // coloca os erros na ordem, de acordo com o número da linha
    std::sort (errorList.begin(), errorList.end());
        
    // mostra todos os erros no terminal
    reportList (errorList);
        
    return 0;
    
}