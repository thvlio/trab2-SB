/*      PRE.H: funções relativas à montagem do codigo fonte        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
int constCheck (std::string&, int&);
int spaceCommand (std::stringstream&, std::string&, std::vector<int>&, int&, std::vector<Label>&);
int constCommand (std::stringstream&, std::string&, std::vector<int>&, int&, std::vector<Label>&);
int assembleInstr (Instr&, int&, std::vector<int>&, std::vector<Label>&, std::stringstream&, std::vector<Instr>&, std::vector<Dir>&);
std::vector<int> asmParser (std::ifstream&, std::vector<Label>&, int&, int&, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&, int&, int&, std::vector<int>&, std::vector<std::string>&, std::vector<Error>&);
void assembleCode (std::string, std::string, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&,std::vector<Error>&);



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
            
        } else {
            if (value.substr(0,2) == "0X" || value.substr(0,3) == "-0X")
                conv = strtol(cValue, &ptr, 16);
            else
                *ptr = 'f'; // so para nao ser '\0'
                
        }
        
    }
    
    // se o ultimo caracter lido foi um '\0', entao a string foi convertida em um numero (decimal ou hexa)
    return (*ptr=='\0');
    
}



/*
spaceCommand: executa a diretiva space, e configura o rótulo
entrada:
saida:
*/
int spaceCommand (std::stringstream &lineStream, std::string &labelName, std::vector<int> &partialMachineCode, int &addrCounter, std::vector<Label> &labelList) {
    
    int amount; // número de espaços a serem reservados
    
    // token do argumento
    std::string token2;
    lineStream >> token2;
    
    if (token2.empty()) // se não tem argumento, só reserva um espaço
        amount = 1;
        
    else { // se tem argumentos, checa se é um número válido
        int status = integerCheck (token2, amount);
        if ((status == 1 && amount < 1) || status == 0)  // não pode ser um número menor que 1
            return -1;
    }
    
    // extrai novo token (se houver mais um, o numero de argumentos eh invalido)
    std::string token3;
    lineStream >> token3;
    if (!token3.empty())
        return -3;
    
    for (int i = 0; i < amount; ++i) { // coloca a reserva no codigo de maquina
        partialMachineCode.push_back(0);
        addrCounter++;
    }
    
    // se não tiver rótulo antes de SPACE, dá erro
    if (labelName.empty()) {
        return -2;
        
    } else {
        // procura o rótulo e ajusta as características
        for (unsigned int i = 0; i < labelList.size(); ++i) {
            if (labelList[i].name == labelName) {
                labelList[i].isConst = 0;
                labelList[i].vectSize = amount;
            }
        }
    }
    
    return 0;
    
}



/*
constCommand: executa a diretiva const, e configura o rótulo
entrada:
saida:
*/
int constCommand (std::stringstream &lineStream, std::string &labelName, std::vector<int> &partialMachineCode, int &addrCounter, std::vector<Label> &labelList) {
    
    int constant;
    
    // le o proximo token
    std::string token2;
    lineStream >> token2;
    
    if (token2.empty()) // checa se foi dado um argumento
        return -1;
        
    int status = constCheck (token2, constant); // checa se é um numero valido
    if (status == 0)
        return -2;
        
    // le mais um token (se conseguir ler, o numero de argumento eh invalido)
    std::string token3;
    lineStream >> token3;
    if (!token3.empty())
        return -1;
        
    partialMachineCode.push_back (constant); // adiciona a constante no código de máquina
    addrCounter++;
    
    if (labelName.empty()) { // checa se foi declarado um rotulo antes
        return -3;
    
    // se foi, arruma o rotulo
    } else {
        for (unsigned int i = 0; i < labelList.size(); ++i) {
            if (labelList[i].name == labelName) {
                if (constant == 0)
                    labelList[i].isConst = 2; // 2 indica que é zero
                else
                    labelList[i].isConst = 1;
                labelList[i].vectSize = 1;
            }
        }
    }
        
    return 0;
    
}



/*
assembleInstr: le a instrucao e seus argumentos, e passa para codigo de maquina
entrada:
saida:
*/
int assembleInstr (Instr &instr, int &addrCounter, std::vector<int> &partialMachineCode, std::vector<Label> &labelList, std::stringstream &lineStream, std::vector<Instr> &instrList, std::vector<Dir> &dirList) {
    
    // salva o codigo de maquina da instrucao
    partialMachineCode.push_back(instr.opcode);
    addrCounter++;
    
    // le os argumentos esperados pela funcao
    for (int i = 0; i < instr.numArg; ++i) {
        
        // le um token
        std::string token;
        lineStream >> token;
        
        // retorna -1 se faltam argumentos (token vazio antes de chegar ao final do for)
        if (token.empty())
            return -1;
            
        // checa se esta tentando declarar outro rotulo
        if (token.back() == ':') {
            token.pop_back();
            int valid = labelCheck(token, instrList, dirList);
            if (valid == 0) // se for um rotulo valido, estava tentando declarar dois rotulos
                return -17;
            else // se nao era, so fala que o rotulo é invalido
                return -11;
        }
        
        int offset; // offset para acesso aos vetores
        
        // se for COPY (2 args) ou qualquer instrução com mais de um argumento (atualmente só tem o COPY mesmo)
        if (instr.numArg > 1 && i < instr.numArg-1) {
            
            // se no final do token tiver virgula
            if (token.back() == ',') {
                token.pop_back(); // tira a virgula
                
                int aux; // checa se está tentando se fazer endereçamento imediato
                int status = integerCheck (token, aux);
                if (status == 1)
                    return -16;
                
                int valid = labelCheck(token, instrList, dirList); // procura erros no rotulo
                if (valid <= -1 && valid >= -4)
                    return valid-8;
                offset = 0; // ta acessando a posicao 0
                
            // se nao tiver virgula
            } else {
                
                int aux; // checa se está tentando se fazer endereçamento imediato
                int status = integerCheck (token, aux);
                if (status == 1)
                    return -16;
                
                int valid = labelCheck(token, instrList, dirList); // procura erros no rotulo
                if (valid <= -1 && valid >= -4)
                    return valid-8;
                
                // le o proximo token (esperado que seja um +)
                std::string token2;
                lineStream >> token2;
                if (token2.empty())
                    return -1;
                if (token2 == ",")
                    return -2; // se for virgula, avisa que a virgula nao deve ser separada por espaço do 1o operador
                else if (token2 != "+") {
                    int valid = labelCheck(token2, instrList, dirList); // procura erros no rotulo
                    if (valid == 0)
                        return -7; // faltando virgula entre os operandos
                    else
                        return -6; // op de indexacao invalida/rotulo invalido
                }
                
                // le o proximo token (esperado que seja um numero valido)
                std::string token3;
                lineStream >> token3;
                if (token3.empty())
                    return -13;
                    
                // esperado que tenha uma virgula depois do numero
                if (token3.back() == ',') {
                    token3.pop_back();
                    if (token3.empty())
                        return -13;
                    int status = integerCheck (token3, offset);
                    if ((status == 1 && offset < 0) || status == 0)  // tem que ser maior ou igual a 0
                        return -8;
                        
                // se nao tem virgula
                } else { 
                    int status = integerCheck (token3, offset);
                    if ((status == 1 && offset < 0) || status == 0)  // tem que ser maior ou igual a 0
                        return -8;
                    //  se for valido, le o proximo token (esse token ta errado de qualquer forma)
                    std::string token4;
                    lineStream >> token4;
                    if (token4.empty())
                        return -1; // de estiver vazio, o numero de argumento esta errado
                    if (token4 == ",")
                        return -2; // se o token for um virgula, nao deveria ter sido serpada por espaço
                    else {
                        int valid = labelCheck(token4, instrList, dirList); // procura erros no rotulo
                        if (valid == 0)
                            return -7; // faltando virgula entre os operandos
                        else
                            return -6; // op de indexacao invalida/rotulo invalido
                    }
                        
                }
            }
            
        } else { // se nao for esperada uma virgula
            
            int aux; // checa se está tentando se fazer endereçamento imediato
            int status = integerCheck (token, aux);
            if (status == 1)
                return -16;
                
            // le o proximo token
            std::string token2;
            lineStream >> token2;
            
            // se no final tiver virgula, ve se foi colocado mais um argumento (invalido)
            if (token.back() == ',' && !token2.empty())
                return -1;
            
            // procura erros no rotulo (se tinha virgula e nao tinha argumento a mais, da erro aqui)
            int valid = labelCheck(token, instrList, dirList);
            if (valid <= -1 && valid >= -4)
                return valid-8;
            
            if (token2.empty())
                offset = 0; // se ta no final da linha, nao tem offset
            
            else if (token2 == "+") {
                
                // le o ultimo token
                std::string token3;
                lineStream >> token3;
                if (token3.empty())
                    return -13;
                    
                // se no final tiver virgula, ve se foi colocado mais um argumento (invalido)
                if (token3.back() == ',') {
                    std::string token4;
                    lineStream >> token4;
                    if (!token4.empty())
                        return -1;
                }
                    
                // verica se o numero eh valido
                int status = integerCheck (token3, offset);
                if ((status == 1 && offset < 0) || status == 0)  // tem que ser maior ou igual a 0
                    return -8;
            
            } else {
                
                // checa se esta tentando declarar outro rotulo
                if (token2.back() == ':') {
                    token2.pop_back();
                    int valid = labelCheck(token2, instrList, dirList);
                    if (valid == 0) // se for um rotulo valido, estava tentando declarar dois rotulos
                        return -17;
                    else // se nao era, operacao de indexacao invalida
                        return -6;
                } else {
                    int valid = labelCheck(token2, instrList, dirList); // checa se eh um rotulo valido
                    if (valid == 0)
                        return -1; // se for, numero de argumento invalido
                    else
                        return -6; // se nao for, indexacao invalida
                }
                
            }
            
        }
        
        // procura o token na tabela de simbolos
        int found = -1;
        for (unsigned int i = 0; (i < labelList.size()) && (found < 0); ++i) {
            if (token == labelList[i].name)
                found = i;
        }
        
        // se nao ta na tabela, bota na tabela e coloca a pendencia
        if (found < 0) {
            
            Label label;
            label.name = token;
            label.isDefined = 0;
            
            if (instr.name == "DIV")
                label.auxInfoList.push_back(1); // 1 indica instrucao de divisao
            else if (instr.name == "JMP" || instr.name == "JMPP" || instr.name == "JMPN" || instr.name == "JMPZ")
                label.auxInfoList.push_back(2); // 2 indica instrucao de pulo
            else if ((instr.name == "COPY" && i == 1) || instr.name == "STORE" || instr.name == "INPUT")
                label.auxInfoList.push_back(3); // 3 indica instrucao modificando a memoria
            else
                label.auxInfoList.push_back(0); // 0 indica instrucao padrao
                
            label.pendList.push_back(addrCounter);
            labelList.push_back(label);
            partialMachineCode.push_back(offset);
            
        } else {
            
            // se ta na tabela e nao ta definido, coloca mais uma pendencia
            if (!labelList[found].isDefined) {
                
                if (instr.name == "DIV")
                    labelList[found].auxInfoList.push_back(1); // 1 indica instrucao de divisao
                else if (instr.name == "JMP" || instr.name == "JMPP" || instr.name == "JMPN" || instr.name == "JMPZ")
                    labelList[found].auxInfoList.push_back(2); // 2 indica instrucao de pulo
                else if ((instr.name == "COPY" && i == 1) || instr.name == "STORE" || instr.name == "INPUT")
                    labelList[found].auxInfoList.push_back(3); // 3 indica instrucao modificando a memoria
                else
                    labelList[found].auxInfoList.push_back(0); // 0 indica instrucao padrao
                
                labelList[found].pendList.push_back(addrCounter);
                partialMachineCode.push_back(offset);
            }
            
            // se ta na tabela e ta definido, ja indica os problemas e copia o endereço no codigo de maquina
            else {
                
                if (instr.name == "DIV") {
                    if (labelList[found].isConst == 2)
                        return -3; // -3 indica divisao por zero
                } else if (instr.name == "JMP" || instr.name == "JMPP" || instr.name == "JMPN" || instr.name == "JMPZ") {
                    if (labelList[found].vectSize != 0) // vectSize = 0 indica que o rotulo é da seção de texto
                        return -4; // -4 indica pulo para a seção de dados
                } else if ((instr.name == "COPY" && i == 1) || instr.name == "STORE" || instr.name == "INPUT") {
                    if (labelList[found].isConst != 0)
                        return -5; // -5 indica tentativa de modificar valor constante
                }
                        
                // o acesso a memoria não pode usar um rótulo da seção de texto
                if (labelList[found].vectSize == 0 && instr.name != "JMP" && instr.name != "JMPP" && instr.name != "JMPN" && instr.name != "JMPZ")
                    return -14;
                
                // nao se pode usar offset com pulos
                if (instr.opcode >= 5 && instr.opcode <= 8) {
                    if (offset != 0)
                        return -15;
                }
                
                // checa se o tamanho do rotulo bate com o indice n (rotulo + n)
                if (offset >= labelList[found].vectSize && labelList[found].vectSize > 0)
                    return -(found+18); // retorna onde ta o rotulo
                
                int address = labelList[found].value;
                partialMachineCode.push_back(address+offset);
            }
            
        }
        
        addrCounter++;
        
    }
    
    // le mais um token
    std::string token2;
    lineStream >> token2;
    
    // checa se esta tentando declarar outro rotulo
    if (token2.back() == ':') {
        token2.pop_back();
        int valid = labelCheck(token2, instrList, dirList);
        if (valid == 0) // se for um rotulo valido, estava tentando declarar dois rotulos
            return -17;
        else // se nao era, so fala que o rotulo é invalido
            return -11;
    }
    
    // se houver mais um token, a quantidade de argumentos é maior q o esperado
    if (!token2.empty())
        return -1;
    
    return 0;
}



/*
asmParser: traduz uma linha em código máquina
entrada:
saida: código de máquina parcial num vetor de inteiros
*/
std::vector<int> asmParser (std::ifstream &mcrFile, std::vector<Label> &labelList, int &lineCounter, int &addrCounter, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList, int &section, int &sectionText, std::vector<int> &addrDict, std::vector<std::string> &lines, std::vector<Error> &errorList) {
    
    std::vector<int> partialMachineCode;
    
    // le a proxima linha
    std::string line;
    getline (mcrFile, line);
    std::stringstream lineStream (line);
    
    // salva a linha do arquivo (para uso no final da execucao, para mostrar alguns erros
    lines.push_back(line);
    
    // nao le a ultima linha em branco do arquivo
    if (line.empty())
        return partialMachineCode;
    
    std::string token;
    lineStream >> token;
    
    std::string labelNameBackup;
    
    if (token.back() == ':') {
        
        // verifica se o rótulo é válido
        token.pop_back();
        int valid = labelCheck(token, instrList, dirList);
        if (valid == -1)
            errorList.push_back(Error("tamanho o rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineDict[lineCounter-1], line));
        else if (valid == -2)
            errorList.push_back(Error("rótulos não podem começar com números", "léxico", lineDict[lineCounter-1], line));
        else if (valid == -3)
            errorList.push_back(Error("caracter inválido encontrado no rótulo", "léxico", lineDict[lineCounter-1], line));
        else if (valid == -4)
            errorList.push_back(Error("rótulo não pode ter nome de instrução ou diretiva", "semântico", lineDict[lineCounter-1], line));
        else if (valid == -5)
            errorList.push_back(Error("declaração de rótulo vazia", "sintático", lineDict[lineCounter-1], line));
        
        // verifica se o rótulo já está na lista de rótulos e se já foi definido
        int alreadyDefined = 0;
        int alreadyMentioned = 0;
        int labelPos = -1;
        for (unsigned int i = 0; i < labelList.size(); ++i) {
            if (labelList[i].name == token) {
                alreadyMentioned = 1;
                labelPos = i;
                if (labelList[i].isDefined)
                    alreadyDefined = 1;
            }
        }
        
        // ja foi definido (da erro de simbolo ja definido)
        if (alreadyDefined)
            errorList.push_back(Error("símbolo já definido", "semântico", lineDict[lineCounter-1], line));
            
        // nao foi definido mas ja foi mencionado (define ele agora)
        else if (alreadyMentioned) {
            labelList[labelPos].value = addrCounter;
            labelList[labelPos].isDefined = 1;
            labelList[labelPos].vectSize = 0; // indica que é rotulo da area de texto (pode mudar se for chamada uma diretiva da área de dados)
            labelList[labelPos].isConst = 0;
        }
        
        // nunca foi chamado nem definido (acrescenta novo rotulo definido)
        else {
            Label label;
            label.name = token;
            label.value = addrCounter;
            label.isDefined = 1;
            label.vectSize = 0;
            label.isConst = 0;
            labelList.push_back(label);
        }
        
        // salva o nome do rotulo
        labelNameBackup = token;
        
        // coloca um novo token no lugar do rótulo, que já foi processado
        lineStream >> token;
        
        // ja checa pra ver se não é mais um rótulo
        if (token.back() == ':')
            errorList.push_back(Error ("mais de um rótulo em uma linha", "sintático", lineDict[lineCounter-1], line));
        
    }
    
    // agora que lidou com os possiveis rotulos, analisa o primeiro token e ve se é instrucao ou diretiva
    int isInstruction = -1;
    for (unsigned int i = 0; i < instrList.size(); ++i) {
        if (token == instrList[i].name)
            isInstruction = i;
    }
    int isDirective = -1;
    for (unsigned int i = 0; i < dirList.size(); ++i) {
        if (token == dirList[i].name)
            isDirective = i;
    }
    
    // se nao encontrar o comando em nenhuma tabela, nao é um comando reconhecido
    if (isInstruction == -1 && isDirective == -1)
        errorList.push_back(Error("comando não reconhecido", "sintático", lineDict[lineCounter-1], line));
        
    else {
        
        // se for uma instrução, monta
        if (isInstruction >= 0) {
            Instr instr = instrList[isInstruction];
            int status = assembleInstr (instr, addrCounter, partialMachineCode, labelList, lineStream, instrList, dirList);
            if (status == -1) {
                if (instr.numArg == 0)
                    errorList.push_back(Error("não é esperado nenhum argumento para "+instr.name, "sintático", lineDict[lineCounter-1], line));
                else if (instr.numArg == 1)
                    errorList.push_back(Error("é esperado 1 argumento para "+instr.name, "sintático", lineDict[lineCounter-1], line));
                else if (instr.numArg == 2)
                    errorList.push_back(Error("são esperados 2 argumentos para "+instr.name, "sintático", lineDict[lineCounter-1], line));
            } else if (status == -2)
                errorList.push_back(Error("a vírgula não deve ser separada do primeiro operando por espaço", "sintático", lineDict[lineCounter-1], line));
            else if (status == -3)
                errorList.push_back(Error ("divisão por zero", "semântico", lineDict[lineCounter-1], line));
            else if (status == -4)
                errorList.push_back(Error ("pulo para seção inválida", "semântico", lineDict[lineCounter-1], line));
            else if (status == -5)
                errorList.push_back(Error ("valores constantes não podem ser modificados", "semântico", lineDict[lineCounter-1], line));
            else if (status == -6)
                errorList.push_back(Error ("operação de indexacão inválida", "sintático", lineDict[lineCounter-1], line));
            else if (status == -7)
                errorList.push_back(Error ("deve haver uma vírgula entre os dois operandos da instrução", "sintático", lineDict[lineCounter-1], line));
            else if (status == -8)
                errorList.push_back(Error ("o deslocamento deve ser um número inteiro maior ou igual a zero", "sintático", lineDict[lineCounter-1], line));
            else if (status == -9)
                errorList.push_back(Error ("tamanho o rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineDict[lineCounter-1], line));
            else if (status == -10)
                errorList.push_back(Error ("rótulos não podem começar com números", "léxico", lineDict[lineCounter-1], line));
            else if (status == -11)
                errorList.push_back(Error ("caracter inválido encontrado no rótulo", "léxico", lineDict[lineCounter-1], line));
            else if (status == -12)
                errorList.push_back(Error ("rótulo não pode ter nome de instrução ou diretiva", "sintático", lineDict[lineCounter-1], line));
            else if (status == -13)
                errorList.push_back(Error ("estrutura da indexação incompleta", "sintático", lineDict[lineCounter-1], line));
            else if (status == -14)
                errorList.push_back(Error ("acesso à seção de texto só é permitido para pulos", "semântico", lineDict[lineCounter-1], line));
            else if (status == -15)
                errorList.push_back(Error ("o deslocamento de pulos deve ser zero", "semântico", lineDict[lineCounter-1], line));
            else if (status == -16)
                errorList.push_back(Error ("tentativa de endereçamento imediato", "sintático", lineDict[lineCounter-1], line));
            else if (status == -17)
                errorList.push_back(Error ("só um rótulo pode ser declarado, e no começo da linha", "sintático", lineDict[lineCounter-1], line));
            else if (status <= -18) {
                int pos = -(status+18); // recupera a posicao do rotulo
                errorList.push_back(Error ("indíce excede o tamanho do vetor "+labelList[pos].name, "semântico", lineDict[lineCounter-1], line));
            }
                
            if (section != 0)
                errorList.push_back(Error("instruções devem estar na seção de texto", "semântico", lineDict[lineCounter-1], line));
            
        // se for uma diretiva, faz uma função específica
        } else if (isDirective >= 0) {
            
            Dir dir = dirList[isDirective];
            
            // se for SECTION, atualiza a informação da seção
            if (dir.name == "SECTION") {
                std::string token2;
                lineStream >> token2;
                if (token2 == "TEXT") {
                    section = 0;
                    sectionText = 0;
                } else if (token2 == "DATA")
                    section = 1;
                else
                    errorList.push_back(Error("seção não reconhecida", "sintático", lineDict[lineCounter-1], line));
                    
            // se for SPACE, verifica os argumentos e coloca no código de máquina as reservas
            } else if (dir.name == "SPACE") {
                int status = spaceCommand (lineStream, labelNameBackup, partialMachineCode, addrCounter, labelList);
                if (status == -1)
                    errorList.push_back(Error("número de elementos inválido", "léxico", lineDict[lineCounter-1], line));
                else if (status == -2)
                    errorList.push_back(Error("a diretiva SPACE precisa ser precedida de um rótulo", "sintático", lineDict[lineCounter-1], line));
                else if (status == -3)
                    errorList.push_back(Error("é esperado um ou nenhum argumento para SPACE", "léxico", lineDict[lineCounter-1], line));
                    
                if (section != 1)
                    errorList.push_back(Error("SPACE deve estar na seção de dados", "semântico", lineDict[lineCounter-1], line));
            
            // se for CONST, verifica o argumento e salva no código de máquina
            } else if (dir.name == "CONST") {
                int status = constCommand (lineStream, labelNameBackup, partialMachineCode, addrCounter, labelList);
                if (status == -1)
                    errorList.push_back(Error("é esperado 1 argumento para a reserva de constante", "sintático", lineDict[lineCounter-1], line));
                else if (status == -2)
                    errorList.push_back(Error("número constante inválido", "léxico", lineDict[lineCounter-1], line));
                else if (status == -3)
                    errorList.push_back(Error("a diretiva CONST precisa ser precedida de um rótulo", "sintático", lineDict[lineCounter-1], line));
                    
                if (section != 1)
                    errorList.push_back(Error("CONST deve estar na seção de dados", "semântico", lineDict[lineCounter-1], line));   
            }
        }
    }
    
    // coloca os endereços no dicionario de endereços
    for (unsigned int i = 0; i < partialMachineCode.size(); ++i)
        addrDict.push_back(lineCounter);
    
    return partialMachineCode;
    
}



/*
assembleCode: faz a passagem de montagem no arquivo, que inclui:
    - (todo o processo de passagem unica)
    - (detectar erros blabla)
entrada: nome do arquivo de entrada '.mcr'
saida: nome do arquivo de saida '.o'
*/
void assembleCode (std::string mcrFileName, std::string outFileName, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList, std::vector<Error> &errorList) {
    
    std::ifstream mcrFile (mcrFileName);
    std::ofstream outFile (outFileName);
    
    std::vector<Label> labelList; // lista de rotulos
    
    std::vector<int> machineCode; // codigo de maquina
    
    std::vector<int> addrDict; // look up table pra traduzir um endereco em uma linha
    
    std::vector<std::string> lines; // linhas do arquivo .mcr (para uso nas mensagens de erro)
    
    int lineCounter = 1;
    int addrCounter = 0;
    int section = -1; // -1: nenhuma, 0: text, 1: data
    int sectionText = -1; // -1: não encontrou seção texto, 0: encontrou
    
    while (!mcrFile.eof()) {
        
        // le o codigo de maquina parcial da linha
        std::vector<int> partialMachineCode = asmParser(mcrFile, labelList, lineCounter, addrCounter, lineDict, instrList, dirList, section, sectionText, addrDict, lines, errorList);
        
        // anexa os codigos parciais
        for (unsigned int i = 0; i < partialMachineCode.size(); ++i)
            machineCode.push_back(partialMachineCode[i]);
        
        lineCounter++;
        
    }
    
    if (sectionText == -1)
        errorList.push_back(Error ("seção texto é obrigatória", "semântico", -1, ""));
    
    // resolve as listas de pendências (e reporta erros)
    for (unsigned int i = 0; i < labelList.size(); ++i) {
        
        if (!labelList[i].isDefined) { // rotulo nunca foi definido
            
            for (unsigned int j = 0; j < labelList[i].pendList.size(); ++j) {
                int mcrLine = addrDict[labelList[i].pendList[j]]; // linha do arquivo .mcr
                int origLine = lineDict[mcrLine-1]; // linha do arquivo original
                errorList.push_back(Error ("rótulo "+labelList[i].name+" não definido", "semântico", origLine, lines[mcrLine-1]));
            }
            
        } else {
            
            for (unsigned int j = 0; j < labelList[i].pendList.size(); ++j) {
                
                // pega o proximo endereço na lista de pendencias
                int address = labelList[i].pendList[j];
                
                // pega a informacao adicional para indicar problemas
                int auxInfo = labelList[i].auxInfoList[j];
                
                // ja faz uma traducao das linhas
                int mcrLine = addrDict[address]; // linha do arquivo .mcr
                int origLine = lineDict[mcrLine-1]; // linha do arquivo original
                
                // recupera o offset da instrucao
                int offset = machineCode[address];
                
                if (auxInfo == 1) { // é uma divisão
                    if (labelList[i].isConst == 2)
                        errorList.push_back(Error ("divisão por zero", "semântico", origLine, lines[mcrLine-1]));
                } else if (auxInfo == 2) { // é um pulo
                    if (labelList[i].vectSize != 0)
                        errorList.push_back(Error ("pulo para seção inválida", "semântico", origLine, lines[mcrLine-1]));
                } else if (auxInfo == 3) { // tá modificando o rótulo
                    if (labelList[i].isConst != 0)
                        errorList.push_back(Error ("valores constantes não podem ser modificados", "semântico", origLine, lines[mcrLine-1]));
                }
                
                // se não for pulo, não pode acessar a área de texto
                if (labelList[i].vectSize == 0 && auxInfo != 2)
                    errorList.push_back(Error ("acesso à seção de texto só é permitido para pulos", "semântico", origLine, lines[mcrLine-1]));
                
                // nao se pode usar offset com pulos
                if (auxInfo == 2 && offset != 0)
                    errorList.push_back(Error ("o deslocamento de pulos deve ser zero", "semântico", origLine, lines[mcrLine-1]));
                
                // checa se o tamanho do rotulo bate com o indice n (rotulo + n)
                if (offset >= labelList[i].vectSize && auxInfo != 2 && labelList[i].vectSize > 0)
                    errorList.push_back(Error ("indíce excede o tamanho do vetor "+labelList[i].name, "semântico", origLine, lines[mcrLine-1]));
                
                machineCode[address] = labelList[i].value+offset;
                
            }
            
        }
        
    }
    
    // escreve o codigo de maquina final no arquivo
    for (unsigned int i = 0; i < machineCode.size(); ++i)
        outFile << machineCode[i] << " ";
    
    mcrFile.close();
    outFile.close();
    
    return;
}