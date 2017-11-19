# trab1-SB

Trabalho 2 de Software Básico - 2017/2

Grupo:
* Thúlio Noslen Silva Santos - 14/0164090
* Daniel Amaral Ribeiro - 14/0135219

# Montador
Para compilar, use:
* g++ -std=c++11 -Wall assembler.cpp `nome.out`
* `nome.out`: o nome do arquivo executável (ex: `assembler.out`)
    
Para executar, use:
* `./nome` `xxx.asm yyy.asm zzz.asm`
* `./nome`: o nome dado ao executável (ex: `./main.out`)
* `xxx.asm yyy.asm zzz.asm`: nomes de 1 a 3 arquivos de entrada diferentes (ex: `bin1.asm bin2.asm bin3.asm`)

Exemplo de compilação e execução:
* `g++ -std=c++11 -Wall assembler.cpp assembler.out`
* `./assembler.out -o bin1.asm bin2.asm bin3.asm`

# Ligador
Para compilar, use:
* g++ -std=c++11 -Wall linker.cpp `nome.out`
* `nome.out`: o nome do arquivo executável (ex: `linker.out`)
    
Para executar, use:
* `./nome` `xxx.o yyy.o zzz.o`
* `./nome`: o nome dado ao executável (ex: `./main.out`)
* `xxx.o yyy.o zzz.o`: nomes de 1 a 3 arquivos de entrada diferentes (ex: `bin1.o bin2.o bin3.o`)

Exemplo de compilação e execução:
* `g++ -std=c++11 -Wall linker.cpp linker.out`
* `./linker.out bin1.o bin2.o bin3.o`

# Carregador
.
