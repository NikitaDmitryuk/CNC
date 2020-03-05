#include <stdio.h>
#include <sys/ioctl.h>
#include <math.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#define PORT_BASE 0x378
#define PORT_SIZE 0x10

using namespace std;

char dataPort = 0x0;
mutex mtx;

class Engine
{
public:
    virtual ~Engine(){}
    virtual void outPort()= 0;

protected:
    unsigned long step = 2000000000;
    char dataPortClass;
    void setDataPort(){
        dataPort = dataPortClass;
        step++;
    }

};

class LeftEngine : public Engine
{
public:
    LeftEngine(){}
    ~LeftEngine(){}
    void outPort(){
        mtx.lock();
        dataPortClass = dataPort & 0xF0 | (0x33 >> (step & 3)) & 0xF;
        outb(dataPortClass, PORT_BASE);
        setDataPort();
        mtx.unlock();
    }

};

class RightEngine : public Engine
{
public:
    RightEngine(){}
    ~RightEngine(){}
    void outPort(){
        mtx.lock();
        dataPortClass = dataPort & 0x0F | ((0x33 >> (step & 3)) & 0xF) << 4;
        outb(dataPortClass, PORT_BASE);
        setDataPort();
        mtx.unlock();
    }
};

void loop(Engine *eng, vector<int> speeds, vector<int> steps){
    int delay;
    for(int i = 0; i < speeds.size(); i++){
        if(speeds[i] != 0){
            for(int k = 0; k < steps[i]; k++){
                delay = round(1000000 / speeds[i]);
                eng->outPort();
                this_thread::sleep_for(chrono::microseconds(delay));
            }
        }
    }
}


int main(){

    ioperm(PORT_BASE, PORT_SIZE, 1);

    Engine *leftEng = new LeftEngine();
    Engine *rightEng = new RightEngine();

    vector<int> speeds;
    vector<int> steps;

    for(int i = 30; i < 1000; i++){
        speeds.push_back(i);
    }


    for(int i = 30; i < 1000; i++){
        steps.push_back(5);
    }

    thread th1(loop, leftEng, speeds, steps);
    thread th2(loop, rightEng, speeds, steps);

    th1.join();
    th2.join();

    outb(0, PORT_BASE);

    delete leftEng;
    delete rightEng;

    return 0;

}