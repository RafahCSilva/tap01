/**
 * @author Rafael Cardoso da Silva 21048012
 * Tecnicas Avancadas de Programacao – 2018.Q1
 * Projeto 1: O Jogo dos Peoes
 *
 * Compile com C++ 11:
 *   g++ rafaelcardoso_tap01.cpp -o rafaelcardoso_tap01.exe -std=c++11
 */

#include <iostream>
#include <iomanip>
#include <vector>

#define tab_ALTURA 5
#define tab_POSSIB 20
#define BOLA  0  // O BRANCO sobe
#define XIS   1  // X PRETO  desce

// flag do DFS
#define NAO_VISITANDO   0   // = nao visitado
#define VISITANDO       1   // = visitando
#define SEM_ESTRATEGIA  2   // = SEM estrategia
#define COM_ESTRATEGIA  3   // = COM estrategia


int posicao[2][tab_POSSIB];
int index[tab_POSSIB][tab_POSSIB];
std::vector<std::vector<std::vector<int> > > grafo_coluna;
std::vector<std::vector<std::vector<int> > > grafo_tabuleiro;
int vencedor[2][8000];
std::vector<int> grafo[2][8000];
int ATUAL = 1263; // ID da Configuracao Inicial

/**
 * Struct do ID do tabuleiro de 3 colunas.
 */
struct ID_t {
    /**
     * Constructor do ID_t.
     *
     * @param id0 Coluna 0
     * @param id1 Coluna 1
     * @param id2 Coluna 2
     */
    ID_t( int id0, int id1, int id2 ) : id0( id0 ), id1( id1 ), id2( id2 ) {}

    ID_t() = default;

    int id0, id1, id2;
};

/**
 * Funcao q devolve o ID_t com os parametros.
 *
 * @param id0 Coluna 0
 * @param id1 Coluna 1
 * @param id2 Coluna 2
 * @return ID_t
 */
static ID_t ID( int id0, int id1, int id2 ) { return { id0, id1, id2 }; }

/**
 * Converte um Inteiro na base 20 para ID_t.
 *
 * @param b20   Inteiro na base 20
 * @return ID_t
 */
ID_t int2ID( int b20 ) {
    ID_t id{};
    id.id2 = b20 / 400;
    b20 = b20 % 400;
    id.id1 = b20 / 20;
    id.id0 = b20 % 20;
    return id;
}

/**
 * Converte um ID_t para um inteiro na base 20.
 *
 * @param id ID_t
 * @return
 */
int ID2int( ID_t *id ) {
    return 1 * id->id0 +
           20 * id->id1 +
           400 * id->id2;
}

/**
 * Print do proximo movimento.
 *
 * @param id_prox   ID do proximo movimento partir de um ID.
 * @param jog       Jogador
 * @return int
 */
int convert_ID_para_PASSO( int id_prox, int jog ) {
    ID_t origem = int2ID( ATUAL );
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

/**
 * Verifica se a Configuracao é Terminal.
 * @param k     Configuracao
 * @return      0 = ninguem (nao eh terminal)
 *              1 = Ganha BOLA
 *              2 = Ganha XIS
 *              3 = Ganha os dois (ambos nas posicoes contrarias)
 */
int terminal( int k ) {
    int ganhou0 = 0, ganhou1 = 0;
    ID_t id = int2ID( k );
    int v[] = { id.id0, id.id1, id.id2 };

    // Se todos os BOLA estao na ultima linha
    if ( posicao[ BOLA ][ v[ 0 ]] == 4 &&
         posicao[ BOLA ][ v[ 1 ]] == 4 &&
         posicao[ BOLA ][ v[ 2 ]] == 4 )
        ganhou0 = 1;
    // Se todos os XIS estao na primeira linha
    if ( posicao[ XIS ][ v[ 0 ]] == 0 &&
         posicao[ XIS ][ v[ 1 ]] == 0 &&
         posicao[ XIS ][ v[ 2 ]] == 0 )
        ganhou1 = 1;

    return ganhou0 + 2 * ganhou1;
}

/**
 * Constroi a Estrategia por Busca em Profundidade.
 *
 * @param jog   Jogador
 * @param k     Configuracao
 */
void ganha( int jog, int k ) {
    // Visitando vertice
    vencedor[ jog ][ k ] = VISITANDO;

    // Se eh terminal entao nao tem estrategia
    if ( terminal( k )) {
        vencedor[ jog ][ k ] = SEM_ESTRATEGIA;
        return;
    }

    // Visita todas os proximos movimentos
    for ( int prox : grafo[ jog ][ k ] ) {
        int adv = prox / 8000;
        int y = prox % 8000;
        // se nao visitou, entao visita
        if ( vencedor[ adv ][ y ] == NAO_VISITANDO )
            ganha( adv, y );
        // se nao vizinho nao tem estrategia, entao este vertice tem
        if ( vencedor[ adv ][ y ] == SEM_ESTRATEGIA )
            vencedor[ jog ][ k ] = COM_ESTRATEGIA;
    }

    // senao nao tem estrategia mesmo
    if ( vencedor[ jog ][ k ] == VISITANDO )
        vencedor[ jog ][ k ] = SEM_ESTRATEGIA;
}

/**
 * Movimentos validos para cada configuracao do tabuleiro.
 *
 * @param jog   Jogador
 * @param K     Configuracao
 * @return      Vizinhos
 */
std::vector<int> mov_validos_tabuleiro( const int jog, const int K ) {
    std::vector<int> M;

    // Se nao eh terminal entao nao em vizinhos
    if ( terminal( K ))
        return M;

    // todas as possibilidades validas de movimento
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

    // se nao tem movimento, entao passa a vez retornando para vc msm
    if ( M.empty())
        M.push_back( K );

    return M;
}

/**
 * Movimentos Validos para cada coluna para BOLA e XIS.
 */
void mov_valido_X_e_O_col() {
    for ( int jogador = 0; jogador < 2; jogador++ ) {
        std::vector<std::vector<int>> vec_possib;
        for ( int k = 0; k < 20; k++ ) {
            int pos_O = posicao[ BOLA ][ k ];
            int pos_X = posicao[ XIS ][ k ];
            std::vector<int> vec_coluna;
            if ( jogador ) { // se jogador eh o XIS
                if ( pos_X == pos_O + 1 && pos_O != 0 )
                    // XIS pula BOLA
                    vec_coluna.push_back( index[ pos_O ][ pos_X - 2 ] );
                else {
                    // XIS anda pra frente
                    int limite = ( pos_O < pos_X ) ? pos_O + 1 : 0;
                    for ( int i = limite; i < pos_X; i++ )
                        vec_coluna.push_back( index[ pos_O ][ i ] );
                }
                vec_possib.push_back( vec_coluna );
            } else { // se jogador eh o BOLA
                if (( pos_X == pos_O + 1 ) && pos_X != tab_ALTURA - 1 )
                    // BOLA pula XIS
                    vec_coluna.push_back( index[ pos_O + 2 ][ pos_X ] );
                else {
                    // BOLA anda pra frente
                    int limite = ( pos_O < pos_X ) ? pos_X : tab_ALTURA;
                    for ( int i = pos_O + 1; i < limite; i++ )
                        vec_coluna.push_back( index[ i ][ pos_X ] );
                }
                vec_possib.push_back( vec_coluna );
            }
        }
        grafo_coluna.push_back( vec_possib );
    }
}

/**
 * Imprime a Coluna que tem tal ID.
 *
 * @param id
 */
void print_col( int id ) {
    for ( int i = 0; i < tab_ALTURA; i++ ) {
        if ( i == posicao[ BOLA ][ id ] )
            std::cout << "0 ";
        else if ( i == posicao[ XIS ][ id ] )
            std::cout << "X ";
        else
            std::cout << ". ";
    }
    std::cout << std::endl;
}

/**
 * Imprime o Tabuleiro naquela Configuracao.
 *
 * @param ID
 */
void print_tabuleiro( int ID ) {
    ID_t id = int2ID( ID );
    std::cout << ID << std::endl;
    print_col( id.id0 );
    print_col( id.id1 );
    print_col( id.id2 );
    std::cout << std::endl;
}

/**
 * Jogada do Bot.
 */
void bot_joga() {
    std::vector<int> M;
    std::vector<int> m = grafo_tabuleiro[ 0 ][ ATUAL ];
    for ( int i = 0; i < m.size(); i++ ) {
        int possib = m[ i ];
        if ( vencedor[ XIS ][ possib ] == SEM_ESTRATEGIA )
            M.insert( M.begin(), possib );
        else
            M.push_back( possib );
    }
    int prox = convert_ID_para_PASSO( M[ 0 ], BOLA );

    int coluna = ( prox % 4 );
    int passo = ( prox / 4 );
    fprintf(stdout, "%d %d\n", coluna, passo );
    fprintf(stderr, "[O] %d %d\n", coluna, passo );
    ATUAL = M[ 0 ];
}

/**
 * Jogada do Adversario.
 */
void adversario_joga() {
    int coluna, passo;
    fscanf(stdin, "%d %d", &coluna, &passo );
    fprintf(stderr, "[O] %d %d\n", coluna, passo );

    std::vector<int> m = grafo_tabuleiro[ 1 ][ ATUAL ];
    for ( int i = 0; i < m.size(); i++ ) {
        if (( passo * 4 + coluna ) == convert_ID_para_PASSO( m[ i ], XIS )) {
            ATUAL = m[ i ];
        };
    }
}

int main( int argc, char **argv ) {

    // indexa a Posicao e o ID
    int id = 0;
    for ( int i = 0; i < tab_ALTURA; i++ )
        for ( int j = 0; j < tab_ALTURA; j++ )
            if ( i != j ) {
                posicao[ BOLA ][ id ] = i;
                posicao[ XIS ][ id ] = j;
                index[ i ][ j ] = id;
                id++;
            }

    // montas todos os movimentos validos
    mov_valido_X_e_O_col();

    // constroi o grafo do tabuleiro
    for ( int jog = 0; jog < 2; ++jog ) {
        std::vector<std::vector<int>> vec;
        for ( int k = 0; k < 8000; ++k )
            vec.push_back( mov_validos_tabuleiro( jog, k ));
        grafo_tabuleiro.push_back( vec );
    }

    // constroi grafo de movimentos
    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            for ( int l: grafo_tabuleiro[ jog ][ k ] )
                grafo[ jog ][ k ].push_back((( 1 - jog ) * 8000 ) + l );

    // DFS para construir a Estrategia
    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            vencedor[ jog ][ k ] = NAO_VISITANDO;
    for ( int jog = 0; jog < 2; ++jog )
        for ( int k = 0; k < 8000; ++k )
            if ( vencedor[ jog ][ k ] == NAO_VISITANDO )
                ganha( jog, k );

    // Quem comeca?
    int vez = 1;
    if ( argc > 1 && std::string( argv[ 1 ] ) == "primeiro" )
        vez = 0;

    //print_tabuleiro( ATUAL );
    while ( true ) {
        // Quem joga?
        if ( vez == 0 )
            bot_joga();
        else
            adversario_joga();
        //print_tabuleiro( ATUAL );

        // Jogo terminou?
        if ( terminal( ATUAL ))
            return 0;

        // Troca a vez
        vez = 1 - vez;
    }

    return 0;
}
