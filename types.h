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

struct Instr {
    // membros
    std::string name;
    int opcode,
        numArg;
    // metodos
    // constroi a estrutura sem nada
    Instr () {};
    // constroi a estrutura a partir de uma string, um opcode e o num de argumentos
    Instr (std::string nm, int op, int na): name(nm), opcode(op), numArg(na) {};
};