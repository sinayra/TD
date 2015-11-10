#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string>
#include <iostream>

using namespace std;

//Verifica se device é válido
void lpcap_setDevice();

//Encerra sessao
void lpcap_close();
