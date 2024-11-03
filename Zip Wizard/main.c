#include "menu.h"

int main()
{
    terminalSize(110,35);
    menu();
    while (1)
    {
        validatechoices();
    }
    return 0;
}
