#include <ctime>
#include <Windows.h>

#include "Board.h"
#include "Poisk.h"

#include "LGApp.h"

LGraphics::LGApp app;
void tick()
{
    Board<8, 8> b;
    while (b.won() == -1)
    {
        Sleep(300);
        if (b.whoseTurn() == White)
        {
            auto p_turnsB = b.getVariants();
            if (b.whoseTurn() == Black) continue;
            auto turnsB = *p_turnsB;

            if (false) //human
            {
                std::cout << "\n";
                for (size_t i = 0; i < turnsB.size(); i++)
                    std::cout << i << ": " << turnsB[i] << "\n";

                int i2;
                std::cin >> i2;
                b.doTurn(turnsB[i2],&app);
            }
            else //bot
                b.doTurn(turnsB[turnsB.size() / 2], &app);
        }

        else
        {
            auto p_turnsW = b.getVariants();
            if (b.whoseTurn() == White) continue;
            auto turnsW = *p_turnsW;
            DeepPoisk<decltype(b), Turn> ces(&app);
            b.doTurn(ces.selectTurn(b), &app);
        }
        //b.drawBoard();
    }
}


int main()
{
    app.setTick(tick);
    app.loop();
    std::cout << "\n end of game!\n";
}
