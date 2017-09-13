#include "header.h"

// compilar com
// g++ -std=c++11 main.cpp -o main

int main () {
    
    std::ifstream asmFile ("arquivos teste - moodle/triangulo.asm");
    
    if (asmFile.is_open()) {
        
        while (!asmFile.eof()) {
            
            std::string line;
            getline(asmFile, line);
            std::cout << "linha: " << line << "\n";
            
        }
        
        asmFile.close();
        
    } else
        std::cout << "Erro ao abrir o arquivo." << "\n";
    
    return 0;
    
}