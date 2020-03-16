//
// Created by nikitalaba on 16.03.2020.
//

#ifndef CNC_LEFTENGINE_H
#define CNC_LEFTENGINE_H

#include "Engine.h"

class LeftEngine : public Engine{
public:
    ~LeftEngine() override;

    void updateDataPort() override;
};


#endif //CNC_LEFTENGINE_H
