//
// Created by nikitalaba on 16.03.2020.
//

#ifndef CNC_PRINTER_H
#define CNC_PRINTER_H

#include "Engine.h"
#include "LeftEngine.h"
#include "RightEngine.h"

typedef double(*Func)(double);

class Printer {
public:
    Printer();
    virtual ~Printer();
    void moveAxes(int _moveX, int _moveY);
    void setCoord(int steps, double x0, double x1, Func fooX, double y0, double y1, Func fooY);
    void startPrinting();

private:
    Engine *leftEng;
    Engine *rightEng;

    vector<double> x;
    vector<double> y;

    vector<int> stepsX;
    vector<int> delaysX;

    vector<int> stepsY;
    vector<int> delaysY;

    static void loop(Engine *eng, vector<int> steps, vector<int> delays);
    void calcWaitTime(int scale);
};


#endif //CNC_PRINTER_H
