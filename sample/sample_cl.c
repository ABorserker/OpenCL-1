#include<stdio.h>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

int main(void)
{
  int a = 1, b = 2, c;

  cl_int status;
  cl_platform_id platforms;
  cl_uint num_platforms;
  status = clGetPlatformIDs(1, &platforms, &num_platforms);

  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)&platforms, 0};

  cl_device_id device_list;
  cl_uint num_device;
  status = clGetDeviceIDs(platforms, CL_DEVICE_TYPE_CPU, 1, &device_list, &num_device);

  cl_context context;
  context = clCreateContext(properties, num_device, &device_list, NULL, NULL, &status);

  cl_command_queue queue;
  queue = clCreateCommandQueue(context, device_list, 0, &status);

  static const char *source[] = 
  {
    "__kernel void\n\
      calc(__global const int *a,\n\
          __global const int *b,\n\
          __global int *c)\n\
      {\n\
        *c = *a + *b;\n\
      }\n"
  };

  cl_program program;
  program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &status);

  status = clBuildProgram(program, num_device, &device_list, NULL, NULL, NULL);

  cl_kernel kernel;
  kernel = clCreateKernel(program, "calc", &status);

  cl_mem mem_a, mem_b, mem_c;
  mem_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &a, &status);
  mem_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &b, &status);
  mem_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int), NULL, &status);

  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_a);
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_b);
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&mem_c);

  size_t globalsize[10];
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalsize, NULL, 0, NULL, NULL);

  status = clEnqueueReadBuffer(queue, mem_c, CL_TRUE, 0, sizeof(cl_int), &c, 0, NULL, NULL);

  printf("kekka : %d\n", c);

  clReleaseMemObject(mem_a);
  clReleaseMemObject(mem_b);
  clReleaseMemObject(mem_c);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}
