#include<stdio.h>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

int main(void)
{
  printf("start\n");

  int a = 1, b = 2, c;

  printf("start \n");

  cl_int status;
  cl_platform_id platforms;
  cl_uint num_platforms;
  status = clGetPlatformIDs(1, &platforms, &num_platforms);
printf("%d\n",status);

  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)&platforms, 0};

  cl_device_id device_list;
  cl_uint num_device;
  status = clGetDeviceIDs(platforms, CL_DEVICE_TYPE_CPU, 1, &device_list, &num_device);
  printf("%d\n", status);

  cl_context context;
  context = clCreateContext(properties, num_device, &device_list, NULL, NULL, &status);

  cl_command_queue queue;
  queue = clCreateCommandQueue(context, device_list, 0, &status);

  static const char source[] = 
  {
    "__kernel void\n\
      calc(__global const int *a,\n\
          __global const int *b,\n\
          __global int *c)\n\
      {\n\
        *c = *a + *b;\n\
      }\n"
  };
const char* sourcelist[]={source};
  cl_program program;
  program = clCreateProgramWithSource(context, 1, sourcelist, NULL, &status);
  printf("program : %d\n" , status);

  status = clBuildProgram(program, num_device, &device_list, NULL, NULL, NULL);
  printf("build : %d\n" , status);
  ///////////////////////////////////
  size_t logsize;
  status = clGetProgramBuildInfo(program,device_list,CL_PROGRAM_BUILD_LOG,0,NULL,&logsize);
  if(status == CL_SUCCESS)
  {
    char *logbuffer;
    logbuffer = new char[logsize + 1]; 
    if(logbuffer == NULL)
    {   
      printf( "memory allocation failed.\n");
      return 0;
    }   

    status = clGetProgramBuildInfo(program,device_list,CL_PROGRAM_BUILD_LOG,logsize,logbuffer,NULL);
    if(status == CL_SUCCESS)
    {   
      logbuffer[logsize]='\0';
      printf( "build log\n" );
      printf("%s\n", logbuffer);
    }   
    delete [] logbuffer;
  }
  //////////////////////////////////////////



  cl_kernel kernel;
  kernel = clCreateKernel(program, "calc", &status);
  printf("%d\n" , status);


  cl_mem mem_a, mem_b, mem_c;
  mem_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &a, &status);
  mem_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &b, &status);
  mem_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int), NULL, &status);

  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_a);
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_b);
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&mem_c);

  size_t globalsize[10];
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalsize, NULL, 0, NULL, NULL);
  printf("zikkou : %d\n" ,status);

  status = clEnqueueReadBuffer(queue, mem_c, CL_TRUE, 0, sizeof(cl_int), &c, 0, NULL, NULL);
printf("%d\n" , status);




  printf("kekka : %d\n", c);

  //11.リソースの開放
  clReleaseMemObject(mem_a);
  clReleaseMemObject(mem_b);
  clReleaseMemObject(mem_c);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}
