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
int minTau = 3000;

class Engine
{
public:
    virtual ~Engine(){}
    virtual void updateDataPort()= 0;

    void moveEng(int _step){
    	mtx.lock();
        updateDataPort();
        outb(dataPortClass, PORT_BASE);
        dataPort = dataPortClass;
        if(_step == 1){
        	step++;
        }else{
        	step--;
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

void loop(Engine *eng, vector<int> steps, vector<int> delays){

	for(int i = 0; i < steps.size(); i++){
		if(steps[i] != 0){
			eng->moveEng(steps[i]);
		}
		this_thread::sleep_for(chrono::microseconds(delays[i]));
	}

    if(cancel){
        cancelCounter++;
        return;
    }
}

void getCoordCircle(vector<double> &x,vector<double> &y){
	double dFi = 0.01;
	
	for(double fi = 0; fi < 2*M_PI; fi+=dFi){
		x.push_back(cos(fi));
		y.push_back(sin(fi));
	}
}

void getCoordParabl(vector<double> &x,vector<double> &y){
	double dx = 0.01;
	
	for(double xt = -5.0; xt < 5.0; xt+=dx){
		x.push_back(xt);
		y.push_back(xt*xt);
	}
}


void calcWaitTime(vector<double> x, vector<double> y,
 vector<int> &_stepsX, vector<int> &_delaysX,
 vector<int> &_stepsY, vector<int> &_delaysY, int scale){
 	double dx;
 	double dy;
 	double ratio;
 	vector<int> delaysX;
 	vector<int> delaysY;
 	vector<int> stepsX;
 	vector<int> stepsY;

 	for (int i = 0; i < x.size()-1; i++)
 	{
 		dx = x[i+1]-x[i];
 		dy = y[i+1]-y[i];
 		if(fabs(dy) > fabs(dx)){
 			delaysY.push_back(minTau);
 			stepsY.push_back(copysign(1, dy));
 			if(dx != 0){
 				delaysX.push_back(round(fabs(dy / dx * minTau)));
 				stepsX.push_back(copysign(1, dx));
 			}else{
 				stepsX.push_back(0);
 				delaysX.push_back(minTau);
 			}

 		}else{
 			delaysX.push_back(minTau);
 			stepsX.push_back(copysign(1, dx));
 			if(dy != 0){
 				delaysY.push_back(round(fabs(dx / dy * minTau)));
 				stepsY.push_back(copysign(1, dy));
 			}else{
 				stepsY.push_back(0);
 				delaysY.push_back(minTau);
 			}
 		}
 	}

 	for(int i = 0 ; i < stepsX.size(); i++){
 		for(int j = 0; j < scale; j++){
 			_stepsX.push_back(stepsX[i]);
 			_stepsY.push_back(stepsY[i]);
 			_delaysX.push_back(delaysX[i]);
 			_delaysY.push_back(delaysY[i]);
 		}
 	}
}


int main(){

	ioperm(PORT_BASE, PORT_SIZE, 1);

    Engine *leftEng = new LeftEngine();
    Engine *rightEng = new RightEngine();

    vector<double> x;
	vector<double> y;

    vector<int> stepsX;
    vector<int> delaysX;

    vector<int> stepsY;
    vector<int> delaysY;

    getCoordParabl(x, y);
    calcWaitTime(x, y, stepsX, delaysX, stepsY, delaysY, 5);

    for(int i = 0; i < stepsX.size(); i++){
    	cout << stepsX[i] << "\t" << delaysX[i] << endl;
    	cout << stepsY[i] << "\t" << delaysY[i] << endl;
    }

    thread th1(loop, leftEng, stepsX, delaysX);
    thread th2(loop, rightEng, stepsY, delaysY);

    th1.join();
    th2.join();

    outb(0, PORT_BASE);

    delete leftEng;
    delete rightEng;

    cancelCounter++;

    return 0;

}