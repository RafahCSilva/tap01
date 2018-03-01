#include <iostream>
#include "Base.h"
#include "Config.h"

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
    for ( int i = 0; i < tab_ALTURA; ++i )
        for ( int j = tab_ALTURA - 1; j >= 0; --j )
            if ( i != j ) {
                configs[ id ] = config( id, i, j );
                id++;
            }
    for ( int i = 0; i < tab_possib; ++i )
        printConfig( &configs[ i ] );

    return 0;
}