#include <ctime>
#include <Windows.h>

#include "Board.h"
#include "Poisk.h"

#include "LGApp.h"

LGraphics::LGApp app;


Turn* findTurn(std::vector<Turn>& pool, LGraphics::LGApp::TryTurn turn)
{
    for (auto& t : pool)
        if (t.from == turn.from && t.to == turn.to)
            return &t;
    return nullptr;
}

void tick()
{
    Board<8, 8> b(&app);
    bool player = true;
    while (b.won() == -1)
    {
        if (b.whoseTurn() == White)
        {
            auto p_turnsB = b.getVariants();
            if (b.whoseTurn() == Black) continue;
            auto turnsB = *p_turnsB;

            if (player) //human
            {
                app.playerTurn = true;

                // choose turn
                while (app.turnTodo.from == szvect2{ 0,0 });
                Turn* turn;
                if (turn = findTurn(turnsB, app.turnTodo))
                {
                    b.doTurn(*turn, &app);
                    app.turnTodo.from = { 0,0 };
                }
                else continue;
            }
            else //bot
            {
                DeepPoisk<decltype(b), Turn> ces;
                b.doTurn(ces.selectTurn_random(b), &app);
                //b.doTurn(ces.selectTurn_simple(b), &app);
            }
        }
        else
        {
            //auto p_turnsW = b.getVariants();
            if (b.whoseTurn() == White) continue;
            //auto turnsW = *p_turnsW;
            DeepPoisk<decltype(b), Turn> ces;
            b.doTurn(ces.selectTurn(b), &app);
        }
        Sleep(300);
    }
}


int main()
{
    std::thread thread(tick);
    app.loop();
    thread.join();
    std::cout << "\n end of game!\n";
}
