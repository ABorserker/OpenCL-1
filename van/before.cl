    #define SIZE
#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global float *a,
              __global float *b,
              __global float *c)
{
for(int i=0;i<SIZE;i++){;
        for(int j=0;j<SIZE;j++){
                c[i*SIZE+j] = a[i*SIZE+j]+ b[i*SIZE+j];
                }
                }
                }

