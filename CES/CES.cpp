#include "Board.h"
#include <ctime>
#include <Windows.h>

int main()
{
    srand(time(NULL));
    Board<8,8> b;
    std::cout << b.getVariants().size() << " possible turns\n";
    while (b.won() == -1)
    {
        b.drawBoard();
        auto turns = b.getVariants();
        b.doTurn(turns[turns.size()/2]);
        Sleep(200);
    }
    std::cout << "\n end of game!\n";
}
