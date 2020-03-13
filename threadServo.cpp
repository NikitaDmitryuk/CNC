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

typedef double(*Func)(double);

char dataPort = 0x0;
mutex mtx;
bool cancel = false;
int cancelCounter = 0;
int minTau = 2000;

class Engine
{
public:
    virtual ~Engine(){}
    virtual void updateDataPort()= 0;

    void moveEng(int _step){

    	if (_step != 0){
    		mtx.lock();
        	updateDataPort();
        	outb(dataPortClass, PORT_BASE);
        	dataPort = dataPortClass;
        	mtx.unlock();
        	if(_step > 0){
        		step++;
        	}else{
        		step--;
        	}
    	}
    }

protected:
    int step = 5000;
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
		for(int j = 0; j < fabs(steps[i]); j++){

			if(steps[i] != 0){
				eng->moveEng(steps[i]);
			}
			this_thread::sleep_for(chrono::microseconds(delays[i]));
		}
	}

    if(cancel){
        cancelCounter++;
        return;
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
 		dx = (x[i+1]-x[i]) * scale;
 		dy = (y[i+1]-y[i]) * scale;
 		if(fabs(dy) > fabs(dx)){
 			delaysY.push_back(minTau);
 			stepsY.push_back(round(dy));
 			stepsX.push_back(round(dx));
 			if(dx != 0){
 				delaysX.push_back(round(fabs(dy / dx * minTau)));
 			}else{
 				delaysX.push_back(minTau);
 			}

 		}else{
 			delaysX.push_back(minTau);
 			stepsX.push_back(round(dx));
 			stepsY.push_back(round(dy));

 			if(dy != 0){
 				delaysY.push_back(round(fabs(dx / dy * minTau)));
 			}else{
 				delaysY.push_back(minTau);
 			}
 		}
 	}

 	_stepsX = stepsX;
 	_stepsY = stepsY;
 	_delaysX = delaysX;
 	_delaysY = delaysY;
}

void moveAxes(int _stepsX, int _stepsY){

	Engine *leftEng = new LeftEngine();
    Engine *rightEng = new RightEngine();

    vector<double> x;
	vector<double> y;

    vector<int> stepsX = {_stepsX};
    vector<int> delaysX = {minTau * 2};

    vector<int> stepsY = {_stepsY};
    vector<int> delaysY = {minTau * 2};

	thread th1(loop, leftEng, stepsX, delaysX);
    thread th2(loop, rightEng, stepsY, delaysY);

    th1.join();
    th2.join();

    outb(0, PORT_BASE);

    delete leftEng;
    delete rightEng;
}


double parabl(double x){
	return x * x;
}

double sin4(double x){
	return sin(4*x);
}

double line(double x){
	return x;
}

double zero(double x){
	return 0.0;
}


void setCoord(vector<double> &x, double a, double b, int steps, Func fooX){

	double dx = (b - a) / steps;

	if (dx > 0){
		for(double xt = a; xt < b; xt+=dx){
			x.push_back(fooX(xt));
		}
	}else{
		for(double xt = a; xt > b; xt+=dx){
			x.push_back(fooX(xt));
		}
	}
}


int main(){

	ioperm(PORT_BASE, PORT_SIZE, 1);

	//moveAxes(1000, 0);

    Engine *leftEng = new LeftEngine();
    Engine *rightEng = new RightEngine();

    vector<double> x;
	vector<double> y;

    vector<int> stepsX;
    vector<int> delaysX;

    vector<int> stepsY;
    vector<int> delaysY;

    setCoord(x, 0, 1, 200, &zero);
    setCoord(x, 0, 1, 200, &line);
    // setCoord(x, 0, 1, 200, &zero);
    // setCoord(x, 1, 0, 200, &line);

    setCoord(y, 0, 1, 200, &line);
    setCoord(y, 0, 1, 200, &zero);
    // setCoord(y, 1, 0, 200, &line);
    // setCoord(y, 0, 1, 200, &zero);

    calcWaitTime(x, y, stepsX, delaysX, stepsY, delaysY, 1000);

    int sumTimeX = 0;
    int sumTimeY = 0;
    int sumStepsX = 0;
    int sumStepsY = 0;

    for(int i = 0; i < stepsX.size(); i++){
    	sumTimeX += delaysX[i];
    	sumTimeY += delaysY[i];
    	sumStepsX += 1;
    	sumStepsY += 1;

    	cout << stepsX[i] << "\t" << delaysX[i] << endl;
    	cout << stepsY[i] << "\t" << delaysY[i] << endl;
    }
    // cout << sumStepsX << "\t" << sumStepsY << endl;
    // cout << sumTimeX << "\t" << sumTimeY << endl;

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