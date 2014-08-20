#include "EnergyFunctionCalculatorStatistics.h"
#include "EnergyFunction.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

#include <BasicUtils/BasicString.h>
#include <BasicUtils/BasicException.h>
#include <CompuCell3D/Simulator.h>
#include <PublicUtilities/ParallelUtilsOpenMP.h>
#include <sstream>
#include <CompuCell3D/PottsParseData.h>
#include <XMLUtils/CC3DXMLElement.h>

using namespace CompuCell3D;
using namespace std;

EnergyFunctionCalculatorStatistics::EnergyFunctionCalculatorStatistics(Simulator* _sim, Potts3D* _potts) 
: EnergyFunctionCalculator(_sim, _potts)
{
	DBG_ONLY(cerr<<"Constructing EnergyFunctionCalculatorStatistics"<<endl;);
	
    NTot=0;
    NAcc=0;
    NRej=0;
    lastFlipAttempt=-1;
    out = nullptr;
    outAccSpinFlip = nullptr;
    outRejSpinFlip = nullptr;
    outTotSpinFlip = nullptr;

    wroteHeader=false;
    outputEverySpinFlip=false;
    gatherResultsSpinFlip=false;
    outputAcceptedSpinFlip=false;
    outputRejectedSpinFlip=false;
    outputTotalSpinFlip=false;

    gatherResultsFilesPrepared=false;

    mcs=0;
    fieldWidth=30;
    analysisFrequency=1;
    singleSpinFrequency=1;
}

EnergyFunctionCalculatorStatistics::~EnergyFunctionCalculatorStatistics()
{
    if ( out )
    {
        out->close();
        delete out;
        out = nullptr;
    }

    if ( outAccSpinFlip )
    {
        outAccSpinFlip->close();
        delete outAccSpinFlip;
        outAccSpinFlip = nullptr;
    }

    if ( outRejSpinFlip )
    {
        outRejSpinFlip->close();
        delete outRejSpinFlip;
        outRejSpinFlip = nullptr;
    }

    if ( outTotSpinFlip )
    {
        outTotSpinFlip->close();
        delete outTotSpinFlip;
        outTotSpinFlip = nullptr;
    }
}

double EnergyFunctionCalculatorStatistics::changeEnergy ( Point3D &pt, 
														  const CellG *newCell,
														  const CellG *oldCell,
														  const unsigned int _flipAttempt )
{
    auto pUtils=sim->getParallelUtils();

    if ( pUtils->getNumberOfWorkNodesPotts() ==1 )
    {
        if ( lastFlipAttempt<0 )
        {
            initialize();
        }

        if ( _flipAttempt<lastFlipAttempt )
        {
            ++mcs;
            if ( ! ( mcs % analysisFrequency ) )
            {
                outputResults();
            }
            if ( !gatherResultsSpinFlip && outputEverySpinFlip && ! ( mcs % singleSpinFrequency ) )
            {
                outputResultsSingleSpinFlip();
            }

            if ( gatherResultsSpinFlip && outputEverySpinFlip && ! ( mcs % singleSpinFrequency ) )
            {
                if ( !gatherResultsFilesPrepared )
                {
                    prepareGatherResultsFiles();
                }
                outputResultsSingleSpinFlipGatherResults();
            }

            prepareNextStep();
        }


        lastFlipAttempt=_flipAttempt;
    }
    
    double change {0};
    if ( pUtils->getNumberOfWorkNodesPotts() == 1 )
    {
        for ( unsigned int i = 0; i < energyFunctions.size(); i++ )
        {
            lastEnergyVec[i]=energyFunctions[i]->changeEnergy ( pt, newCell, oldCell );
            change+=lastEnergyVec[i];

        }
        totEnergyDataList.push_back ( lastEnergyVec ); //inserting lastEnergyVec into  totEnergyVecVec - for later stdDev calculations
    }
    else
    {
        for ( auto &elem : energyFunctions )
            change += elem->changeEnergy ( pt, newCell, oldCell );
    }
    return change;
}

//this function will inccrement appropriate energy vectors based on whether flip was accepted or not
void EnergyFunctionCalculatorStatistics::setLastFlipAccepted ( bool _accept )
{
    lastFlipAccepted=_accept;

    if ( lastFlipAccepted )
    {
        accNotAccList.push_back ( true );
    }
    else
    {
        accNotAccList.push_back ( false );
    }
}

void EnergyFunctionCalculatorStatistics::writeHeader()
{

    ( *out ) <<setw ( fieldWidth ) <<"1.STEP"<<setw ( fieldWidth ) <<"2.NAcc"<<setw ( fieldWidth ) <<"3.NRej"<<setw ( fieldWidth ) <<"4.NTot";
    int n=5;
    for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
    {
        ostringstream str1;
        ostringstream str2;
        str1<<n<<".Acc_"<<energyFunctionsNameVec[i]<<"_AVG";
        str2<<n+1<<". +/-";
        ( *out ) <<setw ( fieldWidth ) <<str1.str() <<setw ( fieldWidth ) <<str2.str();
        n+=2;
    }

    for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
    {
        ostringstream str1;
        ostringstream str2;
        str1<<n<<".Rej_"<<energyFunctionsNameVec[i]<<"_AVG";
        str2<<n+1<<". +/-";
        ( *out ) <<setw ( fieldWidth ) <<str1.str() <<setw ( fieldWidth ) <<str2.str();
        n+=2;
    }

    for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
    {
        ostringstream str1;
        ostringstream str2;
        str1<<n<<".Tot_"<<energyFunctionsNameVec[i]<<"_AVG";
        str2<<n+1<<". +/-";
        ( *out ) <<setw ( fieldWidth ) <<str1.str() <<setw ( fieldWidth ) <<str2.str();
        n+=2;
    }

    ( *out ) <<endl;
}

void EnergyFunctionCalculatorStatistics::initialize()
{
    if ( !wroteHeader && out )
    {
        writeHeader();
        wroteHeader=true;
    }
    NAcc=0;
    NRej=0;
    NTot=0;

    int energyFunctionsVecSize=energyFunctions.size();

    lastEnergyVec.assign ( energyFunctionsVecSize,0.0 );
    stdDevEnergyVectorTot.assign ( energyFunctionsVecSize,0.0 );
    stdDevEnergyVectorAcc.assign ( energyFunctionsVecSize,0.0 );
    stdDevEnergyVectorRej.assign ( energyFunctionsVecSize,0.0 );

    //allocating vectors needed for std_dev calculations
    totEnergyDataList.clear();
    accNotAccList.clear();
}

void EnergyFunctionCalculatorStatistics::prepareNextStep()
{
    initialize();
}

void EnergyFunctionCalculatorStatistics::calculateStatData()
{
    const unsigned int energyFunctionCount=energyFunctions.size();

    avgEnergyVectorTot.assign ( energyFunctionCount,0.0 );
    stdDevEnergyVectorTot.assign ( energyFunctionCount,0.0 );

    avgEnergyVectorAcc.assign ( energyFunctionCount,0.0 );
    stdDevEnergyVectorAcc.assign ( energyFunctionCount,0.0 );

    avgEnergyVectorRej.assign ( energyFunctionCount,0.0 );
    stdDevEnergyVectorRej.assign ( energyFunctionCount,0.0 );

    NTot=0;
    NAcc=0;
    NRej=0;

    //calculating averages
    auto lItr=accNotAccList.begin();
	for (const auto& EnergyData : totEnergyDataList)
	{
        ++NTot;
        for ( unsigned int i = 0 ; i < energyFunctionCount ; ++i )
        {
            avgEnergyVectorTot[i]+=EnergyData[i];
        }
        if ( *lItr ) //accepted flip
        {
            ++NAcc;
            for ( unsigned int i = 0 ; i < energyFunctionCount ; ++i )
            {
                avgEnergyVectorAcc[i]+=EnergyData[i];
            }
        }
        else  //rejected flip
        {
            ++NRej;
            for ( unsigned int i = 0 ; i < energyFunctionCount ; ++i )
            {
                avgEnergyVectorRej[i]+=EnergyData[i];
            }
        }
        ++lItr;
    }

    for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
    {
        if ( NTot )
            avgEnergyVectorTot[i]/=NTot;
        if ( NAcc )
            avgEnergyVectorAcc[i]/=NAcc;
        if ( NRej )
            avgEnergyVectorRej[i]/=NRej;
    }

    //calculating stdDev
    stdDevEnergyVectorTot.assign ( energyFunctionCount,0. );
    stdDevEnergyVectorAcc.assign ( energyFunctionCount,0. );
    stdDevEnergyVectorRej.assign ( energyFunctionCount,0. );

    lItr=accNotAccList.begin();
	for (const auto& EnergyData : totEnergyDataList)
	{
        for ( unsigned int i = 0 ; i < energyFunctionCount ; ++i )
        {
            stdDevEnergyVectorTot[i]+= ( EnergyData[i]-avgEnergyVectorTot[i] ) * ( EnergyData[i]-avgEnergyVectorTot[i] );
        }
        if ( *lItr ) //accepted flip
        {
            for ( unsigned int i = 0 ; i < energyFunctionCount ; ++i )
            {
                stdDevEnergyVectorAcc[i]+= ( EnergyData[i]-avgEnergyVectorAcc[i] ) * ( EnergyData[i]-avgEnergyVectorAcc[i] );
            }
        }
        else  //rejected flip
        {
            for ( unsigned int i = 0 ; i < energyFunctionCount ; ++i )
            {
                stdDevEnergyVectorRej[i]+= ( EnergyData[i]-avgEnergyVectorRej[i] ) * ( EnergyData[i]-avgEnergyVectorRej[i] );
            }
        }
        ++lItr;
    }

    // "Normalizing" stdDev
    for ( unsigned int j = 0 ; j < energyFunctions.size() ; ++j )
    {
        if ( NTot )
            stdDevEnergyVectorTot[j]=sqrt ( stdDevEnergyVectorTot[j] ) /NTot;
        if ( NAcc )
            stdDevEnergyVectorAcc[j]=sqrt ( stdDevEnergyVectorAcc[j] ) /NAcc;
        if ( NRej )
            stdDevEnergyVectorRej[j]=sqrt ( stdDevEnergyVectorRej[j] ) /NRej;
    }
}

void EnergyFunctionCalculatorStatistics::writeHeaderFlex ( std::ofstream & _out )
{
    int n=1;

	for (const auto& energyFunctionName : energyFunctionsNameVec)
    {
        ostringstream str1;
        str1<<n<<". "<< energyFunctionName;
        _out<<setw ( fieldWidth ) <<str1.str();
        n+=1;
    }

    _out<<endl;
}

void 
EnergyFunctionCalculatorStatistics::writeDataLineFlex 
( std::ofstream & _out, const std::vector<double> & _energies )
{
    for ( const auto &_energie : _energies )
    {
        _out << setw ( fieldWidth ) << _energie;
    }
    _out<<endl;
}

void EnergyFunctionCalculatorStatistics::prepareGatherResultsFiles()
{
    if ( outputAcceptedSpinFlip )
    {
        ostringstream outSingleSpinFlipStreamNameAccepted;
        outSingleSpinFlipStreamNameAccepted<<outFileCoreNameSpinFlips<<"."<<"accepted"<<"."<<"txt";
        outAccSpinFlip =new ofstream ( outSingleSpinFlipStreamNameAccepted.str().c_str() );

        writeHeaderFlex ( *outAccSpinFlip );
    }

    if ( outputRejectedSpinFlip )
    {
        ostringstream outSingleSpinFlipStreamNameRejected;
        outSingleSpinFlipStreamNameRejected<<outFileCoreNameSpinFlips<<"."<<"rejected"<<"."<<"txt";
        outRejSpinFlip =new ofstream ( outSingleSpinFlipStreamNameRejected.str().c_str() );

        writeHeaderFlex ( *outRejSpinFlip );
    }

    if ( outputTotalSpinFlip )
    {
        ostringstream outSingleSpinFlipStreamNameTotal;
        outSingleSpinFlipStreamNameTotal<<outFileCoreNameSpinFlips<<"."<<"total"<<"."<<"txt";
        outTotSpinFlip =new ofstream ( outSingleSpinFlipStreamNameTotal.str().c_str() );

        writeHeaderFlex ( *outTotSpinFlip );
    }

    gatherResultsFilesPrepared=true;
}

void EnergyFunctionCalculatorStatistics::outputResultsSingleSpinFlipGatherResults()
{
    auto lItr=accNotAccList.begin();
    for (const auto& EnergyData : totEnergyDataList)
	{
        if ( outputTotalSpinFlip )
            writeDataLineFlex ( *outTotSpinFlip, EnergyData );

        if ( *lItr ) //accepted flip
        {
            if ( outputAcceptedSpinFlip )
                writeDataLineFlex ( *outAccSpinFlip, EnergyData );
        }
        else  //rejected flip
        {
            if ( outputRejectedSpinFlip )
                writeDataLineFlex ( *outRejSpinFlip, EnergyData );
        }
        ++lItr;
    }
}

void EnergyFunctionCalculatorStatistics::outputResultsSingleSpinFlip()
{
    ostringstream outSingleSpinFlipStreamNameAccepted;
    outSingleSpinFlipStreamNameAccepted<<outFileCoreNameSpinFlips<<"."<<"accepted"<<"."<<mcs<<"."<<"txt";

    ostringstream outSingleSpinFlipStreamNameRejected;
    outSingleSpinFlipStreamNameRejected<<outFileCoreNameSpinFlips<<"."<<"rejected"<<"."<<mcs<<"."<<"txt";

    ostringstream outSingleSpinFlipStreamNameTotal;
    outSingleSpinFlipStreamNameTotal<<outFileCoreNameSpinFlips<<"."<<"total"<<"."<<mcs<<"."<<"txt";

    ofstream outSingleSpinFlipAccepted ( outSingleSpinFlipStreamNameAccepted.str().c_str() );
    ofstream outSingleSpinFlipRejected ( outSingleSpinFlipStreamNameRejected.str().c_str() );
    ofstream outSingleSpinFlipTotal ( outSingleSpinFlipStreamNameTotal.str().c_str() );

    writeHeaderFlex ( outSingleSpinFlipAccepted );
    writeHeaderFlex ( outSingleSpinFlipRejected );
    writeHeaderFlex ( outSingleSpinFlipTotal );

    auto lItr=accNotAccList.begin();
	for (const auto& EnergyData : totEnergyDataList)
	{
        writeDataLineFlex ( outSingleSpinFlipTotal, EnergyData );
        if ( *lItr ) //accepted flip
        {
            writeDataLineFlex ( outSingleSpinFlipAccepted, EnergyData );
        }
        else  //rejected flip
        {
            writeDataLineFlex ( outSingleSpinFlipRejected, EnergyData );
        }
        ++lItr;
    }
}

void EnergyFunctionCalculatorStatistics::outputResults()
{
    cerr<<"-------------ENERGY CALCULATOR STATISTICS-------------"<<endl;
    cerr<<"Accepted Energy:"<<endl;
    double totAccEnergyChange=0;

    calculateStatData(); //this actually calculates stat data and allocates all necessary vectors

    for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
    {
        cerr<<"TOT "<<energyFunctionsNameVec[i]<<" "<<avgEnergyVectorTot[i]*NTot<<" avg: "<<avgEnergyVectorTot[i]<<" stdDev: "<<stdDevEnergyVectorTot[i]<<endl;
        cerr<<"ACC "<<energyFunctionsNameVec[i]<<" "<<avgEnergyVectorAcc[i]*NAcc<<" avg: "<<avgEnergyVectorAcc[i]<<" stdDev: "<<stdDevEnergyVectorAcc[i]<<endl;
        cerr<<"REJ "<<energyFunctionsNameVec[i]<<" "<<avgEnergyVectorRej[i]*NRej<<" avg: "<<avgEnergyVectorRej[i]<<" stdDev: "<<stdDevEnergyVectorRej[i]<<endl;

        totAccEnergyChange+=avgEnergyVectorAcc[i]*NAcc;
    }

    if ( out )
    {
        ( *out ) <<setw ( fieldWidth ) <<mcs<<setw ( fieldWidth ) <<NAcc<<setw ( fieldWidth ) <<NRej<<setw ( fieldWidth ) <<NTot;
        for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
        {
            ( *out ) <<setw ( fieldWidth ) <<avgEnergyVectorAcc[i]<<setw ( fieldWidth ) <<stdDevEnergyVectorAcc[i];
        }
        for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
        {
            ( *out ) <<setw ( fieldWidth ) <<avgEnergyVectorRej[i]<<setw ( fieldWidth ) <<stdDevEnergyVectorRej[i];
        }
        for ( unsigned int i = 0 ; i < energyFunctions.size() ; ++i )
        {
            ( *out ) <<setw ( fieldWidth ) <<avgEnergyVectorTot[i]<<setw ( fieldWidth ) <<stdDevEnergyVectorTot[i];
        }
        ( *out ) <<endl;

    }

    cerr<<"TOTAL ACC ENERGY CHANGE="<<totAccEnergyChange<<endl;
    cerr<<"-------------End of ENERGY CALCULATOR STATISTICS-------------"<<endl;
    cerr<<"Output File name = "<<outFileName<<endl;

}

void EnergyFunctionCalculatorStatistics::init ( CC3DXMLElement *_xmlData )
{
    outFileName="";
    outFileCoreNameSpinFlips="";
    outputEverySpinFlip=false;
    gatherResultsSpinFlip=false;
    outputAcceptedSpinFlip=false;
    outputRejectedSpinFlip=false;
    outputTotalSpinFlip=false;
    analysisFrequency=1;
    singleSpinFrequency=1;


    CC3DXMLElement * outputFileNameElem=_xmlData->getFirstElement ( "OutputFileName" );
    if ( outputFileNameElem )
    {
        outFileName=outputFileNameElem->getText();
        out=new ofstream ( outFileName.c_str() );
        if ( outputFileNameElem->findAttribute ( "Frequency" ) )
            analysisFrequency=outputFileNameElem->getAttributeAsUInt ( "Frequency" );
    }

    CC3DXMLElement * outputCoreFileNameSpinFlipsElem=_xmlData->getFirstElement ( "OutputCoreFileNameSpinFlips" );

    if ( outputCoreFileNameSpinFlipsElem )
    {
        outFileCoreNameSpinFlips=outputCoreFileNameSpinFlipsElem->getText();
        outputEverySpinFlip=true;
        if ( outputCoreFileNameSpinFlipsElem->findAttribute ( "Frequency" ) )
            singleSpinFrequency=outputCoreFileNameSpinFlipsElem->getAttributeAsUInt ( "Frequency" );
        if ( outputCoreFileNameSpinFlipsElem->findAttribute ( "GatherResults" ) )
            gatherResultsSpinFlip=true;
        if ( outputCoreFileNameSpinFlipsElem->findAttribute ( "OutputAccepted" ) )
            outputAcceptedSpinFlip=true;
        if ( outputCoreFileNameSpinFlipsElem->findAttribute ( "OutputRejected" ) )
            outputRejectedSpinFlip=true;
        if ( outputCoreFileNameSpinFlipsElem->findAttribute ( "OutputTotal" ) )
            outputTotalSpinFlip=true;

    }

    cerr<<"outFileName="<<outFileName<<endl;
    cerr<<"outFileCoreNameSpinFlips="<<outFileCoreNameSpinFlips<<endl;
    cerr<<"outputEverySpinFlip="<<outputEverySpinFlip<<endl;
    cerr<<"gatherResultsSpinFlip="<<gatherResultsSpinFlip<<endl;
    cerr<<"outputAcceptedSpinFlip="<<outputAcceptedSpinFlip<<endl;
    cerr<<"outputRejectedSpinFlip="<<outputRejectedSpinFlip<<endl;
    cerr<<"outputTotalSpinFlip="<<outputTotalSpinFlip<<endl;
    cerr<<"analysisFrequency="<<analysisFrequency<<endl;
    cerr<<"singleSpinFrequency="<<singleSpinFrequency<<endl;
}