#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string>
#include <iostream>

using namespace std;

//Verifica se device é válido
//Argumento deve ser argv que veio da main
void setDevice(char *argv[]);
