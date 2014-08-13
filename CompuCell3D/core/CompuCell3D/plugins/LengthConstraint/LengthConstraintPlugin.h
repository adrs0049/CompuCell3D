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

#ifndef LENGTHCONSTRAINTPLUGIN_H
#define LENGTHCONSTRAINTPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "LengthConstraintData.h"
#include "LengthConstraintDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{
class Potts3D;
class CellG;

class LENGTHCONSTRAINT_EXPORT LengthEnergyParam
{
public:
    LengthEnergyParam() :targetLength ( 0 ),lambdaLength ( 0.0 ),minorTargetLength ( 0.0 ) {}
    LengthEnergyParam ( std::string _cellTypeName,
                        double _targetLength, double _lambdaLength )
        : targetLength ( _targetLength ), lambdaLength ( _lambdaLength ),
          cellTypeName ( std::move ( _cellTypeName ) )
    {}
    
    double targetLength;
	float lambdaLength;
    double minorTargetLength;
    std::string cellTypeName;
};

class LENGTHCONSTRAINT_EXPORT LengthConstraintPlugin : public Plugin,public EnergyFunction
{
public:
	// ATTENTION SWIG doesnt know about using yet
    //using DataAccessor_t = BasicClassAccessor<LengthConstraintData>;
	typedef BasicClassAccessor<LengthConstraintData> DataAccessor_t;
	
private:
    CC3DXMLElement *xmlData;
    Potts3D *potts;
    //energy function parse data
    DataAccessor_t lengthConstraintDataAccessor;

	std::vector<LengthEnergyParam> lengthEnergyParamVector;
    std::vector<std::string> typeNameVec;//temporary vector for storage type names
    BoundaryStrategyPtr boundaryStrategy;

public:
    typedef double ( LengthConstraintPlugin::*changeEnergyFcnPtr_t ) ( const Point3D &pt, const CellG *newCell,
            const CellG *oldCell );

    LengthConstraintPlugin();
    virtual ~LengthConstraintPlugin();

    // Plugin interface
    virtual void init ( Simulator *simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( Simulator *simulator ) override;
    virtual std::string toString() override;

    DataAccessor_t * getLengthConstraintDataPtr()
    {
        return & lengthConstraintDataAccessor;
    }

    void setLengthConstraintData ( CellG * _cell, double _lambdaLength=0.0, double _targetLength=0.0 ,double _minorTargetLength=0.0 );
    double getLambdaLength ( CellG * _cell );
    double getTargetLength ( CellG * _cell );
    double getMinorTargetLength ( CellG * _cell );

    //EnergyFunction interface
    virtual double changeEnergy ( const Point3D &pt,
                                  const CellG *newCell,
                                  const CellG *oldCell ) override;

    double changeEnergy_xz ( const Point3D &pt,const CellG *newCell,const CellG *oldCell );
    double changeEnergy_xy ( const Point3D &pt,const CellG *newCell,const CellG *oldCell );
    double changeEnergy_yz ( const Point3D &pt,const CellG *newCell,const CellG *oldCell );
    double changeEnergy_3D ( const Point3D &pt,const CellG *newCell,const CellG *oldCell );

    changeEnergyFcnPtr_t changeEnergyFcnPtr;

    void initTypeId ( Potts3D * potts );

    //SteerableObject interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
};

} // end namespace
#endif
