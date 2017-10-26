/*      COMMON.H: arquivo contendo as definições das funções comuns     */



/*      DECLARAÇÕES DAS FUNÇÕES     */
int errorCheck (int, char**, std::string, std::string);
std::string asm2pre (std::string);
std::string asm2mcr (std::string);
std::string asm2o (std::string);
std::vector<Instr> getInstrList (std::string);
std::vector<Dir> getDirList (std::string);
int integerCheck (std::string, int&);
void reportError (std::string, std::string, int, std::string);
int labelCheck (std::string, std::vector<Instr>&, std::vector<Dir>&, int&);
void reportList (std::vector<Error>&, std::string);
bool operator< (const Error&, const Error&);


/*      DEFINIÇÕES DAS FUNÇÕES     */

/*
errorCheck: verifica se ha algum erro nos argumentos de entrada do programa, nos arquivos de entrada fornecidos e nos arquivos das tabelas de instruções e diretivas
entradas: argc e argv e os nomes dos arquivos com as tabelas de instruções e diretivas
saída: 0 se não deu erro, -1 se deu erro
*/
int errorCheck (int argc, char *argv[], std::string instrFileName, std::string dirFileName) {
    
    // verifica o numero de argumentos dados
    if (argc < 2 || argc > 4) {
        std::cout << "Devem ser fornecidos 1, 2 ou 3 arquivos de entrada. (" << argc-1 << " fornecidos)\n";
        return -1;
    }
    
    // guarda os argumentos em strings
    std::vector<std::string> inFileNames;
    for (int i = 1; i < argc; ++i)
        inFileNames.push_back(std::string(*(argv+i)));
    
    // verifica se as extensao dos arquivos de entrada são .asm
    bool extError = false;
    for (unsigned int i = 0; i < inFileNames.size(); ++i) {
        if (inFileNames[i].size() < 5) {
            std::cout << "Extensão do arquivo de entrada " << inFileNames[i] << " não suportada (somente .asm)" << "\n";
            extError = true;
        } else {
            std::string inExt = inFileNames[i].substr(inFileNames[i].size() - 4);
            if (inExt != ".asm") {
                std::cout << "Extensão do arquivo de entrada " << inFileNames[i] << " não suportada (somente .asm)" << "\n";
                extError = true;
            }
        }
    }
    if (extError)
        return -1;
        
    // verifica se os arquivos de entrada existem
    bool openError = false;
    for (unsigned int i = 0; i < inFileNames.size(); ++i) {
        std::ifstream asmFile (inFileNames[i]);
        if (!asmFile.is_open()) {
            std::cout << "Erro ao abrir o arquivo de entrada " << inFileNames[i] << "\n";
            openError = true;
        } else
            asmFile.close();
    }
    if (openError)
        return -1;
        
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
asm2pre: passa uma string com extensao '.asm' para extensao '.pre'
entrada: string com o nome original com a extensao '.asm'
saida: string com o nome alterado com a extensao '.pre'
*/
std::string asm2pre (std::string name) {
    
    name.erase(name.end()-3, name.end());
    name.append("pre");
    return name;
    
}



/*
asm2mcr: passa uma string com extensao '.asm' para extensao '.mcr'
entrada: string com o nome original com a extensao '.asm'
saida: string com o nome alterado com a extensao '.mcr'
*/
std::string asm2mcr (std::string name) {
    
    name.erase(name.end()-3, name.end());
    name.append("mcr");
    return name;
    
}



/*
asm2o: passa uma string com extensao '.asm' para extensao '.o'
entrada: string com o nome original com a extensao '.asm'
saida: string com o nome alterado com a extensao '.o'
*/
std::string asm2o (std::string name) {
    
    name.erase(name.end()-3, name.end());
    name.append("o");
    return name;
    
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
saida: inteiro que determina se houve ou não erro na conversão (1 ok, 0 erro)
*/
int integerCheck (std::string value, int &conv) {
    
    char *ptr = nullptr; // ponteiro para um caracter
    const char *cValue = value.c_str(); // array de caracteres equivalente à string
    conv = strtol(cValue, &ptr, 10); // converte o array para um numero e altera ptr para o ultimo caracter lido
    
    // se o ultimo caracter lido foi um '\0', entao a string foi convertida em um numero sem nenhum problema
    return (*ptr == '\0'); 
    
}



/*
labelCheck: verifica se o rotulo eh valido
entrada: rotulo a ser verificado
saida: inteiro indicando se rotulo é válido
*/
int labelCheck (std::string label, std::vector<Instr> &instrList, std::vector<Dir> &dirList, int &pos) {
    
    // checa se o rótulo está vazio
    if (label.empty())
        return -5;
    
    // checa o tamanho da string
    if (label.size() > 100) {
        pos = label.size()-1;
        return -1;
    }
    
    // checa se o primeiro caracter é um número
    if (label.front() >= '0' && label.front() <= '9')
        return -2;
    
    // verifica se só existem números, caracteres ou _ na string
    for (unsigned int i = 0; i < label.size(); ++i) {
        if ((label[i] < 'A' || label[i] > 'Z') && (label[i] < '0' || label[i] > '9') && (label[i] != '_')) {
            pos = i;
            return -3; 
        }
    }
    
    // verifica se o rótulo tem o nome de uma instrução
    for (unsigned int i = 0; i < instrList.size(); ++i) {
        if (label == instrList[i].name)
            return -4;
    }
    
    // verifica se o rótulo tem o nome de uma diretiva
    for (unsigned int i = 0; i < dirList.size(); ++i) {
        if (label == dirList[i].name)
            return -4;
    }
        
    return 0;
}



/*
reportList: reporta todos os erros, na ordem das linhas. mostra no terminal a mensagem de erro passada pelo programa, junto com o tipo de erro e a linha
entrada: lista de erros e nome do arquivo de entrada
saida: nenhuma (erros no terminal)
*/
void reportList (std::vector<Error> &errorList, std::string fileName) {
    
    // configura algumas cores
    std::string escRed = "\033[31;1m",
    escGreen = "\033[32;1m",
    escYellow = "\033[33;1m",
    escBlue = "\033[34;1m",
    escReset = "\033[0m";
    
    // mostra os erros
    for (unsigned int i = 0; i < errorList.size(); ++i) {
        
        // erro atual
        Error error = errorList[i];
        
        // offset para ajustar o apontador de erro na linha
        std::string offset;
        for (int i = 0; i < error.pos; ++i)
            offset.push_back(' ');
        
        // para o caso de não ter linha específica
        if (error.lineNum == -1) {
            std::cout << escRed << "Erro" << escReset << " no arquivo de entrada " << escGreen << fileName << escReset << ": " << escYellow << error.message << escReset << " (erro " << error.type << ")" << "\n\n";
        
        // quando tem linha específica
        } else {
            std::cout << escRed << "Erro" << escReset << " na linha " << escRed << error.lineNum << escReset << " no arquivo de entrada " << escGreen << fileName << escReset << ": " << escYellow << error.message << escReset << " (erro " << error.type << ")" << "\n";
            std::cout << "\t" << escBlue << error.line << escReset << "\n";
            std::cout << "\t" << offset << escGreen << "^" << escReset << "\n\n";
        }
        
    }
    
}



/*
operator<: overload do operator < para poder usar std::sort
entrada: duas structs do tipo Error
saida: se a primeira é menor que a segunda (compara o num de linhas)
*/
bool operator< (const Error &A, const Error &B) {
    return (A.lineNum < B.lineNum);
}