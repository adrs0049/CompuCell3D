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

#ifndef SURFACEPLUGIN_H
#define SURFACEPLUGIN_H

#include <CompuCell3D/CC3D.h>
#include "SurfaceDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{
class Potts3D;
class CellG;

class SURFACE_EXPORT SurfaceEnergyParam
{
public:
    SurfaceEnergyParam() :targetSurface ( 0.0 ),lambdaSurface ( 0.0 ) {}
    double targetSurface;
    double lambdaSurface;
    std::string typeName;
};

class SURFACE_EXPORT SurfacePlugin : public Plugin , public EnergyFunction
{
    Potts3D *potts;
    CC3DXMLElement *xmlData;
    ParallelUtilsOpenMP *pUtils;
    ExpressionEvaluatorDepot eed;
    bool energyExpressionDefined;
    std::string pluginName;

    BoundaryStrategyPtr boundaryStrategy;
    unsigned int maxNeighborIndex;
    LatticeMultiplicativeFactors lmf;
    WatchableField3D<CellG *> *cellFieldG;

	enum FunctionType {GLOBAL=0,BYCELLTYPE=1,BYCELLID=2};
    FunctionType functionType;

    double targetSurface;
    double lambdaSurface;
    double scaleSurface;

    std::vector<SurfaceEnergyParam> surfaceEnergyParamVector;

    typedef double ( SurfacePlugin::*changeEnergy_t ) ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );

    SurfacePlugin::changeEnergy_t changeEnergyFcnPtr;

    double changeEnergyGlobal ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );
    double changeEnergyByCellType ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );
    double changeEnergyByCellId ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );

    std::pair<double,double> getNewOldSurfaceDiffs ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );
    double diffEnergy ( double lambda, double targetSurface,double surface,  double diff );

public:
    SurfacePlugin()
        : potts ( nullptr ), energyExpressionDefined ( false ), pUtils ( nullptr ),
          pluginName ( "Surface" ), scaleSurface ( 1.0 ),
          boundaryStrategy ( nullptr ) {};
    virtual ~SurfacePlugin();



    // SimObject interface
    virtual void extraInit ( Simulator *simulator ) override;
    virtual void init ( Simulator *simulator,
                        CC3DXMLElement *_xmlData ) override;
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;

    virtual void handleEvent ( CC3DEvent &_event ) override;

    //EnergyFunction interface
    //virtual double localEnergy(const Point3D &pt);
    virtual double changeEnergy ( const Point3D &pt,
                                  const CellG *newCell,
                                  const CellG *oldCell ) override;

    virtual std::string steerableName() override;
    virtual std::string toString() override;
};
} // end namespace
#endif