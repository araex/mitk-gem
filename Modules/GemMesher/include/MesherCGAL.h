#pragma once

#include "IMesher.h"

class MesherCGAL : public IMesher
{
protected:
    virtual void compute(void) override;
};