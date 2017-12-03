/*      LOADER.H: funções relativas ao carregamento do código        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
void getChunks (int, char**, std::vector<Chunk>&);
void getData (std::string, int&, std::string&, int&, std::vector<int>&);
void simulateCode (int, std::vector<int>);
bool operator< (const Chunk&, const Chunk&);
int fitCode (int, std::vector<Chunk>&, std::vector<Chunk>&);
void fragmentCode (int, std::string, std::vector<int>&, std::vector<Chunk>&, int&);


/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
getChunks: armazena as informações sobre os chunks dados
entrada: argc, argv e referência para a lista de chunks
saída: nenhuma (lista de chunks dada por referência)
*/
void getChunks (int argc, char *argv[], std::vector<Chunk> &chunkList) {
    
    // faz o tamanho da lista ser numero de chunks
    int numChunks = stoi (std::string(*(argv + 2)));
    
    // cria a lista de chunks
    for (int i = 0; i < numChunks; ++i) {
        int size = stoi (std::string(*(argv + 3 + i)));
        int start = stoi (std::string(*(argv + 3 + i + numChunks)));
        Chunk tempChunk (start, size);
        chunkList.push_back(tempChunk); 
    }
    
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
                std::cout << "\033[34;1m" << ">> " << "\033[0m";
                std::cin >> machineCode[machineCode[currAddr+1]];
                break;
            case 13:    // OUTPUT
                std::cout << "\033[31;1m" << ">> " << "\033[0m" << machineCode[machineCode[currAddr+1]] << "\n";
                break;
            case 14:    // STOP
                break;
        }
        
        currAddr = nextAddr;        
        
    }
    
}

/*
operator<: overload do operador < para poder usar std::sort com os chunks
entrada: duas structs do tipo Chunk
saida: se o primeiro é menor que o segundo (compara os começos)
*/
bool operator< (const Chunk &A, const Chunk &B) {
    return (A.start < B.start);
}

/*
fitCode: determina em quantos chunks o código cabe, e quais são eles
entrada: tamanho do código e referências para a lista original de chunks e para a lista de chunks minimos
saida: se coube (0) ou não (-1) (lista de chunks minimos dada por referencia)
*/
int fitCode (int codeSize, std::vector<Chunk> &chunkList, std::vector<Chunk> &minChunkList) {
    
    int fit = -1;
    
    // reodena a lista com base no começo do chunk
    std::sort (chunkList.begin(), chunkList.end()); 
    
    // será usado para determinar a menor soma de chunks em que o codigo caiba
    int sum = std::numeric_limits<int>::max();
    
    do {
        
        // soma os chunks, na ordem da combinacao atual, pra ver se a soma cabe
        for (unsigned int n = 0; n < chunkList.size(); ++n) {
            
            // soma os n primeiros chunks da combinacao
            int tempSum = 0;
            for (unsigned int i = 0; i < n+1; ++i)
                tempSum += chunkList[i].size;
            
            // ve se o codigo cabe nessa soma de n chunks e se é menor que a soma minima. ou, se a soma for igual, verifica se o numero n de chunks é menor do que o da soma anterior
            if (tempSum >= codeSize && (tempSum < sum || (tempSum == sum && (n+1) < minChunkList.size()))) {
                fit = 0;
                sum = tempSum;
                minChunkList = chunkList; // salva a lista de chunks que resultou nessa soma minima
                for (unsigned int i = 0; i < chunkList.size()-(n+1); ++i)
                    minChunkList.pop_back();
            }
            
        }
        
    // o laço itera entre todas as permutações possíveis da lista de chunks
    } while (std::next_permutation(chunkList.begin(), chunkList.end()));
    
    return fit;
    
}

/*
fragmentCode: realoca o código no conjunto mínimo de chunks necessários determinado anteriormente
entrada: tamanho do código, mapas de bits, código de máquina, conjunto de chunks mínimo e começo da seção de texto
saída: nenhuma (altera o começo da seção de texto por referência)
*/
void fragmentCode (int codeSize, std::string bitMap, std::vector<int> &machineCode, std::vector<Chunk> &minChunkList, int &codeStart) {
    
    // determina para qual chunk aponta o endereço e faz a realocação
    for (int i = 0; i < codeSize; ++i) {
        if (bitMap[i] == '1') {
            int chunk = -1;
            int totalSize = 0;
            for (unsigned int n = 0; (n < minChunkList.size() && chunk == -1); ++n) {
                totalSize += minChunkList[n].size;
                if (machineCode[i] < totalSize)
                    chunk = n;
            }
            machineCode[i] += minChunkList[chunk].start;
        }
    }
    
    // determina o novo começo da seção de texto
    codeStart += minChunkList[0].start;
    
}

/*
displayChunks: mostra na tela como ficou a alocação do código
entrada: código de máquina e lista de chunks
saída: nenhuma (código no terminal)
*/
void displayChunks (std::vector<int> &machineCode, std::vector<Chunk> &minChunkList) {
    
    // configura algumas cores
    std::string escRed = "\033[31;1m",
    escGreen = "\033[32;1m",
    escYellow = "\033[33;1m",
    escBlue = "\033[34;1m",
    escReset = "\033[0m";
    
    // mostra em quais chunks o programa será carregado
    std::cout << "\nO programa será alocado nos chunks:\n\t" << escRed;
    for (unsigned int i = 0; i < minChunkList.size(); ++i)
        std::cout << minChunkList[i].start << "(" << minChunkList[i].size << ") ";
    std::cout << escReset << "\n";
    
    // escreve o arquivo no terminal
    std::cout << "\nImagem de memória:\n";
    int n = 0;
    int totalSize = minChunkList[0].size;
    std::cout << escRed << "[" << minChunkList[0].start << "]: " << escReset;
    for (int i = 0; i < (int)machineCode.size(); ++i) {
        if (i == totalSize) {
            n++;
            std::cout << escRed << "[" << minChunkList[n].start << "]: " << escReset;
            totalSize += minChunkList[n].size;
        }
        std::cout << machineCode[i] << " ";
    }
    std::cout << "\n";
    
}