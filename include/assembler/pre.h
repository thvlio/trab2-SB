/*      PRE.H: funções relativas ao preprocessamento        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
void preReadLine (std::string&, std::ifstream&, std::vector<Label>&);
void appendNextLine (std::string&, std::stringstream&, std::ifstream&, std::vector<Label>&, int&);
int equCommand (std::stringstream&, std::vector<Label>&, std::string&, int&);
int ifCommand (std::stringstream&, std::ifstream&, int&, int&);
void preParser (std::string&, std::ifstream&, std::vector<Label>&, int&, std::vector<Instr>&, std::vector<Dir>&, std::vector<Error>&);
int preProcessFile (std::string, std::string, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&, std::vector<Error>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
preReadLine: le uma linha do arquivo na etapa de preprocessamento e passa tudo para caixa alta, ignora comentários e ignora espaços em branco no começo e no final da linha. depois, procura por rotulos numa linha e substitui pela definicao se encontrar
entrada: stream do arquivo de entrada e lista de rotulos definidos
saida: nenhuma (string com a linha lida e alterada dada por referência)
*/
void preReadLine (std::string &line, std::ifstream &asmFile, std::vector<Label> &labelList)  {
    
    // le uma linha do arquivo
    getline(asmFile, line);
    
    // transforma a string p caixa alta
    std::transform (line.begin(), line.end(), line.begin(), ::toupper);
    
    // ignora os comentarios, se houver algum
    std::stringstream lineStream (line);
    getline(lineStream, line, ';');
    
    // retira os espaços em branco entre os tokens
    lineStream.str(line);
    lineStream.clear();
    std::vector<std::string> tokenList;
    while (!lineStream.eof()) {
        std::string tempToken;
        lineStream >> tempToken;
        tokenList.push_back(tempToken);
    }
    line.clear();
    for (unsigned int i = 0; i < tokenList.size(); ++i) {
        line = line + tokenList[i] + ' ';
        if (i == tokenList.size()-1)
            line.pop_back();
    }
    
    // retira os espaços em branco, caso existam, no final da linha
    while (line.back() == ' ' || line.back() == '\t' || line.back() == '\n' || line.back() == '\v' || line.back() == '\f' || line.back() == '\r')
        line.pop_back();
    
    // retira os espaços em branco, caso existam, no começo da linha
    while (line.front() == ' ' || line.front() == '\t' || line.front() == '\n' || line.front() == '\v' || line.front() == '\f' || line.front() == '\r')
        line = line.substr(1);
    
    // itera na lista de rotulos definidos procurando por um desses rotulos na linha
    for (unsigned int i = 0; i < labelList.size(); ++i) {
        
        // tamanho do nome do rótulo
        int nameSize = labelList[i].name.size();
        
        // procura só a partir desta posição
        std::size_t minPos = 0;
        
        // procura pelo nome do rotulo na linha
        std::size_t pos = line.find(labelList[i].name, minPos);
        
        while (pos != std::string::npos) {
            
            // se encontrar em alguma posicao, substitui pelo texto associado ao rotulo
            // verifica se o nome encontrado é um token individual
            if (pos == 0) {
                if (line[pos+nameSize] == ':' || line[pos+nameSize] == ' ' || pos+nameSize >= line.size())
                    line.replace (pos, nameSize, labelList[i].equ);
            } else {
                if (line[pos-1] == ' ' && (line[pos+nameSize] == ' ' || pos+nameSize >= line.size()))
                    line.replace (pos, nameSize, labelList[i].equ);
            }
            
            // atualiza a posição mínima da busca
            minPos = pos+1;
            
            // procura de novo
            pos = line.find(labelList[i].name, minPos);
            
        }
            
    }
    
}



/*
appendNextLine: le a proxima linha, anexa na atual e retorna a linha composta
entrada: a linha atual, a stream da linha atual, a stream do arquivo e a lista de rotulos
saida: nenhuma (linha e stream da linha alteradas por referencia)
*/
void appendNextLine (std::string &line, std::stringstream &lineStream, std::ifstream &asmFile, std::vector<Label> &labelList, int &lineCounter) {
    
    // le e anexa à linha atual a proxima linha
    std::string nextLine;
    while (nextLine.size() == 0 && !asmFile.eof()) {
        preReadLine (nextLine, asmFile, labelList);
        lineCounter++;
    }
    line = line + " " + nextLine;
    
    // e corrige a stream da linha
    lineStream.str(nextLine); // salva a linha anexada como stream
    lineStream.clear(); // limpa as flags de estado
    
}



/*
equCommand: associa um valor ao rotulo
entrada: linha atual, stream da linha atual, lista de rotulos e nome do rotulo
saida: inteiro indicando se houve erro (lista de rotulos alterada por referencia)
*/
int equCommand (std::stringstream &lineStream, std::vector<Label> &labelList, std::string &token, int &pos) {
    
    // le o texto a ser substituido
    std::string equ;
    lineStream >> equ;
    
    if (equ.empty()) // se nao foram passados argumentos
        return -1;
        
    pos = equ.size()+1;
    
    Label label (token, equ); // cria o rotulo com o valor associado
    labelList.push_back(label); // coloca o rotulo na lista de rotulos definidos
    
    // le um proximo token
    std::string token2;
    lineStream >> token2;
    
    if (!token2.empty()) // se nao estiver vazio, deu mais argumentos que o necessario
        return -2;
    
    pos += token2.size()+1;
        
    return 0;
    
}



/*
ifCommand: se o valor do if for 1, compila a linha abaixo, senao esvazia a linha
entrada: linha atual, stream da linha atual, stream do arquivo de entrada e contador de linhas
saida: retorna se o numero lido eh inteiro (linha e contador de linhas alterados por referencia)
*/
int ifCommand (std::stringstream &lineStream, std::ifstream &asmFile, int &lineCounter, int &pos) {
    
    // le o numero seguinte (a busca ja trocou o rotulo por um valor)
    std::string value;
    lineStream >> value;
    
    if (value.empty())
        return -3;
        
    pos = value.size()+1;
    
    // tenta converter o numero para um inteiro
    int conv = -1;
    int isInt = integerCheck (value, conv);
    
    if (!isInt)
        return -1;
    
    // se conseguiu, executa a diretiva
    else {
        if (conv != 1) {
            std::string line;
            getline (asmFile, line); // le a proxima linha do arquivo (que vai ser descartada)
            lineCounter++; // pula uma linha
        }
    }
    
    // le mais um token
    std::string token2;
    lineStream >> token2;
    
    // se nao estiver vazio, avisa que foram dados mais argumentos que o necessário
    if (!token2.empty())
        return -2;
    
    return 0;
    
}



/*
preParser: processa uma linha do arquivo fonte
entrada: linha atual, stream do arquivo de entrada, a lista de rotulos e o contador de linhas
saida: nenhuma (linha lida e contador de linhas alterados por referência)
*/
void preParser (std::string &line, std::ifstream &asmFile, std::vector<Label> &labelList, int &lineCounter, std::vector<Instr> &instrList, std::vector<Dir> &dirList, std::vector<Error> &errorList) {
    
    // le uma linha, corrige algumas coisas e procura na linha por rotulos que ja tenham sido definidos por equs
    preReadLine (line, asmFile, labelList);
    
    // cria um stream para a leitura de tokens
    std::stringstream lineStream (line);
    
    // le um token da linha
    std::string token;
    lineStream >> token;
    
    // se o ultimo caracter for ':', entao ta definindo um rotulo
    if (token.back() == ':') {
        
        // pega o token seguinte
        std::string token2;
        lineStream >> token2;
        
        // se token2 estiver vazio, anexa a proxima linha
        if (token2.empty()) {
            appendNextLine (line, lineStream, asmFile, labelList, lineCounter);
            lineStream >> token2; // pega o token correto
        }
        
        // se for equ, salva o valor associado ao rotulo na lista
        if (token2 == "EQU") {
            
            // verifica se o rótulo é válido
            token.pop_back();
            int pos = 0;
            int valid = labelCheck(token, instrList, dirList, pos);
            if (valid == -1)
                errorList.push_back(Error("tamanho do rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineCounter, line, pos));
            else if (valid == -2)
                errorList.push_back(Error("rótulos não podem começar com números", "léxico", lineCounter, line, pos));
            else if (valid == -3)
                errorList.push_back(Error("caracter inválido encontrado no rótulo", "léxico", lineCounter, line, pos));
            else if (valid == -4)
                errorList.push_back(Error("rótulo não pode ter nome de instrução ou diretiva", "semântico", lineCounter, line, pos));
            else if (valid == -5)
                errorList.push_back(Error("declaração de rótulo vazia", "sintático", lineCounter, line, pos));
            
            // executa o comando da diretiva
            pos = 0;
            int status = equCommand (lineStream, labelList, token, pos);
            pos += token.size()+1 + token2.size()+1 + 1;
            if (status == -1)
                errorList.push_back(Error("definição de EQU vazia", "sintático", lineCounter, line, pos));
            else if (status == -2)
                errorList.push_back(Error("é esperado somente um argumento para EQU (sem espaços em branco)", "sintático", lineCounter, line, pos));
            
            // esvazia a string p nao salvar a linha no codigo
            line.clear();
                
        }
            
    // se for if, determina se vai ou nao manter a proxima linha
    } else if (token == "IF") {
        
        // executa o comando da diretiva
        int pos = 0;
        int status = ifCommand (lineStream, asmFile, lineCounter, pos);
        if (status == -1) {
            pos = 3;
            errorList.push_back(Error("parâmetro de IF deveria ser um número decimal", "sintático", lineCounter, line, pos));
        } else if (status == -2) {
            pos += 3;
            errorList.push_back(Error("IF só recebe um argumento", "sintático", lineCounter, line, pos));
        } else if (status == -3) {
            pos = 3;
            errorList.push_back(Error("declaração de IF sem argumentos", "sintático", lineCounter, line, pos));
        }
            
        // esvazia a string p nao salvar a linha no codigo
        line.clear();
    }
        
}



/*
preProcessFile: faz a passagem de preprocessamento no arquivo, que inclui:
    - passa tudo para caixa alta
    - ignora comentarios
    - avalia EQU e IF
    - (detectar erros)
entrada: nome do arquivo de entrada '.asm', nome do arquivo de saida '.pre' e dicionario de linhas
saida: inteiro indicando se houve erros
*/
int preProcessFile (std::string inFileName, std::string preFileName, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList, std::vector<Error> &errorList) {
    
    std::ifstream asmFile (inFileName);
    std::ofstream preFile (preFileName);
    
    std::vector<Label> labelList;
    
    int lineCounter = 1;

    while (!asmFile.eof()) {
        
        // chama o parser especifico do preprocessamento        
        std::string line;
        preParser(line, asmFile, labelList, lineCounter, instrList, dirList, errorList);
            
        // se a linha nao retornar vazia, copia no arquivo '.pre'        
        if (!line.empty()) {
            preFile << line << "\n";
            lineDict.push_back(lineCounter);
        }
        
        lineCounter++;
    }
    
    asmFile.close();
    preFile.close();
    
    // futuramente, indicara erros no valor de retorno
    return 0;
}