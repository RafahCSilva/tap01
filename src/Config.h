//
// Created by Rafah on 01/03/2018.
//

#ifndef TAP01_CONFIG_H
#define TAP01_CONFIG_H
const int tab_ALTURA = 5;
const int tab_possib = tab_ALTURA * tab_ALTURA - tab_ALTURA - 1;

struct config_t {
//    config_t( int id, int bola, int xis ) : id( id ), bola( bola ), xis( xis ) {}

    int id;
    int xis;
    int bola;
};

static config_t config( int id, int xis, int bola ) { return { id, xis, bola }; }

void printConfig( config_t *conf ) {
    std::cout << "  " << conf->id << ": ";
    for ( int i = 0; i < tab_ALTURA; ++i ) {
        if ( conf->xis == i )
            std::cout << "X";
        else if ( conf->bola == i )
            std::cout << "O";
        else
            std::cout << ".";
    }
    std::cout << std::endl;
}

class Config {
public:

};

#endif //TAP01_CONFIG_H
