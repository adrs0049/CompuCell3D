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

#include <CompuCell3D/Simulator.h>
#include <BasicUtils/debugHelpers.h>
#include "CustomAcceptanceFunction.h"
#include <iostream>
using namespace CompuCell3D;
using namespace std;

double CustomAcceptanceFunction::accept(const double temp, const double change)
{
	int currentWorkNodeNumber=pUtils->getCurrentWorkNodeNumber();	
	ExpressionEvaluator & ev=eed[currentWorkNodeNumber];
	double acceptance=0.0;
	
    DBG_ONLY(
    cerr<<"pUtils="<<pUtils<<endl;
    cerr<<"currentWorkNodeNumber="<<currentWorkNodeNumber<<endl;
    cerr<<"size eed="<<eed.size()<<endl;
    ASSERT_OR_THROW("size of eed has to be greater than currentWorkNodeNumber",eed.size()>currentWorkNodeNumber);
    cerr<<"size ev="<<ev.getNumberOfVars()<<endl;
	cerr<<"temp="<<temp<<endl;
	cerr<<"change="<<change<<endl;
    ASSERT_OR_THROW("number of vars in ev should be 2!\n", ev.getNumberOfVars()==2);
    );

	ev[0]=temp;
	ev[1]=change;
	
	acceptance=ev.eval();
	DBG_ONLY(cerr<<"acceptance="<<acceptance<<endl);

	return acceptance;
}

void CustomAcceptanceFunction::initialize(Simulator *_sim){
	if (eed.size()){//this means initialization already happened
		return;
	}
	simulator=_sim;
	pUtils=simulator->getParallelUtils();
	unsigned int maxNumberOfWorkNodes=pUtils->getMaxNumberOfWorkNodesPotts();
	eed.allocateSize(maxNumberOfWorkNodes);
	vector<string> variableNames;

	variableNames.push_back("T");
	variableNames.push_back("DeltaE");	

	eed.addVariables(variableNames.begin(),variableNames.end());
	eed.initializeUsingParseData();
}

void CustomAcceptanceFunction::update(CC3DXMLElement *_xmlData, bool _fullInitFlag){
	eed.getParseData(_xmlData);
	if (_fullInitFlag){
		eed.initializeUsingParseData();
	}
}
