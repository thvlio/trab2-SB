/*      TYPES.H: arquivo contendo as bibliotecas e definições de tipos        */



/*      BIBLIOTECAS     */

#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <string> // std::string
#include <sstream> // std::stringstream
#include <algorithm> // ::toupper



/*      DECLARAÇÕES DOS TIPOS       */

struct Instr;



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

// Label: armazena um label e suas caracteristicas
struct Label {
    // membros
    std::string name; // nome do label
    int value; // definicao do label (uma linha, um endereço ou um valor)
    // metodos
    Label () {};
    Label (std::string nm, int vl): name(nm), value(vl) {}; 
};