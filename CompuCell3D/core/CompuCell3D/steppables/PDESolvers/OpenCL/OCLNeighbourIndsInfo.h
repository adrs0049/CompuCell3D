#ifndef OCL_NEIGHBOUR_INDS_INFO_H
#define OCL_NEIGHBOUR_INDS_INFO_H

namespace CompuCell3D{

struct OCLNeighbourIndsInfo{
	std::vector<cl_int4> mh_nbhdDiffShifts;
	std::vector<cl_int4> mh_nbhdConcShifts;

	cl_int m_nbhdConcLen;
	cl_int m_nbhdDiffLen;

	inline
	static OCLNeighbourIndsInfo Init(LatticeType lt, Dim3D dim, BoundaryStrategyPtr bs, 
		std::vector<std::vector<Point3D> > const &hexOffsetArray,
		std::vector<Point3D> const &offsetVecCartesian);
};


OCLNeighbourIndsInfo OCLNeighbourIndsInfo::Init(LatticeType latticeType, Dim3D dim, BoundaryStrategyPtr bs,
		std::vector<std::vector<Point3D> > const &hexOffsetArray,
		std::vector<Point3D> const &offsetVecCartesian)
{
	int layers;

	OCLNeighbourIndsInfo res;

	if(latticeType==HEXAGONAL_LATTICE){
		cerr<<"Hexagonal lattice used"<<endl;
		if(dim.z==1){
            cerr<<"setting res.m_nbhdConcLen=6"<<endl;
			res.m_nbhdConcLen=6;
			res.m_nbhdDiffLen=3;
			layers=2;
		}else{
            cerr<<"setting res.m_nbhdConcLen=12"<<endl;
			res.m_nbhdConcLen=12;
			res.m_nbhdDiffLen=6;
			layers=6;
		}
	}else{
		cerr<<"Cartesian lattice used"<<endl;
		if(dim.z==1){
			res.m_nbhdConcLen=4;
			res.m_nbhdDiffLen=2;
		}else{
			res.m_nbhdConcLen=6;
			res.m_nbhdDiffLen=3;
		}
	}

	if(latticeType==HEXAGONAL_LATTICE)
	{
		cerr<<"fieldDim.z="<<dim.z<<endl;

		res.mh_nbhdDiffShifts.resize(layers*res.m_nbhdDiffLen);
		res.mh_nbhdConcShifts.resize(layers*res.m_nbhdConcLen);

		if(dim.z!=1 || dim.z==1){
					
			std::vector<std::vector<Point3D> > bhoa;
			bs->getHexOffsetArray(bhoa);
			for(int i=0; i<layers; ++i){
				int offset=res.m_nbhdDiffLen*i;
				for(size_t j=0; j<hexOffsetArray[i].size(); ++j)
				{
					ASSERT_OR_THROW("wrong index 1", (unsigned int)i<hexOffsetArray.size());
					ASSERT_OR_THROW("wrong index 2", (unsigned int)j<hexOffsetArray[i].size());
					cl_int4 shift={hexOffsetArray[i][j].x, hexOffsetArray[i][j].y, hexOffsetArray[i][j].z};

					ASSERT_OR_THROW("wrong index", (offset+j<res.mh_nbhdDiffShifts.size()));
					res.mh_nbhdDiffShifts[offset+j]=shift;
				}
			}
		
			//cerr<<"bhoa.size()="<<bhoa.size()<<endl;
			//cerr<<"bndMaxOffset="<<getBoundaryStrategy()->getMaxOffset()<<endl;
	
			for(int i=0; i<layers; ++i){
				int offset=res.m_nbhdConcLen*i;
				for(int j=0; j<bs->getMaxOffset(); ++j)
				{
					ASSERT_OR_THROW("wrong index 1", (size_t)i<bhoa.size());
					ASSERT_OR_THROW("wrong index 2", (size_t)j<bhoa[i].size());
					cl_int4 shift={bhoa[i][j].x, bhoa[i][j].y, bhoa[i][j].z};
					ASSERT_OR_THROW("wrong index", ((size_t)(offset+j)<res.mh_nbhdConcShifts.size()));
					res.mh_nbhdConcShifts[offset+j]=shift;
				}
			}
		}
	}//if(latticeType==HEXAGONAL_LATTICE)
	else{
		//std::cerr<<"resizing here to "<<res.m_nbhdConcLen<<std::endl;
		res.mh_nbhdDiffShifts.resize(res.m_nbhdDiffLen);
		res.mh_nbhdConcShifts.resize(res.m_nbhdConcLen);

		if(dim.z==1){
			res.mh_nbhdConcShifts[0].s[0]=1;  res.mh_nbhdConcShifts[0].s[1]=0;  res.mh_nbhdConcShifts[0].s[2]=0; res.mh_nbhdConcShifts[0].s[3]=0;
			res.mh_nbhdConcShifts[1].s[0]=0;  res.mh_nbhdConcShifts[1].s[1]=1;  res.mh_nbhdConcShifts[1].s[2]=0; res.mh_nbhdConcShifts[1].s[3]=0;
			res.mh_nbhdConcShifts[2].s[0]=-1; res.mh_nbhdConcShifts[2].s[1]=0;  res.mh_nbhdConcShifts[2].s[2]=0; res.mh_nbhdConcShifts[2].s[3]=0;
			res.mh_nbhdConcShifts[3].s[0]=0;  res.mh_nbhdConcShifts[3].s[1]=-1; res.mh_nbhdConcShifts[3].s[2]=0; res.mh_nbhdConcShifts[3].s[3]=0;
					
			res.mh_nbhdDiffShifts[0].s[0]=1;  res.mh_nbhdDiffShifts[0].s[1]=0;  res.mh_nbhdDiffShifts[0].s[2]=0; res.mh_nbhdDiffShifts[0].s[3]=0;
			res.mh_nbhdDiffShifts[1].s[0]=0;  res.mh_nbhdDiffShifts[1].s[1]=1;  res.mh_nbhdDiffShifts[1].s[2]=0; res.mh_nbhdDiffShifts[1].s[3]=0;
		}
		else{

			res.mh_nbhdConcShifts[0].s[0]=1;  res.mh_nbhdConcShifts[0].s[1]=0;  res.mh_nbhdConcShifts[0].s[2]=0;
			res.mh_nbhdConcShifts[1].s[0]=0;  res.mh_nbhdConcShifts[1].s[1]=1;  res.mh_nbhdConcShifts[1].s[2]=0;
			res.mh_nbhdConcShifts[2].s[0]=0;  res.mh_nbhdConcShifts[2].s[1]=0;  res.mh_nbhdConcShifts[2].s[2]=1;
			res.mh_nbhdConcShifts[3].s[0]=-1; res.mh_nbhdConcShifts[3].s[1]=0;  res.mh_nbhdConcShifts[3].s[2]=0;
			res.mh_nbhdConcShifts[4].s[0]=0;  res.mh_nbhdConcShifts[4].s[1]=-1; res.mh_nbhdConcShifts[4].s[2]=0;
			res.mh_nbhdConcShifts[5].s[0]=0;  res.mh_nbhdConcShifts[5].s[1]=0;  res.mh_nbhdConcShifts[5].s[2]=-1;
		
			res.mh_nbhdDiffShifts[0].s[0]=1;  res.mh_nbhdDiffShifts[0].s[1]=0;  res.mh_nbhdDiffShifts[0].s[2]=0;
			res.mh_nbhdDiffShifts[1].s[0]=0;  res.mh_nbhdDiffShifts[1].s[1]=1;  res.mh_nbhdDiffShifts[1].s[2]=0;
			res.mh_nbhdDiffShifts[2].s[0]=0;  res.mh_nbhdDiffShifts[2].s[1]=0;  res.mh_nbhdDiffShifts[2].s[2]=1;
		}
	}

	return res;
}

}//namespace OCLNeighbourIndsInfo

#endif