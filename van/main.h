#ifndef MAIN_H_
#define MAIN_H_

#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include"clapi.h"

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif //__APPLE

#define MAX_SOURCE_SIZE (0x100000)


#endif /* MAIN_H_ */
