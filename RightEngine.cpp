//
// Created by nikitalaba on 16.03.2020.
//

#include "RightEngine.h"

RightEngine::~RightEngine() = default;

void RightEngine::updateDataPort() {
    dataPortClass = (dataPort & 0x0F) | ((0x33 >> (step & 3)) & 0xF) << 4;
}
