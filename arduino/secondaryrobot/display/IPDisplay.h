#include "../../common/PeriodicProcess.h"


#ifndef IPDISPLAY_H
#define IPDISPLAY_H

class Ipdisplay : public PeriodicProcess {

public:


private:
    virtual void process(float timestep);


};

#endif