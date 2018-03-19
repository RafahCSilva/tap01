/**
 * @author Rafael Cardoso da Silva 21048012
 *
 */
#include <iostream>
#include <iomanip>
#include <vector>

#define V false     // VERBOSE

#define tab_ALTURA 5
#define tab_POSSIB 19
#define BOLA 0 // O BRANCO sobe
#define XIS 1  // X PRETO  desce

#define NAO_VISITANDO   0   // = nao visitados
#define VISITANDO       1   // = visitando
#define SEM_ESTRATEGIA  2   // = SEM estrategia
#define COM_ESTRATEGIA  3   // = COM estrategia

#define estado( j, k ) (j *8000+ k)

int **posicao;
int **index;
int grafo_coluna[2][tab_POSSIB][3];
std::vector<int> grafo_tabuleiro[2][8000];
int vencedor[2][8000];
std::vector<int> grafo[2 * 8000];
int estrategia[2 * 8000];

/**
 * Struct do ID
 */
struct ID_t {
    ID_t( int id0, int id1, int id2 ) : id0( id0 ), id1( id1 ), id2( id2 ) {}

    ID_t() = default;

    int id0, id1, id2;
};

static ID_t ID( int id0, int id1, int id2 ) { return { id0, id1, id2 }; }

class Base {
public:
    /**
     * Base::baseID converte ID_b20 para ID_t
     *
     * @param int b20
     * @return
     */
    static ID_t int2ID( int b20 ) {
        ID_t id{};
        id.id2 = b20 / 400;
        b20 = b20 % 400;
        id.id1 = b20 / 20;
        id.id0 = b20 % 20;
        return id;
    }

    /**
     * Base::ID2int converte ID_t para ID_b20
     *
     * @param ID_t id
     * @return
     */
    static int ID2int( ID_t *id ) {
        return 1 * id->id0 +
               20 * id->id1 +
               400 * id->id2;
    }
};

struct config_t {
//    config_t( int id, int bola, int xis ) : id( id ), bola( bola ), xis( xis ) {}

    int id;
    int bola;
    int xis;
};

int convert_ID_para_PASSO( int atual, int prox );

int convert_PASSO_para_ID( int atual, int coluna, int passo );

static config_t config( int id, int bola, int xis ) { return { id, bola, xis }; }

void printConfig( config_t *conf ) {
    std::cout << "  " << std::setw( 2 ) << conf->id << ": ";
    for ( int i = 0; i < tab_ALTURA; ++i ) {
        if ( conf->bola == i ) // 0
            std::cout << "O";
        else if ( conf->xis == i ) // 1
            std::cout << "X";
        else
            std::cout << ".";
    }
    std::cout << std::endl;
}

void mov_valido_O_col( int k, int *M ) {
    int pos_O = posicao[ BOLA ][ k ];
    int pos_X = posicao[ XIS ][ k ];

    if ( pos_O == 4 )
        // o BOLA ja chegou no final
        return;
    if ( pos_O == 3 && pos_X == 4 )
        // aqui o BOLA está na penúltima casa, mas
        // não pode se mover porque o XIS está na última casa
        return;
    if ( pos_X == pos_O + 1 ) {
        // o BOLA só pode "pular" o XIS e parar
        // na casa imediatamente após o XIS
        M[ 0 ] = index[ pos_O + 2 ][ pos_X ];
        return;
    }

    // Se nenhuma das condições anteriores foi satisfeita, então o peão
    // branco pode se mover para qualquer casa entre ele até peão preto ou
    // até o fim da linha no caso dele já ter ultrapassado o peão preto.
    int limite = ( pos_X < pos_O ) ? 5 : pos_X;
    int j = 0;
    for ( int i = pos_O + 1; i < limite; i++ ) {
        M[ j ] = index[ i ][ pos_O ];
        j++;
    }
}

void mov_valido_X_col( int k, int *M ) {
    int pos_O = posicao[ BOLA ][ k ];
    int pos_X = posicao[ XIS ][ k ];

    if ( pos_X == 0 )
        // o XIS ja chegou no final
        return;
    if ( pos_X == 1 && pos_O == 0 )
        // aqui o XIS está na penúltima casa, mas
        // não pode se mover porque o BOLA está na última casa
        return;
    if ( pos_X == pos_O + 1 ) {
        // o branco só pode "pular" o preto e parar
        // na casa imediatamente após o preto
        M[ 0 ] = index[ pos_O ][ pos_X - 2 ];
        return;
    }

    // Se nenhuma das condições anteriores foi satisfeita, então o peão
    // XIS pode se mover para qualquer casa entre ele até peão BOLA ou
    // até o fim da linha no caso dele já ter ultrapassado o peão BOLA.
    int limite = ( pos_O < pos_X ) ? pos_O : -1;
    int j = 0;
    for ( int i = pos_X - 1; i > limite; i-- ) {
        M[ j ] = index[ pos_O ][ i ];
        j++;
    }
}

int terminal( int k ) {
    int ganhou0 = 0, ganhou1 = 0;
    ID_t id = Base::int2ID( k );
    int v[] = { id.id0, id.id1, id.id2 };
    if ( posicao[ 1 ][ 3 ] == posicao[ 0 ][ v[ 0 ]] &&
         posicao[ 1 ][ 3 ] == posicao[ 0 ][ v[ 1 ]] &&
         posicao[ 1 ][ 3 ] == posicao[ 0 ][ v[ 2 ]] )
        ganhou0 = 1;
    if ( posicao[ 1 ][ v[ 0 ]] == posicao[ 0 ][ 3 ] &&
         posicao[ 1 ][ v[ 1 ]] == posicao[ 0 ][ 3 ] &&
         posicao[ 1 ][ v[ 2 ]] == posicao[ 0 ][ 3 ] )
        ganhou1 = 1;
    return ganhou0 + 2 * ganhou1;
}

// (1876) -> 9876
void printTabuleiro( int id ) {
    ID_t a2 = Base::int2ID( id );
    std::cout << "( " << id << " ) -> " << a2.id0 << " " << a2.id1 << " " << a2.id2 << std::endl;
    config_t c1 = { a2.id0, posicao[ BOLA ][ a2.id0 ], posicao[ XIS ][ a2.id0 ] };
    config_t c2 = { a2.id1, posicao[ BOLA ][ a2.id1 ], posicao[ XIS ][ a2.id1 ] };
    config_t c3 = { a2.id2, posicao[ BOLA ][ a2.id2 ], posicao[ XIS ][ a2.id2 ] };
    printConfig( &c1 );
    printConfig( &c2 );
    printConfig( &c3 );
}

void mov_validos_tabuleiro( const int jog, const int K, std::vector<int> *M ) {
    if ( terminal( K ))
        return;
    ID_t id = Base::int2ID( K );
    for ( int col = 0; col <= 2; ++col )
        for ( int l = 0; l < 3; ++l ) {
            int k[] = { id.id0, id.id1, id.id2 };
            k[ col ] = grafo_coluna[ jog ][ k[ col ]][ l ];
            if ( k[ col ] != -1 ) {
                ID_t iaa( k[ 0 ], k[ 1 ], k[ 2 ] );
                int K2 = Base::ID2int( &iaa );
                if ( K != K2 )
                    M->push_back( K2 );
            }
        }

    // Se não há jogadas válidas então "passar a vez" é válida!
    if ( M->empty())
        M->push_back( K );
}

void ganha( int jog, int k ) {
/*  vencedor[(i, x)]
  0 = nao visitados
  1 = visitando
  2 = SEM estrategia
  3 = COM estrategia
def ganha(i, x):
    # mark as visited:
    vencedor[ ( i, x ) ] = 1
    # no terminal node has a winning strategy:
    if terminal(x):
      vencedor[ ( i, x ) ] = 2
      return
    for j, y in grafo[(i, x)]:
      # Se você implementou o grafo reduzido descrito na seção anterior,
      # então use phi(y) no lugar de y até o final desta função.
      if not ( j, y ) in vencedor.keys():
          ganha( j, y )
      if vencedor[ ( j, y ) ] == 2:
          vencedor[ ( i, x ) ] = 3
      if vencedor[ ( i, x ) ] == 1:
          vencedor[ ( i, x ) ] = 2
*/
    vencedor[ jog ][ k ] = VISITANDO;
    if ( terminal( k )) {
        vencedor[ jog ][ k ] = SEM_ESTRATEGIA;
        return;
    }
    for ( int prox : grafo[ jog * 8000 + k ] ) {
        int adv = prox / 8000;
        int y = prox % 8000;
        if ( V )
            std::cout << std::setw( 5 ) << jog * 8000 + k << " "
                      << std::setw( 5 ) << adv * 8000 + y
                      << std::endl;
        if ( vencedor[ adv ][ y ] == NAO_VISITANDO )
            ganha( adv, y );
        if ( vencedor[ adv ][ y ] == SEM_ESTRATEGIA )
            vencedor[ jog ][ k ] = COM_ESTRATEGIA;
        if ( vencedor[ jog ][ k ] == VISITANDO )
            vencedor[ jog ][ k ] = SEM_ESTRATEGIA;
    }
}

void bot() {
    for ( int jog = 0; jog < 2; ++jog ) {
        for ( int K = 0; K < 8000; ++K ) {
            if ( terminal( K ))
                continue;
            std::vector<int> jogadas;
            ID_t id = Base::int2ID( K );
            for ( int col = 0; col <= 2; ++col ) {
                for ( int l = 0; l < 3; ++l ) {
                    int k[] = { id.id0, id.id1, id.id2 };
                    k[ col ] = grafo_coluna[ jog ][ k[ col ]][ l ];
                    if ( k[ col ] != -1 ) {
                        ID_t iaa( k[ 0 ], k[ 1 ], k[ 2 ] );
                        int K2 = Base::ID2int( &iaa );
                        if ( K != K2 ) //?
                            jogadas.push_back( K2 );
                    }
                }
            }
            // Verifica se é preciso "passar a vez":
            if ( jogadas.empty())
                jogadas.push_back( K );

            // define estrategia padrao: jogar aleatoriamente
            auto tam = ( int ) jogadas.size();
            int chute = std::rand() % tam;
            if ( V ) std::cout << tam << " " << chute << " " << jogadas[ chute ] << std::endl;
            estrategia[ jog * 8000 + K ] = jogadas[ chute ];
            for ( int y : jogadas ) {
                // mas se uma das jogadas deixa o oponente em posicao de derrota...
                if ( vencedor[ 1 - jog ][ y ] == SEM_ESTRATEGIA ) {
                    estrategia[ jog * 8000 + K ] = y;
                    break;
                }
            }
        }
    }

    /**
     * imprime uma lista de jogadas
     * vez do 0 : vez do 1
     * Sinal + quer dizer ganhador
     *       - quer dizer perdedor
     */
    if ( V )
        for ( int K = 0; K < 8000; ++K )
            std::cout << (( vencedor[ 0 ][ K ] == COM_ESTRATEGIA ) ? "+" : "-" )
                      << K
                      << " : "
                      << (( vencedor[ 1 ][ estrategia[ K ]] == COM_ESTRATEGIA ) ? "+" : "-" )
                      << estrategia[ K ]
                      << std::endl;
}

int main( int argc, char **argv ) {
    if ( V )
        std::cout << "Eae Rafao" << std::endl;

    // MUDANCA DE BASE
    if ( V ) {
        std::cout << "Testando ID_b20 -> ID_t" << std::endl;
        ID_t a = Base::int2ID( 512 );
        std::cout << "  ( " << 512 << " ) -> " << a.id0 << " " << a.id1 << " " << a.id2 << std::endl;

        std::cout << "Testando ID_t -> ID_b20" << std::endl;
        ID_t b( 3, 2, 1 );
        int c = Base::ID2int( &b );
        std::cout << "  ( " << b.id0 << " " << b.id1 << " " << b.id2 << " ) -> " << c << std::endl;
    }

    // POSSIBILIDADE DA COLUNA
    if ( V ) std::cout << "Possibilidades de Config da Coluna:" << std::endl;
    int id = 0;
    auto *configs = new config_t[tab_POSSIB];

    posicao = new int *[2];
    posicao[ BOLA ] = new int[tab_POSSIB];
    posicao[ XIS ] = new int[tab_POSSIB];

    index = new int *[tab_ALTURA];
    for ( int i = 0; i < tab_ALTURA; ++i )
        index[ i ] = new int[tab_ALTURA];

    for ( int i = 0; i < tab_ALTURA; ++i ) // BOLA
        for ( int j = tab_ALTURA - 1; j >= 0; --j ) // XIS
            if ( i != j ) {
                configs[ id ] = config( id, i, j );
                posicao[ BOLA ][ id ] = i;
                posicao[ XIS ][ id ] = j;
                index[ i ][ j ] = id;
                id++;
            }
    if ( V )
        for ( int i = 0; i <= tab_POSSIB; ++i )
            printConfig( &configs[ i ] );

    if ( V ) {
        // INDEX
        std::cout << "POSICAO:" << std::endl;
        for ( int i = 0; i <= tab_POSSIB; ++i ) {
            std::cout << "  (" << BOLA << " "
                      << std::setw( 2 ) << i << ") -> "
                      << posicao[ BOLA ][ i ] << std::endl;
            std::cout << "  (" << XIS << " "
                      << std::setw( 2 ) << i << ") -> "
                      << posicao[ XIS ][ i ] << std::endl;
        }
        std::cout << "INDEX:" << std::endl;
        for ( int i = 0; i < tab_ALTURA; ++i ) {
            std::cout << "  ";
            for ( int j = 0; j < tab_ALTURA; ++j ) {
                if ( i != j )
                    std::cout << std::setw( 2 ) << index[ i ][ j ] << " ";
                else
                    std::cout << ". ";
            }
            std::cout << std::endl;
        }
    }

    if ( V ) std::cout << "grafo_coluna:" << std::endl;
    for ( int jog = 0; jog < 2; ++jog )
        for ( int col = 0; col <= tab_POSSIB; ++col )
            for ( int i = 0; i < 3; ++i )
                grafo_coluna[ jog ][ col ][ i ] = -1;
    for ( int k = 0; k <= tab_POSSIB; ++k ) {
        mov_valido_O_col( k, grafo_coluna[ BOLA ][ k ] );
        mov_valido_X_col( k, grafo_coluna[ XIS ][ k ] );
    }
    if ( V )
        for ( int j = 0; j < 2; ++j )
            for ( int k = 0; k <= tab_POSSIB; ++k )
                std::cout << "  (" << j << ", " << std::setw( 2 ) << k << ") -> "
                          << std::setw( 2 ) << grafo_coluna[ j ][ k ][ 0 ] << " "
                          << std::setw( 2 ) << grafo_coluna[ j ][ k ][ 1 ] << " "
                          << std::setw( 2 ) << grafo_coluna[ j ][ k ][ 2 ]
                          << std::endl;

    if ( V ) std::cout << "grafo_tabuleiro:" << std::endl;
    for ( int k = 0; k < 8000; ++k ) {
        for ( int jog = 0; jog < 2; ++jog ) {
            mov_validos_tabuleiro( jog, k, &grafo_tabuleiro[ jog ][ k ] );
            if ( V ) {
                std::cout << "  (" << jog << " " << k << ") -> ";
                for ( int l : grafo_tabuleiro[ jog ][ k ] )
                    std::cout << l << " ";
                std::cout << std::endl;
            }
        }
    }

    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            for ( int l : grafo_tabuleiro[ jog ][ k ] )
                grafo[ jog * 8000 + k ].push_back((( 1 - jog ) * 8000 ) + l );
    if ( V ) {
        std::cout << "GRAFO:" << std::endl;
        for ( int k = 0; k < 2 * 8000; ++k ) {
            std::cout << "  (" << k << ") -> ";
            for ( int l : grafo[ k ] )
                std::cout << l << " ";
            std::cout << std::endl;
        }
    }

    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            vencedor[ jog ][ k ] = NAO_VISITANDO;

    /*
    # Decide se existe uma estratégia vencedora para cada possível estado do jogo:
    for i in range(2):
        # Se você implementou o grafo reduzido descrito na seção anterior,
        # então use representantes.keys() no lugar de range(8000) a seguir
        for K in range(8000):
            if not (i, K) in vencedor.keys():
                ganha(i, K)
     */
    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            if ( vencedor[ jog ][ k ] == NAO_VISITANDO )
                ganha( jog, k );

    if ( V ) {
        // TESTANDO (3,3,3)
        ID_t t( 3, 3, 3 );
        int idT = Base::ID2int( &t );
        std::cout << vencedor[ 0 ][ idT ] << std::endl
                  << vencedor[ 1 ][ idT ]
                  << std::endl;
        printTabuleiro( idT );
        std::cout << std::endl;
        for ( auto p : grafo_tabuleiro[ 0 ][ idT ] ) {
            printTabuleiro( p );
            std::cout << vencedor[ 0 ][ p ] << std::endl;
        }
        std::cout << std::endl;
        for ( auto p : grafo_tabuleiro[ 1 ][ idT ] ) {
            printTabuleiro( p );
            std::cout << vencedor[ 1 ][ p ] << std::endl;
        }
        std::cout << std::endl;
    }

    bot();

    if ( V )
        std::cout << "Pronto pro combate!" << std::endl;

    int vez = 0;

    if ( std::string( argv[ 1 ] ) == "primeiro" ) {
        // Se o argumento primeiro estiver presente, seu programa deve comecar jogando
        if ( V )
            std::cout << "Primeiro(BOT) comeca Jogando" << std::endl;
        vez = 0;
    } else if ( std::string( argv[ 1 ] ) == "segundo" ) {
        // Se o argumento segundo estiver presente, seu programa deve esperar (ler da entrada padrao) a jogada do usuario
        if ( V )
            std::cout << "Segundo(VOCE) comeca Jogando" << std::endl;
        vez = 1;
    } else {
        std::cout << "Sem Argumento primeiro/segundo" << std::endl;
        return 1;
    }

    int coluna, passo, config_atual = 0, config_prox;
    while ( true ) {
        if ( V )
            printTabuleiro( config_atual );

        if ( vez == 0 ) { // bot joga
//            config_prox = estrategia[ vez * 8000 + ( config_atual % 8000 ) ];
            config_prox = estrategia[ ( 1 - vez ) * 8000 + ( config_atual % 8000 ) ];
//            config_prox = estrategia[ config_atual ];
            if ( V )
                std::cout << " >>> ";
            config_atual = convert_ID_para_PASSO( config_atual, config_prox );

        } else if ( vez == 1 ) { // le jogada adversaria (jogador)
            if ( V )
                std::cout << " <<< ";
            std::cin >> coluna >> passo;
            config_atual = convert_PASSO_para_ID( config_atual, coluna, passo );

        } else {
            std::cout << "deu ruim no vez" << std::endl;
            return 1;
        }

        if ( terminal( config_atual )) {
            if ( V ) {
                printTabuleiro( config_atual );
                std::cout << "!!!! Ganhou: " << ( vez ? "O" : "X" ) << std::endl;
            }
            return 0;
        }
        // proximo jogador
        vez = 1 - vez;
    }

    return 0;
}

int convert_ID_para_PASSO( int id_atual, int id_prox ) {
    int coluna = 0, passo = 0;
    // convert pata config
    ID_t atual = Base::int2ID( id_atual );
    ID_t prox = Base::int2ID( id_prox );

//    config_t c1 = { prox.id0, posicao[ BOLA ][ prox.id0 ], posicao[ XIS ][ prox.id0 ] };
//    config_t c2 = { prox.id1, posicao[ BOLA ][ prox.id1 ], posicao[ XIS ][ prox.id1 ] };
//    config_t c3 = { prox.id2, posicao[ BOLA ][ prox.id2 ], posicao[ XIS ][ prox.id2 ] };

    // descobre coluna
    if ( atual.id0 != prox.id0 ) {
        coluna = 1;
        // descobre qtos passos
        passo = posicao[ XIS ][ atual.id0 ] - posicao[ XIS ][ prox.id0 ];

    } else if ( atual.id1 != prox.id1 ) {
        coluna = 2;
        // descobre qtos passos
        passo = posicao[ XIS ][ atual.id1 ] - posicao[ XIS ][ prox.id1 ];

    } else if ( atual.id2 != prox.id2 ) {
        coluna = 3;
        // descobre qtos passos
        passo = posicao[ XIS ][ atual.id2 ] - posicao[ XIS ][ prox.id2 ];
    }

    // print coluna passos
    std::cout << coluna << " " << passo << std::endl;

    return id_prox;
}

int convert_PASSO_para_ID( int id_atual, int coluna, int passo ) {
    ID_t atual = Base::int2ID( id_atual );

//    config_t c1 = { atual.id0, posicao[ BOLA ][ atual.id0 ], posicao[ XIS ][ atual.id0 ] };
//    config_t c2 = { atual.id1, posicao[ BOLA ][ atual.id1 ], posicao[ XIS ][ atual.id1 ] };
//    config_t c3 = { atual.id2, posicao[ BOLA ][ atual.id2 ], posicao[ XIS ][ atual.id2 ] };
    // Na coluna
    if ( coluna == 1 ) {
        // qto XIS andou
        atual.id0 = index[ posicao[ BOLA ][ atual.id0 ] + passo ][ posicao[ XIS ][ atual.id0 ]];
//        c1 = { index[ posicao[ BOLA ][ atual.id0 ] - passo ][ posicao[ XIS ][ atual.id0 ]],
//               posicao[ BOLA ][ atual.id0 ] - passo,
//               posicao[ XIS ][ atual.id0 ] };
    } else if ( coluna == 2 ) {
        // qto XIS andou
//        std::cout << "posicao[ BOLA ][ atual.id1 ]: " << posicao[ BOLA ][ atual.id1 ] << std::endl;
//        std::cout << "posicao[ XIS ][ atual.id1 ]: " << posicao[ XIS ][ atual.id1 ] << std::endl;
//        std::cout << "index[ posicao[ BOLA ][ atual.id1 ] + passo ][ posicao[ XIS ][ atual.id1 ]]"
//                  << index[ posicao[ BOLA ][ atual.id1 ] + passo ][ posicao[ XIS ][ atual.id1 ]] << std::endl;
        atual.id1 = index[ posicao[ BOLA ][ atual.id1 ] + passo ][ posicao[ XIS ][ atual.id1 ]];
//        c2 = { index[ posicao[ BOLA ][ atual.id1 ] - passo ][ posicao[ XIS ][ atual.id1 ]],
//               posicao[ BOLA ][ atual.id1 ] - passo,
//               posicao[ XIS ][ atual.id1 ] };

    } else if ( coluna == 3 ) {
        // qto XIS andou
        atual.id2 = index[ posicao[ BOLA ][ atual.id2 ] + passo ][ posicao[ XIS ][ atual.id2 ]];
//        c3 = { index[ posicao[ BOLA ][ atual.id2 ] - passo ][ posicao[ XIS ][ atual.id2 ]],
//               posicao[ BOLA ][ atual.id2 ] - passo,
//               posicao[ XIS ][ atual.id2 ] };
    }

//    ID_t prox = ID( c1.id, c2.id, c3.id );
    ID_t prox = ID( atual.id0, atual.id1, atual.id2 );
    return Base::ID2int( &prox );
}
