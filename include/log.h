#pragma once
#include <iostream>
#include <string>
#include <sstream>

#define SHOW_DEBUG 1		//mostrar debug
#define SHOW_WARNING 1		//mostrar warning
#define SHOW_ERROR 1			//mostrar erro

using namespace std;

typedef enum{
	debug, 
	warning, 
	error
}typeLog;

void showLog(typeLog lvl, string message); //exibe mensagem de log de acordo com nivel