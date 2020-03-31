/**
* @brief
* WinTelloAR boot entry source
* Boot.cpp
*/
#include "std.h"
#include "Main.h"

/**
* @brief
*/
int main(int argc, char* argv[])
{
	wintelloar::Main wintelloar_main;
	while (wintelloar_main.Do());
	return 0;
}

// EOF