# trab2-SB

Trabalho 2 de Software Básico - 2017/2

Grupo:
* Thúlio Noslen Silva Santos - 14/0164090
* Daniel Amaral Ribeiro - 14/0135219
* Guilherme Caetano Gonçalves - 13/0112925

# Montador
Para compilar, use:
* g++ -std=c++11 -Wall assembler.cpp -o `nome.out`
* `nome.out`: o nome do arquivo executável (ex: `assembler.out`)
    
Para executar, use:
* `./nome` `xxx.asm yyy.asm zzz.asm`
* `./nome`: o nome dado ao executável (ex: `./assembler.out`)
* `xxx.asm yyy.asm zzz.asm`: nomes de 1 a 3 arquivos de entrada diferentes (ex: `bin1.asm bin2.asm bin3.asm`)

Exemplo de compilação e execução:
* `g++ -std=c++11 -Wall assembler.cpp -o assembler.out`
* `./assembler.out -o bin1.asm bin2.asm bin3.asm`

# Ligador
Para compilar, use:
* g++ -std=c++11 -Wall linker.cpp -o `nome.out`
* `nome.out`: o nome do arquivo executável (ex: `linker.out`)
    
Para executar, use:
* `./nome` `xxx.o yyy.o zzz.o`
* `./nome`: o nome dado ao executável (ex: `./linker.out`)
* `xxx.o yyy.o zzz.o`: nomes de 1 a 3 arquivos de entrada diferentes (ex: `bin1.o bin2.o bin3.o`)

Exemplo de compilação e execução:
* `g++ -std=c++11 -Wall linker.cpp -o linker.out`
* `./linker.out bin1.o bin2.o bin3.o`

# Carregador
Para compilar, use:
* g++ -std=c++11 -Wall loader.cpp -o `nome.out`
* `nome.out`: o nome do arquivo executável (ex: `loader.out`)
    
Para executar, use:
* `./nome` `xxx` `numCh` `tam1 tam2 ... tamN` `end1 end2 ... endN`
* `./nome`: o nome dado ao executável (ex: `./loader.out`)
* `xxx`: nome de um único arquivo de entrada, sem extensão (ex: `bin1`)
* `numCh`: número N de chunks de memória disponíveis
* `tam1 tam2 ... tamN`: tamanhos dos N chunks de memória disponíveis
* `end1 end2 ... endN`: endereços dos N chunks de memória disponíveis

Exemplo de compilação e execução:
* `g++ -std=c++11 -Wall loader.cpp -o loader.out`
* `./loader.out bin1 3 5 15 7 100 150 190`
