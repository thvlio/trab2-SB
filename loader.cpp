#include "include/loader/types.h"
#include "include/loader/loader.h"

// compilar com
// g++ -std=c++11 -Wall loader.cpp -o loader.out
// ou entao com CTRL SHIFT B no VSCODE

// rodar com
// ./loader.out xxx nc tc1 .. tcn ec1 .. ecn

int main (int argc, char *argv[]) {
    
    // nome do arquivo de entrada
    std::string fileName (std::string(*(argv+1))); 
    
    // armazenas as informações dos chunks
    std::vector<Chunk> chunkList;
    getChunks (argc, argv, chunkList);
    
    // dados contidos no arquivo
    int codeSize;
    std::string bitMap;
    int codeStart;
    std::vector<int> machineCode;
    
    // extrai as informações de cabeçalho e codigo maquina do arquivo
    getData (fileName, codeSize, bitMap, codeStart, machineCode);
    
    // simula a execucao do codigo
    std::cout << "./" << fileName << "\n";
    simulateCode (codeStart, machineCode);
    
    // determina em quantos chunks o código cabe
    std::vector<Chunk> minChunkList;
    int fit = fitCode (codeSize, chunkList, minChunkList);
    
    if (fit == -1) {
        std::cout << "OUT OF MEMORY - YOUR PROGRAM WILL NOT BE LOADED\n";
        return 0;
    }
    
    // fragmenta o código nos chunks dados
    fragmentCode (codeSize, bitMap, machineCode, minChunkList, codeStart);
    
    // escreve a imagem de memória num arquivo .im
    std::ofstream outFile (fileName + ".im");
    for (unsigned int i = 0; i < machineCode.size(); ++i)
        outFile << machineCode[i] << " ";
    outFile.close();

    displayChunks(machineCode, minChunkList);
    
    return 0;
    
}