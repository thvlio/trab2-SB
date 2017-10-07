/*      FUNCTIONS.H: arquivo contendo as definições das funções     */



/*      DECLARAÇÕES DAS FUNÇÕES     */

int errorCheck (int, char**, std::string, std::string);
std::string o2pre (std::string);
std::string o2mcr (std::string);
std::vector<Instr> getInstrList (std::string);
std::vector<Dir> getDirList (std::string);
std::string preReadLine (std::ifstream&);
std::string searchAndReplace (std::vector<Label>, std::string);
void preProcessFile (std::string, std::string);
void expandMacros (std::string, std::string);
void assembleCode (std::string, std::string);



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
    
    std::ifstream instrFile (instrFileName);
    std::vector<Instr> instrList;
    
    while (!instrFile.eof()) {
        
        std::string name;
        int opcode,
            numArg;
            
        instrFile >> name;
        // std::cout << "Name: " << name << "\n";
        if (name != "#") {
            instrFile >> numArg;
            instrFile >> opcode;
            // std::cout << "numeros: " << opcode << ", " << numArg << "\n";
            Instr instr (name, opcode, numArg);
            instrList.push_back(instr);
        } else
            getline(instrFile, name); // le o resto da linha e ignora
    
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
    
    std::ifstream dirFile (dirFileName);
    std::vector<Dir> dirList;
    
    while (!dirFile.eof()) {
        
        std::string name;
            
        dirFile >> name;
        // std::cout << "Name: " << name << "\n";
        if (name != "#") {
            Dir dir (name);
            dirList.push_back(dir);
        } else
            getline(dirFile, name); // le o resto da linha e ignora
    
    }
    
    dirFile.close();
    return dirList;
}

/*
preReadLine: le uma linha do arquivo na etapa de preprocessamento
entrada: stream contendo a linha a ser lida (passada por referencia)
saida: string com os dados lidos
*/
std::string preReadLine (std::ifstream &input)  {
    
    // le uma linha do arquivo
    std::string line;
    getline(input, line);
    
    // transforma a string p caixa alta
    std::transform (line.begin(), line.end(), line.begin(), ::toupper);
    
    // ignora os comentarios, se houver algum (e retira os espaços em branco)
    std::stringstream lineStream (line);
    getline(lineStream, line, ';');
    
    // retira os espaços em branco, caso existam, no final da linha
    while (line.back() == ' ' || line.back() == '\t' || line.back() == '\n' || line.back() == '\v' || line.back() == '\f' || line.back() == '\r')
        line.pop_back();
    
    // retira os espaços em branco, caso existam, no começo da linha
    while (line.front() == ' ' || line.front() == '\t' || line.front() == '\n' || line.front() == '\v' || line.front() == '\f' || line.front() == '\r')
        line = line.substr(1);
    
    return line;    
}

/*
searchAndReplace: procura um rotulo numa linha e substitui pela definicao
entrada: lista de rotulos definidos e a linha
saida: nova linha alterada
*/
std::string searchAndReplace (std::vector<Label> list, std::string line) {
    
    for (int i = 0; i < list.size(); ++i) {
        
        // procura pelo nome do rotulo na linha
        std::size_t pos = line.find(list[i].name);
        
        // se encontrar em alguma posicao, substitui pelo numero associado ao rotulo
        if (pos != std::string::npos) {
            line.replace (pos, list[i].name.size(), std::to_string(list[i].value));
        }
    }
    
    return line;
    
}

/*
preProcessFile: faz a passagem de preprocessamento no arquivo, que inclui:
    - passa tudo para caixa alta
    - ignora comentarios
    - avalia EQU e IF
    - (detectar erros blabla)
entrada: nome do arquivo de entrada '.asm'
saida: nome do arquivo de saida '.pre'
*/
void preProcessFile (std::string inFileName, std::string preFileName) {
    
    std::ifstream asmFile (inFileName);
    std::ofstream preFile (preFileName);
    
    std::vector<Label> labelList;

    while (!asmFile.eof()) {
        
        std::string line = preReadLine (asmFile); // le uma linha (e corrige algumas coisas)
        
        line = searchAndReplace (labelList, line); // procura na linha por rotulos que ja tenham sido definidos por equs
        
        std::stringstream lineStream (line); // cria um stream para a leitura de tokens
        
        std::string token;
        lineStream >> token; // le um token
        
        if (token.back() == ':') { // se for ':', entao ta definindo um label
        
            std::string token2; // pega o token seguinte
            lineStream >> token2;
            
            if (token2.empty()) {
                
                // se token2 estiver vazio, anexa a proxima linha
                std::string nextLine = preReadLine (asmFile);
                nextLine = searchAndReplace (labelList, nextLine);
                line = line + " " + nextLine;
                
                // e corrige a stream da linha
                lineStream.str(nextLine); // salva a linha anexada como stream
                lineStream.clear(); // limpa as flags de estado
                lineStream >> token2; // le token2 de novo
                
            }
            
            if (token2 == "EQU") { // se for equ, pega o valor seguinte e associa o valor ao label
                
                int value;
                lineStream >> value;
                
                token.pop_back(); // apaga o ':'
                Label label (token, value); // cria o label com o valor associado
                labelList.push_back(label); // coloca o label na lista de labels definidos
                
                line.clear(); // esvazia a string p nao salvar a linha do equ no codigo
                
            }
            
        } else if (token == "IF") {
            
            int value; // le o numero seguinte (a busca ja trocou o label por um valor)
            lineStream >> value;
            
            if (value == 0)
                preReadLine (asmFile); // le a proxima linha do arquivo e nao salva
            line.clear(); // limpa a linha atual, ja que era um if
                
        }
            
        // se a linha nao estiver vazia, copia no arquivo '.pre'        
        if (!line.empty())
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