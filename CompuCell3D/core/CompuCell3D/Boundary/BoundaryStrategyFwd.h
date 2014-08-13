#ifndef BOUNDARY_STRATEGY_FWD_H
#define BOUNDARY_STRATEGY_FWD_H

#include <memory>

namespace CompuCell3D {

class BoundaryStrategy;

typedef std::shared_ptr<BoundaryStrategy> BoundaryStrategyPtr;
// ATTENTION SWIG doesn't support these yet
// using BoundaryStrategyPtr = std::shared_ptr<BoundaryStrategy>;
	
} // end namespace

#endif