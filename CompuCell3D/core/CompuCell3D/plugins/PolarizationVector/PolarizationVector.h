
#ifndef POLARIZATIONVECTOR_H
#define POLARIZATIONVECTOR_H

#include "PolarizationVectorDLLSpecifier.h"
#include <BasicUtils/BasicClassAccessor.h>

namespace CompuCell3D
{
class CellG;

class POLARIZATIONVECTOR_EXPORT PolarizationVector
{
public:

    PolarizationVector ( float _x=0.0 ,float _y=0.0 , float _z=0.0 )
        :x ( _x ),y ( _y ),z ( _z )
    {}
    float x,y,z;
};

// ATTENTION SWIG doesnt know about using yet
//using PolVectorAccessor_t =  BasicClassAccessor<PolarizationVector>;
typedef std::shared_ptr<BasicClassAccessor<PolarizationVector> > PolVectorAccessor_t;

} // end namespace
#endif


