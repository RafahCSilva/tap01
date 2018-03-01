//
// Created by Rafah on 28/02/2018.
//

#ifndef TAP01_BASE_H
#define TAP01_BASE_H

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

#endif //TAP01_BASE_H
