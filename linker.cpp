#include "include/linker/types.h"
#include "include/linker/lkr.h"

// compilar com
// g++ -std=c++11 -Wall linker.cpp -o linker.out
// ou entao com CTRL SHIFT B no VSCODE

// rodar com
// ./linker.out xxx.o yyy.o zzz.o

int main (int argc, char *argv[]){    
    
    if(argc == 1){
        std::cout << "\nO usuario nao entrou com arquivos para o processo de ligacao \n\n";
        
    }else if(argc == 2){
        std::cout << "\nO arquivo nao necessita ser ligado";

        oneObject(std::string(*(argv+1)));
            
    }else if(argc > 2){

        // coloca os argumentos em strings        
        std::vector<std::string> inFileNames;
        for (int i = 1; i < argc; ++i)
            inFileNames.push_back(std::string(*(argv+i)));

        // define o nome do arquivo de saída
        std::string outFileName;
        for (unsigned int i = 0; i < (inFileNames[0].size() - 2); i++)
            outFileName.push_back(inFileNames[0][i]);

        
        std::vector<tDef> defTable;
        int offset = 0;
        std::vector<int> offset_vector (3);   
            
        //Cria a tabela de definição geral        
        for (int i = 0 ; i < (argc-1); i++){
            DefLoad(inFileNames[i], defTable, offset);     
            offset_vector[i+1] = offset;  //pega o offset de cada módulo            
        }                  
        
        std::string bitMap;
        std::vector<int> machineCode;

        // junta os mapas de bits e os codigos de maquina
        for(int i = 0; i < (argc-1); i++)
            mergeLines(inFileNames[i], offset_vector[i], bitMap, machineCode);           
                   
        // liga o arquivo
        linkerCode(inFileNames, defTable, offset_vector, outFileName, bitMap, machineCode);
        
        std::cout << "\narquivos ligados: " << (argc-1) << "\n\nnome do arquivo gerado: " << outFileName << "\n\n";
    
    }
    
}