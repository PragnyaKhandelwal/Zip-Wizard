#ifndef MENU_H
#define MENU_H

#define MENU_ITEMS 11
#define MAX_OUTPUT_LINES 999
#include <conio.h>

void menu();
void heading(const char *text);
void userchoice(int n);
void quitProgram();
int getConsoleWidth();
void reprintOutputs();

#endif
