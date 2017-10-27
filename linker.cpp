// compilar com
// g++ -std=c++11 -Wall linker.cpp -o linker.out
// ou entao com CTRL SHIFT B no VSCODE

// rodar com
// ./linker.out xxx.o yyy.o zzz.o

int main (int argc, char *argv[]){

    // coloca os argumentos em strings
    std::vector<std::string> inFileNames;
    for (int i = 1; i < argc; ++i)
        inFileNames.push_back(std::string(*(argv+i)));

    for (i = 1 , i < argc; ++i){
        //Monta as tabelas
        tableLoad(inFileNames[i]);
    }

    for (i = 1; i < argv; ++i){
        //Preenche o arquivo
        linkerCode();
    }


}