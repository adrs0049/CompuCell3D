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

#ifndef DYNAMICACCEPTANCEFUNCTION_H
#define DYNAMICACCEPTANCEFUNCTION_H

#include "AcceptanceFunction.h"
#include <cassert>
#include <ctgmath>

namespace CompuCell3D
{

/**
 * The default Boltzman acceptance function.
 */
class DynamicAcceptanceFunction: public AcceptanceFunction
{
    double k;
    double offset;

public:
    DynamicAcceptanceFunction ( const double _k = 1.0, const double _offset=0.0 ) 
	: k ( _k ), offset ( _offset ) {}
    
    virtual void setOffset ( double _offset ) override
    {
        offset = _offset;
    }
    
    virtual void setK ( double _k ) override
    {
        k = _k;
    }
    
    double accept ( const double temp, const double change ) override
    {
		assert(temp>=0);
		if ( change <= offset ) return tanh(temp);
		return tanh(temp) * exp ( - ( change - offset ) / ( k * temp ) );
    }
};

} // end namespace
#endif