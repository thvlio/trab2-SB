/*      TYPES.H: arquivo contendo as bibliotecas e definições de tipos        */



/*      BIBLIOTECAS     */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>



/*      DECLARAÇÕES DOS TIPOS       */

struct Instr;
struct Dir;
struct Label;
struct Macro;
struct Error;




/*      DEFINIÇÕES DOS TIPOS        */

// Instr: armazena uma instrucao e suas caracteristicas
struct Instr {
    // membros
    std::string name; // nome da instrucao
    int opcode, // opcode da instrucao
        numArg; // numero de argumentos da instrucao
    // metodos
    // constroi a estrutura sem nada
    Instr () {};
    // constroi a estrutura a partir de uma string, um opcode e o numero de argumentos
    Instr (std::string nm, int op, int na): name(nm), opcode(op), numArg(na) {};
};



// Dir: armazena uma diretiva e suas caracteristicas
struct Dir {
    // membros
    std::string name; // nome da diretiva
    // metodos
    Dir () {};
    Dir (std::string nm): name(nm) {};  
};



// Label: armazena um rotulo e suas caracteristicas
struct Label {
    // membros
    std::string name; // nome do rotulo
    std::string equ; // definicao vinda de um equ
    int value; // definicao do rotulo (um endereço)
    int isDefined; // se o rotulo ja foi ou nao definido
    std::vector<int> pendList; // lista de pendencias
    std::vector<int> auxInfoList; // lista de informacoes auxiliares
    std::vector<int> posList; // lista de posição do rótulo na linha
    int isConst; // se é um const ou não (0: nao eh const, 1: eh const, 2: é const = 0)
    int vectSize; // tamanho do vetor, para o caso de ser um space. 0 indica que o rótulo é da área de texto
    bool isExtern; // se o símbolo é externo
    bool isPublic; // se o símbolo é público
    // metodos
    Label () {};
    Label (std::string nm, std::string eq): name(nm), equ(eq) {};
};



// Macro: armazena uma macro e suas caracteristcas
struct Macro {
    // membros
    std::string name; // nome da macro
    std::string definition; // definicao da macro
    int initLine; // linha inicial da macro
    int numLines; // quantas linhas tem a macro
    // metodos
    Macro () {};
    Macro (std::string nm, std::string df, int ln, int nl): name(nm), definition(df), initLine(ln), numLines(nl) {};
};



// Error: armazrna um erro e suas informações
struct Error {
    // membros
    std::string message; // mensagem do erro
    std::string type; // tipo de erro
    int lineNum; // linha do erro
    std::string line; // conteudo da linha do erro
    int pos; // posição na linha do erro
    // metodos
    Error () {};
    Error (std::string msg, std::string tp, int lnn, std::string ln, int ps=0): message(msg), type(tp), lineNum(lnn), line(ln), pos(ps) {};
};

