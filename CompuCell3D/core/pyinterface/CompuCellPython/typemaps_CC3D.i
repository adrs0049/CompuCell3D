// typemap(in) CompuCell3D::Dim3D will not overshadow earlier default conversion of list to std::vector
%typemap(in) CompuCell3D::Dim3D (CompuCell3D::Dim3D dim)  {
  /* Check if is a list */
    if (PyList_Check($input)) {
        int size = PyList_Size($input);        
        if (size==3){
            
            dim.x= (short)PyInt_AsLong(PyList_GetItem($input,0));
            dim.y=(short)PyInt_AsLong(PyList_GetItem($input,1));
            dim.z=(short)PyInt_AsLong(PyList_GetItem($input,2));
            $1=dim;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case

        }

    }else if (PyTuple_Check($input)){
        //check if it is a tuple
        int size = PyTuple_Size($input);        
        if (size==3){
            
            dim.x= (short)PyInt_AsLong(PyTuple_GetItem($input,0));
            dim.y=(short)PyInt_AsLong(PyTuple_GetItem($input,1));
            dim.z=(short)PyInt_AsLong(PyTuple_GetItem($input,2));
            $1=dim;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
        }                
    }else{
        
         int res = SWIG_ConvertPtr($input,(void **) &$1, $&1_descriptor,0);
         
         
        if (SWIG_IsOK(res)) {
            //Notice that we have to call PyDECREF on return value from PyObject_GetAttrString otherwise we leak memory!    
            PyObject * xPyRef=PyObject_GetAttrString($input,"x"); dim.x=(short)PyInt_AsLong(xPyRef); Py_DECREF(xPyRef);
            PyObject * yPyRef=PyObject_GetAttrString($input,"y"); dim.y=(short)PyInt_AsLong(yPyRef); Py_DECREF(yPyRef);
            PyObject * zPyRef=PyObject_GetAttrString($input,"z"); dim.z=(short)PyInt_AsLong(zPyRef); Py_DECREF(zPyRef);    
            $1=dim;
        } else {
        
            SWIG_exception(SWIG_ValueError,"Expected CompuCell.Dim3D object."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
            
          
        }
         
    }
}

%typemap(in) CompuCell3D::Dim3D &  (CompuCell3D::Dim3D dim)  { // note that (CompuCell3D::Point3D pt) causes pt to be allocated on the stack - no need to worry abuot freeing memory
  /* Check if is a list */
  // cerr<<"inside Dim3D conversion typemap"<<endl;
    if (PyList_Check($input)) {
        // cerr<<"GOT LIST"<<endl;
        int size = PyList_Size($input);        
        if (size==3){
            
            dim.x= (short)PyInt_AsLong(PyList_GetItem($input,0));
            dim.y=(short)PyInt_AsLong(PyList_GetItem($input,1));
            dim.z=(short)PyInt_AsLong(PyList_GetItem($input,2));
            $1=&dim;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case

        }

    }else if (PyTuple_Check($input)){
        //check if it is a tuple
        int size = PyTuple_Size($input);        
        // cerr<<"GOT TUPLE"<<endl;
        if (size==3){            
            dim.x= (short)PyInt_AsLong(PyTuple_GetItem($input,0));
            dim.y=(short)PyInt_AsLong(PyTuple_GetItem($input,1));
            dim.z=(short)PyInt_AsLong(PyTuple_GetItem($input,2));
            $1=&dim;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
        }                
    }else{
        // cerr<<" CHECKING FOR DIM3D"<<endl;
         int res = SWIG_ConvertPtr($input,(void **) &$1, $1_descriptor,0);
         
         
        if (SWIG_IsOK(res)) {            
            //Notice that we have to call PyDECREF on return value from PyObject_GetAttrString otherwise we leak memory!    
            PyObject * xPyRef=PyObject_GetAttrString($input,"x"); dim.x=(short)PyInt_AsLong(xPyRef); Py_DECREF(xPyRef);
            PyObject * yPyRef=PyObject_GetAttrString($input,"y"); dim.y=(short)PyInt_AsLong(yPyRef); Py_DECREF(yPyRef);
            PyObject * zPyRef=PyObject_GetAttrString($input,"z"); dim.z=(short)PyInt_AsLong(zPyRef); Py_DECREF(zPyRef);    
            $1=&dim;
        } else {
        
            SWIG_exception(SWIG_ValueError,"Expected CompuCell.Dim3D object."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
                      
        }
         
    }
}

// we dont really need this
// %typemap(in) CompuCell3D::Dim3D *  (CompuCell3D::Dim3D dim) = CompuCell3D::Dim3D &  (CompuCell3D::Dim3D dim); 



%typemap(in) CompuCell3D::Point3D  (CompuCell3D::Point3D pt)  {
  /* Check if is a list */
//     cerr<<"inside point3D conversion typemap"<<endl;
    if (PyList_Check($input)) {
        int size = PyList_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            pt.x= (short)PyInt_AsLong(PyList_GetItem($input,0));
            pt.y=(short)PyInt_AsLong(PyList_GetItem($input,1));
            pt.z=(short)PyInt_AsLong(PyList_GetItem($input,2));
            $1=pt;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case

        }

    }else if (PyTuple_Check($input)){
        //check if it is a tuple
        int size = PyTuple_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            pt.x= (short)PyInt_AsLong(PyTuple_GetItem($input,0));
            pt.y=(short)PyInt_AsLong(PyTuple_GetItem($input,1));
            pt.z=(short)PyInt_AsLong(PyTuple_GetItem($input,2));
            $1=pt;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
        }                
    }else if(PyArray_Check($input)){
    
        if (PyArray_NDIM($input)!=1 || PyArray_DIM($input,0)!=3){ // checking if the argument is a vector with 3 values
            SWIG_exception(SWIG_ValueError,"Expected a list/numpy array of 3 double values e.g. [12,31,48].");
        }
        
        if (PyArray_ISFLOAT ($input)){
            double * arrayContainerPtr=(double *) PyArray_DATA($input);
            pt.x=(short)arrayContainerPtr[0];
            pt.y=(short)arrayContainerPtr[1];
            pt.z=(short)arrayContainerPtr[2];
        
            
        }  else  if (PyArray_ISINTEGER ($input)){

            int * arrayContainerPtr=(int *) PyArray_DATA($input);
            pt.x=(short)arrayContainerPtr[0];
            pt.y=(short)arrayContainerPtr[1];
            pt.z=(short)arrayContainerPtr[2];

        
        }else{
            SWIG_exception(SWIG_ValueError,"The values in the array should be either floating point numbers or inttegers. Please use explicit type conversion for all the values");
        }
        
        $1=pt;        
    }else{
        
         int res = SWIG_ConvertPtr($input,(void **) &$1, $&1_descriptor,0);
         
         
        if (SWIG_IsOK(res)) {
            //Notice that we have to call PyDECREF on return value from PyObject_GetAttrString otherwise we leak memory!    
            PyObject * xPyRef=PyObject_GetAttrString($input,"x"); pt.x=(short)PyInt_AsLong(xPyRef); Py_DECREF(xPyRef);
            PyObject * yPyRef=PyObject_GetAttrString($input,"y"); pt.y=(short)PyInt_AsLong(yPyRef); Py_DECREF(yPyRef);
            PyObject * zPyRef=PyObject_GetAttrString($input,"z"); pt.z=(short)PyInt_AsLong(zPyRef); Py_DECREF(zPyRef);    
            $1=pt;
        } else {
        
            SWIG_exception(SWIG_ValueError,"Expected CompuCell.Point3D object."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
                      
        }
         
    }
}


%typemap(in) CompuCell3D::Point3D &  (CompuCell3D::Point3D pt)  { // note that (CompuCell3D::Point3D pt) causes pt to be allocated on the stack - no need to worry abuot freeing memory
  /* Check if is a list */
//     cerr<<"inside point3D conversion typemap"<<endl;
    
    if (PyList_Check($input)) {
        int size = PyList_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            pt.x= (short)PyInt_AsLong(PyList_GetItem($input,0));
            pt.y=(short)PyInt_AsLong(PyList_GetItem($input,1));
            pt.z=(short)PyInt_AsLong(PyList_GetItem($input,2));
            $1=&pt;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case

        }

    }else if (PyTuple_Check($input)){
        //check if it is a tuple
        int size = PyTuple_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            pt.x= (short)PyInt_AsLong(PyTuple_GetItem($input,0));
            pt.y=(short)PyInt_AsLong(PyTuple_GetItem($input,1));
            pt.z=(short)PyInt_AsLong(PyTuple_GetItem($input,2));
            $1=&pt;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 integer values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
        }                
    }else if(PyArray_Check($input)){
    
        if (PyArray_NDIM($input)!=1 || PyArray_DIM($input,0)!=3){ // checking if the argument is a vector with 3 values
            SWIG_exception(SWIG_ValueError,"Expected a list/numpy array of 3 double values e.g. [12,31,48].");
        }
        
        if (PyArray_ISFLOAT ($input)){
            double * arrayContainerPtr=(double *) PyArray_DATA($input);
            pt.x=(short)arrayContainerPtr[0];
            pt.y=(short)arrayContainerPtr[1];
            pt.z=(short)arrayContainerPtr[2];
        
            
        }  else  if (PyArray_ISINTEGER ($input)){

            int * arrayContainerPtr=(int *) PyArray_DATA($input);
            pt.x=(short)arrayContainerPtr[0];
            pt.y=(short)arrayContainerPtr[1];
            pt.z=(short)arrayContainerPtr[2];

        
        }else{
            SWIG_exception(SWIG_ValueError,"The values in the array should be either floating point numbers or inttegers. Please use explicit type conversion for all the values");
        }
        
        $1=&pt;        
    }else{
        
         int res = SWIG_ConvertPtr($input,(void **) &$1, $1_descriptor,0);
         
         
        if (SWIG_IsOK(res)) {
            //Notice that we have to call PyDECREF on return value from PyObject_GetAttrString otherwise we leak memory!    
            PyObject * xPyRef=PyObject_GetAttrString($input,"x"); pt.x=(short)PyInt_AsLong(xPyRef); Py_DECREF(xPyRef);
            PyObject * yPyRef=PyObject_GetAttrString($input,"y"); pt.y=(short)PyInt_AsLong(yPyRef); Py_DECREF(yPyRef);
            PyObject * zPyRef=PyObject_GetAttrString($input,"z"); pt.z=(short)PyInt_AsLong(zPyRef); Py_DECREF(zPyRef);    
            $1=&pt;
        } else {
        
            SWIG_exception(SWIG_ValueError,"Expected CompuCell.Point3D object."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
                      
        }
         
    }
}

// we dont really need this
// %typemap(in) CompuCell3D::Point3D *  (CompuCell3D::Point3D pt) = CompuCell3D::Point3D &  (CompuCell3D::Point3D pt);



%typemap(in) Coordinates3D<double>  (Coordinates3D<double> coord)  {
  /* Check if is a list */  
    if (PyList_Check($input)) {
        int size = PyList_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            coord.x= (double)PyFloat_AsDouble(PyList_GetItem($input,0));
            coord.y=(double)PyFloat_AsDouble(PyList_GetItem($input,1));
            coord.z=(double)PyFloat_AsDouble(PyList_GetItem($input,2));
            $1=coord;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list/numpy array of 3 double values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case

        }

    }else if (PyTuple_Check($input)){
        //check if it is a tuple
        int size = PyTuple_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            coord.x= (double)PyFloat_AsDouble(PyTuple_GetItem($input,0));
            coord.y=(double)PyFloat_AsDouble(PyTuple_GetItem($input,1));
            coord.z=(double)PyFloat_AsDouble(PyTuple_GetItem($input,2));
            $1=coord;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list/numpy array of 3 double values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
        }                
    }else if(PyArray_Check($input)){
    
        if (PyArray_NDIM($input)!=1 || PyArray_DIM($input,0)!=3){ // checking if the argument is a vector with 3 values
            SWIG_exception(SWIG_ValueError,"Expected a list/numpy array of 3 double values e.g. [12,31,48].");
        }
        
        if (PyArray_ISFLOAT ($input)){
            double * arrayContainerPtr=(double *) PyArray_DATA($input);
            coord.x=arrayContainerPtr[0];
            coord.y=arrayContainerPtr[1];
            coord.z=arrayContainerPtr[2];
        
            
        }  else  if (PyArray_ISINTEGER ($input)){

            int * arrayContainerPtr=(int *) PyArray_DATA($input);
            coord.x=arrayContainerPtr[0];
            coord.y=arrayContainerPtr[1];
            coord.z=arrayContainerPtr[2];

        
        }else{
            SWIG_exception(SWIG_ValueError,"The values in the array should be either floating point numbers or inttegers. Please use explicit type conversion for all the values");
        }
        
        
        $1=coord;        
    }
    else{
        
         int res = SWIG_ConvertPtr($input,(void **) &$1, $&1_descriptor,0);
         
         
        if (SWIG_IsOK(res)) {
            //Notice that we have to call PyDECREF on return value from PyObject_GetAttrString otherwise we leak memory!    
            PyObject * xPyRef=PyObject_GetAttrString($input,"x"); coord.x=(double)PyFloat_AsDouble(xPyRef); Py_DECREF(xPyRef);
            PyObject * yPyRef=PyObject_GetAttrString($input,"y"); coord.y=(double)PyFloat_AsDouble(yPyRef); Py_DECREF(yPyRef);
            PyObject * zPyRef=PyObject_GetAttrString($input,"z"); coord.z=(double)PyFloat_AsDouble(zPyRef); Py_DECREF(zPyRef);    
            $1=coord;
        } else {
        
            SWIG_exception(SWIG_ValueError,"Expected CompuCell.Coordinates3DDouble object."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
                      
        }
         
    }
}


%typemap(in) Coordinates3D<double> &  (Coordinates3D<double> coord)  { // note that (CompuCell3D::Point3D pt) causes pt to be allocated on the stack - no need to worry abuot freeing memory
  /* Check if is a list */  
    if (PyList_Check($input)) {
        int size = PyList_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            coord.x= (double)PyFloat_AsDouble(PyList_GetItem($input,0));
            coord.y=(double)PyFloat_AsDouble(PyList_GetItem($input,1));
            coord.z=(double)PyFloat_AsDouble(PyList_GetItem($input,2));
            $1=&coord;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 double values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case

        }

    }else if (PyTuple_Check($input)){
        //check if it is a tuple
        int size = PyTuple_Size($input);        
        if (size==3){
            // CompuCell3D::Point3D pt;    
            coord.x= (double)PyFloat_AsDouble(PyTuple_GetItem($input,0));
            coord.y=(double)PyFloat_AsDouble(PyTuple_GetItem($input,1));
            coord.z=(double)PyFloat_AsDouble(PyTuple_GetItem($input,2));
            $1=&coord;
        }else{
            SWIG_exception(SWIG_ValueError,"Expected a list of 3 double values e.g. [12,31,48]."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
        }                
    }else if(PyArray_Check($input)){
    
        if (PyArray_NDIM($input)!=1 || PyArray_DIM($input,0)!=3){ // checking if the argument is a vector with 3 values
            SWIG_exception(SWIG_ValueError,"Expected a list/numpy array of 3 double values e.g. [12,31,48].");
        }
        
        if (! PyArray_ISFLOAT ($input)){
            SWIG_exception(SWIG_ValueError,"The values in the array appear not to be floating point numbers. Please use explicit casting to double for all the values");
        }        
        
        double * arrayContainerPtr=(double *) PyArray_DATA($input);
        coord.x=arrayContainerPtr[0];
        coord.y=arrayContainerPtr[1];
        coord.z=arrayContainerPtr[2];
        
        
        $1=&coord;        
    }else{
        
         int res = SWIG_ConvertPtr($input,(void **) &$1, $1_descriptor,0);
         
         
        if (SWIG_IsOK(res)) {
            //Notice that we have to call PyDECREF on return value from PyObject_GetAttrString otherwise we leak memory!    
            PyObject * xPyRef=PyObject_GetAttrString($input,"x"); coord.x=(double)PyFloat_AsDouble(xPyRef); Py_DECREF(xPyRef);
            PyObject * yPyRef=PyObject_GetAttrString($input,"y"); coord.y=(double)PyFloat_AsDouble(yPyRef); Py_DECREF(yPyRef);
            PyObject * zPyRef=PyObject_GetAttrString($input,"z"); coord.z=(double)PyFloat_AsDouble(zPyRef); Py_DECREF(zPyRef);    
            $1=&coord;
        } else {
        
            SWIG_exception(SWIG_ValueError,"Expected CompuCell.Coordinates3DDouble object."); //have to use SWIG_exception to throw exception from typemap - simple throw seems not to work in this case
                      
        }
         
    }
}