#include "../include/lpcap.h"

void lpcap_setDevice(){
    char errbuf[PCAP_ERRBUF_SIZE];
    string dev;

    dev = pcap_lookupdev(errbuf);
    if(dev.empty()){
        cout << "Device nao encontrado" << errbuf << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Device: " << dev << endl;
}


void lpcap_close(){
    pcap_close();
}
