/*      LKR.H: funções relativas à ligação dos codigos objetos        */



/*      DECLARAÇÕES DAS FUNÇÕES      */

void mergeLines(std::string, int, std::string&, std::vector<int>&, std::vector<tUso>&);
void DefLoad (std::string, std::vector<tDef>&, int&);
void linkerCode(std::vector<std::string>, std::vector<tDef>&, std::vector<tUso>&, std::vector<int>, std::string, std::string, std::vector<int>);
void UseLoad(std::vector<tUso>&, std::string, int);
void oneObject (std::string);
int checkTables (std::vector<tDef>&, std::vector<tUso>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
mergelines: junta o mapa de bits e os codigos maquina dos arquivos
entrada: arquivos objeto, vetor de offsets, mapa de bits e codigos maquina
saida: arquivo ligado
*/
void mergeLines(std::string inFileNames, int offset_vector, std::string &bitMap, std::vector<int> &machineCode, std::vector<tUso> &useTable){
    
    std::ifstream instrFile (inFileNames);

    std::string bitMapTemp;
    int machineCodeTemp;    
    std::string linha,
                token;

    // ignora as 4 primeiras linhas
    getline(instrFile, linha);
    getline(instrFile, linha);
    getline(instrFile, linha);
    getline(instrFile, linha);

    instrFile >> token; // ignora o H:
    instrFile >> bitMapTemp; //recupera o mapa de bits

    bitMap += bitMapTemp; // se houver, concatena com o mapa de bits do arquivo anterior

    getline(instrFile, linha); // ignora o resto da linha do mapa de bits     
    getline(instrFile, linha); // ignora o offset de inicio da seção de texto

    instrFile >> token; // ignora o T:

    //preenche o vetor com o codigo maquina de todos os arquivos
    for(unsigned int i = 0; i < bitMapTemp.size(); i++){

        instrFile >> machineCodeTemp;
        int currAddr = machineCode.size(); // endereço atual
        
        // se o valor nao for absoluto, soma-se o offset de endereco
        if (bitMapTemp[i] == '0')
            machineCode.push_back(machineCodeTemp);
        else {
            int isExtern = 0; // se é um endereço externo
            // checa se o endereço atual é externo
            for (unsigned int i = 0; (i < useTable.size() && !isExtern); ++i) {
                for (unsigned int j = 0; (j < useTable[i].listUso.size() && !isExtern); ++j) {
                    if (currAddr == useTable[i].listUso[j])
                        isExtern = 1;
                }
            }
            if (!isExtern)
                machineCode.push_back(machineCodeTemp+offset_vector);
            else // nao soma o offset em endereços que nao foram resolvidos
                machineCode.push_back(machineCodeTemp);
        }
        
    }    
}
    
/*
linkerCode: gera o arquivo ligado
entrada: arquivos objeto, tabela de definições, vetor de offsets, mapa de bits e codigo maquina
saida: arquivo ligado
*/
void linkerCode(std::vector<std::string> inFileNames, std::vector<tDef> &defTable, std::vector<tUso> &useTable, std::vector<int> offset_vector, std::string outFileName, std::string bitMap, std::vector<int> machineCode){
    
    // pega o offset da seção de texto do primeiro modulo
    std::ifstream inFile (inFileNames[0]);
    std::string tempLine;
    int textOffset;
    for (int i = 0; i < 5; ++i)
        getline(inFile, tempLine);
    inFile >> tempLine >> textOffset;
    inFile.close();
    
    // abre o arquivo de saida
    std::ofstream outFile (outFileName);

    std::transform (outFileName.begin(), outFileName.end(), outFileName.begin(), ::toupper);
    outFile << "H: " << outFileName << '\n'; // coloca o nome do arquivo
    outFile << "H: " << machineCode.size() << '\n'; // coloca o tamanho do programa
    outFile << "H: " << bitMap << '\n'; // coloca o mapa de bits
    outFile << "H: " << textOffset << "\n"; // começo da seção de texto do primeiro modulo
    outFile << "T: ";

    //std::cout << useTable.size() << '\n';

    // coloca o codigo maquina com os endereços ligados
    for(unsigned int i = 0; i < useTable.size(); i++){

        std::string label = useTable[i].name; // pega a label da tabela de uso em sequencia
        
        int labelValue = 0;

        // procura na tabela de definicoes o valor do label
        for(unsigned int j = 0; j < defTable.size(); j++){
            
            if(defTable[j].name == label){
                labelValue = defTable[j].valor;                
            }      
        }

        // substitui o valor correto nos enderecos indicados na tabela de uso, no codigo de maquina
        for(unsigned int j = 0; j < useTable[i].listUso.size(); j++)
            machineCode[useTable[i].listUso[j]] += labelValue; // += é necessário quando estiver acessando um vetor
            
    }
    
    // preenche o arquivo de saida com o codigo de maquina
    for(unsigned int i = 0; i < machineCode.size(); i++)
        outFile << machineCode[i] << " ";    

    outFile.close();

}

/*
UseLoad: constroi a tabela de uso
entrada: tabela de uso, tabela de uso extraido do arquivo objeto
saida: tabela de definições preenchida
*/
void UseLoad(std::vector<tUso> &useTable, std::string inFileNames, int offset_vector){

    // abre o arquivo
    std::ifstream instrFile (inFileNames);

    std::string token;
    int num_labels=0;

    instrFile >> token; // ignora TU:

    instrFile >> num_labels; // recupera o numero de labels da tabela

    for(int i = 0; i < num_labels; i++){

        std::string name;
        instrFile >> name; // recupera o nome da label

        int label_rep=0;
        instrFile >> label_rep; // recupera a quantidade de vezes que o label aparece

        std::vector<int> listUso (label_rep);

        for(int i = 0; i < label_rep; i++){ // recupera as posições do label
            instrFile >> listUso[i];
            listUso[i]+=offset_vector;// soma o offset na posição
        }

        //for (unsigned int i = 0; i < listUso.size(); i++)
            //  std::cout << listUso[i] << '\n';

        // coloca o label na lista    
        tUso label (name, listUso);
        useTable.push_back(label);       

    }

    instrFile.close();

}


/*
DefLoad: constroi a tabela de definições
entrada: arquivo objeto, tabela de definições 
saida: tabela de definições preenchida
*/
void DefLoad (std::string inFileNames, std::vector<tDef> &defTable, int &offset){

    // abre o arquivo
    std::ifstream instrFile (inFileNames);
    
    std::string token;
    std::string linha;

    int num; // numero de labels na tabela

    getline(instrFile, linha); //ignora a primeira linha da tabela de uso
    instrFile >> token;        //ignora o "TD:""
    instrFile >> num;          //pega a quantidade de labels

    
    for(int i=0; i<num ; i++){    
        std::string name; // nome do label
        int valor;  // valor do label

        instrFile >> name;
        instrFile >> valor;

        valor += offset;

        // coloca o label na lista
        tDef label (name, valor);
        defTable.push_back(label);        
    }

    getline(instrFile, linha); // ignora a linha TD
    getline(instrFile, linha); // ignora o nome do modulo
    instrFile >> token;        // ignora o H:
    instrFile >> num;          // pega o tamanho do modulo para o offset
    
    offset += num;
    
    instrFile.close();

}

/*
oneObject: apaga as tabelas de uso e definições quando o usuario entra com apenas um arquivo
entrada: arquivo objeto 
saida: arquivo de saida
*/
void oneObject (std::string inFileNames){

    std::ifstream instrFile (inFileNames);
    
    std::string outFileName = inFileNames;
    outFileName = outFileName.substr(0, outFileName.size() - 2);
    
    std::ofstream outFile (outFileName);

    std::string linha;

    getline(instrFile,linha);
    getline(instrFile,linha);

    while(!instrFile.eof()){
        getline(instrFile,linha);
        outFile << linha << '\n';
    }

    std::cout << "\n\nnome do arquivo gerado: " << outFileName << "\n\n";

    instrFile.close();
    outFile.close();

}

/*
checkTables: procura por erros de dupla definição e rótulos públicos sem definição
entrada: tabela de definições e tabela de uso
saída: indicação de erro (0: sem erro, -1; erro)
*/
int checkTables (std::vector<tDef> &defTable, std::vector<tUso> &useTable) {
    
    // configura algumas cores
    std::string escRed = "\033[31;1m",
    escGreen = "\033[32;1m",
    escYellow = "\033[33;1m",
    escBlue = "\033[34;1m",
    escReset = "\033[0m";
    
    // verifica se há entradas duplicadas na tabela de definições
    for (unsigned int i = 0; i < defTable.size(); ++i) {
        for (unsigned int j = i+1; j < defTable.size(); ++j) {
            if (defTable[i].name == defTable[j].name) {
                std::cout << escRed << "\nErro" << escReset << ": " << escYellow << "o rótulo " << defTable[i].name << " foi definido mais de uma vez." << escReset << " (erro semântico)" << "\n\n";
                return -1;
            }
        }
    }
        
    // verifica se todos os rótulos públicos foram definidos
    for (unsigned int i = 0; i < useTable.size(); ++i) {
        bool found = false;
        for (unsigned int j = 0; j < defTable.size(); ++j) {
            if (useTable[i].name == defTable[j].name) {
                found = true;
            }
        }
        if (!found) {
            std::cout << escRed << "\nErro" << escReset << ": " << escYellow << "o rótulo " << useTable[i].name << " não foi definido em nenhum arquivo." << escReset << " (erro semântico)" << "\n\n";
            return -1;
        }
    }
    
    return 0;
    
}