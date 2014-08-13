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

#ifndef ALGORITHMFACTORY_H
#define ALGORITHMFACTORY_H

#include <string>
#include "Algorithm.h"
#include "ChengbangAlgorithm.h"
#include "DefaultAlgorithm.h"
#include <BasicUtils/memory_include.h>

using namespace std;

namespace CompuCell3D
{

/*
 * Factory class for instantiating  boundary conditions for each axis
 */
class AlgorithmFactory
{
public:
    static const string chengbang;
    static const string Default;

    static std::unique_ptr<Algorithm> createAlgorithm ( string algorithm, int index,
                                        int size, string inputfile )
    {
        if ( algorithm == chengbang )
        {
            std::unique_ptr<Algorithm> ca = std::make_unique<ChengbangAlgorithm> ();
            ca->readFile ( index, size, inputfile );
            return ca;
        }
        else
        {
            return std::make_unique<DefaultAlgorithm> ();
        }
    }
};

const string AlgorithmFactory::chengbang ( "Chengbang" );
const string AlgorithmFactory::Default ( "Default" );
	
} // end namespace

#endif