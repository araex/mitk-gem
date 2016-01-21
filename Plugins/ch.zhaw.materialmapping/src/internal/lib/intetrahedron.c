#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h>

/*
  nt   : number of triangles
  nx   : number of pixels in x direction
  ny   : number of pixels in y direction
  nz   : number of pixels in z direction
  tri  : index matrix for triangles
  cor  : coordinate matrix for corner points
  x    : x-coordinates for pixels
  y    : y-coordinates for pixels
  z    : z-coordinates for pixels
  ct   : ct data in pixels
*/

typedef struct {
  double x;
  double y;
  double z;
} point3;

typedef struct {
  int a;
  int b;
  int c;
} triangle;

typedef struct {
  double x;
  double y;
} point2;

typedef struct {
  point2 a;
  point2 b;
} line;

int comp(const void *a, const void *b)
{
  return *((double *) a) > *((double *) b) ? 1 : 
    *((double *) a) < *((double *) b) ? -1 : 0;
}

int intetrahedron(int nt, int nx, int ny, int nz, triangle *tri, 
		   point3 *cor, double *x, double *y, double *z, char *ct_bool)
{
  int i, j, k, npg, ncr, ypos, a, b, c, v[3];
  double s[3], *cr;
  line *pg;
  FILE *out = fopen("intetra_problem.txt", "w");
	
  pg = (line *) malloc(nt * sizeof(line));
  cr = (double *) malloc(nt * sizeof(double));
 
  /* Loop through planes */
  for(i = 0; i < nz; i++) {
    npg = 0;
    for(j = 0; j < nt; j++) {
      a = tri[j].a - 1;
      b = tri[j].b - 1;
      c = tri[j].c - 1;

      /* Check if triangle is in plane. */
      if(cor[a].z <= cor[b].z && cor[a].z <= cor[c].z) {
	v[0] = a;
	if(cor[b].z <= cor[c].z) {
	  v[1] = b;
	  v[2] = c;
	}
	else {
	  v[1] = c;
	  v[2] = b;
	}
      }
      else if(cor[b].z <= cor[a].z && cor[b].z <= cor[c].z) {
	v[0] = b;
	if(cor[a].z <= cor[c].z) {
	  v[1] = a;
	  v[2] = c;
	}
	else {
	  v[1] = c;
	  v[2] = a;
	}
      }
      else {
	v[0] = c;
	if(cor[a].z <= cor[b].z) {
	  v[1] = a;
	  v[2] = b;
	}
	else {
	  v[1] = b;
	  v[2] = a;
	}
      }
      
      if(cor[v[0]].z < z[i] - DBL_EPSILON && cor[v[2]].z >= z[i]) {
	s[0] = (z[i] - cor[b].z) / (cor[c].z - cor[b].z);
	s[1] = (z[i] - cor[a].z) / (cor[c].z - cor[a].z);
	s[2] = (z[i] - cor[a].z) / (cor[b].z - cor[a].z);
	if(s[0] >= 0.0 && s[0] <= 1.0)
	  if(s[1] >= 0.0 && s[1] <= 1.0) {
	    pg[npg].a.x = (1 - s[0]) * cor[b].x + s[0] * cor[c].x;
	    pg[npg].a.y = (1 - s[0]) * cor[b].y + s[0] * cor[c].y;
	    pg[npg].b.x = (1 - s[1]) * cor[a].x + s[1] * cor[c].x;
	    pg[npg].b.y = (1 - s[1]) * cor[a].y + s[1] * cor[c].y;
	    npg++;
	  }
	  else {
	    pg[npg].a.x = (1 - s[0]) * cor[b].x + s[0] * cor[c].x;
	    pg[npg].a.y = (1 - s[0]) * cor[b].y + s[0] * cor[c].y;
	    pg[npg].b.x = (1 - s[2]) * cor[a].x + s[2] * cor[b].x;
	    pg[npg].b.y = (1 - s[2]) * cor[a].y + s[2] * cor[b].y;
	    npg++;
	  }
	else if(s[1] >= 0.0 && s[1] <= 1.0) {
	  pg[npg].a.x = (1 - s[1]) * cor[a].x + s[1] * cor[c].x;
	  pg[npg].a.y = (1 - s[1]) * cor[a].y + s[1] * cor[c].y;
	  pg[npg].b.x = (1 - s[2]) * cor[a].x + s[2] * cor[b].x;
	  pg[npg].b.y = (1 - s[2]) * cor[a].y + s[2] * cor[b].y;
	  npg++;
	}
      }
    }

    for(j = 0; j < nx; j++) {
      ncr = 0;
      for(k = 0; k < npg; k++) {
	s[0] = (x[j] - pg[k].a.x) / (pg[k].b.x - pg[k].a.x);
	if(s[0] >= -DBL_EPSILON && s[0] <= 1.0 + DBL_EPSILON) {

	  /* Check thoroughly if segment is counted. */
	  if(pg[k].a.x > pg[k].b.x) {
	    if(pg[k].b.x < x[j] - 2.0 * DBL_EPSILON) {
	      cr[ncr] = (1.0 - s[0]) * pg[k].a.y + s[0] * pg[k].b.y;
	      ncr++;
	    }
	  }
	  else if(pg[k].a.x < pg[k].b.x) {
	    if(pg[k].a.x < x[j] - 2.0 * DBL_EPSILON) {
	      cr[ncr] = (1.0 - s[0]) * pg[k].a.y + s[0] * pg[k].b.y;
	      ncr++;
	    }
	  }
	}
      }
      qsort(cr, ncr, sizeof(double), comp);
      /*
      for(k = 1, q = 0; k < ncr; k++) {
	if(fabs(cr[k] - cr[q]) > 2.0 * DBL_EPSILON)
	  q++;
	cr[q] = cr[k];
      }
      if(q)
	ncr = q + 1;
      */
      if(ncr % 2) {
	fprintf(out, "x = %g, z = %g, ", x[j], z[i]);
	fprintf(out, "y = ");
	for(k = 0; k < ncr; k++)
	  fprintf(out, "%g, ", cr[k]);
	fprintf(out, "\n");
	/*
	free(pg);
	free(cr);
	return 1;
	*/
      }	
      else {
	for(k = 0; k < (ncr / 2); k++) {
	  ypos = 0;
	  while(cr[2*k] > y[ypos]) {
	    /*	  ct_bool[ypos + j * ny + i * ny * nx] = 0;*/
	    ypos++;
	  }
	  while(cr[2*k+1] > y[ypos]) {
	    ct_bool[ypos + j * ny + i * ny * nx] = 1;
	    ypos++;
	  }
	}
      }
    }
  }
  free(pg);
  free(cr);
  fclose(out);
  return 0;
}

#ifdef MATLAB_MEX_FILE

#include "mex.h"

#define mxGetInt8(x) ((char *) mxGetData(x))
#define mxGetInt32(x) ((int *) mxGetData(x))
#define mxGetSingle(x) ((float *) mxGetData(x))

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int k;
  mwSize dim[3];
  
  dim[0] = mxGetM(prhs[3]);
  dim[1] = mxGetM(prhs[2]);
  dim[2] = mxGetM(prhs[4]);
  plhs[0] = mxCreateNumericArray(3, dim, mxINT8_CLASS, mxREAL);
  
  if(intetrahedron(mxGetN(prhs[0]), dim[1], dim[0], dim[2],
		   (triangle *) mxGetInt32(prhs[0]),
		   (point3 *) mxGetPr(prhs[1]), mxGetPr(prhs[2]),
		   mxGetPr(prhs[3]), mxGetPr(prhs[4]), mxGetInt8(plhs[0])))
    mexErrMsgTxt("Problem inside polygon, distort coordinates.");
}

#endif