#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

char *pri = "primeiro", *seg = "segundo";
char *JOG[2];

void vence(int k) {
  printf("Vencedor: %s jogador.\n", JOG[k]);
}

int main(int ac, char **av) {
  // Número de linhas e de colunas do tabuleiro.
  int m = 5, n = 3;

  // Primeiro argumento é o número de linhas do tabuleiro (padrão 5).
  if (ac > 1) 
    m = atoi(av[1]);

  // O segundo argumento é o número de colunas (padrão 3).
  if (ac > 2)
    n = atoi(av[2]);

  // Inicializando...
  JOG[0] = pri;
  JOG[1] = seg;
  
  // Disable output buffering.
  setvbuf(stdout, NULL, _IONBF, 0);

  // File descriptors for named pipes.
  int in[2], out[2];  

  // Create named pipes.
  if (mkfifo("./in1", 0666) != 0 && errno != EEXIST)
    return 1;
  if (mkfifo("./out1", 0666) != 0 && errno != EEXIST)
    return 1;
  if (mkfifo("./in2", 0666) != 0 && errno != EEXIST)
    return 1;
  if (mkfifo("./out2", 0666) != 0 && errno != EEXIST)
    return 1;
  
  // Open named pipes (always succeeds in RW mode).
  // fprintf(stderr, "Waiting to open in1... ");
  in[0] = open("./in1", O_RDWR);
  if (in[0] < 0) {
    perror("open");
    exit(-1);
  }
  // fprintf(stderr, "done.\nWaiting to open out1... ");
  out[0] = open("./out1", O_RDWR);
  if (out[0] < 0) {
    perror("open");
    exit(-1);
  }
  // fprintf(stderr, "done.\nWaiting to open in2... ");
  in[1] = open("./in2", O_RDWR);
  if (in[1] < 0) {
    perror("open");
    exit(-1);
  }
  // fprintf(stderr, "done.\nWaiting to open out2... ");
  out[1] = open("./out2", O_RDWR);
  if (out[1] < 0) {
    perror("open");
    exit(-1);
  }
  // fprintf(stderr, "done.\n");

  char num[64];
  int k = 0, l;
  int c, d, ganhou = 0;

  // Inicialização do tabuleiro
  // pos[i][j] = quantas casas o i-ésimo jogador avançou a peça na coluna j
  int *pos[2];

  pos[0] = malloc(n * sizeof(int));
  pos[1] = malloc(n * sizeof(int));
  
  while (!ganhou) {
    // fprintf(stderr, "Waiting to read from in[%d]... ", k);
    l = read(in[k], num, sizeof(num));
    if (l <= 0) {
      fprintf(stderr, "Erro de leitura (%s jogador, i.e., buffer in[%d]).\n", JOG[k], k);
      vence(1 - k);
      break; // exit(1)
    }
    // fprintf(stderr, "done.\n");
    
    // Faz um log das jogadas na saída padrão. 
    printf("%s", num); 
    if (sscanf(num, "%d %d", &c, &d) != 2) {
      fprintf(stderr, "Jogada inválida (%s jogador: %s).\n", JOG[k], num);
      vence(1 - k);
      break; // exit(1);
    }
    if (c < 0 || c > n || d < 0 || d >= m || c == 0 && d != 0 || c > 0 && d == 0) {
      fprintf(stderr, "Jogada inválida (%s jogador: %s).\n", JOG[k], num);
      vence(1 - k);
      break; // exit(1);
    }
    if (c > 0) {
      c --;
      int x = pos[k][c], y = m - 1 - pos[1 - k][c], z = x + d;
      if (z >= m || x < y - 1 && z >= y || y == x + 1 && z != x + 2) {
	fprintf(stderr, "Jogada inválida (%s jogador: %s).\n", JOG[k], num);
	vence(1 - k);
	break; // exit(1);
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
      fprintf(stderr, "Fim do jogo! Vencedor: %s jogador.\n", JOG[k]);
      vence(k);
      break;
    }
      
    
    k = 1 - k;
    if (write(out[k], num, l) < 0) {
      fprintf(stderr, "Erro de escrita (%s jogador, i.e., buffer out[%d]).\n", JOG[k], k);
      vence(1 - k);
      break; // exit(1);
    }
  }

  close(in[0]);
  close(out[0]);
  close(in[1]);
  close(out[1]);

  free(pos[0]);
  free(pos[1]);
  
  return 0;
}
    
