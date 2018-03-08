#!/bin/bash

#######################################################
#                                                     #
#   Modo de usar:                                     #
#                                                     #
#   ./compete.bash  ./prog1  ./prog2                  #
#                                                     #
#                                                     #
#                                                     #
#   O primeiro a jogar será prog1. A saída lista os   #
#   movimentos feitos pelos jogadores alternadamen-   #
#   te. No final, o veredito do jogo.                 #
#                                                     #
#######################################################

DOT=$(echo $PATH | grep -E ':.' --color=none)
if [[ "$DOT" == "" ]]; then
    PATH="$PATH:."
fi

mkfifo in1 2> /dev/null
mkfifo in2 2> /dev/null
mkfifo out1 2> /dev/null
mkfifo out2 2> /dev/null

gcc referee.c -o referee
./referee >./saida.txt 2>./log0&

stdbuf -o0 $2 segundo <out2 >in2 2>./log2 &
stdbuf -o0 $1 primeiro <out1 >in1 2>./log1 

rm -f in1 in2 out1 out2 2>/dev/null
cat ./saida.txt 2> /dev/null
