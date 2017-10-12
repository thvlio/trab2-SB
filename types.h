/*      TYPES.H: arquivo contendo as bibliotecas e definições de tipos        */



/*      BIBLIOTECAS     */

#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <string> // std::string
#include <sstream> // std::stringstream
#include <algorithm> // ::toupper



/*      DECLARAÇÕES DOS TIPOS       */

struct Instr;
struct Dir;
struct Label;
struct Macro;
struct Simbolo;


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
    int value; // definicao do rotulo (uma linha, um endereço ou um valor)
    int defined; // se o rotulo ja foi ou nao definido
    int defition; // indica o proximo endereço na lista de pendencias
    // metodos
    Label () {};
    Label (std::string nm, int vl): name(nm), value(vl) {};
    Label (std::string nm, int vl, int dfd, int dft): name(nm), value(vl), defined(dfd), defition(dft) {};
};

// Macro: armazena uma macro e suas caracteristcas
struct Macro {
    // membros
    std::string name; // nome da macro
    std::string definition; // definicao da macro
    // metodos
    Macro () {};
    Macro (std::string nm, std::string df): name(nm), definition(df) {}; 
};

// a struct Label ja implementa estas informacoes, nao?
struct Simbolo{
    //membros
    std::string label;
    int value;
    bool def;
    int list;
    //metodos
    Simbolo () {};
    Simbolo (std:: string lb, int vl, bool df, int lt): label(lb), value(vl), def(df), list(lt) {};
};