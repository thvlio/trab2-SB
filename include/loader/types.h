/*      TYPES.H: arquivo contendo as bibliotecas e definições de tipos        */



/*      BIBLIOTECAS     */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>



/*      DECLARAÇÕES DOS TIPOS       */

struct Chunk;




/*      DEFINIÇÕES DOS TIPOS        */

// Chunk: armazena as características de um chunk de memória
struct Chunk {
    // membros
    int start; // onde começa o chunk
    int size; // qual o tamanho em bytes do chunk
    // metodos
    // constroi a estrutura sem nada
    Chunk () {};
    // constroi a estrutura a partir de onde começa e do tamanho
    Chunk (int st, int si): start(st), size(si) {};
};