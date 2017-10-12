/*      PRE.H: funções relativas à montagem do codigo fonte        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
void assembleCode (std::string, std::string);



/*      DEFINIÇÕES DAS FUNÇÕES      */
/*
assembleCode: faz a passagem de montagem no arquivo, que inclui:
    - (todo o processo de passagem unica)
    - (detectar erros blabla)
entrada: nome do arquivo de entrada '.mcr'
saida: nome do arquivo de saida '.o'
*/
void assembleCode (std::string mcrFileName, std::string outFileName) {
    
    std::ifstream mcrFile (mcrFileName);
    std::ofstream outFile (outFileName);

    while (!mcrFile.eof()) {
        
        std::string line;
        getline (mcrFile, line);
        outFile << line;
        // algoritmo de passagem unica
    }
    
    mcrFile.close();
    outFile.close();
    
    return;
}