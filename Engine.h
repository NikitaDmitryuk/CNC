//
// Created by nikitalaba on 16.03.2020.
//

#ifndef CNC_ENGINE_H
#define CNC_ENGINE_H

#include <cmath>
#include <sys/io.h>
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <unistd.h>

using namespace std;

extern char dataPort;
extern mutex mtx;
extern bool cancel;
extern int cancelCounter;
extern int minTau;
extern unsigned short int PORT_BASE;
extern unsigned long PORT_SIZE;

class Engine {
public:
    virtual ~Engine();
    virtual void updateDataPort()= 0;
    void moveEng(int _step);
protected:
    int step = 5000;
    unsigned char dataPortClass = 0;
};


#endif //CNC_ENGINE_H
