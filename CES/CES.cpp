#include <ctime>
#include <Windows.h>

#include "Board.h"
#include "Poisk.h"

int main()
{
    srand(time(NULL));
    Board<8,8> b;
    //std::cout << b.getVariants().size() << " possible turns\n";

    b.drawBoard();

    //жывтоне чочо упячка

    while (b.won() == -1)
    {
        auto p_turnsW = b.getVariants();
        auto turnsW = *p_turnsW;
        Poisk<decltype(b), Turn> ces;
        ces.selectTurn(b);
        b.doTurn(turnsW[turnsW.size()/2]);
        b.drawBoard();
        Sleep(200);

        auto p_turnsB = b.getVariants();
        auto turnsB = *p_turnsB;
        b.doTurn(turnsB[turnsB.size() / 2]);
        b.drawBoard();
        Sleep(200);
    }
    std::cout << "\n end of game!\n";
}
