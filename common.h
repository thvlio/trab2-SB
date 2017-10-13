/*      COMMON.H: arquivo contendo as definições das funções comuns     */



/*      DECLARAÇÕES DAS FUNÇÕES     */
int errorCheck (int, char**, std::string, std::string);
std::string o2pre (std::string);
std::string o2mcr (std::string);
std::vector<Instr> getInstrList (std::string);
std::vector<Dir> getDirList (std::string);
int integerCheck (std::string&, int&);
void reportError (std::string, std::string, int);



/*      DEFINIÇÕES DAS FUNÇÕES     */

/*
errorCheck: verifica se ha algum erro nos argumentos de entrada do programa ou no arquivo do codigo de entrada
entrada: argc e argv recebidos pela funcao main()
saida: um inteiro indicando se houve erro (0 se nao, -1 se sim)
*/
int errorCheck (int argc, char *argv[], std::string instrFileName, std::string dirFileName) {
    
    // verifica o numero de argumentos dados
    if (argc != 4) {
        std::cout << "Número inválido de argumentos: " << argc-1 << " (3 esperados)" << "\n";
        return -1;
    }
    
    // guarda os argumentos em strings
    std::string operation (*(argv+1)),
        inFileName (*(argv+2)),
        outFileName (*(argv+3));
    
    // verifica se a operacao eh valida
    if (operation != "-p" && operation != "-m" && operation != "-o") {
        std::cout << "Operação inválida: " << operation << "\n";
        return -1;
    }
    
    // verifica se as extensao do arquivo de entrada eh .asm
    std::string inExt = inFileName.substr(inFileName.size() - 4);
    if (inExt != ".asm") {
        std::cout << "Extensão do arquivo de entrada não suportada (somente .asm)" << "\n";
        return -1;
    }
    
    // verifica se as extensao do arquivo de saida eh .o
    std::string outExt = outFileName.substr(outFileName.size() - 2);
    if (outExt != ".o") {
        std::cout << "Extensão do arquivo de saída não suportada (somente .o)" << "\n";
        return -1;
    }
    
    // verifica se o arquivo de entrada existe
    std::ifstream asmFile (inFileName);
    if (!asmFile.is_open()) {
        std::cout << "Erro ao abrir o arquivo de entrada: " << inFileName << "\n";
        return -1;
    } else
        asmFile.close();
        
    // verifica se o arquivo com a lista de instrucoes existe
    std::ifstream instrFile (instrFileName);
    if (!instrFile.is_open()) {
        std::cout << "Erro ao abrir a tabela de instruções: " << instrFileName << "\n";
        return -1;
    } else
        instrFile.close();
    
    // verifica se o arquivo com a lista de diretivas existe
    std::ifstream dirFile (dirFileName);
    if (!dirFile.is_open()) {
        std::cout << "Erro ao abrir a tabela de diretivas: " << dirFileName << "\n";
        return -1;
    } else
        dirFile.close();
    
    return 0;
}



/*
o2pre: passa uma string com extensao '.o' para extensao '.pre'
entrada: string com o nome original com a extensao '.o'
saida: string com o nome alterado com a extensao '.pre'
*/
std::string o2pre (std::string original) {
    
    std::string altered (original);
    
    altered.pop_back();
    altered.append("pre");
    
    return altered;
}



/*
o2mcr: passa uma string com extensao '.o' para extensao '.mcr'
entrada: string com o nome original com a extensao '.o'
saida: string com o nome alterado com a extensao '.mcr'
*/
std::string o2mcr (std::string original) {
    
    std::string altered (original);
    
    altered.pop_back();
    altered.append("mcr");
    
    return altered;
}



/*
getInstrList: constroi a tabela de instrucoes num vetor 
entrada: nome do arquivo que contem a tabela
saida: vetor com a lista de instrucoes
*/
std::vector<Instr> getInstrList (std::string instrFileName) {
    
    // abre o arquivo
    std::ifstream instrFile (instrFileName);
    
    // cria uma lista de instrucoes
    std::vector<Instr> instrList;
    
    while (!instrFile.eof()) {
        
        std::string name; // nome da instrucao
        int opcode, // opcode
            numArg; // num de operandos
        
        // le o nome da instrucao
        instrFile >> name;
        
        // '#' serve para indicar "comentarios" 
        if (name != "#") {
            
            // le o num de operandos e o opcode
            instrFile >> numArg;
            instrFile >> opcode;
            
            // cria uma instrucao com essas caracteristicas e salva no vetor
            Instr instr (name, opcode, numArg);
            instrList.push_back(instr);
            
        } else {
            // le o resto da linha e ignora
            getline(instrFile, name);
        }
            
    }
    
    instrFile.close();
    return instrList;
}



/*
getDirList: constroi a tabela de diretivas num vetor 
entrada: nome do arquivo que contem a tabela
saida: vetor com a lista de diretivas
*/
std::vector<Dir> getDirList (std::string dirFileName) {
    
    // abre o arquivo e cria uma lista de diretivas vazia
    std::ifstream dirFile (dirFileName);
    std::vector<Dir> dirList;
    
    while (!dirFile.eof()) {
        
        std::string name;
        dirFile >> name;
        
        // novamente, '#' indica "comentarios"
        if (name != "#") {
            // cria uma diretiva e salva no vetor
            Dir dir (name);
            dirList.push_back(dir);
        } else
            getline(dirFile, name); // le o resto da linha e ignora
    
    }
    
    dirFile.close();
    return dirList;
}



/*
integerCheck: checa se a string pode ser convertida em um numero sem erros
entrada: string a ser convertida e inteiro que armazenará o resultado
saida: inteiro que determina se houve ou não erro na conversão
*/
int integerCheck (std::string &value, int &conv) {
    
    char *ptr; // ponteiro para um caracter
    const char *cValue = value.c_str(); // array de caracteres equivalente à string
    conv = strtol(cValue, &ptr, 10); // converte o array para um numero e altera ptr para o ultimo caracter lido
    
    // se o ultimo caracter lido foi um '\0', entao a string foi convertida em um numero sem nenhum problema
    return (*ptr == '\0'); 
    
}



/*
reportError: mostra no terminal a mensagem de erro passada pelo programa, junto com o tipo de erro e a linha
entrada: mensagem de erro, tipo de erro e linha do erro
saida: nenhuma (mensagem de erro no terminal)
*/
void reportError (std::string message, std::string type, int line) {
    std::cout << "Na linha " << line << " do arquivo de entrada:" << "\n";
    std::cout << "\tErro: " << message << " (erro " << type << ")" << "\n\n";   
}



/*
labelCheck: verifica se o rotulo eh valido
entrada: rotulo a ser verificado
saida: inteiro indicando se rotulo é válido
*/
int labelCheck (std::string label) {
    
    // checa o tamanho da string
    if (label.size() > 100)
        return -1;

    // checa se o primeiro caracter é um número
    if (label.front() >= '0' && label.front() <= '9')
        return -2;
    
    // verifica se só existem números, caracteres ou _ na string
    for (int i = 0; i < label.size(); ++i) {
        if ((label[i] < 'A' || label[i] > 'Z') && (label[i] < '0' || label[i] > '9') && (label[i] != '_'))
            return -3; 
    }
        
    return 0;

}