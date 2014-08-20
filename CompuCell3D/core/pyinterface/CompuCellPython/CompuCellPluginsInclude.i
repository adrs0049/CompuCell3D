// ************************************************************
// Module Includes
// ************************************************************
// These are copied directly to the .cxx file and are not parsed
// by SWIG.  Include include files or definitions that are required
// for the module to build correctly.
%inline %{
#include <memory>
#include <CompuCell3D/plugins/ConnectivityLocalFlex/ConnectivityLocalFlexData.h>
#include <CompuCell3D/plugins/ConnectivityGlobal/ConnectivityGlobalData.h>
#include <CompuCell3D/plugins/LengthConstraint/LengthConstraintData.h>
#include <CompuCell3D/plugins/ChemotaxisSimple/ChemotaxisSimpleEnergy.h>
#include <CompuCell3D/plugins/Chemotaxis/ChemotaxisData.h>
#include <CompuCell3D/plugins/NeighborTracker/NeighborTracker.h>
#include <CompuCell3D/plugins/PixelTracker/PixelTracker.h>
#include <CompuCell3D/plugins/BoundaryPixelTracker/BoundaryPixelTracker.h>
#include <CompuCell3D/plugins/ContactLocalFlex/ContactLocalFlexData.h>
#include <CompuCell3D/plugins/ContactLocalProduct/ContactLocalProductData.h>
#include <CompuCell3D/plugins/ContactMultiCad/ContactMultiCadData.h>
#include <CompuCell3D/plugins/AdhesionFlex/AdhesionFlexData.h>
#include <CompuCell3D/plugins/PersistentMotion/PersistentMotionData.h>
#include <CompuCell3D/plugins/CellOrientation/CellOrientationVector.h>
#include <CompuCell3D/plugins/PolarizationVector/PolarizationVector.h>
#include <CompuCell3D/plugins/ElasticityTracker/ElasticityTracker.h>
#include <CompuCell3D/plugins/PlasticityTracker/PlasticityTracker.h>
#include <CompuCell3D/plugins/FocalPointPlasticity/FocalPointPlasticityTracker.h>
#include <CompuCell3D/plugins/Secretion/FieldSecretor.h>
#include <CompuCell3D/plugins/ContactOrientation/ContactOrientationData.h>
#include <CompuCell3D/plugins/Polarization23/Polarization23Data.h>
%}

%{
#include <memory>
#include <CompuCell3D/plugins/ConnectivityLocalFlex/ConnectivityLocalFlexData.h>
#include <CompuCell3D/plugins/ConnectivityGlobal/ConnectivityGlobalData.h>
#include <CompuCell3D/plugins/LengthConstraint/LengthConstraintData.h>
#include <CompuCell3D/plugins/ChemotaxisSimple/ChemotaxisSimpleEnergy.h>
#include <CompuCell3D/plugins/Chemotaxis/ChemotaxisData.h>
#include <CompuCell3D/plugins/NeighborTracker/NeighborTracker.h>
#include <CompuCell3D/plugins/PixelTracker/PixelTracker.h>
#include <CompuCell3D/plugins/BoundaryPixelTracker/BoundaryPixelTracker.h>
#include <CompuCell3D/plugins/ContactLocalFlex/ContactLocalFlexData.h>
#include <CompuCell3D/plugins/ContactLocalProduct/ContactLocalProductData.h>
#include <CompuCell3D/plugins/ContactMultiCad/ContactMultiCadData.h>
#include <CompuCell3D/plugins/AdhesionFlex/AdhesionFlexData.h>
#include <CompuCell3D/plugins/PersistentMotion/PersistentMotionData.h>
#include <CompuCell3D/plugins/CellOrientation/CellOrientationVector.h>
#include <CompuCell3D/plugins/PolarizationVector/PolarizationVector.h>
#include <CompuCell3D/plugins/ElasticityTracker/ElasticityTracker.h>
#include <CompuCell3D/plugins/PlasticityTracker/PlasticityTracker.h>
#include <CompuCell3D/plugins/FocalPointPlasticity/FocalPointPlasticityTracker.h>
#include <CompuCell3D/plugins/Secretion/FieldSecretor.h>
#include <CompuCell3D/plugins/ContactOrientation/ContactOrientationData.h>
#include <CompuCell3D/plugins/Polarization23/Polarization23Data.h>
%}