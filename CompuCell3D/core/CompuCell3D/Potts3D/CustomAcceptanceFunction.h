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

#ifndef CUSTOMACCEPTANCEFUNCTION_H
#define CUSTOMACCEPTANCEFUNCTION_H

#include <PublicUtilities/ParallelUtilsOpenMP.h>
#include <muParser/ExpressionEvaluator/ExpressionEvaluator.h>
#include "AcceptanceFunction.h"
#include <math.h>

class CC3DXMLElement;

namespace CompuCell3D
{

class ParallelUtilsOpenMP;
class Simulator;

class CustomAcceptanceFunction: public AcceptanceFunction
{
    ExpressionEvaluatorDepot eed;
    Simulator *simulator;
    ParallelUtilsOpenMP * pUtils;

public:
    CustomAcceptanceFunction() : simulator ( nullptr ), pUtils ( nullptr ) {}
    
    //AcceptanceFunction interface
    virtual double accept ( const double temp, const double change ) override;
    virtual void setOffset ( double _offset ) override {};
    virtual void setK ( double _k ) override {};

    virtual void initialize ( Simulator *_sim ) override;
    virtual void update ( CC3DXMLElement *_xmlData, bool _fullInitFlag=false ) override;
};

} // end namespace
#endif