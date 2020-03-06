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
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT_BASE 0x378
#define PORT_SIZE 0x10

using namespace std;

char dataPort = 0x0;
mutex mtx;
bool cancel = false;
int cancelCounter = 0;
int maxSpeed = 1000;

class Engine
{
public:
    virtual ~Engine(){}
    virtual void updateDataPort()= 0;

    void moveEng(int speed, bool reverse){
    	mtx.lock();
        updateDataPort();
        outb(dataPortClass, PORT_BASE);
        dataPort = dataPortClass;
        if(reverse){
        	step--;
        }else{
        	step++;
        }
        mtx.unlock();
    }

protected:
    unsigned long step = 2000000000;
    char dataPortClass;
};

class LeftEngine : public Engine
{
public:
    LeftEngine(){}
    ~LeftEngine(){}
    void updateDataPort(){
        dataPortClass = dataPort & 0xF0 | (0x33 >> (step & 3)) & 0xF;
    }

};

class RightEngine : public Engine
{
public:
    RightEngine(){}
    ~RightEngine(){}
    void updateDataPort(){
        dataPortClass = dataPort & 0x0F | ((0x33 >> (step & 3)) & 0xF) << 4;
    }
};

void loop(Engine *eng, vector<int> speeds, vector<int> times){
    int delay;
    int speed;
    int steps;
    bool reverse;

    for(int i = 0; i < speeds.size(); i++){

    	speed = speeds[i];

    	if(speed < 0){
    		reverse = true;
    		speed = -speed;
    	}
    	else{
    		reverse = false;
    	}

    	steps = round(speed * times[i] / 1000000);
    	if(steps == 0) steps = 1;

    	cout << steps << endl;

    	if(speed != 0){

    		delay = round(1000000 / steps);

    		for(int k = 0; k < steps; k++){
            	eng->moveEng(speed, reverse);
            	this_thread::sleep_for(chrono::microseconds(delay));
        	}

    	}else{
    		this_thread::sleep_for(chrono::microseconds(times[i]));
    	}

        if(cancel){
        	// outb(0, PORT_BASE);
        	cancelCounter++;
        	return;
        }
    }
}

void getCoordCircle(vector<double> &x,vector<double> &y){
	double dFi = 0.01;
	
	for(double fi = 0; fi < 2*M_PI; fi+=dFi){
		x.push_back(cos(fi));
		y.push_back(sin(fi));
	}
}


int main(){

	ioperm(PORT_BASE, PORT_SIZE, 1);

    Engine *leftEng = new LeftEngine();
    Engine *rightEng = new RightEngine();

    vector<double> x;
	vector<double> y;

    vector<int> speedsEng1;
    vector<int> timesEng1;

    vector<int> speedsEng2;
    vector<int> timesEng2;

    getCoordCircle(x, y);
    scaleSpeed(x, y, speedsEng1, speedsEng2, timesEng1, timesEng2, 100000);
    for(int i = 0; i < speedsEng1.size(); i++){
    	cout << speedsEng1[i] << "\t" << speedsEng2[i] << endl;
    }

    thread th1(loop, leftEng, speedsEng1, timesEng1);
    thread th2(loop, rightEng, speedsEng2, timesEng2);

    th1.join();
    th2.join();

    outb(0, PORT_BASE);

    delete leftEng;
    delete rightEng;

    cancelCounter++;

    return 0;

}