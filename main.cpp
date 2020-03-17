
#include "Printer.h"

char dataPort = 0x0;
mutex mtx;
bool cancel = false;
int cancelCounter = 0;
int minTau = 2000;
unsigned short int PORT_BASE = 0x378;
unsigned long PORT_SIZE = 0x10;

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

int main() {
	
    Printer printer = Printer();

    printer.moveAxes(100, 100);
    printer.setCoord(1000, 0, 2, &sin, 0, 1, &line);
    printer.startPrinting();

    return 0;
}
