#ifndef MENU_H
#define MENU_H

#define MENU_ITEMS 11
#define MAX_OUTPUT_LINES 999
#include <conio.h>

void menu();
void validatechoices();
void heading(const char *text);
void userchoice(int n);
void quitProgram();

#endif
