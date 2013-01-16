//SIZE=配列のサイズという仕様
//行列計算をする場合はSIZEを計算して縦横の長さを出す

#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global double *a,
              __global double *b,
              __global double *c)
{
  for(int i = 0;i < SIZE/10;i++){;
    for(int j = 0;j < SIZE/10;j++){
      c[i*SIZE/10+j]=a[i*SIZE/10+j] + b[i*SIZE/10+j];
    }
   }
}

