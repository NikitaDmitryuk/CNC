//
// Created by nikitalaba on 16.03.2020.
//

#include "Printer.h"

Printer::Printer() {
    ioperm(PORT_BASE, PORT_SIZE, 1);
    leftEng = new LeftEngine();
    rightEng = new RightEngine();
}

Printer::~Printer() {
    delete leftEng;
    delete rightEng;
    outb(0, PORT_BASE);
}

void Printer::moveAxes(int _moveX, int _moveY) {
    vector<double> _x;
    vector<double> _y;

    vector<int> _stepsX = {_moveX};
    vector<int> _delaysX = {minTau * 2};

    vector<int> _stepsY = {_moveY};
    vector<int> _delaysY = {minTau * 2};

    thread th1(loop, leftEng, _stepsX, _delaysX);
    thread th2(loop, rightEng, _stepsY, _delaysY);

    th1.join();
    th2.join();

    outb(0, PORT_BASE);
}

void Printer::loop(Engine *eng, vector<int> steps, vector<int> delays) {

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

void Printer::calcWaitTime(int scale) {
    double dx;
    double dy;

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
}

void Printer::setCoord(int steps, double x0, double x1, Func fooX, double y0, double y1, Func fooY) {
    double dx = (x1 - x0) / steps;
    double dy = (y1 - y0) / steps;

    if (dx > 0){
        for(double xt = x0; xt < x1; xt+=dx){
            x.push_back(fooX(xt));
        }
    }else{
        for(double xt = x1; xt > x0; xt-=dx){
            x.push_back(fooX(xt));
        }
    }

    if (dy > 0){
        for(double xt = y0; xt < y1; xt+=dy){
            y.push_back(fooY(xt));
        }
    }else{
        for(double xt = y1; xt > y0; xt-=dy){
            y.push_back(fooY(xt));
        }
    }
}

void Printer::startPrinting() {
    calcWaitTime(100);
    thread th1(loop, leftEng, stepsX, delaysX);
    thread th2(loop, rightEng, stepsY, delaysY);

    th1.join();
    th2.join();

    outb(0, PORT_BASE);
}
