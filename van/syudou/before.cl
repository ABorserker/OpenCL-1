//SIZE=配列のサイズという仕様
//行列計算をする場合はSIZEを計算して縦横の長さを出す
#define l 100

#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global double *a,
              __global double *b,
              __global double *c)
{
  c[get_global_id(0)*1024 + get_global_id(1)]=a[get_global_id(0)*1024+get_global_id(1)] + b[get_global_id(0)*1024+get_global_id(1)];
}

