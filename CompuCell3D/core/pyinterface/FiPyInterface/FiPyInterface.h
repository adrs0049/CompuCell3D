#ifndef FIPYINTERFACE_H
#define FIPYINTERFACE_H

#include <vector>
#include <PythonInclude.h>
#include <CompuCell3D/Field3D/Field3D.h>

#include "FiPyInterfaceDLLSpecifier.h"

// template <typename Y> class Field3D;
typedef float precision_t;

namespace CompuCell3D{
  class FIPYINTERFACEEXTRACTOR_EXPORT  FiPyInterfaceBase{
    public:
      FiPyInterfaceBase(int _dim);
      ~FiPyInterfaceBase();
      
      void (FiPyInterfaceBase::*dimPtr)(void);
      void test1();
      void test2();
      void fillArray3D(PyObject * _FiPyArray,CompuCell3D::Field3D<float>* _Field);
      void ExampleFunc(PyObject * _FiPyArray,CompuCell3D::Field3D<float>* _Field);
      void doNotDiffuseList(vector<int>& _vec);
      std::vector<std::vector<int> > getDoNoDiffuseVec();
      void test(std::vector<int>& v);
      
      
      std::vector<std::vector<int> > doNotDiffuseVec;
  };
};

#endif