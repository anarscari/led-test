#include "cmd.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sstream>

static pthread_mutex_t _exec_mutex = PTHREAD_MUTEX_INITIALIZER;


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
	if (item.size()) elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

Cmd::Cmd(){
}


void Cmd::exec(char *str, int len){
    std::string arg;
    std::string cmd;
    std::string res;
    bool set;
    int i = 0;
    bool ok = false;

    pthread_mutex_lock(&_exec_mutex);
    std::vector<std::string> vect = split(str, ' ');
    if (vect.size() > 2) {
	snprintf(str, len, "FAILED\n");
	pthread_mutex_unlock(&_exec_mutex);
	return;
    }
    cmd = vect[0];
    if (std::isspace(cmd[cmd.size() - 1])) cmd.erase(cmd.size() - 1);
    if (vect.size() == 2) {
	arg = vect[1];
	if (std::isspace(arg[arg.size() - 1])) arg.erase(arg.size() - 1);
    }

    for (int i = 0;i < _commands.size();i++) {
	if (_commands[i] == cmd.c_str()) {
	    LEDFN fn = (LEDFN)_fns[i];
	    set = _cmdset[i];
	    if (set ^ (bool)arg.size()) {
		snprintf(str, len, "FAILED\n");
		pthread_mutex_unlock(&_exec_mutex);
		return;
	    }
	    int intArg = 0;
	    if (set) {
		std::map<std::string, int>::iterator it = _args[i].find(arg.c_str());
		if (it == _args[i].end()) {
		    snprintf(str, len, "FAILED\n");
		    pthread_mutex_unlock(&_exec_mutex);
		    return;
		} else intArg = (*it).second;
	    }
	    ok = fn(&intArg);
	    if (ok) {
		std::map<std::string, int>::iterator it = _args[i].begin();
		while (it != _args[i].end()) {
		    if ((*it).second == intArg) {
			res = (*it).first;
			break;
		    }
		    it++;
		}
	    }
	    break;
	}
    }
    if ((i < _commands.size()) && ok) {
	if (set) sprintf(str, "OK\n");
	else snprintf(str, len - 5, "OK %s\n", res.c_str());
    } else {
	snprintf(str, len, "FAILED\n");
    }
    pthread_mutex_unlock(&_exec_mutex);
}
int Cmd::addCommand(std::string str, LEDFN fn, bool set) {
    int r = _commands.size();
    _commands.push_back(str);
    _cmdset.push_back(set);
    _fns.push_back((void*)fn);
    return r;
}
void Cmd::addCommandArg(int n, std::string str, int i){
    _args[n][str] = i;
}
