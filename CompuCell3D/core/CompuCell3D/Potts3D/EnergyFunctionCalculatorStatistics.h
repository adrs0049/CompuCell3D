#ifndef ENERGYFUNCTIONCALCLUATORSTATISTICS_H
#define ENERGYFUNCTIONCALCLUATORSTATISTICS_H

#include "EnergyFunctionCalculator.h"
#include <list>
#include <string>
#include <fstream>

namespace CompuCell3D
{

class EnergyFunctionCalculatorStatistics : public EnergyFunctionCalculator
{

public:
    EnergyFunctionCalculatorStatistics();
    virtual void init ( CC3DXMLElement *_xmlData ) override;
    virtual ~EnergyFunctionCalculatorStatistics();
    virtual double changeEnergy ( Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell,
                                  const unsigned int _flipAttempt ) override;

    virtual void setLastFlipAccepted ( bool _accept ) override;
   
private:
    int NTot;
    int NAcc;
    int NRej;
    int lastFlipAttempt;
    std::vector<double> lastEnergyVec;
    std::list<std::vector<double> > totEnergyDataList; // sotres energies for each spin flip attempt
    std::list<bool> accNotAccList; //tells whether entry in totEnergyVecVec is accepted or not

    //stat data vectors
    std::vector<double> avgEnergyVectorTot;
    std::vector<double> stdDevEnergyVectorTot;
    std::vector<double> avgEnergyVectorAcc;
    std::vector<double> stdDevEnergyVectorAcc;
    std::vector<double> avgEnergyVectorRej;
    std::vector<double> stdDevEnergyVectorRej;
    int fieldWidth;
    std::string outFileName;
    std::string outFileCoreNameSpinFlips;
    std::ofstream *out;
    std::ofstream *outAccSpinFlip;
    std::ofstream *outRejSpinFlip;
    std::ofstream *outTotSpinFlip;
    int mcs;
    int analysisFrequency;
    int singleSpinFrequency;

    bool wroteHeader;
    bool outputEverySpinFlip;
    bool gatherResultsSpinFlip;
    bool outputAcceptedSpinFlip;
    bool outputRejectedSpinFlip;
    bool outputTotalSpinFlip;
    bool gatherResultsFilesPrepared;

    void writeHeader();
    void writeHeaderFlex ( std::ofstream & _out );
    void writeDataLineFlex ( std::ofstream & _out, std::vector<double> & _energies );
    void initialize();
    void prepareNextStep();
    void calculateStatData();
    void outputResults();
    void outputResultsSingleSpinFlip();
    void prepareGatherResultsFiles();
    void outputResultsSingleSpinFlipGatherResults();
};

} // end namespace
#endif