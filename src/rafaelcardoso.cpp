/**
 * @author Rafael Cardoso da Silva 21048012
 *
 * Compile:
 *   g++ rafaelcardoso.cpp -o rafaelcardoso.exe -std=c++11
 */


#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstdio>

#define tab_ALTURA 5
#define tab_POSSIB 20
#define BOLA  0  // O BRANCO sobe
#define XIS   1  // X PRETO  desce

// flag do DFS
#define NAO_VISITANDO   0   // = nao visitados
#define VISITANDO       1   // = visitando
#define SEM_ESTRATEGIA  2   // = SEM estrategia
#define COM_ESTRATEGIA  3   // = COM estrategia


int posicao[2][tab_POSSIB];
int index[tab_POSSIB][tab_POSSIB];
std::vector<std::vector<std::vector<int> > > grafo_coluna;
std::vector<std::vector<std::vector<int> > > grafo_tabuleiro;
int vencedor[2][8000];
std::vector<int> grafo[2][8000];
int atual = 1263;

struct ID_t {
    ID_t( int id0, int id1, int id2 ) : id0( id0 ), id1( id1 ), id2( id2 ) {}

    ID_t() = default;

    int id0, id1, id2;
};

static ID_t ID( int id0, int id1, int id2 ) { return { id0, id1, id2 }; }

ID_t int2ID( int b20 ) {
    ID_t id{};
    id.id2 = b20 / 400;
    b20 = b20 % 400;
    id.id1 = b20 / 20;
    id.id0 = b20 % 20;
    return id;
}

int ID2int( ID_t *id ) {
    return 1 * id->id0 +
           20 * id->id1 +
           400 * id->id2;
}

int convert_ID_para_PASSO( int id_prox, int jog ) {
    ID_t origem = int2ID( atual );
    ID_t prox = int2ID( id_prox );
    int coluna = 0, passo = 0;

    if ( origem.id0 != prox.id0 ) {
        coluna = 1;
        passo = posicao[ jog ][ prox.id0 ] - posicao[ jog ][ origem.id0 ];
    }

    if ( origem.id1 != prox.id1 ) {
        coluna = 2;
        passo = posicao[ jog ][ prox.id1 ] - posicao[ jog ][ origem.id1 ];
    }

    if ( origem.id2 != prox.id2 ) {
        coluna = 3;
        passo = posicao[ jog ][ prox.id2 ] - posicao[ jog ][ origem.id2 ];
    }

    return abs( passo ) * 4 + coluna;
}


int convert_PASSO_para_ID( int id_atual, int coluna, int passo ) {
    ID_t atual_ = int2ID( id_atual );
    int col[] = {
            atual_.id0,
            atual_.id1,
            atual_.id2
    };

    // Verifica se nao estoura numa jogada aleatoria do adversario
    if (( posicao[ BOLA ][ col[ coluna - 1 ]] + passo ) > 4 ) {
        return id_atual;
    }

    // Na coluna
    if ( coluna == 1 ) {
        // qto XIS andou
        atual_.id0 = index[ posicao[ BOLA ][ atual_.id0 ] + passo ][ posicao[ XIS ][ atual_.id0 ]];
    } else if ( coluna == 2 ) {
        // qto XIS andou
        atual_.id1 = index[ posicao[ BOLA ][ atual_.id1 ] + passo ][ posicao[ XIS ][ atual_.id1 ]];
    } else if ( coluna == 3 ) {
        // qto XIS andou
        atual_.id2 = index[ posicao[ BOLA ][ atual_.id2 ] + passo ][ posicao[ XIS ][ atual_.id2 ]];
    }

    ID_t prox = ID( atual_.id0, atual_.id1, atual_.id2 );
    return ID2int( &prox );
}

int terminal( int k ) {
    int ganhou0 = 0, ganhou1 = 0;
    ID_t id = int2ID( k );
    int v[] = { id.id0, id.id1, id.id2 };
    if ( posicao[ BOLA ][ v[ 0 ]] == 4 &&
         posicao[ BOLA ][ v[ 1 ]] == 4 &&
         posicao[ BOLA ][ v[ 2 ]] == 4 )
        ganhou0 = 1;
    if ( posicao[ XIS ][ v[ 0 ]] == 0 &&
         posicao[ XIS ][ v[ 1 ]] == 0 &&
         posicao[ XIS ][ v[ 2 ]] == 0 )
        ganhou1 = 1;
    return ganhou0 + 2 * ganhou1;
}

void ganha( int jog, int k ) {
    vencedor[ jog ][ k ] = VISITANDO;

    if ( terminal( k )) {
        vencedor[ jog ][ k ] = SEM_ESTRATEGIA;
        return;
    }

    for ( int prox : grafo[ jog ][ k ] ) {
        int adv = prox / 8000;
        int y = prox % 8000;
        if ( vencedor[ adv ][ y ] == NAO_VISITANDO )
            ganha( adv, y );
        if ( vencedor[ adv ][ y ] == SEM_ESTRATEGIA )
            vencedor[ jog ][ k ] = COM_ESTRATEGIA;
    }

    if ( vencedor[ jog ][ k ] == VISITANDO )
        vencedor[ jog ][ k ] = SEM_ESTRATEGIA;
}

std::vector<int> mov_validos_tabuleiro( const int jog, const int K ) {
    std::vector<int> M;
    if ( terminal( K ))
        return M;
    ID_t id = int2ID( K );
    for ( int l = 0; l < 3; ++l ) {
        int k[] = { id.id0, id.id1, id.id2 };
        std::vector<int> m = grafo_coluna[ jog ][ k[ l ]];
        for ( int col = 0; col < m.size(); ++col ) {
            k[ l ] = m[ col ];
            ID_t iaa( k[ 0 ], k[ 1 ], k[ 2 ] );
            int K2 = ID2int( &iaa );
            M.push_back( K2 );
        }
    }
    if ( M.empty())
        M.push_back( K );
    return M;
}

void mov_valido_X_e_O_col() {
    for ( int p = 0; p < 2; p++ ) {
        std::vector<std::vector<int>> vec20;
        for ( int k = 0; k < 20; k++ ) {
            int pos_O = posicao[ BOLA ][ k ];
            int pos_X = posicao[ XIS ][ k ];
            if ( p ) {
                std::vector<int> vec03;
                if ( pos_X == pos_O + 1 && pos_O != 0 )
                    vec03.push_back( index[ pos_O ][ pos_X - 2 ] );
                else {
                    int limite = ( pos_O < pos_X ) ? pos_O + 1 : 0;
                    for ( int i = limite; i < pos_X; i++ )
                        vec03.push_back( index[ pos_O ][ i ] );
                }
                vec20.push_back( vec03 );
            } else {
                std::vector<int> vec03;
                if (( pos_X == pos_O + 1 ) && pos_X != tab_ALTURA - 1 )
                    vec03.push_back( index[ pos_O + 2 ][ pos_X ] );
                else {
                    int limite = ( pos_O < pos_X ) ? pos_X : tab_ALTURA;
                    for ( int i = pos_O + 1; i < limite; i++ )
                        vec03.push_back( index[ i ][ pos_X ] );
                }
                vec20.push_back( vec03 );
            }
        }
        grafo_coluna.push_back( vec20 );
    }
}

void print_col( int id ) {
    for ( int i = 0; i < tab_ALTURA; i++ ) {
        std::string p = ". ";
        if ( i == posicao[ BOLA ][ id ] )
            p = "0 ";
        if ( i == posicao[ XIS ][ id ] )
            p = "X ";
        std::cout << p;
    }
    std::cout << std::endl;
}

void print_tab( int ID ) {
    ID_t id = int2ID( ID );
    std::cout << ID << std::endl;
    print_col( id.id0 );
    print_col( id.id1 );
    print_col( id.id2 );
    std::cout << std::endl;
}

void bot_joga() {
    std::vector<int> M, m = grafo_tabuleiro[ 0 ][ atual ];
    for ( int i = 0; i < m.size(); i++ ) {
        int tab = m[ i ];
        if ( vencedor[ 1 ][ tab ] == SEM_ESTRATEGIA )
            M.insert( M.begin(), tab );
        else
            M.push_back( tab );
    }
    int x = convert_ID_para_PASSO( M[ 0 ], BOLA );

    int coluna = ( x % 4 );
    int passo = ( x / 4 );
    fprintf(stdout, "%d %d\n", coluna, passo );
    fprintf(stderr, "[O] %d %d\n", coluna, passo );
    atual = M[ 0 ];
}

int main( int argc, char **argv ) {

    int id = 0;
    for ( int i = 0; i < tab_ALTURA; i++ )
        for ( int j = 0; j < tab_ALTURA; j++ )
            if ( i != j ) {
                posicao[ BOLA ][ id ] = i;
                posicao[ XIS ][ id ] = j;
                index[ i ][ j ] = id;
                id++;
            }

    mov_valido_X_e_O_col();

    for ( int jog = 0; jog < 2; ++jog ) {
        std::vector<std::vector<int>> vec;
        for ( int k = 0; k < 8000; ++k )
            vec.push_back( mov_validos_tabuleiro( jog, k ));
        grafo_tabuleiro.push_back( vec );
    }

    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            for ( int l: grafo_tabuleiro[ jog ][ k ] )
                grafo[ jog ][ k ].push_back((( 1 - jog ) * 8000 ) + l );

    // DFS para Estrategia
    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            vencedor[ jog ][ k ] = NAO_VISITANDO;
    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            if ( vencedor[ jog ][ k ] == NAO_VISITANDO )
                ganha( jog, k );

    int vez = 1;
    for ( int i = 0; i < argc; i++ )
        if ( std::string( argv[ i ] ) == "primeiro" )
            vez = 0;

    while ( true ) {

        if ( vez == 0 ) {
            bot_joga();  //uh!
            //print_tab(atual);

        } else {
            int coluna, passo;
//            std::cin >> a >> b;
            fscanf(stdin, "%d %d", &coluna, &passo );
            fprintf(stderr, "[O] %d %d\n", coluna, passo );

            std::vector<int> m = grafo_tabuleiro[ 1 ][ atual ];
            for ( int i = 0; i < m.size(); i++ ) {
                if (( passo * 4 + coluna ) == convert_ID_para_PASSO( m[ i ], XIS )) {
                    atual = m[ i ];
                };
            }
        }

        if ( terminal( atual ))
            return 0;
        vez = 1 - vez;
    }

    return 0;
}
