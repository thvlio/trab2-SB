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
    std::vector<std::string> inFileNames;
    for (int i = 1; i < argc; ++i)
        inFileNames.push_back(std::string(*(argv+i)));
    
    // cria os dicionarios de linhas para o preprocessamento, para a passagem de macros e o dicionario composto dos dois
    // o indice representa a linha atual, e o valor no indice é a linha original
    // um conjunto de tres dicionarios é criado para cada arquivo de entrada
    std::vector< std::vector<int> > lineDictPre (argc-1);
    std::vector< std::vector<int> > lineDictMcr (argc-1);
    std::vector< std::vector<int> > lineDict (argc-1);
    
    // lista de erros a serem mostrados no final da execução
    // um vetor de erros para cada arquivo
    std::vector< std::vector<Error> > errorList (argc-1);
    
    // montagem de cada arquivo individualmente
    for (unsigned int i = 0; i < inFileNames.size(); ++i) {
        
        // passagem de pre processamento
        preProcessFile (inFileNames[i], asm2pre(inFileNames[i]), lineDictPre[i], instrList, dirList, errorList[i]);
        
        // passagem de macros
        expandMacros (asm2pre(inFileNames[i]), asm2mcr(inFileNames[i]), lineDictMcr[i], lineDictPre[i], instrList, dirList, errorList[i]);
        
        // faz o dicionario "composto"
        for (unsigned int j = 0; j < lineDictMcr.size(); ++j)
            lineDict[i].push_back(lineDictPre[i][lineDictMcr[i][j]-1]);
        
        // passagem de montagem
        assembleCode (asm2mcr(inFileNames[i]), asm2o(inFileNames[i]), lineDict[i], instrList, dirList, errorList[i]);
        
        // coloca os erros na ordem e mostra no terminal
        std::sort (errorList[i].begin(), errorList[i].end());
        reportList (errorList[i], inFileNames[i]);
        
        // deleta os arquivos .pre e .mcr
        std::remove (asm2pre(inFileNames[i]).c_str());
        std::remove (asm2mcr(inFileNames[i]).c_str());
        
    }
        
    return 0;
    
}