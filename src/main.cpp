#include <iostream>
#include "Base.h"
#include "Config.h"

const int XIS = 0;
const int BOLA = 1;

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

    int **posicao;
    posicao = new int *[2];
    posicao[ XIS ] = new int[tab_possib];
    posicao[ BOLA ] = new int[tab_possib];

    int **index;
    index = new int *[tab_ALTURA];
    for ( int i = 0; i < tab_ALTURA; ++i )
        index[ i ] = new int[tab_ALTURA];

    for ( int i = 0; i < tab_ALTURA; ++i ) // X
        for ( int j = tab_ALTURA - 1; j >= 0; --j ) // O
            if ( i != j ) {
                configs[ id ] = config( id, i, j );
                posicao[ XIS ][ id ] = i;
                posicao[ BOLA ][ id ] = j;
                index[ i ][ j ] = id;
                id++;
            }
    for ( int i = 0; i < tab_possib; ++i )
        printConfig( &configs[ i ] );

    // INDEX
    std::cout << "POSICAO:" << std::endl;
    for ( int i = 0; i < tab_possib; ++i ) {
        std::cout << "  (" << XIS << " " << i << ") -> " << posicao[ XIS ][ i ] << std::endl;
        std::cout << "  (" << BOLA << " " << i << ") -> " << posicao[ BOLA ][ i ] << std::endl;
    }
    std::cout << "INDEX:" << std::endl;
    for ( int i = 0; i < tab_ALTURA; ++i ) {
        std::cout << "  ";
        for ( int j = 0; j < tab_ALTURA; ++j ) {
            if ( i != j )
                std::cout << index[ i ][ j ] << " ";
            else
                std::cout << ". ";
        }
        std::cout << std::endl;
    }


    return 0;
}