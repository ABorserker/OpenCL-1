#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global double *a,
              __global double *b,
              __global double *c)
{
for(int i=0;i<SIZE;i++){;
 c[i]=a[i] + b[i];
                }
}

