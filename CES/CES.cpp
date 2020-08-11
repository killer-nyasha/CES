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
        if (b.whoseTurn() == White)
        {
            auto p_turnsB = b.getVariants();
            auto turnsB = *p_turnsB;

            if (true) //human
            {
                std::cout << "\n";
                for (size_t i = 0; i < turnsB.size(); i++)
                    std::cout << i << ": " << turnsB[i] << "\n";

                int i2;
                std::cin >> i2;
                b.doTurn(turnsB[i2]);
            }
            else //bot
                b.doTurn(turnsB[turnsB.size() / 2]);

            b.drawBoard();
            Sleep(200);
        }
        else
        {
            auto p_turnsW = b.getVariants();
            auto turnsW = *p_turnsW;
            DeepPoisk<decltype(b), Turn> ces;
            b.doTurn(ces.selectTurn(b));
            b.drawBoard();
            Sleep(200);
        }
    }
    std::cout << "\n end of game!\n";
}
