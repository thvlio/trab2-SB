/*      FUNCTIONS.H: arquivo contendo as definições das funções     */



/*      DECLARAÇÕES DAS FUNÇÕES     */

int errorCheck (int, char**);
std::string o2pre (std::string);
std::string o2mcr (std::string);
void preProcessFile (std::string, std::string);
void expandMacros (std::string, std::string);
void assembleCode (std::string, std::string);



/*      DEFINIÇÕES DAS FUNÇÕES     */

/*
errorCheck: verifica se ha algum erro nos argumentos de entrada do programa ou no arquivo do codigo de entrada
entrada: argc e argv recebidos pela funcao main()
saida: um inteiro indicando se houve erro (0 se nao, -1 se sim)
*/
int errorCheck (int argc, char *argv[]) {
    
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
preProcessFile: faz a passagem de preprocessamento no arquivo, que inclui:
    - passa tudo para caixa alta
    - ignora comentarios
    - (avalia EQU e IF)
    - (detectar erros blabla)
entrada: nome do arquivo de entrada '.asm'
saida: nome do arquivo de saida '.pre'
*/
void preProcessFile (std::string inFileName, std::string preFileName) {
    
    std::ifstream asmFile (inFileName);
    std::ofstream preFile (preFileName);

    while (!asmFile.eof()) {
        
        std::string line;
        getline(asmFile, line);
        
        // transforma a string p caixa alta
        std::transform (line.begin(), line.end(), line.begin(), ::toupper);
        
        // ignora os comentarios, se houver algum
        std::stringstream ss1 (line);
        getline(ss1, line, ';');
        
        // stringstream para extrair os tokens da linha
        std::stringstream ss2 (line);
        
        while (!ss2.eof()) {
            std::string token;
            ss2 >> token;
        }
        
        // le os equs e anota os labels
        // tipo, label e valor
        // vai dando pushback num vetor e depois implementa uma busca com std::find?
        
        // vai lendo os labels e literalmente substituindo
        
        // chega nos ifs e verifica se o label tem msm
        // se tiver e for 1, compila a linha de baixo
        // se tiver e for 0, elimina a linha de baixo
        // se nao tiver, nem termina de compilar
        
        // o codigo tem q saber se ta em algum secao, e em qual
        // determinar de alguma forma que a linha eh de um equ
        
        preFile << line << "\n";
    }
    
    asmFile.close();
    preFile.close();
    
    return;
}

/*
expandMacros: faz a passagem para expandir macros no arquivo, que inclui:
    - (substitui as macros)
    - (detectar erros blabla)
entrada: nome do arquivo de entrada '.pre'
saida: nome do arquivo de saida '.mcr'
*/
void expandMacros (std::string preFileName, std::string mcrFileName) {
    
    std::ifstream preFile (preFileName);
    std::ofstream mcrFile (mcrFileName);

    while (!preFile.eof()) {
        std::string line;
        getline(preFile, line);
        mcrFile << line << "\n";
    }
    
    // salva todo o trecho de codigo depois da macro
    // sai colando por ai depois
    
    // tipo: le que tem macro e salva ate o endmacro
    // ai salva como se fosse numa string, todas as linhas
    // em um vetor, no outro salva so nome
    // ai vai lendo macro e batendo com um vetor e puxando de outro
    
    preFile.close();
    mcrFile.close();
    
    return;
}

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