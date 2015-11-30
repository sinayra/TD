#pragma once
#include <iostream>
#include <string>
#include <sstream>

#define SHOW_DEBUG false		//mostrar debug
#define SHOW_WARNING true		//mostrar warning
#define SHOW_ERROR true			//mostrar erro

using namespace std;

typedef enum{
	debug, 
	warning, 
	error
}typeLog;

void showLog(typeLog lvl, string message); //exibe mensagem de log de acordo com nivel