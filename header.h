/*      BIBLIOTECAS     */

#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <string> // std::string
#include <sstream> // std::stringstream
#include <algorithm> // ::toupper



/*      CABEÇALHOS      */

int errorCheck (int, char**);
std::string o2pre (std::string);
std::string o2mcr (std::string);
void preProcessFile (std::string, std::string);
void expandMacros (std::string, std::string);
void assembleCode (std::string, std::string);



/*      FUNÇÕES     */

int errorCheck (int argc, char *argv[]) {
    
    if (argc != 4) {
        std::cout << "Número inválido de argumentos: " << argc-1 << "\n";
        return -1;
    }
    
    std::string operation (*(argv+1));
    
    if (operation != "-p" && operation != "-m" && operation != "-o") {
        std::cout << "Operação inválida: " << operation << "\n";
        return -1;
    }
    
    std::string inFileName (*(argv+2));
    std::ifstream asmFile (inFileName);
    
    if (!asmFile.is_open()) {
        std::cout << "Erro ao abrir o arquivo de entrada: " << inFileName << "\n";
        return -1;
    } else
        asmFile.close();
    
    return 0;
}

std::string o2pre (std::string original) {
    
    std::string altered (original);
    
    altered.pop_back();
    altered.append("pre");
    
    return altered;
}

std::string o2mcr (std::string original) {
    
    std::string altered (original);
    
    altered.pop_back();
    altered.append("mcr");
    
    return altered;
}

void preProcessFile (std::string inFileName, std::string preFileName) {
    
    std::ifstream asmFile (inFileName);
    std::ofstream preFile (preFileName);

    while (!asmFile.eof()) {
        
        std::string line;
        getline(asmFile, line);
        
        // transforma a string p caixa alta
        std::transform (line.begin(), line.end(), line.begin(), ::toupper);
        
        preFile << line << "\n";
    }
    
    asmFile.close();
    preFile.close();
    
    return;
}

void expandMacros (std::string preFileName, std::string mcrFileName) {
    
    std::ifstream preFile (preFileName);
    std::ofstream mcrFile (mcrFileName);

    while (!preFile.eof()) {
        std::string line;
        getline(preFile, line);
        mcrFile << line << "\n";
    }
    
    preFile.close();
    mcrFile.close();
    
    return;
}

void assembleCode (std::string mcrFileName, std::string outFileName) {
    
    std::ifstream mcrFile (mcrFileName);
    std::ofstream outFile (outFileName);

    while (!mcrFile.eof()) {
        
        std::string line;
        getline(mcrFile, line);
        // std::cout << "linha: " << line << "\n";
        
        std::stringstream ss (line);
        
        while (!ss.eof()) {
            std::string token;
            ss >> token;
            // std::cout << "\ttoken: " << token << "\n";
        }
        
        outFile << line << "\n";
    }
    
    mcrFile.close();
    outFile.close();
    
    return;
}