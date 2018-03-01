#include <iostream>
#include "Base.h"

int main() {
    std::cout << "Eae Rafao" << std::endl;

    std::cout << "Testando ID_b20 -> ID_t" << std::endl;
    ID_t a = Base::int2ID( 512 );
    std::cout << "  ( " << 512 << " ) -> " << a.id0 << " " << a.id1 << " " << a.id2 << std::endl;

    std::cout << "Testando ID_t -> ID_b20" << std::endl;
    ID_t b( 3, 2, 1 );
    int c = Base::ID2int( &b );
    std::cout << "  ( " << b.id0 << " " << b.id1 << " " << b.id2 << " ) -> " << c << std::endl;

    return 0;
}