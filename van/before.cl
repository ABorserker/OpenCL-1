//SIZE=配列のサイズという仕様
//行列計算をする場合はSIZEを計算して縦横の長さを出す

#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global double *a,
              __global double *b,
              __global double *c)
{
  for(int i = 0;i < SIZE;i++){
      c[i]=a[i] + b[i];
   }
}

