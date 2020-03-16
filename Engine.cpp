//
// Created by nikitalaba on 16.03.2020.
//

#include "Engine.h"

Engine::~Engine() = default;

void Engine::moveEng(int _step){
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