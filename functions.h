/*      FUNCTIONS.H: arquivo contendo as definições das funções     */



/*      DECLARAÇÕES DAS FUNÇÕES     */

// funções gerais
int errorCheck (int, char**, std::string, std::string);
std::string o2pre (std::string);
std::string o2mcr (std::string);
std::vector<Instr> getInstrList (std::string);
std::vector<Dir> getDirList (std::string);

// funções de preprocessamento
std::string preReadLine (std::ifstream&);
std::string searchAndReplace (std::vector<Label>, std::string);
void preProcessFile (std::string, std::string);

// funções de processamento de macros
void expandMacros (std::string, std::string);

// funções de montagem
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
        
        // le uma linha (e corrige algumas coisas)
        std::string line = preReadLine (asmFile);
        
        // cria um stream para a leitura de tokens
        std::stringstream lineStream (line);
        
        // cria a string token e le um token
        std::string token;
        lineStream >> token;
        
        // token != "IF" preserva o rotulo na diretiva IF (para detectar erros)
        if (token != "IF") {
            // procura na linha por rotulos que ja tenham sido definidos por equs
            line = searchAndReplace (labelList, line); 
            // coloca a linha com os rotulos substituidos na stream
            lineStream.str(line); 
            // rele o primeiro token para colocar o ponteiro de leitura no mesmo lugar
            lineStream >> token;
        }
        
        // se o ultimo caracter for ':', entao ta definindo um rotulo
        if (token.back() == ':') {
            
            // apaga o ':'
            token.pop_back();
            
            // pega o token seguinte
            std::string token2;
            lineStream >> token2;
            
            // se token2 estiver vazio, anexa a proxima linha
            if (token2.empty()) {
                
                // le e anexa à linha atual a proxima linha
                std::string nextLine = preReadLine (asmFile);
                nextLine = searchAndReplace (labelList, nextLine);
                line = line + " " + nextLine;
                
                // e corrige a stream da linha
                lineStream.str(nextLine); // salva a linha anexada como stream
                lineStream.clear(); // limpa as flags de estado
                lineStream >> token2; // le token2 de novo
                
            }
            
            // se for equ, pega o valor seguinte e associa o valor ao rotulo
            if (token2 == "EQU") {
                
                int value;
                lineStream >> value;
                
                Label label (token, value); // cria o rotulo com o valor associado
                labelList.push_back(label); // coloca o rotulo na lista de rotulos definidos
                
                // esvazia a string p nao salvar a linha do equ no codigo
                line.clear();
                
            }
            
        } else if (token == "IF") {
                        
            // pega o token seguinte
            std::string token2; 
            lineStream >> token2;
            
            // procura o token2 lido na lista de rotulos definidos
            int found = 0; 
            Label label;
            for (int i = 0; ((i < labelList.size()) && (!found)); ++i) {
                if (labelList[i].name == token2) {
                    found = 1;
                    label = labelList[i]; // salva o rotulo encontrado
                }
            }
            
            // se encontrou o rotulo, executa a diretiva
            if (found) {
                if (label.value != 1)
                    preReadLine (asmFile); // le a proxima linha do arquivo e nao salva
                line.clear(); // limpa a linha atual, ja que era um if
            }
                
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

    std::vector<Macro> macroList;
    
    while (!preFile.eof()) {
        
        // le uma linha do arquivo
        std::string line; 
        getline(preFile, line);
        
        // cria o stream para a linha e le um token
        std::stringstream lineStream (line); 
        std::string token;
        lineStream >> token;
        
        // se o ultimo char for ':', esta definindo um rotulo
        if (token.back() == ':') {
            
            // apaga o ':'
            token.pop_back();
            
            // le o token seguinte
            std::string token2;
            lineStream >> token2;
            
            if (token2 == "MACRO") {
                
                // cria uma string p guardar a definicao da macro
                std::string def;
                
                // indica se achou end indicando o final da macro
                int reachedEnd = 0;
                
                // laco que vai armazenando todas as linhas ate o final da macro
                while (!reachedEnd) {
                    
                    // a proxima linha eh lida
                    std::string auxLine;
                    getline (preFile, auxLine);
                    
                    if (auxLine != "END") {
                        // se nao for o final, anexa na definicao
                        def = def + auxLine + '\n';
                    } else {
                        // se for o final, ajusta a flag e tira o '\n' do final da definicao
                        reachedEnd = 1;
                        def.pop_back();
                    }
                }
                
                // armazena a macro na lista de macros
                Macro macro (token, def);
                macroList.push_back(macro);
                
                // nao escreve a linha que define o rotulo da macro
                line.clear();
                
            }
            
        } else {
            
            // procura o token lido na lista de macros
            int found = 0;
            for (int i = 0; ((i < macroList.size()) && (!found)); ++i) {
                if (token == macroList[i].name) {
                    // se encontrar, escreve a definicao no lugar da linha atual
                    line = macroList[i].def;
                    found = 1;
                }
            }
            
        }
        
        // se a linha nao estiver vazia, copia no arquivo '.mcr'        
        if (!line.empty())
            mcrFile << line << "\n";
        
    }
    
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
        
        std::stringstream ss (line);
        
        while (!ss.eof()) {
            std::string token;
            ss >> token;
        }
        
        outFile << line << "\n";
    }
    
    mcrFile.close();
    outFile.close();
    
    return;
}