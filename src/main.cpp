/**
 * @author Rafael Cardoso da Silva 21048012
 *
 */
#include <iostream>
#include <iomanip>
#include <vector>

#define tab_ALTURA 5
#define tab_possib 19
#define BOLA 0 // O BRANCO
#define XIS 1  // X PRETO

int **posicao;
int **index;
int grafo_coluna[2][tab_possib][3];

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

class Config {
public:
};

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
    for ( int i = pos_X - 1; i > limite; --i ) {
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

void mov_validos_tabuleiro( const int jog, const int K, std::vector<int> *M ) {
    if ( terminal( K )) return;
    ID_t id = Base::int2ID( K );
    for ( int col = 0; col <= 2; ++col ) {
        int k[] = { id.id0, id.id1, id.id2 };
        for ( int l = 0; l < 3; ++l ) {
            k[ col ] = grafo_coluna[ jog ][ k[ col ]][ col ];
            if ( k[ col ] == -1 )
                break;
            ID_t iaa( k[ 0 ], k[ 1 ], k[ 2 ] );
            M->push_back( Base::ID2int( &iaa ));
        }
    }
    if ( M->empty())
        M->push_back( K );
}

int main() {
    std::cout << "Eae Rafao" << std::endl;

    // MUDANCA DE BASE
    std::cout << "Testando ID_b20 -> ID_t" << std::endl;
    ID_t a = Base::int2ID( 512 );
    std::cout << "  ( " << 512 << " ) -> " << a.id0 << " " << a.id1 << " " << a.id2 << std::endl;

    std::cout << "Testando ID_t -> ID_b20" << std::endl;
    ID_t b( 3, 2, 1 );
    int c = Base::ID2int( &b );
    std::cout << "  ( " << b.id0 << " " << b.id1 << " " << b.id2 << " ) -> " << c << std::endl;

    // POSSIBILIDADE DA COLUNA
    std::cout << "Possibilidades de Config da Coluna:" << std::endl;
    int id = 0;
    auto *configs = new config_t[tab_possib];

    posicao = new int *[2];
    posicao[ BOLA ] = new int[tab_possib];
    posicao[ XIS ] = new int[tab_possib];

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
    for ( int i = 0; i <= tab_possib; ++i )
        printConfig( &configs[ i ] );

    // INDEX
    std::cout << "POSICAO:" << std::endl;
    for ( int i = 0; i <= tab_possib; ++i ) {
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

    std::cout << "grafo_coluna:" << std::endl;
    for ( int jog = 0; jog < 2; ++jog )
        for ( int col = 0; col <= tab_possib; ++col )
            for ( int i = 0; i < 3; ++i )
                grafo_coluna[ jog ][ col ][ i ] = -1;
    for ( int k = 0; k <= tab_possib; ++k ) {
        mov_valido_O_col( k, grafo_coluna[ BOLA ][ k ] );
        mov_valido_X_col( k, grafo_coluna[ XIS ][ k ] );
    }
    for ( int j = 0; j < 2; ++j )
        for ( int k = 0; k <= tab_possib; ++k )
            std::cout << "  (" << j << ", " << std::setw( 2 ) << k << ") -> "
                      << std::setw( 2 ) << grafo_coluna[ j ][ k ][ 0 ] << " "
                      << std::setw( 2 ) << grafo_coluna[ j ][ k ][ 1 ] << " "
                      << std::setw( 2 ) << grafo_coluna[ j ][ k ][ 2 ]
                      << std::endl;

    std::cout << "grafo_tabuleiro:" << std::endl;
    std::vector<int> grafo_tabuleiro[2][8000];
    for ( int k = 0; k < 8000; ++k ) {
        for ( int jog = 0; jog < 2; ++jog ) {
            mov_validos_tabuleiro( jog, k, &grafo_tabuleiro[ jog ][ k ] );
            std::cout << "  (" << jog << " " << k << ") -> ";
            for ( int l : grafo_tabuleiro[ jog ][ k ] )
                std::cout << l << " ";
            std::cout << std::endl;
        }
    }

    return 0;
}