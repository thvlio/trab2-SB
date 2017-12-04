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
    
    // testa o arquivo de entrada
    std::ifstream file (fileName);
    if (!file.is_open()) {
        std::cout << "Erro ao abrir o arquivo " << fileName << "\n";
        return 0;
    } else
        file.close();
        
    // testa se as informacoes sobre os chunks estao consistentes
    int numChunks = stoi (std::string(*(argv+2)));
    if (2*numChunks != (argc-3)) {
        std::cout << "As informações sobre os chunks estão inconsistentes (número incorreto de parâmetros).\n";
        return 0;
    }
    
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
    simulateCode (codeStart, machineCode);
    
    // determina em quantos chunks o código cabe
    std::vector<Chunk> minChunkList;
    int fit = fitCode (codeSize, chunkList, minChunkList);
    
    if (fit == -1) {
        std::cout << "\033[31;1m" << "\nSem memória - o programa não será carregado!\n";
        int totalMemory = 0;
        for (unsigned int i = 0; i < chunkList.size(); ++i)
            totalMemory += chunkList[i].size;
        std::cout << "Memória requerida: " << machineCode.size() << ", memória disponível: " << totalMemory << "\033[0;1m" << "\n";        
        return 0;
    }
    
    // fragmenta o código nos chunks dados
    fragmentCode (codeSize, bitMap, machineCode, minChunkList, codeStart);
    
    // escreve a imagem de memória num arquivo .im
    std::ofstream outFile (fileName + ".im");
    for (unsigned int i = 0; i < machineCode.size(); ++i)
        outFile << machineCode[i] << " ";
    outFile.close();

    // mostra a imagem de memória na tela
    displayChunks(machineCode, minChunkList);
    
    return 0;
    
}