#define SIZE
#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global double *a,
              __global double *b,
              __global double *c)
{
for(int i=0;i<SIZE;i++){;
        for(int j=0;j<SIZE;j++){
 c[i*SIZE+j]=a[i*SIZE+j] + b[i*SIZE+j];
                }
                }
                }

