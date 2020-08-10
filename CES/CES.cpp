#include "Board.h"

int main()
{
    Board<8,8> b;
    for (size_t i = 1; i <= 8; ++i)
        for (size_t j = 1; j <=8; ++j)
            if ((i+j)%2==0) std::cout << b.getField(i, j) << '\n';
    b(1, 1);
}
