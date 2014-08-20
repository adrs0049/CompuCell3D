// ************************************************************
// Module Includes
// ************************************************************
// These are copied directly to the .cxx file and are not parsed
// by SWIG.  Include include files or definitions that are required
// for the module to build correctly.
%{

// CompuCell3D Include Files
// #include <Potts3D/Cell.h>
#include <CompuCell3D/Plugin.h>
#include <CompuCell3D/Potts3D/Stepper.h>
#include <CompuCell3D/plugins/VolumeTracker/VolumeTrackerPlugin.h> //necesssary to make slicing in cellfield work
#include <CompuCell3D/Steppable.h>
#include <CompuCell3D/Field3D/Neighbor.h>
#include <CompuCell3D/Boundary/BoundaryStrategy.h>
#include <CompuCell3D/Field3D/Field3D.h>
#include <CompuCell3D/Field3D/Field3DImpl.h>
#include <CompuCell3D/Field3D/WatchableField3D.h>
#include <CompuCell3D/ClassRegistry.h>
#include <CompuCell3D/CC3DEvents.h>
#include <CompuCell3D/Simulator.h>

#include <CompuCell3D/PluginManager.h>
#include <CompuCell3D/Potts3D/CellInventory.h>
#include <CompuCell3D/Potts3D/TypeChangeWatcher.h>
#include <CompuCell3D/Potts3D/TypeTransition.h>
#include <CompuCell3D/Potts3D/CellGChangeWatcher.h>

#include <CompuCell3D/Potts3D/Potts3D.h>
//NeighborFinderParams
#include <NeighborFinderParams.h>

// Third Party Libraries
#include <PublicUtilities/NumericalUtils.h>
#include <PublicUtilities/Vector3.h>

#include <BasicUtils/BasicException.h>
#include <BasicUtils/BasicClassFactory.h>
#include <BasicUtils/BasicPluginManager.h>

// System Libraries
#include <iostream>
#include <stdlib.h>

#include "STLPyIterator.h"
#include "STLPyIteratorMap.h"
#include "STLPyIteratorRefRetType.h"

//PyObjectAdapetrs
//EnergyFcns
//#include <CompuCell3D/Potts3D/EnergyFunction.h>
//#include <CompuCell3D/Potts3D/AttributeAdder.h>
#include <PyCompuCellObjAdapter.h>
#include <EnergyFunctionPyWrapper.h>
#include <ChangeWatcherPyWrapper.h>
#include <TypeChangeWatcherPyWrapper.h>
#include <StepperPyWrapper.h>
#include <PyAttributeAdder.h>
#include <CompuCell3D/steppables/PDESolvers/DiffusableVector.h>
#include <CompuCell3D/ParseData.h>
#include <CompuCell3D/ParserStorage.h>

#include <numpy/arrayobject.h>

// Namespaces
using namespace std;
using namespace CompuCell3D;

// CompuCell3D Include Files
#include <CompuCell3D/ClassRegistry.h>
#include <CompuCell3D/Simulator.h>
#include <CompuCell3D/PluginManager.h>
#include <CompuCell3D/Steppable.h>
#include <CompuCell3D/Potts3D/Potts3D.h>
//NeighborFinderParams
#include <NeighborFinderParams.h>
#include <Python.h>
#include <Utils/Coordinates3D.h>
#include <CompuCell3D/Field3D/Array3D.h>

// Third Party Libraries
// System Libraries
#include <iostream>
#include <stdlib.h>

#include <memory>

// Namespaces
using namespace std;
using namespace CompuCell3D;

#define SWIG_EXPORT_ITERATOR_METHODS

%}