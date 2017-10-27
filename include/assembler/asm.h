/*      PRE.H: funções relativas à montagem do codigo fonte        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
int constCheck (std::string&, int&);
int spaceCommand (std::stringstream&, std::string&, std::vector<int>&, int&, std::vector<Label>&, int&);
int constCommand (std::stringstream&, std::string&, std::vector<int>&, int&, std::vector<Label>&, int&);
int assembleInstr (Instr&, int&, std::vector<int>&, std::vector<Label>&, std::stringstream&, std::vector<Instr>&, std::vector<Dir>&, int&);
std::vector<int> asmParser (std::ifstream&, std::vector<Label>&, int&, int&, std::vector<int>&, std::vector<Instr>&, std::vector<Dir>&, int&, int&, std::vector<int>&, std::vector<std::string>&, std::vector<Error>&, std::string&);
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
saida: codigo de erro
*/
int spaceCommand (std::stringstream &lineStream, std::string &labelName, std::vector<int> &partialMachineCode, int &addrCounter, std::vector<Label> &labelList, int& pos) {
    
    int amount; // número de espaços a serem reservados
    
    // token do argumento
    std::string token2;
    lineStream >> token2;
    
    pos = 6;
    if (!labelName.empty())
        pos += labelName.size()+1 + 1;
    
    if (token2.empty()) // se não tem argumento, só reserva um espaço
        amount = 1;    
        
    else { // se tem argumentos, checa se é um número válido
        int status = integerCheck (token2, amount);
        if ((status == 1 && amount < 1) || status == 0)  // não pode ser um número menor que 1
            return -1;
    }
    
    // extrai novo token 
    std::string token3;
    lineStream >> token3;
    
    pos += token2.size() + 1;
    
    // se houver mais um, o numero de argumentos eh invalido
    if (!token3.empty())
        return -3;
    
    for (int i = 0; i < amount; ++i) { // coloca a reserva no codigo de maquina
        partialMachineCode.push_back(0);
        addrCounter++;
    }
    
    pos = 0;
    
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
saida: codigo de erro
*/
int constCommand (std::stringstream &lineStream, std::string &labelName, std::vector<int> &partialMachineCode, int &addrCounter, std::vector<Label> &labelList, int& pos) {
    
    int constant;
    
    // le o proximo token
    std::string token2;
    lineStream >> token2;
    
    pos = 6;
    if (!labelName.empty())
        pos += labelName.size()+1 + 1;
    
    if (token2.empty()) // checa se foi dado um argumento
        return -1;
        
    int status = constCheck (token2, constant); // checa se é um numero valido
    if (status == 0)
        return -2;
        
    // le mais um token
    std::string token3;
    lineStream >> token3;
    
    pos += token2.size() + 1;
    
    // se conseguir ler, o numero de argumento eh invalido
    if (!token3.empty())
        return -1;
        
    partialMachineCode.push_back (constant); // adiciona a constante no código de máquina
    addrCounter++;
    
    pos = 0;
    
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
saida: codigo de erro da montagem
*/
int assembleInstr (Instr &instr, int &addrCounter, std::vector<int> &partialMachineCode, std::vector<Label> &labelList, std::stringstream &lineStream, std::vector<Instr> &instrList, std::vector<Dir> &dirList, int &pos) {
    
    // salva o codigo de maquina da instrucao
    partialMachineCode.push_back(instr.opcode);
    addrCounter++;
    
    int firArgPos = pos,
        secArgPos = 0,
        argPos = 0;
    
    // le os argumentos esperados pela funcao
    for (int i = 0; i < instr.numArg; ++i) {
        
        // le um token
        std::string token;
        lineStream >> token;
        
        // retorna -1 se faltam argumentos (token vazio antes de chegar ao final do for)
        if (token.empty()) // pos += 0
            return -1;
        
        // checa se esta tentando declarar outro rotulo
        if (token.back() == ':') // pos += 0
            return -17;
        
        int offset = 0; // offset para acesso aos vetores
            
        // se for COPY (2 args) ou qualquer instrução com mais de um argumento (atualmente só tem o COPY mesmo), verifica todos menos o último argumento esperado (somente o 1o, no caso de COPY)
        if (instr.numArg > 1 && i < instr.numArg-1) {
            
            // procura virgula no meio do token
            std::size_t comma = token.find(",");
            if (comma != std::string::npos) {
                if (comma != 0 && comma != token.size()-1) {
                    pos += comma;
                    return -18; // tá no meio do token, entao nao separou por virgula do segundo argumento
                }
            }
            
            if (token.front() == ',') // pos += 0
                return -20; // virgula no começo do token = falta argumento antes
                
            // procura sinal de soma no token
            std::size_t add = token.find("+");
            if (add != std::string::npos) {
                pos += add;
                if (add == 0)
                    return -13; // indexacao incompleta
                else
                    return -19; // precisa ter espaço entre a soma
            }
            
            // se no final do token tiver virgula
            if (token.back() == ',') {
                
                token.pop_back(); // tira a virgula
                
                if (token.empty()) // pos += 0
                    return -20; // se tiver colocado só uma vírgula
                
                int aux = 0; // checa se está tentando fazer endereçamento imediato
                int status = integerCheck (token, aux);
                if (status == 1) // pos += 0
                    return -16;
                
                aux = 0;
                int valid = labelCheck(token, instrList, dirList, aux); // procura erros no rotulo
                if (valid <= -1 && valid >= -4) {
                    pos += aux;
                    return valid-8;
                }
                
                // ajeita a posicao para o segundo argumento
                pos = pos + token.size()+1 + 1;
                secArgPos = pos;
                
            // se nao tiver virgula
            } else {
                
                int aux = 0; // checa se está tentando se fazer endereçamento imediato
                int status = integerCheck (token, aux);
                if (status == 1) // pos += 0
                    return -16;
                
                aux = 0;
                int valid = labelCheck(token, instrList, dirList, aux); // procura erros no rotulo
                if (valid <= -1 && valid >= -4) {
                    pos += aux;
                    return valid-8;
                }
                
                // avança a posicao da seta para o proximo token
                pos = pos + token.size() + 1;
                
                // le o proximo token (esperado que seja um +)
                std::string token2;
                lineStream >> token2;
                
                if (token2.empty()) // pos += 0
                    return -1; // falta um argumento para copy
                
                if (token2.front() == ',') // pos += 0
                    return -2; // se for virgula, avisa que a virgula nao deve ser separada por espaço do 1o operador
                    
                else if (token2 != "+") {
                    int auxPos;
                    int valid = labelCheck(token2, instrList, dirList, auxPos); // procura erros no rotulo
                    if (valid == 0) // pos += 0
                        return -7; // faltando virgula entre os operandos
                    else {
                        if (token2.front() == '+') // pos += 0
                            return -19; // deve separar o sinal de soma por espaços
                        else // pos += 0
                            return -6; // operacao de indexacao invalida
                    }
                }
                
                // le o proximo token (esperado que seja um numero valido)
                std::string token3;
                lineStream >> token3;
                
                // ajeita a posicao de novo
                pos = pos + token2.size() + 1;
                
                if (token3.empty()) // pos += 0
                    return -13;
                
                // esperado que tenha uma virgula depois do numero
                if (token3.back() == ',') {
                    token3.pop_back();
                    if (token3.empty()) // pos += 0
                        return -13;
                    int status = integerCheck (token3, offset);
                    if ((status == 1 && offset < 0) || status == 0)  // tem que ser maior ou igual a 0
                        return -8; // pos += 0
                        
                // se nao tem virgula
                } else { 
                    int status = integerCheck (token3, offset);
                    if ((status == 1 && offset < 0) || status == 0)  // tem que ser maior ou igual a 0
                        return -8; // pos += 0
                    //  se for valido, le o proximo token (esse token ta errado de qualquer forma)
                    std::string token4;
                    lineStream >> token4;
                    pos = pos + token3.size() + 1;
                    if (token4.empty()) // pos += 0
                        return -1; // de estiver vazio, o numero de argumento esta errado
                    if (token4 == ",") // pos += 0
                        return -2; // se o token for um virgula, nao deveria ter sido serpada por espaço
                    else // pos += 0
                        return -7; // faltando virgula entre os operandos
                }
                
                // ajeita a posicao para o segundo argumento
                pos = pos + token3.size()+1 + 1;
                secArgPos = pos;
            }
            
        } else { // se nao for esperada uma virgula
            
            // procura virgula no meio do token
            std::size_t comma = token.find(",");
            if (comma != std::string::npos) {
                if (comma != 0 && comma != token.size()-1) {
                    pos += comma;
                    return -18; // tá no meio do token, entao nao separou por virgula do segundo argumento
                }
            }
            
            // procura sinal de soma no token
            std::size_t add = token.find("+");
            if (add != std::string::npos) {
                pos += add;
                if (add == 0)
                    return -13; // indexacao incompleta
                else
                    return -19; // precisa ter espaço entre a soma
            }
            
            if (token.front() == ',') // pos += 0
                return -20; // virgula no começo do token = falta argumento
            
            // se no final tiver virgula, ve se foi colocado mais um argumento (invalido)
            if (token.back() == ',') {
                std::string token2;
                lineStream >> token2;
                if (!token2.empty()) {
                    pos = pos + token.size() + 1;
                    return -1; // numero de argumentos invalido
                }
            }
            
            int aux; // checa se está tentando fazer endereçamento imediato
            int status = integerCheck (token, aux);
            if (status == 1) // pos += 0
                return -16;
            
            // procura erros no rotulo
            aux = 0;
            int valid = labelCheck(token, instrList, dirList, aux);
            if (valid <= -1 && valid >= -4) {
                pos += aux;
                return valid-8;    
            }
            
            pos = pos + token.size() + 1;
            
            // le o proximo token
            std::string token2;
            lineStream >> token2;
            
            if (token2 == "+" && !token2.empty()) {
                
                // le mais um token
                std::string token3;
                lineStream >> token3;
                
                pos = pos + token2.size() + 1;
                
                if (token3.empty()) // pos += 0
                    return -13;
                
                if (token3.front() == ',') // pos += 0
                    return -20; // virgula no começo do token = falta argumento
                
                int popped = 0;
                if (token3.back() == ',') {
                    popped = 1;
                    token3.pop_back();
                }    
                
                // verica se o numero eh valido
                int status = integerCheck (token3, offset);
                if ((status == 1 && offset < 0) || status == 0)  // tem que ser maior ou igual a 0
                    return -8;
                
                if (popped) 
                    token3.push_back(',');
                    
                // se no final tiver virgula, ve se foi colocado mais um argumento (invalido)
                if (token3.back() == ',') {
                    std::string token4;
                    lineStream >> token4;
                    if (!token4.empty()) {
                        pos = pos + token3.size() + 1;
                        return -1;
                    }
                }
            
            } else if (!token2.empty()) {
                
                if (token2.front() == '+') // pos += 0
                    return -19; // deve separar o sinal de soma por espaços
                
                // checa se esta tentando declarar outro rotulo
                if (token2.back() == ':') {
                    token2.pop_back();
                    int aux;
                    int valid = labelCheck(token2, instrList, dirList, aux);
                    if (valid == 0) // se for um rotulo valido, estava tentando declarar dois rotulos
                        return -17; // pos += 0
                    else // se nao era, operacao de indexacao invalida
                        return -6; // pos += 0
                } else {
                    int aux;
                    int valid = labelCheck(token2, instrList, dirList, aux); // checa se eh um rotulo valido
                    if (valid == 0) // pos += 0
                        return -1; // se for, numero de argumento invalido
                    else // pos += 0
                        return -6; // se nao for, indexacao invalida
                }
                
            }
            
        }
        
        // ajusta a posicao do argumento
        if (i == 0)
            argPos = firArgPos;
        else
            argPos = secArgPos;
        
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
            
            label.posList.push_back(argPos);
                
            label.pendList.push_back(addrCounter);
            labelList.push_back(label);
            partialMachineCode.push_back(offset);
            
        } else {
            
            // se ta na tabela e nao ta definido ou é externo, coloca mais uma pendencia
            if (!labelList[found].isDefined || labelList[found].isExtern) {
                
                if (instr.name == "DIV")
                    labelList[found].auxInfoList.push_back(1); // 1 indica instrucao de divisao
                else if (instr.name == "JMP" || instr.name == "JMPP" || instr.name == "JMPN" || instr.name == "JMPZ")
                    labelList[found].auxInfoList.push_back(2); // 2 indica instrucao de pulo
                else if ((instr.name == "COPY" && i == 1) || instr.name == "STORE" || instr.name == "INPUT")
                    labelList[found].auxInfoList.push_back(3); // 3 indica instrucao modificando a memoria
                else
                    labelList[found].auxInfoList.push_back(0); // 0 indica instrucao padrao
                
                labelList[found].posList.push_back(argPos);
                    
                labelList[found].pendList.push_back(addrCounter);
                partialMachineCode.push_back(offset);
            }
            
            // se ta na tabela e ta definido, ja indica os problemas e copia o endereço no codigo de maquina
            else {
                
                int posBkp = pos;
                pos = argPos;
                
                if (instr.name == "DIV") {
                    if (labelList[found].isConst == 2 && !labelList[found].isExtern)
                        return -3; // -3 indica divisao por zero
                } else if (instr.name == "JMP" || instr.name == "JMPP" || instr.name == "JMPN" || instr.name == "JMPZ") {
                    if (labelList[found].vectSize != 0 && !labelList[found].isExtern) // vectSize = 0 indica que o rotulo é da seção de texto
                        return -4; // -4 indica pulo para a seção de dados
                } else if ((instr.name == "COPY" && i == 1) || instr.name == "STORE" || instr.name == "INPUT") {
                    if (labelList[found].isConst != 0 && !labelList[found].isExtern)
                        return -5; // -5 indica tentativa de modificar valor constante
                }
                        
                // o acesso a memoria não pode usar um rótulo da seção de texto
                if (labelList[found].vectSize == 0 && instr.name != "JMP" && instr.name != "JMPP" && instr.name != "JMPN" && instr.name != "JMPZ" && !labelList[found].isExtern)
                    return -14;
                
                pos = pos + token.size() + 1 + 1 + 1;
                    
                // nao se pode usar offset com pulos
                if (instr.opcode >= 5 && instr.opcode <= 8) {
                    if (offset != 0)
                        return -15;
                }
                
                // checa se o tamanho do rotulo bate com o indice n (rotulo + n)
                if (offset >= labelList[found].vectSize && labelList[found].vectSize > 0 && !labelList[found].isExtern)
                    return -(found+21); // retorna onde ta o rotulo
                
                pos = posBkp;
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
        int auxPos;
        int valid = labelCheck(token2, instrList, dirList, auxPos);
        if (valid == 0) // se for um rotulo valido, estava tentando declarar dois rotulos
            return -17; // pos += 0
        else // se nao era, so fala que o rotulo é invalido
            return -11; // pos += 0
    }
    
    // se houver mais um token, a quantidade de argumentos é maior q o esperado
    if (!token2.empty())
        return -1; // pos += 0
    
    return 0;
}



/*
asmParser: traduz uma linha em código máquina
entrada:
saida: código de máquina parcial num vetor de inteiros
*/
std::vector<int> asmParser (std::ifstream &mcrFile, std::vector<Label> &labelList, int &lineCounter, int &addrCounter, std::vector<int> &lineDict, std::vector<Instr> &instrList, std::vector<Dir> &dirList, int &section, int &sectionText, std::vector<int> &addrDict, std::vector<std::string> &lines, std::vector<Error> &errorList, std::string &bitMap) {
    
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
    
    int colon = 0;
    
    if (token.back() == ':') {
        
        // verifica se o rótulo é válido
        token.pop_back();
        int pos = 0;
        int valid = labelCheck(token, instrList, dirList, pos);
        if (valid == -1)
            errorList.push_back(Error("tamanho do rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineDict[lineCounter-1], line, pos));
        else if (valid == -2)
            errorList.push_back(Error("rótulos não podem começar com números", "léxico", lineDict[lineCounter-1], line, pos));
        else if (valid == -3)
            errorList.push_back(Error("caracter inválido encontrado no rótulo", "léxico", lineDict[lineCounter-1], line, pos));
        else if (valid == -4)
            errorList.push_back(Error("rótulo não pode ter nome de instrução ou diretiva", "semântico", lineDict[lineCounter-1], line, pos));
        else if (valid == -5)
            errorList.push_back(Error("declaração de rótulo vazia", "sintático", lineDict[lineCounter-1], line, pos));
        
        if (!token.empty()) {
            
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
            if (alreadyDefined) {
                int pos = 0;
                errorList.push_back(Error("símbolo já definido", "semântico", lineDict[lineCounter-1], line, pos));
            }   
                
            // nao foi definido mas ja foi mencionado (define ele agora)
            else if (alreadyMentioned) {
                labelList[labelPos].value = addrCounter;
                labelList[labelPos].isDefined = 1;
                labelList[labelPos].vectSize = 0; // indica que é rotulo da area de texto (pode mudar se for chamada uma diretiva da área de dados)
                labelList[labelPos].isConst = 0;
                labelList[labelPos].isExtern = false;
                if (labelList[labelPos].isPublic != true)
                    labelList[labelPos].isPublic = false;
            }
            
            // nunca foi chamado nem definido (acrescenta novo rotulo definido)
            else {
                Label label;
                label.name = token;
                label.value = addrCounter;
                label.isDefined = 1;
                label.vectSize = 0;
                label.isConst = 0;
                label.isExtern = false;
                label.isPublic = false;
                labelList.push_back(label);
            }
            
            // salva o nome do rotulo
            labelNameBackup = token;
            
        } else
            colon = 1;
        
        // coloca um novo token no lugar do rótulo, que já foi processado
        lineStream >> token;
        
        // ja checa pra ver se não é mais um rótulo
        if (token.back() == ':') {
            int pos = labelNameBackup.size()+1 + 1;
            errorList.push_back(Error ("mais de um rótulo em uma linha", "sintático", lineDict[lineCounter-1], line, pos));
        }
        
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
    if (isInstruction == -1 && isDirective == -1) {
        if (token.back() != ':') {
            int pos = 0;
            if (!labelNameBackup.empty())
                pos = labelNameBackup.size()+1 + 1;
            else if (colon)
                pos += 2;
            errorList.push_back(Error("comando não reconhecido", "léxico", lineDict[lineCounter-1], line, pos));
        }
            
    } else {
        
        // se for uma instrução, monta
        if (isInstruction >= 0) {
            int pos = 0;
            if (!labelNameBackup.empty())
                pos = labelNameBackup.size()+1 + 1;
            else if (colon)
                pos += 2;
            Instr instr = instrList[isInstruction];
            pos += instr.name.size()+1;
            int status = assembleInstr (instr, addrCounter, partialMachineCode, labelList, lineStream, instrList, dirList, pos);
            if (status == -1) {
                if (instr.numArg == 0)
                    errorList.push_back(Error("não é esperado nenhum argumento para "+instr.name, "sintático", lineDict[lineCounter-1], line, pos));
                else if (instr.numArg == 1)
                    errorList.push_back(Error("é esperado 1 argumento para "+instr.name, "sintático", lineDict[lineCounter-1], line, pos));
                else if (instr.numArg == 2)
                    errorList.push_back(Error("são esperados 2 argumentos para "+instr.name, "sintático", lineDict[lineCounter-1], line, pos));
            } else if (status == -2)
                errorList.push_back(Error("a vírgula não deve ser separada do primeiro operando por espaço", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -3)
                errorList.push_back(Error ("divisão por zero", "semântico", lineDict[lineCounter-1], line, pos));
            else if (status == -4)
                errorList.push_back(Error ("pulo para seção inválida", "semântico", lineDict[lineCounter-1], line, pos));
            else if (status == -5)
                errorList.push_back(Error ("valores constantes não podem ser modificados", "semântico", lineDict[lineCounter-1], line, pos));
            else if (status == -6)
                errorList.push_back(Error ("operação de indexacão inválida, somente a soma é válida", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -7)
                errorList.push_back(Error ("deve haver uma vírgula entre os dois operandos da instrução", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -8)
                errorList.push_back(Error ("o deslocamento deve ser um número inteiro maior ou igual a zero", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -9)
                errorList.push_back(Error ("tamanho do rótulo deve ser menor ou igual a 100 caracteres", "léxico", lineDict[lineCounter-1], line, pos));
            else if (status == -10)
                errorList.push_back(Error ("rótulos não podem começar com números", "léxico", lineDict[lineCounter-1], line, pos));
            else if (status == -11)
                errorList.push_back(Error ("caracter inválido encontrado no rótulo", "léxico", lineDict[lineCounter-1], line, pos));
            else if (status == -12)
                errorList.push_back(Error ("rótulo não pode ter nome de instrução ou diretiva", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -13)
                errorList.push_back(Error ("estrutura da indexação incompleta", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -14)
                errorList.push_back(Error ("acesso à seção de texto só é permitido para pulos", "semântico", lineDict[lineCounter-1], line, pos));
            else if (status == -15)
                errorList.push_back(Error ("o deslocamento de pulos deve ser zero", "semântico", lineDict[lineCounter-1], line, pos));
            else if (status == -16)
                errorList.push_back(Error ("tentativa de endereçamento imediato", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -17)
                errorList.push_back(Error ("só um rótulo pode ser declarado, e no começo da linha", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -18)
                errorList.push_back(Error ("deve haver espaço em branco entre a vírgula e o segundo argumento", "sintático", lineDict[lineCounter-1], line, pos));  
            else if (status == -19)
                errorList.push_back(Error ("deve haver espaço em branco antes e depois do sinal de soma", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status == -20)
                errorList.push_back(Error ("é esperado um argumento antes da vírgula", "sintático", lineDict[lineCounter-1], line, pos));
            else if (status <= -21) {
                int labelPos = -(status+21); // recupera a posicao do rotulo
                errorList.push_back(Error ("indíce excede o tamanho do vetor "+labelList[labelPos].name, "semântico", lineDict[lineCounter-1], line, pos));
            }
                
            if (section != 0) {
                int pos = 0;
                if (!labelNameBackup.empty())
                    pos = labelNameBackup.size()+1 + 1;
                else if (colon)
                    pos += 2;
                errorList.push_back(Error("instruções devem estar na seção de texto", "semântico", lineDict[lineCounter-1], line, pos));
            }
            
            // opcode é sempre absoluto, e se houverem codigos adicionais, sao sempre endereços relativos
            bitMap.push_back('0');
            for (unsigned int i = 1; i < partialMachineCode.size(); ++i)
                bitMap.push_back('1');
            
        // se for uma diretiva, faz uma função específica
        } else if (isDirective >= 0) {
            
            Dir dir = dirList[isDirective];
            
            // se for SECTION, atualiza a informação da seção
            if (dir.name == "SECTION") {
                
                if (!labelNameBackup.empty()) {
                    int pos = 0;
                    errorList.push_back(Error("não podem ser declarados rótulos em seções", "sintático", lineDict[lineCounter-1], line, pos));
                }
                
                std::string token2;
                lineStream >> token2;
                if (token2 == "TEXT") {
                    section = 0;
                    sectionText = 0;
                } else if (token2 == "DATA")
                    section = 1;
                else {
                    int pos = token.size()+1;
                    if (!labelNameBackup.empty())
                        pos += labelNameBackup.size()+1 + 1;
                    else if (colon)
                        pos += 2;
                    errorList.push_back(Error("seção não reconhecida", "sintático", lineDict[lineCounter-1], line, pos));
                }
            
            // se for SPACE, verifica os argumentos e coloca no código de máquina as reservas
            } else if (dir.name == "SPACE") {
                int pos = 0;
                int status = spaceCommand (lineStream, labelNameBackup, partialMachineCode, addrCounter, labelList, pos);
                if (colon)
                    pos += 2;
                if (status == -1)
                    errorList.push_back(Error("número de elementos inválido", "léxico", lineDict[lineCounter-1], line, pos));
                else if (status == -2)
                    errorList.push_back(Error("a diretiva SPACE precisa ser precedida de um rótulo", "sintático", lineDict[lineCounter-1], line, pos));
                else if (status == -3)
                    errorList.push_back(Error("é esperado um ou nenhum argumento para SPACE", "léxico", lineDict[lineCounter-1], line, pos));
                    
                if (section != 1) {
                    int pos = 0;
                    if (!labelNameBackup.empty())
                        pos = labelNameBackup.size()+1 + 1;
                    else if (colon)
                        pos += 2;
                    errorList.push_back(Error("SPACE deve estar na seção de dados", "semântico", lineDict[lineCounter-1], line, pos));
                }
                
                // o conteúdo da reserva é sempre absoluto
                for (unsigned int i = 0; i < partialMachineCode.size(); ++i)
                    bitMap.push_back('0');                    
            
            // se for CONST, verifica o argumento e salva no código de máquina
            } else if (dir.name == "CONST") {
                int pos = 0;
                int status = constCommand (lineStream, labelNameBackup, partialMachineCode, addrCounter, labelList, pos);
                if (colon)
                    pos += 2;
                if (status == -1)
                    errorList.push_back(Error("é esperado 1 argumento para a reserva de constante", "sintático", lineDict[lineCounter-1], line, pos));
                else if (status == -2)
                    errorList.push_back(Error("número constante inválido", "léxico", lineDict[lineCounter-1], line, pos));
                else if (status == -3)
                    errorList.push_back(Error("a diretiva CONST precisa ser precedida de um rótulo", "sintático", lineDict[lineCounter-1], line, pos));
                    
                if (section != 1) {
                    int pos = 0;
                    if (!labelNameBackup.empty())
                        pos = labelNameBackup.size()+1 + 1;
                    else if (colon)
                        pos += 2;
                    errorList.push_back(Error("CONST deve estar na seção de dados", "semântico", lineDict[lineCounter-1], line, pos));
                }
                
                // o valor da constante é absoluto
                bitMap.push_back('0');   
                      
            } else if (dir.name == "EXTERN") {
                for (unsigned int i = 0, found = 0; i < labelList.size() && !found; ++i) {
                    if (labelList[i].name == labelNameBackup) {
                        found = 1;
                        labelList[i].isExtern = true;
                        labelList[i].isDefined = 1;
                        labelList[i].value = 0;
                    }
                }
                
            } else if (dir.name == "PUBLIC") {
                std::string token2;    
                lineStream >> token2;      
                
                int alreadyMentioned = 0;
                int labelPos = -1;
                for (unsigned int i = 0; i < labelList.size(); ++i) {
                    if (labelList[i].name == token2) {
                        alreadyMentioned = 1;
                        labelPos = i;
                    }
                }
                
                if (alreadyMentioned)
                    labelList[labelPos].isPublic = true;
                else {
                    Label label;
                    label.name = token2;
                    label.isDefined = 0;
                    label.isPublic = true;
                    labelList.push_back(label);
                }
                
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
    
    std::string bitMap; // mapa de bits para os endereços relativos
    
    while (!mcrFile.eof()) {
        
        // le o codigo de maquina parcial da linha
        std::vector<int> partialMachineCode = asmParser(mcrFile, labelList, lineCounter, addrCounter, lineDict, instrList, dirList, section, sectionText, addrDict, lines, errorList, bitMap);
        
        // anexa os codigos parciais
        for (unsigned int i = 0; i < partialMachineCode.size(); ++i)
            machineCode.push_back(partialMachineCode[i]);
        
        lineCounter++;
        
    }
    
    if (sectionText == -1)
        errorList.push_back(Error ("seção texto é obrigatória", "semântico", -1, "", 0));
    
    // resolve as listas de pendências (e reporta erros), e cria listas com os rotulos publicos e os rotulos externos
    std::vector<Label> publicLabelList;
    std::vector<Label> externLabelList;
    for (unsigned int i = 0; i < labelList.size(); ++i) {
        
        if (labelList[i].isPublic)
            publicLabelList.push_back(labelList[i]);
        
        if (labelList[i].isExtern)
            externLabelList.push_back(labelList[i]);
        
        if (!labelList[i].isDefined) { // rotulo nunca foi definido
            
            for (unsigned int j = 0; j < labelList[i].pendList.size(); ++j) {
                int argPos = labelList[i].posList[j];
                int mcrLine = addrDict[labelList[i].pendList[j]]; // linha do arquivo .mcr
                int origLine = lineDict[mcrLine-1]; // linha do arquivo original
                errorList.push_back(Error ("rótulo "+labelList[i].name+" não definido", "semântico", origLine, lines[mcrLine-1], argPos));
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
                
                // recupera a posição do rótulo na linha
                int argPos = labelList[i].posList[j];
                
                if (auxInfo == 1) { // é uma divisão
                    if (labelList[i].isConst == 2 && !labelList[i].isExtern)
                        errorList.push_back(Error ("divisão por zero", "semântico", origLine, lines[mcrLine-1], argPos));
                } else if (auxInfo == 2) { // é um pulo
                    if (labelList[i].vectSize != 0 && !labelList[i].isExtern)
                        errorList.push_back(Error ("pulo para seção inválida", "semântico", origLine, lines[mcrLine-1], argPos));
                } else if (auxInfo == 3) { // tá modificando o rótulo
                    if (labelList[i].isConst != 0 && !labelList[i].isExtern)
                        errorList.push_back(Error ("valores constantes não podem ser modificados", "semântico", origLine, lines[mcrLine-1], argPos));
                }
                
                // se não for pulo, não pode acessar a área de texto
                if (labelList[i].vectSize == 0 && auxInfo != 2 && !labelList[i].isExtern)
                    errorList.push_back(Error ("acesso à seção de texto só é permitido para pulos", "semântico", origLine, lines[mcrLine-1], argPos));
                
                argPos = argPos + labelList[i].name.size() + 1 + 1 + 1;
                    
                // nao se pode usar offset com pulos
                if (auxInfo == 2 && offset != 0)
                    errorList.push_back(Error ("o deslocamento de pulos deve ser zero", "semântico", origLine, lines[mcrLine-1], argPos));
                
                // checa se o tamanho do rotulo bate com o indice n (rotulo + n)
                if (offset >= labelList[i].vectSize && auxInfo != 2 && labelList[i].vectSize > 0 && !labelList[i].isExtern)
                    errorList.push_back(Error ("indíce excede o tamanho do vetor "+labelList[i].name, "semântico", origLine, lines[mcrLine-1], argPos));
                
                machineCode[address] = labelList[i].value+offset;
                
            }
            
        }
        
    }
    
    // monta a tabela de uso
    std::stringstream useTable;
    useTable << externLabelList.size() << " ";
    for (unsigned int i = 0; i < externLabelList.size(); ++i) {
        useTable << externLabelList[i].name << " ";
        useTable << externLabelList[i].pendList.size() << " ";
        for (unsigned int j = 0; j < externLabelList[i].pendList.size(); ++j)
            useTable << externLabelList[i].pendList[j] << " ";
    }
    
    // monta a tabela de definições
    std::stringstream defTable;
    defTable << publicLabelList.size() << " ";
    for (unsigned int i = 0; i < publicLabelList.size(); ++i) {
        defTable << publicLabelList[i].name << " ";
        defTable << publicLabelList[i].value << " ";
    }
    
    // escreve o cabeçalho no arquivo
    outFileName = outFileName.substr(0, outFileName.size() - 2);
    std::transform (outFileName.begin(), outFileName.end(), outFileName.begin(), ::toupper);
    outFile << "H: " << outFileName << "\n";
    outFile << "H: " << machineCode.size() << "\n";
    outFile << "H: " << bitMap << "\n";
    outFile << "TU: " << useTable.str() << "\n";
    outFile << "TD: " << defTable.str() << "\n";
    
    // escreve o codigo de maquina final no arquivo
    outFile << "T: ";
    for (unsigned int i = 0; i < machineCode.size(); ++i)
        outFile << machineCode[i] << " ";
    outFile << "\n";
    
    mcrFile.close();
    outFile.close();
    
    return;
}