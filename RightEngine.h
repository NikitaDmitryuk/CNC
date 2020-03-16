//
// Created by nikitalaba on 16.03.2020.
//

#ifndef CNC_RIGHTENGINE_H
#define CNC_RIGHTENGINE_H

#include "Engine.h"

class RightEngine : public Engine{
public:
    ~RightEngine() override;

    void updateDataPort() override;
};


#endif //CNC_RIGHTENGINE_H
