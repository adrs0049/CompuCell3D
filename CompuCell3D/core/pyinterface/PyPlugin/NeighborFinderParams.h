#ifndef NEIGHBORFINDERPARAMS_H
#define NEIGHBORFINDERPARAMS_H

#include <CompuCell3D/Field3D/Point3D.h>

#include "PythonInclude.h"

namespace CompuCell3D
{

class NeighborFinderParams
{
public:
    NeighborFinderParams()
    {
        pt=Point3D();
        token=0;
        distance=0.0;
        checkBounds=false;
    }

    ~NeighborFinderParams() {}
    void reset()
    {
        pt=Point3D();
        token=0;
        distance=0.0;
        checkBounds=false;
    }


    Point3D pt;
    unsigned int token;
    double distance;
    bool checkBounds;

};

} // end namespace

#endif