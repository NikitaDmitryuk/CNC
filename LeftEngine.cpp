//
// Created by nikitalaba on 16.03.2020.
//

#include "LeftEngine.h"


LeftEngine::~LeftEngine() = default;

void LeftEngine::updateDataPort() {
    dataPortClass = (dataPort & 0xF0) | (0x33 >> (step & 3)) & 0xF;
}
