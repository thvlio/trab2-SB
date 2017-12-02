/*      LOADER.H: funções relativas ao carregamento do código        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
int errorCheck (int, char**, std::string&);
void getData (std::string, int&, std::string&, int&, std::vector<int>&);
void simulateCode (int, std::vector<int>);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
errorCheck: verifica a quantidade de argumentos dados e se o arquivo de entrada existe
entrada: argc, argv e string para armazenar o nome do arquivo
saída: inteiro indicando erro (0: sem erro, -1: erro) (nome do arquivo dado por referência)
*/
int errorCheck (int argc, char *argv[], std::string &fileName) {
    
    // verifica o numero de argumentos dados
    if (argc != 2) {
        std::cout << "Deve ser fornecido 1 arquivo de entrada. (" << argc-1 << " fornecidos)\n";
        return -1;
    }
    
    // guarda o argumento em uma string
    fileName = std::string(*(argv+1));
    
    // verifica se o arquivo de entrada existe
    std::ifstream file (fileName);
    if (!file.is_open()) {
        std::cout << "Erro ao abrir o arquivo de entrada " << fileName << "\n";
        return -1;
    } else
        file.close();
    
    return 0;
}

/*
getData: extrai informações de cabeçalho e código de máquina do arquivo de entrada
entrada: nome do arquivo de entrada e referências para o tamanho do código, mapa de bits, começo da seção de texto e código de máquina
saída: nenhuma (informações dadas por referência)
*/
void getData (std::string fileName, int &codeSize, std::string &bitMap, int &codeStart, std::vector<int> &machineCode) {
    
    std::ifstream file (fileName);
    std::string tempLine;
    
    // primeira linha: nome do arquivo (ignorada)
    getline (file, tempLine);
    
    // segunda linha: tamanho do código máquina (poderia ser ignorada)
    file >> tempLine >> codeSize;
    machineCode.resize(codeSize);
    getline(file, tempLine);
    
    // terceira linha: mapa de bits
    file >> tempLine >> bitMap;
    getline(file, tempLine);
    
    // quarta linha: começo da seção de texto
    file >> tempLine >> codeStart;
    getline(file, tempLine);
    
    // quinta linha: código máquina
    file >> tempLine;
    for (int i = 0; i < codeSize; ++i)
        file >> machineCode[i];
        
    file.close();
    
}

/*
simulateCode: simula a execução do código
entrada: nome do arquivo de entrada
saida: nenhuma
*/
void simulateCode (int codeStart, std::vector<int> machineCode) {
    
    // configurações iniciais
    int currAddr = codeStart;
    int nextAddr;
    int opcode = 0;
    
    // registrador acumulador
    int acc;
    
    // enquanto não chegar na instrução STOP
    while (opcode != 14) {
        
        // pega o opcode atual
        opcode = machineCode[currAddr];
        nextAddr = currAddr + 2;
        
        switch (opcode) {
            case 1: // ADD
                acc += machineCode[machineCode[currAddr+1]];
                break;
            case 2: // SUB
                acc -= machineCode[machineCode[currAddr+1]];
                break;
            case 3: // MULT
                acc *= machineCode[machineCode[currAddr+1]];
                break;
            case 4: // DIV
                acc /= machineCode[machineCode[currAddr+1]];
                break;
            case 5: // JMP
                nextAddr = machineCode[currAddr+1];
                break;
            case 6: // JMPN
                if (acc < 0)
                    nextAddr = machineCode[currAddr+1];
                break;
            case 7: // JMPP
                if (acc > 0)
                    nextAddr = machineCode[currAddr+1];
                break;
            case 8: // JMPZ
                if (acc == 0)
                    nextAddr = machineCode[currAddr+1];
                break;
            case 9: // COPY
                machineCode[machineCode[currAddr+2]] = machineCode[machineCode[currAddr+1]];
                nextAddr++;
                break;
            case 10:    // LOAD
                acc = machineCode[machineCode[currAddr+1]];
                break;
            case 11:    // STORE
                machineCode[machineCode[currAddr+1]] = acc;
                break;
            case 12:    // INPUT
                std::cout << ">> ";
                std::cin >> machineCode[machineCode[currAddr+1]];
                break;
            case 13:    // OUTPUT
                std::cout << ">> " << machineCode[machineCode[currAddr+1]] << "\n";
                break;
            case 14:    // STOP
                break;
        }
        
        currAddr = nextAddr;        
        
    }
    
}