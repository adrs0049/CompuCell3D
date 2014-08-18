/*************************************************************************
 *    CompuCell - A software framework for multimodel simulations of     *
 * biocomplexity problems Copyright (C) 2003 University of Notre Dame,   *
 *                             Indiana                                   *
 *                                                                       *
 * This program is free software; IF YOU AGREE TO CITE USE OF CompuCell  *
 *  IN ALL RELATED RESEARCH PUBLICATIONS according to the terms of the   *
 *  CompuCell GNU General Public License RIDER you can redistribute it   *
 * and/or modify it under the terms of the GNU General Public License as *
 *  published by the Free Software Foundation; either version 2 of the   *
 *         License, or (at your option) any later version.               *
 *                                                                       *
 * This program is distributed in the hope that it will be useful, but   *
 *      WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *             General Public License for more details.                  *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *     along with this program; if not, write to the Free Software       *
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        *
 *************************************************************************/

#ifndef MITOSISSTEPPABLE_H
#define MITOSISSTEPPABLE_H
#include <CompuCell3D/CC3D.h>
#include "MitosisSteppableDLLSpecifier.h"

namespace CompuCell3D
{
class Potts3D;
class PixelTracker;
class PixelTrackerPlugin;
class CellG;
class PixelTrackerData;

class MITOSISSTEPPABLE_EXPORT SteppableOrientationVectorsMitosis
{
public:
    SteppableOrientationVectorsMitosis() {}

    Vector3 semiminorVec;
    Vector3 semimajorVec;
};

class MITOSISSTEPPABLE_EXPORT CompartmentMitosisData
{
public:
    CompartmentMitosisData() :cell ( 0 ),type ( 0 ) {}
    Vector3 com;//center of mass
    CellG * cell;
    unsigned char type;
    Point3D pt;
};

class MITOSISSTEPPABLE_EXPORT MitosisSteppable : public Steppable
{
    BasicRandomNumberGeneratorNonStatic randGen;
    int parentChildPositionFlag;
    bool tryAdjustingCompartmentCOM ( Vector3 & _com, const std::set<PixelTrackerData> & _clusterPixels );
    double xFactor,yFactor,zFactor;
public:

    typedef bool ( MitosisSteppable::*doDirectionalMitosis2DPtr_t ) ();
    doDirectionalMitosis2DPtr_t doDirectionalMitosis2DPtr;

    CellG * childCell;
    CellG *	parentCell;
    BoundaryStrategyPtr boundaryStrategy;

    int maxNeighborIndex;
    Potts3DPtr potts;

    bool divideAlongMinorAxisFlag;
    bool divideAlongMajorAxisFlag;
    bool flag3D;

    void setParentChildPositionFlag ( int _flag );
    int getParentChildPositionFlag();

    BasicClassAccessor<PixelTracker> *pixelTrackerAccessorPtr;
    std::shared_ptr<PixelTrackerPlugin> pixelTrackerPlugin;
    //comaprtment mitosis members
    std::vector<CompartmentMitosisData> parentBeforeMitosis;
    std::vector<CompartmentMitosisData> comOffsetsMitosis;
    std::vector<CompartmentMitosisData> parentAfterMitosis;
    std::vector<CompartmentMitosisData> childAfterMitosis;

    Point3D boundaryConditionIndicator;
    Dim3D fieldDim;
    // typedef OrientationVectorsMitosis (MitosisSteppable::*getOrientationVectorsMitosis2DPtr_t)(CellG *);
    // getOrientationVectorsMitosis2DPtr_t getOrientationVectorsMitosis2DPtr;

    MitosisSteppable();
    virtual ~MitosisSteppable();
    void init ( Simulator *simulator, CC3DXMLElement *_xmlData=0 );

    // Begin Steppable interface
    virtual void start() {}
    virtual void step ( const unsigned int _currentStep ) {}
    virtual void finish() {}
    // End Steppable interface

    typedef SteppableOrientationVectorsMitosis ( MitosisSteppable::*getOrientationVectorsMitosis2DPtr_t ) ( std::set<PixelTrackerData> & );
    getOrientationVectorsMitosis2DPtr_t getOrientationVectorsMitosis2DPtr;


    SteppableOrientationVectorsMitosis getOrientationVectorsMitosis ( CellG * _cell );
    SteppableOrientationVectorsMitosis getOrientationVectorsMitosisCompartments ( long _clusterId );
    SteppableOrientationVectorsMitosis getOrientationVectorsMitosis ( std::set<PixelTrackerData> & clusterPixels );
    SteppableOrientationVectorsMitosis getOrientationVectorsMitosis2D_xy ( std::set<PixelTrackerData> & clusterPixels );
    SteppableOrientationVectorsMitosis getOrientationVectorsMitosis2D_xz ( std::set<PixelTrackerData> & clusterPixels );
    SteppableOrientationVectorsMitosis getOrientationVectorsMitosis2D_yz ( std::set<PixelTrackerData> & clusterPixels );
    SteppableOrientationVectorsMitosis getOrientationVectorsMitosis3D ( std::set<PixelTrackerData> & clusterPixels );

    bool doDirectionalMitosisOrientationVectorBased ( CellG* _cell,double _nx, double _ny, double _nz );
    bool doDirectionalMitosisAlongMajorAxis ( CellG* _cell );
    bool doDirectionalMitosisAlongMinorAxis ( CellG* _cell );
    bool doDirectionalMitosisRandomOrientation ( CellG* _cell );
    bool doDirectionalMitosisOrientationVectorBasedCompartments ( CellG* _cell,double _nx, double _ny, double _nz );
    bool doDirectionalMitosisOrientationVectorBasedCompartments ( long _clusterId,double _nx, double _ny, double _nz );
    bool doDirectionalMitosisRandomOrientationCompartments ( long _clusterId );
    bool doDirectionalMitosisAlongMajorAxisCompartments ( long _clusterId );
    bool doDirectionalMitosisAlongMinorAxisCompartments ( long _clusterId );

    Vector3 getShiftVector ( std::set<PixelTrackerData> & _sourcePixels );
    Vector3 calculateCOMPixels ( std::set<PixelTrackerData> & _pixels );
    CellG * createChildCell ( std::set<PixelTrackerData> & _pixels );
    void shiftCellPixels ( std::set<PixelTrackerData> & _sourcePixels, std::set<PixelTrackerData> & _targetPixels,Vector3 _shiftVec );

    bool divideClusterPixelsOrientationVectorBased ( std::set<PixelTrackerData> & clusterPixels , std::set<PixelTrackerData> & clusterParent , std::set<PixelTrackerData> & clusterChild, double _nx, double _ny, double _nz );
    Vector3 calculateClusterPixelsCOM ( std::set<PixelTrackerData> & clusterPixels );
    //void initializeClusters(std::vector<int> &originalCompartmentVolumeVec, std::set<PixelTrackerData> & clusterPixels, std::set<PixelTrackerData> &clusterKernels,std::vector<double> & attractionRadiusVec);
    void initializeClusters ( std::vector<int> &originalCompartmentVolumeVec, std::set<PixelTrackerData> & clusterPixels, std::vector<CompartmentMitosisData> &clusterKernels,std::vector<double> & attractionRadiusVec,std::vector<CompartmentMitosisData> & parentBeforeMitosisCMDVec, Vector3 shiftVec );
};
} // end namespace
#endif