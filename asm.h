/*      PRE.H: funções relativas à montagem do codigo fonte        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
int constCheck (std::string&, int&);
std::vector<int> asmParser (std::ifstream&, std::vector<Label>&, int&, int&, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&);
void assembleCode (std::string, std::string, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
constCheck: checa se a string pode ser convertida em um numero (decimal ou hexa, negativo ou positivo) sem erros
entrada: string a ser convertida e inteiro que armazenará o resultado
saida: inteiro que determina se houve ou não erro na conversão
*/
int constCheck (std::string &value, int &conv) {
    
    char *ptr; // ponteiro para o ultimo caracter lido por strtol
    const char *cValue = value.c_str(); // array de caracteres equivalente à string
    
    // converte o array para um numero decimal e altera ptr para o ultimo caracter lido
    conv = strtol(cValue, &ptr, 10);
    
    // se o numero nao eh um numero decimal valido, verifica se eh um numero em hexa valido
    if (*ptr != '\0') {
        
        // se for um 'H' no final, troca por '0X' no começo, que strtol entende como hexa
        if (value.back() == 'H') {
            
            value.pop_back();
            if (value.front() == '-')
                value = "-0X" + value.substr(1);
            else
                value = "0X" + value;
                
            const char *cValue2 = value.c_str(); // gera novamente o array de caracteres
            conv = strtol(cValue2, &ptr, 16); // converte o array para um numero hexa
            
        } else
            conv = strtol(cValue, &ptr, 16);
        
    }
    
    // se o ultimo caracter lido foi um '\0', entao a string foi convertida em um numero (decimal ou hexa)
    return (*ptr=='\0');
    
}



/*
asmParser: traduz uma linha em código máquina
entrada:
saida: código de máquina parcial num vetor de inteiros
*/
std::vector<int> asmParser (std::ifstream &mcrFile, std::vector<Label> &labelList, int &lineCounter, int &addrCounter, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList) {
    
    std::string line;
    getline (mcrFile, line);
    std::stringstream lineStream (line);
    
    std::string token;
    lineStream >> token;
    
    if (token.back() == ':') {
        
        // verifica se o rótulo é válido
        token.pop_back();
        int valid = labelCheck(token, instrList, dirList);
        if (valid == -1)
            reportError("tamanho o rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineDict[lineCounter-1], line);
        else if (valid == -2)
            reportError("rótulos não podem começar com números", "léxico", lineDict[lineCounter-1], line);
        else if (valid == -3)
            reportError("caracter inválido encontrado no rótulo", "léxico", lineDict[lineCounter-1], line);
        else if (valid == -4)
            reportError("rótulo não pode ter nome de instrução ou diretiva", "semântico", lineDict[lineCounter-1], line);
        
        // verifica se o rótulo já está na lista de rótulos e se já foi definido
        int alreadyDefined = 0;
        int alreadyMentioned = 0;
        for (int i = 0; i < labelList.size(); ++i) {
            if (labelList[i].name == token) {
                alreadyMentioned = 1;
                if (labelList[i].defined)
                    alreadyDefined = 1;
            }
        }
        
        // ja foi definido (da erro de simbolo ja definido)
        if (alreadyDefined)
            reportError("símbolo já definido", "léxico", lineDict[lineCounter-1], line);
            
        // nao foi definido mas ja foi mencionado ()
        else if (alreadyMentioned) {
            
        } 
            
        
        // se ainda nao está na lista, coloca na lista (name, value, defined, pending)
        Label label (token, 0, 0, addrCounter);
        
        // coloca o label na tabela de simbolos e procura por erros
        // simbolo repetido, labelcheck
        // agora cata mais um token
    }
    
    // analisar o primeiro token e ver se eh instrucao ou diretiva
        // procurar na tabela de instrucoes e diretivas
    
    // se for const space ou section cada um faz uma coisa
        // depois q fizer o q tem q fazer vai pra proxima linha
        
    // se for instrucao verificar quantos argumentos e tal, e ler da linha
    
}



/*
assembleCode: faz a passagem de montagem no arquivo, que inclui:
    - (todo o processo de passagem unica)
    - (detectar erros blabla)
entrada: nome do arquivo de entrada '.mcr'
saida: nome do arquivo de saida '.o'
*/
void assembleCode (std::string mcrFileName, std::string outFileName, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList) {
    
    std::ifstream mcrFile (mcrFileName);
    std::ofstream outFile (outFileName);
    
    std::vector<Label> labelList;
    
    int lineCounter = 1;
    int addrCounter = 0;
    
    while (!mcrFile.eof()) {
        
        std::vector<int> partialMachineCode = asmParser(mcrFile, labelList, lineCounter, addrCounter, lineDict, instrList, dirList);
        
        for (int i = 0; i < partialMachineCode.size(); ++i)
            outFile << partialMachineCode[i];
        
    }
    
    mcrFile.close();
    outFile.close();
    
    return;
}