//SIZE=配列のサイズという仕様
//行列計算をする場合はSIZEを計算して縦横の長さを出す
#define l 100

#pragma OPENCL EXTENSION cl_khr_fp64:enable
__kernel void
calc(__global double *a,
              __global double *b,
  //            __global double *c,
  //            __global double *d,
  //            __global double *e,
  //            __global double *f,
  //            __global double *g,
  //            __global double *h,
              __global double *o)
{
  o[get_global_id(0)*1024 + get_global_id(1)]=a[get_global_id(0)*1024+get_global_id(1)] + b[get_global_id(0)*1024+get_global_id(1)];// + h[get_global_id(0)*1024+get_global_id(1)];
}

