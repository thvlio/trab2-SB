#include "include/loader/types.h"
#include "include/loader/loader.h"

// compilar com
// g++ -std=c++11 -Wall loader.cpp -o loader.out
// ou entao com CTRL SHIFT B no VSCODE

// rodar com
// ./loader.out xxx

int main (int argc, char *argv[]) {
    
    std::string fileName; // nome do arquivo de entrada
    
    // verifica se houve erros na chamada ao carregador
    if (errorCheck (argc, argv, fileName) == -1)
        return 0;
    
    // dados contidos no arquivo
    int codeSize;
    std::string bitMap;
    int codeStart;
    std::vector<int> machineCode;
    
    // extrai as informações de cabeçalho e codigo maquina do arquivo
    getData (fileName, codeSize, bitMap, codeStart, machineCode);
    
    /*
    std::cout << "codeSize: " << codeSize << "\n";
    std::cout << "bitMap: " << bitMap << "\n";
    std::cout << "codeStart: " << codeStart << "\n";
    std::cout << "machineCode: ";
    for (unsigned int i = 0; i < machineCode.size(); ++i)
        std::cout << machineCode[i] << " ";
    std::cout << "(" << machineCode.size() << ")\n";
    */
    
    // simula a execucao do codigo
    simulateCode (codeStart, machineCode);
    
    return 0;
    
}