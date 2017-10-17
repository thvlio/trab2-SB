/*      PRE.H: funções relativas à montagem do codigo fonte        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
int constCheck (std::string&, int&);
void assembleInstr (Instr&, int&, std::vector<int>&, std::vector<Label>&, std::stringstream&);
std::vector<int> asmParser (std::ifstream&, std::vector<Label>&, int&, int&, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&, int&);
void assembleCode (std::string, std::string, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
constCheck: checa se a string pode ser convertida em um numero (decimal ou hexa, negativo ou positivo) sem erros
entrada: string a ser convertida e inteiro que armazenará o resultado
saida: inteiro que determina se houve ou não erro na conversão (1 sem erro, 0 com erro)
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
spaceCommand: executa a diretiva space, e configura o rótulo
entrada:
saida:
*/
int spaceCommand (std::string &token2, std::string &labelName, std::vector<int> &partialMachineCode, int &addrCounter, std::vector<Label> labelList) {
    
    int amount; // número de espaços a serem reservados
    int error = 0; // se houve erro ou nao
    
    if (token2.empty()) // se não tem argumento, só reserva um espaço
        amount = 1;
        
    else { // se tem argumentos, checa se é um número válido
        int status = integerCheck (token2, amount);
        if ((status == 1 && amount < 1) || status == 0)  // não pode ser um número menor que 1
            error = -1;
    }
    
    for (int i = 0; i < amount; ++i) { // coloca a reserva no codigo de maquina
        partialMachineCode.push_back(0);
        addrCounter++;
    }
    
    // se não tiver rótulo antes de SPACE, dá erro
    if (labelName.empty()) {
        error = -2;
        
    } else {
        // procura o rótulo e ajusta as características
        for (int i = 0; i < labelList.size(); ++i) {
            if (labelList[i].name = labelName) {
                labelList[i].isConst = 0;
                labelList[i].vectSize = amount;
            }
        }
    }
    
    return error;
    
}


/*
assembleInstr: le a instrucao e seus argumentos, e passa para codigo de maquina
entrada:
saida:
*/
void assembleInstr (Instr &instr, int &addrCounter, std::vector<int> &partialMachineCode, std::vector<Label> &labelList, std::stringstream &lineStream) {
    
    // -- eh uma instrucao
    // -- le na tabela de instrucoes quantos argumentos ela leva
    // -- le o num de tokens correspondente ao numero de argumentos
    // -- verifica cada argumento na tabela de simbolos e tal
    // -- soma ao contador de endereços o numero certo
    // -- vai adicionando no vetor de codigo de maquina os codigos traduzidos
    
    // salva o codigo de maquina da instrucao
    partialMachineCode.push_back(instr.opcode);
    addrCounter++;
    
    // le os argumentos esperados pela funcao
    for (int i = 0; i < instr.numArg; ++i) {
        
        // le um token
        std::string token;
        lineStream >> token;
        
        if (instr.numArg == 2 && i == 0)
            token.pop_back();
        
        // procura o token na tabela de simbolos
        int found = -1;
        for (int i = 0; (i < labelList.size()) && (found < 0); ++i) {
            if (token == labelList[i].name)
                found = i;
        }
        
        // se nao ta na tabela, bota na tabela e coloca a pendencia
        if (found < 0) {
            Label label;
            label.name = token;
            label.isDefined = 0;
            label.pendList.push_back(addrCounter);
            labelList.push_back(label);
            partialMachineCode.push_back(0); // futuramente, colocar o numero a ser somado ao valor do rotulo, pra poder usar ROTULO + N
            
        } else {
            
            // se ta na tabela e nao ta definido, coloca a pendencia
            if (!labelList[found].isDefined) {
                labelList[found].pendList.push_back(addrCounter);
                partialMachineCode.push_back(0); // futuramente, colocar o numero a ser somado ao valor do rotulo, pra poder usar ROTULO + N
            }
            
            // se ta na tabela e ta definido, so copia o valor no codigo de maquina
            else {
                int address = labelList[found].value;
                partialMachineCode.push_back(address);
            }
            
        }
        
        addrCounter++;
        
    }
}



/*
asmParser: traduz uma linha em código máquina
entrada:
saida: código de máquina parcial num vetor de inteiros
*/
std::vector<int> asmParser (std::ifstream &mcrFile, std::vector<Label> &labelList, int &lineCounter, int &addrCounter, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList, int &section) {
    
    std::vector<int> partialMachineCode;
    
    std::string line;
    getline (mcrFile, line);
    std::stringstream lineStream (line);
    
    // nao le a ultima linha em branco do arquivo
    if (line.empty())
        return partialMachineCode;
    
    std::string token;
    lineStream >> token;
    
    std::string labelName;
    
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
        int labelPos = -1;
        for (int i = 0; i < labelList.size(); ++i) {
            if (labelList[i].name == token) {
                alreadyMentioned = 1;
                labelPos = i;
                if (labelList[i].isDefined)
                    alreadyDefined = 1;
            }
        }
        
        // ja foi definido (da erro de simbolo ja definido)
        if (alreadyDefined)
            reportError("símbolo já definido", "semântico", lineDict[lineCounter-1], line);
            
        // nao foi definido mas ja foi mencionado (define ele agora)
        else if (alreadyMentioned) {
            labelList[labelPos].value = addrCounter;
            labelList[labelPos].isDefined = 1;
        }
        
        // nunca foi chamado nem definido (acrescenta novo rotulo definido)
        else {
            Label label;
            label.name = token;
            label.value = addrCounter;
            label.isDefined = 1;
            labelList.push_back(label);
        }
        
        // salva o nome do rotulo
        labelName = token;
        
        // coloca um novo token no lugar do rótulo, que já foi processado
        lineStream >> token;
        
    }
    
    // agora que lidou com os possiveis rotulos, analisa o primeiro token e ve se é instrucao ou diretiva
    int isInstruction = -1;
    for (int i = 0; i < instrList.size(); ++i) {
        if (token == instrList[i].name)
            isInstruction = i;
    }
    int isDirective = -1;
    for (int i = 0; i < dirList.size(); ++i) {
        if (token == dirList[i].name)
            isDirective = i;
    }
    
    // se nao encontrar o comando em nenhuma tabela, nao é um comando reconhecido
    if (isInstruction == -1 && isDirective == -1)
        reportError("comando não reconhecido", "sintático", lineDict[lineCounter-1], line);
        
    else {
        
        // se for uma instrução, monta
        if (isInstruction >= 0) {
            Instr instr = instrList[isInstruction];
            assembleInstr (instr, addrCounter, partialMachineCode, labelList, lineStream);
            
        // se for uma diretiva, faz uma função específica
        } else if (isDirective >= 0) {
            
            Dir dir = dirList[isDirective];
            std::string token2;
            lineStream >> token2;
            
            // se for SECTION, atualiza a informação da seção
            if (dir.name == "SECTION") {
                if (token2 == "TEXT")
                    section = 0;
                else if (token2 == "DATA")
                    section = 1;
                else
                    reportError("seção não reconhecida", "sintático", lineDict[lineCounter-1], line);
                    
            // se for SPACE, verifica os argumentos e coloca no código de máquina as reservas
            } else if (dir.name == "SPACE") {
                int status = spaceCommand (token2, labelName, partialMachineCode, addrCounter, labelList);
                if (status == -1)
                    reportError("número de elementos inválido", "sintático", lineDict[lineCounter-1], line);
                else if (status == -2)
                    reportError("a diretiva SPACE precisa ser precedida de um rótulo", "sintático", lineDict[lineCounter-1], line);
            
            // se for CONST, verifica o argumento e salva no código de máquina
            } else if (dir.name == "CONST") {
                int constant; // valor da constante
                if (token2.empty())
                    reportError("é esperado um argumento para a reserva de constante", "sintático", lineDict[lineCounter-1], line);
                int status = constCheck (token2, constant);
                if (status == 0)
                    reportError("número constante inválido", "sintático", lineDict[lineCounter-1], line);
                partialMachineCode.push_back(constant); // adiciona a constante no código de máquina
                addrCounter++;
            }
        }
    }
    
    return partialMachineCode;
    
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
    
    std::vector<int> machineCode;
    
    int lineCounter = 1;
    int addrCounter = 0;
    int section = -1; // -1: nenhuma, 0: text, 1: data
    
    while (!mcrFile.eof()) {
        
        // le o codigo de maquina parcial da linha
        std::vector<int> partialMachineCode = asmParser(mcrFile, labelList, lineCounter, addrCounter, lineDict, instrList, dirList, section);
        
        // anexa os codigos parciais
        for (int i = 0; i < partialMachineCode.size(); ++i)
            machineCode.push_back(partialMachineCode[i]);
        
        lineCounter++;
        
    }
    
    // resolve as listas de pendências
    // talvez tenha q ser feito de outra forma (individualmente conforme rotulos forem definidos)
    for (int i = 0; i < labelList.size(); ++i) {
        while (!labelList[i].pendList.empty()) {
            int index = labelList[i].pendList.back();
            labelList[i].pendList.pop_back();
            machineCode[index] = labelList[i].value;
        }
    }
    
    // escreve o codigo de maquina final no arquivo
    for (int i = 0; i < machineCode.size(); ++i)
        outFile << machineCode[i] << " ";
    
    mcrFile.close();
    outFile.close();
    
    return;
}