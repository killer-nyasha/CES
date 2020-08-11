#include "Board.h"
#include <ctime>
#include <Windows.h>

int main()
{
    srand(time(NULL));
    Board<8,8> b;
    std::cout << b.getVariants().size() << " possible turns\n";
    while (true)
    {
        b.drawBoard();
        auto turns = b.getVariants();
        b.doTurn(turns[0]);
        Sleep(1200);
    }
}
