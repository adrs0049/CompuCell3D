

#ifndef FIELDSECRETOR_H
#define FIELDSECRETOR_H

#include <CompuCell3D/CC3D_plugin.h>
#include "SecretionDLLSpecifier.h"

namespace CompuCell3D
{

class CellG;
class Simulator;
class BoundaryPixelTrackerPlugin;
class PixelTrackerPlugin;

template <typename Y> class WatchableField3D;
template <typename Y> class Field3DImpl;

class SECRETION_EXPORT FieldSecretorPixelData
{
public:
    FieldSecretorPixelData()
    {
        pixel=Point3D();
    }
    FieldSecretorPixelData ( Point3D _pixel )
        :pixel ( _pixel )

    {}

    ///have to define < operator if using a class in the set and no < operator is defined for this class
    bool operator< ( const FieldSecretorPixelData & _rhs ) const
    {
        return pixel.x < _rhs.pixel.x || ( ! ( _rhs.pixel.x < pixel.x ) && pixel.y < _rhs.pixel.y )
               || ( ! ( _rhs.pixel.x < pixel.x ) && ! ( _rhs.pixel.y <pixel.y ) && pixel.z < _rhs.pixel.z );
    }

    bool operator== ( const FieldSecretorPixelData & _rhs ) const
    {
        return pixel==_rhs.pixel;
    }

    ///members
    Point3D pixel;
};

class SECRETION_EXPORT  FieldSecretor
{
private:
    double round ( double d )
    {
        return floor ( d + 0.5 );
    }

public:
    FieldSecretor();
    ~FieldSecretor();
    Field3D<float> * concentrationFieldPtr;
    std::shared_ptr<BoundaryPixelTrackerPlugin> boundaryPixelTrackerPlugin;
    std::shared_ptr<PixelTrackerPlugin> pixelTrackerPlugin;
    BoundaryStrategyPtr boundaryStrategy;
    unsigned int maxNeighborIndex;
    cellFieldPtr cellFieldG;

    float getConcentrationOutsideCellAtBoundary ( CellG * _cell );
    float getConcentrationInsideCellAtBoundary ( CellG * _cell );

    //IMPORTANT to handle exceptions properly all _secrete functinos have secrete counterpart defined int he %extend CompuCell3D::Field secretor in the CompuCellExtraDeclarations.i
    bool _secreteInsideCell(CellG * _cell, float _amount);
    bool _secreteInsideCellAtBoundary(CellG * _cell, float _amount);
    bool _secreteInsideCellAtBoundaryOnContactWith(CellG * _cell, float _amount,const std::vector<unsigned char> & _onContactVec);
    bool _secreteOutsideCellAtBoundary(CellG * _cell, float _amount);
    bool _secreteOutsideCellAtBoundaryOnContactWith(CellG * _cell, float _amount,const std::vector<unsigned char> & _onContactVec);
    bool secreteInsideCellAtCOM(CellG * _cell, float _amount);
    
    bool _uptakeInsideCell(CellG * _cell, float _maxUptake, float _relativeUptake);
    bool _uptakeInsideCellAtBoundary(CellG * _cell, float _maxUptake, float _relativeUptake);
    bool _uptakeInsideCellAtBoundaryOnContactWith(CellG * _cell, float _maxUptake, float _relativeUptake,const std::vector<unsigned char> & _onContactVec);
    bool _uptakeOutsideCellAtBoundary(CellG * _cell, float _maxUptake, float _relativeUptake);
    bool _uptakeOutsideCellAtBoundaryOnContactWith(CellG * _cell, float _maxUptake, float _relativeUptake,const std::vector<unsigned char> & _onContactVec);
    bool uptakeInsideCellAtCOM(CellG * _cell, float _maxUptake, float _relativeUptake);
};
} // end namespace
#endif