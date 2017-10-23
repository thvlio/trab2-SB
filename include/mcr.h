/*      PRE.H: funções relativas ao preprocessamento        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
int createMacro (std::string&, std::ifstream&, std::string&, std::vector<Macro>&, int&);
void mcrSearchAndReplace (std::string&, std::string&, std::vector<Macro>&, int&);
void mcrParser (std::string&, std::ifstream&, std::vector<Macro>&, int&, int&, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&, std::vector<Error>&);
int expandMacros (std::string, std::string, std::vector<int>&, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&, std::vector<Error>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
createMacro: le a definicao de uma macro e guarda na lista de macros
entrada: linha atual, stream do arquivo .pre, nome da macro, lista de macros e contador de linha
saida: inteiro indicando erro (lista de macros e contador de linhas alterados por referencia)
*/
int createMacro (std::string &line, std::ifstream &preFile, std::string &token, std::vector<Macro> &macroList, int &lineCounter) {
    
    // cria uma string p guardar a definicao da macro
    std::string definition;
    
    // indica se achou end indicando o final da macro
    int reachedEnd = 0;
    
    // linha inicial da macro
    int initLine = lineCounter+1;
    int numLines = 0;
    
    // laco que vai armazenando todas as linhas ate o final da macro
    while (!reachedEnd) {
        
        // a proxima linha eh lida
        std::string auxLine;
        getline (preFile, auxLine);
        lineCounter++;
        
        if (auxLine != "END") {
            // se nao for o final, anexa na definicao
            definition = definition + auxLine + '\n';
            numLines++;
        } else {
            // se for o final, ajusta a flag e tira o '\n' do final da definicao
            reachedEnd = 1;
            if (!definition.empty())
                definition.pop_back();
        }
        
        if (!reachedEnd && preFile.eof())
            return -1;
    }
    
    // armazena a macro na lista de macros
    Macro macro (token, definition, initLine, numLines);
    macroList.push_back(macro);
    
    // nao escreve a linha que define o rotulo da macro
    line.clear();
    
    return 0;
}



/*
mcrSearchAndReplace: procura o token na lista de macros e substitui a macro no codigo
entrada: linha atual, nome da macro sendo chamada, lista de macros, contador de linhas e flag indicando se uma macro foi chamada
saida: nenhuma (linha atual e flag de macro alterada por referencia)
*/
void mcrSearchAndReplace (std::string &line, std::string &token, std::vector<Macro> &macroList, int &macroCall) {
    
    int found = 0;
    macroCall = -1;
    for (unsigned int i = 0; ((i < macroList.size()) && (!found)); ++i) {
        if (token == macroList[i].name) {
            // se encontrar, escreve a definicao no lugar da linha atual
            line = macroList[i].definition;
            found = 1;
            macroCall = i;
        }
    }
}



/*
mcrParser: parser do processamento de macros. procura definicoes e chamadas de macros nas linhas
entrada: linha atual, stream do arquivo .pre, lista de macros, contador de linhas, flag indicando se uma macro foi chamada e dicionário de linhas do preprocessamento
saida: nenhuma (linha atual, contador de linhas e flag de macro alterados por referencia)
*/
void mcrParser (std::string &line, std::ifstream &preFile, std::vector<Macro> &macroList, int &lineCounter, int &macroCall, std::vector<int> &lineDictPre, std::vector<Instr> &instrList, std::vector<Dir> &dirList, std::vector<Error> &errorList) {
    
    // le uma linha do arquivo
    getline(preFile, line);
    
    // cria o stream para a linha e le um token
    std::stringstream lineStream (line);
    std::string token;
    lineStream >> token;
    
    // se o ultimo char for ':', esta definindo um rotulo
    if (token.back() == ':') {
        
        // le o token seguinte
        std::string token2;
        lineStream >> token2;
        
        // checa se o token seguinte é um rótulo
        /*
        if (token2.back() == ':') {
            int pos = (int) lineStream.tellg() - token2.size();
            errorList.push_back(Error ("mais de um rótulo em uma linha", "sintático", lineDictPre[lineCounter-1], line, pos));
        }
        */
        
        // verifica se esse rótulo já foi definido como uma macro
        token.pop_back();
        int redefinition = 0;
        for (unsigned int i = 0; i < macroList.size(); ++i) {
            if (macroList[i].name == token)
                redefinition = 1;
        }
        if (redefinition) {
            int pos = 0;
            errorList.push_back(Error("redefinição de macro", "semântico", lineDictPre[lineCounter-1], line, pos));
        }
        
        // se for uma diretiva de macro, cria uma macro nova na lista
        if (token2 == "MACRO") {
            
            // verifica se o rótulo é válido
            int pos = 0;
            int valid = labelCheck(token, instrList, dirList, pos);
            if (valid == -1)
                errorList.push_back(Error("tamanho o rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineDictPre[lineCounter-1], line, pos));
            else if (valid == -2)
                errorList.push_back(Error("rótulos não podem começar com números", "léxico", lineDictPre[lineCounter-1], line, pos));
            else if (valid == -3)
                errorList.push_back(Error("caracter inválido encontrado no rótulo", "léxico", lineDictPre[lineCounter-1], line, pos));
            else if (valid == -4)
                errorList.push_back(Error("rótulo não pode ter nome de instrução ou diretiva", "semântico", lineDictPre[lineCounter-1], line, pos));
            else if (valid == -5)
                errorList.push_back(Error("declaração de rótulo vazia", "sintático", lineDictPre[lineCounter-1], line, pos));
            
            // cria uma macro na lista
            int status = createMacro (line, preFile, token, macroList, lineCounter);
            if (status == -1) {
                pos = 0;
                errorList.push_back(Error("a definição de uma macro deve terminar com END", "semântico", lineDictPre[lineCounter-1], line, pos));
            }
                
            
        }
    
    // se nao for definicao de rotulo, eh uma linha que pode ou nao estar chamando uma macro
    } else
        mcrSearchAndReplace (line, token, macroList, macroCall);
    
}



/*
expandMacros: faz a passagem para expandir macros no arquivo, que inclui:
    - substitui as macros
    - (detectar erros)
entrada: nome do arquivo de entrada '.pre', nome do arquivo de saida '.mcr' e dicionario de linhas
saida: inteiro representando a ocorrencia de erro
*/
int expandMacros (std::string preFileName, std::string mcrFileName, std::vector<int> &lineDictMcr, std::vector<int> &lineDictPre, std::vector<Instr> &instrList, std::vector<Dir> &dirList, std::vector<Error> &errorList) {
    
    std::ifstream preFile (preFileName);
    std::ofstream mcrFile (mcrFileName);

    std::vector<Macro> macroList;
    
    int lineCounter = 1;
    int macroCall = -1;
    
    while (!preFile.eof()) {
        
        // chama o parser da passagem de macros
        std::string line;
        mcrParser(line, preFile, macroList, lineCounter, macroCall, lineDictPre, instrList, dirList, errorList);
        
        // se a linha nao estiver vazia, copia no arquivo '.mcr'        
        if (!line.empty()) {
            
            mcrFile << line << "\n";
            
            // se for chamada de macro, coloca no dicionario as linhas originais da macro
            if (macroCall > -1) {
                Macro macro = macroList[macroCall];
                for (int i = macro.initLine; i < macro.initLine+macro.numLines; ++i)
                    lineDictMcr.push_back(i);
            } else
                lineDictMcr.push_back(lineCounter);
        }
        
        lineCounter++;
        
    }
    
    preFile.close();
    mcrFile.close();
    
    // futuramente, indicara erros no valor de retorno
    return 0;
}