/*      TYPES.H: arquivo contendo as bibliotecas e definições de tipos        */



/*      BIBLIOTECAS     */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>



/*      DECLARAÇÕES DOS TIPOS       */

struct tDef;
struct tUso;



/*      DEFINIÇÕES DOS TIPOS        */


// tDef: armazena uma definição de label 
struct tDef {
    // membros
    std::string name; // nome do label
    int valor; // valor do label
    // metodos
    // constroi a estrutura sem nada
    tDef () {};
    // constroi a estrutura a partir de uma string e o valor
    tDef (std::string nm, int vl): name(nm), valor(vl) {};
};

// tUso: armazena o(s) endereco(s) do codigo que o label indica
struct tUso {
    // membros
    std::string name; // nome do label
    std::vector<int> listUso; // usos do label
    // metodos
    // constroi a estrutura sem nada
    tUso () {};
    // constroi a estrutura a partir de uma string e a lista
    tUso (std::string nm, std::vector<int> lu): name(nm), listUso(lu) {};
};