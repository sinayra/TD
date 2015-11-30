#include "../include/log.h"

void showLog(typeLog lvl, string message){
	stringstream ss;

	ss.str(string());
	ss.clear();
	switch(lvl){
		case debug:
			if(SHOW_DEBUG)
				ss << endl << "[DEBUG] " << message << endl;
		break;
		case warning:
			if(SHOW_WARNING)
				ss << endl << "[WARNING] " << message << endl;
		break;
		case error:
			if(SHOW_ERROR)
				ss << endl << "[ERROR] " << message << endl;
		break;
		default:
			ss << endl << "[UNDEFINED] " << message << endl;
		break;
	}
	cout << ss.str();
}