#include <math.h>

void extendsurface(int nx, int ny, int nz, float *E, char *c,
        float *Ex, char *cx)
{
    
    /* Based on call from mexFunction, extendsurface receives
     * nx= number of x points in CT
     * ny= number of y points in CT
     * nz= number of z points in CT
     *E = pointer to CT scan values
     *c = pointer to CT_slim (peeled solid, no external surface)
     *Ex= pointer to CT_ext output
     *cx= pointer to CT_bool output
     */
    
    int ix, iy, iz, i, j, k;
    float R[27], s, t;
    
    for(i = 0; i < 3; i++)  //i=0,1,2
        for(j = 0; j < 3; j++)        //j=0,1,2
            for(k = 0; k < 3; k++) {       //k=0,1,2
        R[i+3*(j+3*k)] = (i != 1 ? (j != 1 ? (k != 1 ? 1.0/sqrt(3.0) :
            1.0/sqrt(2.0)) : (k != 1 ? 1.0/sqrt(2.0) : 1)) :
                (j != 1 ? (k != 1 ? 1.0/sqrt(2.0) : 1) :
                    (k!= 1 ? 1 : 0)));
            }
    /*
     R= [p q p     [q 1 q      [p q p
         q 1 q      1 0 1       q 1 q
         p q p]     q 1 q]      p q p],    q=1/sqrt(2), p=1/sqrt(3)
     */
    
    //loop over z values
    for(iz = 0; iz < nz; iz++) {
        //loop over x values
        for(ix = 0; ix < nx; ix++) {
            //loop over y values
            for(iy = 0; iy < ny; iy++) {
                if (iz==0 || ix==0 || iy==0 || iz==nz-1 || ix==nx-1 || iy==ny-1) {
                    //Edge voxel, keep original value
                    Ex[iy + ny * (ix + iz * nx)] = E[iy + ny * (ix + iz * nx)];
                    cx[iy + ny * (ix + iz * nx)] = 0;
                }
                else {
                    if(! c[iy + ny * (ix + iz * nx)]) {
                        //current voxel not part of peeled solid
                        s = 0;
                        for(i = -1; i < 2; i++) {  //i=-1,0,1
                            for(j = -1; j < 2; j++)  { //j=-1,0,1
                                for(k = -1; k < 2; k++) {  //k=-1,0,1
                                    //use R filter to get s based on CT_slim
                                    s += R[(i+1)+3*((j+1)+3*(k+1))]*c[iy+j+ny*(ix+i+nx*(iz+k))];
                                }
                            }
                        }
                        if(s) {   //If any of the 3x3x3 voxels is in peeled model
                            t = 0;
                            for(i = -1; i <= 1; i++) { //i=-1,0,1
                                for(j = -1; j <= 1; j++) { //j=-1,0,1
                                    for(k = -1; k <= 1; k++) { //k=-1,0,1
                                        t +=(R[(i+1)+3*((j+1)+3*(k+1))] * E[iy+j+ny*(ix+i+nx*(iz+k))]*
                                                c[iy+j+ny*(ix+i+nx*(iz+k))]);
                                    }
                                }
                            }
                            //Use R filter and CT_slim values to modify
                            //the CT values
                            Ex[iy + ny * (ix + iz * nx)] = t / s;
                            cx[iy + ny * (ix + iz * nx)] = 1;
                        }
                        else {       //current voxel too far away from peeled solid
                            Ex[iy + ny * (ix + iz * nx)] = E[iy + ny * (ix + iz * nx)];
                            cx[iy + ny * (ix + iz * nx)] = 0;
                        }
                    }
                    else {    //current voxel is inside peeled solid
                        Ex[iy + ny * (ix + iz * nx)] = E[iy + ny * (ix + iz * nx)];
                        cx[iy + ny * (ix + iz * nx)] = 1;
                    }
                }
            }
        }
    }
}


#ifdef MATLAB_MEX_FILE

#include "mex.h"

#define mxGetInt8(x) ((char *) mxGetData(x))
#define mxGetInt32(x) ((int *) mxGetData(x))
#define mxGetSingle(x) ((float *) mxGetData(x))

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  const mwSize *dim;
  
 /*
   Based on call from partial_vol_correction_rev1.m, this function receives
   prhs inputs as
    *prhs[0]= (CT) CT scan values
    *prhs[1]= (CT_slim) Array of same size as CT with ones inside the solid
                and zeros outside it or in the outer layer (peeled)
   It has two plhs outputs
    *plhs[0]=CT_ext, modified CT data
    *plhs[1]=CT_bool, boolean voxels extended beyond limits given by mesh
 */  
  
  dim = mxGetDimensions(prhs[0]);
  plhs[0] = mxCreateNumericArray(3, dim, mxSINGLE_CLASS, mxREAL);
  plhs[1] = mxCreateNumericArray(3, dim, mxINT8_CLASS, mxREAL);
  extendsurface(dim[1], dim[0], dim[2], mxGetSingle(prhs[0]), 
		mxGetInt8(prhs[1]), mxGetSingle(plhs[0]), mxGetInt8(plhs[1]));
}
#endif