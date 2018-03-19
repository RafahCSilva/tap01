#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

char *jogador[2] = {"primeiro", "segundo"};

// pos[i][j] = quantas casas o i-ésimo jogador avançou a peça na coluna j
int *pos[2];

// Número de linhas e de colunas do tabuleiro.
int m = 5, n = 3;

void print_board() {
  fprintf(stderr, "Situação do tabuleiro no Juiz:\n");
  for (int j = 0; j < n; j ++) {
    for (int i = 0; i < m; i ++) {
      if (pos[0][j] == i) {
	fprintf(stderr, "O");
      }
      else if (m - 1 - pos[1][j] == i) {
	fprintf(stderr, "X");
      }
      else {
	fprintf(stderr, ".");
      }
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}

void vence(int k) {
  print_board();

  fprintf(stderr, "Juiz: declarando o %s jogador como vencedor (motivo abaixo).\n", jogador[k]);
  printf("Vencedor: %s jogador.\n", jogador[k]);  
}

int main(int ac, char **av) {
  // Primeiro argumento é o número de linhas do tabuleiro (padrão 5).
  if (ac > 1) 
    m = atoi(av[1]);

  // O segundo argumento é o número de colunas (padrão 3).
  if (ac > 2)
    n = atoi(av[2]);
  
  // Disable output buffering.
  setvbuf(stdout, NULL, _IONBF, 0);

  // Disable error output buffering.
  setvbuf(stderr, NULL, _IONBF, 0);

  // File descriptors for named pipes.
  int in[2], out[2];  

  // Create named pipes.
  fprintf(stderr, "Juiz: certificando-se de que os named pipes in1, in2, out1 e out2 existem... ");
  if (mkfifo("./in1", 0666) != 0 && errno != EEXIST)
    return 1;
  if (mkfifo("./out1", 0666) != 0 && errno != EEXIST)
    return 1;
  if (mkfifo("./in2", 0666) != 0 && errno != EEXIST)
    return 1;
  if (mkfifo("./out2", 0666) != 0 && errno != EEXIST)
    return 1;
  fprintf(stderr, "sim!\n");
  
  // Open named pipes (always succeeds in RW mode).
  fprintf(stderr, "Juiz: esperando para abrir in1... ");
  in[0] = open("./in1", O_RDWR);
  if (in[0] < 0) {
    fprintf(stderr, "não foi possível abrir.\n");
    exit(-1);
  }
  fprintf(stderr, "feito.\nJuiz: esperando para abrir out1... ");
  out[0] = open("./out1", O_RDWR);
  if (out[0] < 0) {
    fprintf(stderr, "não foi possível abri-lo.\n");
    exit(-1);
  }
  fprintf(stderr, "feito.\nJuiz: esperando para abrir in2... ");
  in[1] = open("./in2", O_RDWR);
  if (in[1] < 0) {
    fprintf(stderr, "não foi possível abri-lo.\n");
    exit(-1);
  }
  fprintf(stderr, "feito.\nJuiz: esperando para abrir out2... ");
  out[1] = open("./out2", O_RDWR);
  if (out[1] < 0) {
    fprintf(stderr, "não foi possível abri-lo.\n");
    exit(-1);
  }
  fprintf(stderr, "feito.\n");

  char num[64];
  int k = 0, l;
  int c, d, ganhou = 0;

  pos[0] = malloc(n * sizeof(int));
  pos[1] = malloc(n * sizeof(int));

  for (int j = 0; j < n; j ++)
    pos[0][j] = pos[1][j] = 0;
  
  
  while (!ganhou) {
    print_board();
    
    fprintf(stderr, "Juiz: esperando para fazer leitura de in[%d]... ", k);
    // Lê jogada
    l = read(in[k], num, sizeof(num));
    if (l <= 0) {
      vence(1 - k);
      fprintf(stderr, "Juiz: erro de leitura (%s jogador, i.e., buffer in[%d]).\n", jogador[k], k);
      break; 
    }
    fprintf(stderr, "feita.\n");
    
    // Faz um log das jogadas na saída padrão e na saida de erro.
    printf("%s", num);
    fprintf(stderr, "Juiz recebeu movimento do %s jogador: %s", jogador[k], num);
    if (sscanf(num, "%d %d", &c, &d) != 2) {
      vence(1 - k);
      fprintf(stderr, "Juiz: última jogada inválida.\n");
      break; 
    }
    if (c < 0 || c > n || d < 0 || d >= m - 1 || (c == 0 && d != 0) || (c > 0 && d == 0)) {
      vence(1 - k);
      fprintf(stderr, "Juiz: última jogada inválida.\n");
      break; 
    }
    if (c > 0) {
      c --;
      int x = pos[k][c], y = m - 1 - pos[1 - k][c], z = x + d;
      if (z >= m || (x < y - 1 && z >= y) || (y == x + 1 && d != 2)) {
	vence(1 - k);
	fprintf(stderr, "Juiz: última jogada inválida.\n");
	break;
      }
      pos[k][c] = z;
    }

    // Verifica se o jogador que fez a jogada ganhou...
    ganhou = 1;
    for (int j = 0; j < n; j ++)
      if (pos[k][j] != m - 1) {
	ganhou = 0;
	break;
      }

    if (ganhou) {
      vence(k);
      fprintf(stderr, "Juiz: fim do jogo!\n");
      break;
    }
      
    k = 1 - k;
    fprintf(stderr, "Juiz: esperando para fazer escrita em out[%d]... ", k);
    if (write(out[k], num, l) != l) {
      vence(1 - k);
      fprintf(stderr, "Juiz: não foi possível enviar a jogada para o %s jogador (i.e. p/ pipe out[%d]).\n", jogador[k], k);
      break; 
    }
    fprintf(stderr, "feito.\n");
  }

  close(in[0]);
  close(out[0]);
  close(in[1]);
  close(out[1]);
  
  free(pos[0]);
  free(pos[1]);
  
  return 0;
}
    
