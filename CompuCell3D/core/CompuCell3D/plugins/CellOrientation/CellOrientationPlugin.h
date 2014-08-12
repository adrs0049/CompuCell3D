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

#ifndef CELLORIENTATIONPLUGIN_H
#define CELLORIENTATIONPLUGIN_H

#include <CompuCell3D/CC3D.h>

#include "CellOrientationDLLSpecifier.h"

namespace CompuCell3D {
class CellOrientationEnergy;
class PolarizationVector;
class Point3D;
class Potts3D;
class Simulator;
class LambdaCellOrientation;
class BoundaryStrategy;

template <class T> class Field3D;

class CELLORIENTATION_EXPORT LambdaCellOrientation {
public:
     LambdaCellOrientation() :lambdaVal ( 0.0 ) {}
     double  lambdaVal;
};

class CELLORIENTATION_EXPORT CellOrientationPlugin : public Plugin,public EnergyFunction {
     Field3D<CellG *> *cellFieldG;

     using PolVectorAccessor_t = BasicClassAccessor<PolarizationVector>;
     using DataAccessor_t = BasicClassAccessor<LambdaCellOrientation>;
     DataAccessor_t lambdaCellOrientationAccessor;

     //EnergyFunction data

     Potts3D *potts;
     double lambdaCellOrientation;
     Simulator *simulator;
     Dim3D fieldDim;
     PolVectorAccessor_t *polarizationVectorAccessorPtr;
     BoundaryStrategy *boundaryStrategy;

     bool lambdaFlexFlag;


public:
     CellOrientationPlugin();
     virtual ~CellOrientationPlugin();

     // SimObject interface
     virtual void init(Simulator *simulator,
                       CC3DXMLElement *_xmlData = nullptr) override;
     virtual void extraInit(Simulator *simulator) override;

     typedef double ( CellOrientationPlugin::*changeEnergy_t ) ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );

     CellOrientationPlugin::changeEnergy_t changeEnergyFcnPtr;

     //EnergyFunctionInterface
     virtual double changeEnergy(const Point3D &pt, const CellG *newCell,
                                 const CellG *oldCell) override;
     double changeEnergyCOMBased ( const Point3D &pt,const CellG *newCell,const CellG *oldCell );
     double changeEnergyPixelBased ( const Point3D &pt,const CellG *newCell,const CellG *oldCell );



     PolVectorAccessor_t * getPolarizationVectorAccessorPtr() {
          return polarizationVectorAccessorPtr;
     }
     DataAccessor_t * getLambdaCellOrientationAccessorPtr() {
          return &lambdaCellOrientationAccessor;
     }
     void setLambdaCellOrientation ( CellG * _cell, double _lambda );
     double getLambdaCellOrientation ( CellG * _cell );

     //Steerable interface
     virtual void update(CC3DXMLElement *_xmlData,
                         bool _fullInitFlag = false) override;
     virtual std::string steerableName() override;
     virtual std::string toString() override;
};
};
#endif
