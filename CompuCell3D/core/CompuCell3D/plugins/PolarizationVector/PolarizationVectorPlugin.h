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

#ifndef POLARIZATIONVECTORPLUGIN_H
#define POLARIZATIONVECTORPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "PolarizationVector.h"
#include "PolarizationVectorDLLSpecifier.h"

namespace CompuCell3D
{
class CellG;
class POLARIZATIONVECTOR_EXPORT PolarizationVectorPlugin : public Plugin
{
public:
    PolarizationVectorPlugin();
    virtual ~PolarizationVectorPlugin();

    PolVectorAccessor_t getPolarizationVectorAccessorPtr()
    {
        return polarizationVectorAccessor;
    }

    // SimObject interface
    virtual void init ( SimulatorPtr simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( SimulatorPtr simulator ) override;

    void setPolarizationVector ( CellG * _cell, float _x, float _y, float _z );
    std::vector<float> getPolarizationVector ( CellG * _cell );

private:
	PolVectorAccessor_t polarizationVectorAccessor;
};
} // end namespace
#endif