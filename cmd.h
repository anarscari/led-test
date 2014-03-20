#ifndef CMD_H
#define CMD_H

#include <string>
#include <vector>
#include <map>
#include <list>

#include <pthread.h>

#include "led.h"

typedef bool (*LEDFN)(int*);

class Cmd {
public:
    Cmd();
    void exec(char *str, int len);
    int addCommand(std::string, LEDFN, bool set);
    void addCommandArg(int cmd, std::string strArg, int intArg);
private:
    std::vector<std::string> _commands;
    std::vector<bool> _cmdset;
    std::map<int, std::map<std::string, int> > _args;
    std::vector<void*> _fns;
};

#endif
