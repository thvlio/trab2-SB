/*      PRE.H: funções relativas ao preprocessamento        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
void createMacro (std::string&, std::ifstream&, std::string&, std::vector<Macro>&);
void mcrSearchAndReplace (std::string&, std::string&, std::vector<Macro>&);
void mcrParser (std::string&, std::ifstream&, std::vector<Macro>&);
void expandMacros (std::string, std::string);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
createMacro: le a definicao de uma macro e guarda na lista de macros
entrada: linha atual, stream do arquivo .pre, nome da macro e lista de macros
saida: nenhuma (lista de macros alterada por referencia)
*/
void createMacro (std::string &line, std::ifstream &preFile, std::string &token, std::vector<Macro> &macroList) {
    
    // cria uma string p guardar a definicao da macro
    std::string definition;
    
    // indica se achou end indicando o final da macro
    int reachedEnd = 0;
    
    // laco que vai armazenando todas as linhas ate o final da macro
    while (!reachedEnd) {
        
        // a proxima linha eh lida
        std::string auxLine;
        getline (preFile, auxLine);
        
        if (auxLine != "END") {
            // se nao for o final, anexa na definicao
            definition = definition + auxLine + '\n';
        } else {
            // se for o final, ajusta a flag e tira o '\n' do final da definicao
            reachedEnd = 1;
            definition.pop_back();
        }
    }
    
    // armazena a macro na lista de macros
    token.pop_back();
    Macro macro (token, definition);
    macroList.push_back(macro);
    
    // nao escreve a linha que define o rotulo da macro
    line.clear();
    
}



/*
mcrSearchAndReplace: procura o token na lista de macros e substitui a macro no codigo
entrada: linha atual, nome da macro sendo chamada e lista de macros
saida: nenhuma (linha atual alterada por referencia)
*/
void mcrSearchAndReplace (std::string &line, std::string &token, std::vector<Macro> &macroList) {
    
    // procura o token lido na lista de macros
    int found = 0;
    for (int i = 0; ((i < macroList.size()) && (!found)); ++i) {
        if (token == macroList[i].name) {
            // se encontrar, escreve a definicao no lugar da linha atual
            line = macroList[i].definition;
            found = 1;
        }
    }
    
}



/*
mcrParser: parser do processamento de macros. procura definicoes e chamadas de macros nas linhas
entrada: linha atual, stream do arquivo .pre e lista de macros
saida: nenhuma (linha atual alterada por referencia)
*/
void mcrParser (std::string &line, std::ifstream &preFile, std::vector<Macro> &macroList) {
    
    // le uma linha do arquivo
    getline(preFile, line);
    
    // cria o stream para a linha e le um token
    std::stringstream lineStream (line);
    std::string token;
    scanner (token, lineStream);
    
    // se o ultimo char for ':', esta definindo um rotulo
    if (token.back() == ':') {
        
        // le o token seguinte
        std::string token2;
        scanner (token2, lineStream);
        
        // se for uma diretiva de macro, cria uma macro nova na lista
        if (token2 == "MACRO")
            createMacro (line, preFile, token, macroList);
    
    // se nao for definicao de rotulo, eh uma linha que pode ou nao estar chamando uma macro
    } else
        mcrSearchAndReplace (line, token, macroList);
    
}



/*
expandMacros: faz a passagem para expandir macros no arquivo, que inclui:
    - substitui as macros
    - (detectar erros)
entrada: nome do arquivo de entrada '.pre'
saida: nome do arquivo de saida '.mcr'
*/
void expandMacros (std::string preFileName, std::string mcrFileName) {
    
    std::ifstream preFile (preFileName);
    std::ofstream mcrFile (mcrFileName);

    std::vector<Macro> macroList;
    
    while (!preFile.eof()) {
        
        // chama o parser da passagem de macros
        std::string line;
        mcrParser(line, preFile, macroList);
        
        // se a linha nao estiver vazia, copia no arquivo '.mcr'        
        if (!line.empty())
            mcrFile << line << "\n";
        
    }
    
    preFile.close();
    mcrFile.close();
    
    return;
}