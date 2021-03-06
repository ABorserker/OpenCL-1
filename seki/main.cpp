#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

#include<iostream>

using namespace std;

int main()
{


  int i, j, k;
  float a[3*3] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  float b[3*3] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
  float c[3*3];

  cl_platform_id platforms[2];
  cl_uint num_platforms;
  cl_int status;
  status = clGetPlatformIDs(2, platforms, &num_platforms);

  cl_char name[1024];
  clGetPlatformInfo(platforms[1],CL_PLATFORM_NAME, sizeof(name), name,NULL);
  //cout << "Platform Name = " <<name<<endl;

  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[1], 0};

  cl_device_id *device_list= new cl_device_id[4];
  cl_uint num_device;
  status = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_GPU, 4, device_list, &num_device);

  cout << status <<endl;

  //cl_device_id devices[] = {device_list};
  cl_context context;
  //   context = clCreateContext(properties, sizeof(device_list)/sizeof(device_list[0]), device_list, NULL, NULL, &status);
  context = clCreateContext(properties, num_device, device_list, NULL, NULL, &status);

  cout << status <<endl;


  ///////////////////////////////
  cl_char buff[1024];
  for(int i= 0 ; i < num_device;i++)
  {
    cout << i <<endl;
    clGetDeviceInfo(device_list[i], CL_DEVICE_NAME, sizeof(buff), buff,NULL);
    cout << "Device Name = "<< buff << endl;
  }

  cl_device_id *getinfo = new cl_device_id[num_device];
  clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id)*num_device, getinfo, NULL);
  for(int i = 0; i < num_device ; i++){
    cout<< i <<endl;
    clGetDeviceInfo(getinfo[i], CL_DEVICE_NAME, sizeof(buff), buff, NULL);
    cout << "context Device Name = "<< buff <<endl;
  }
  ///////////////////////////////////


  cl_command_queue queue;
  queue = clCreateCommandQueue(context, device_list[0], 0, &status);


  /*
   *
   *とりあえず2つぐらい追加してみる
   *
   */

  cl_command_queue queue2 = clCreateCommandQueue(context, device_list[1], 0, &status);
  cl_command_queue queue3 = clCreateCommandQueue(context, device_list[2], 0, &status);



  cout << status <<endl;

  /*
     static const char *source[] = 
     {
     "#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
     __kernel void\n\
     calc(__global const float *a,\n\
     __global const float *b,\n\
     __global float *c)\n\
     {\n\
     int i, j, k;\n\
     for(i = 0; i < 3; i++)\n\
     {\n\
     for(j = 0; j < 3 ; j++)\n\
     {\n\
     c[i*3+j] = 0;\n\
     for(k = 0; k < 3; k++)\n\
     {\n\
     c[i*3+j] += a[i*3+k] * b[k*3+j];\n\
     }\n\
     }\n\
     }\n\
     }\n"
     "#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
     __kernel void\n\
     calc(__global const float *a,\n\
     __global const float *b,\n\
     __global float *c)\n\
     {c[0] = 2;}\n"
     };*/

  static const char *source1[] = 
  {
    "#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *a)\n\
      {\n\
        int index = get_global_id(0);\n\
          int i;\n\
          //for(i = 0 ; i< 9 ; i++)\n\
          //{\n\
            a[index] = (float)index;\n\
          //}\n\
      }\n"
  };    

  static const char *source2[] = 
  {
    "#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *b)\n\
      {\n\
        int index = get_global_id(0);\n\
        int i;\n\
          for(i = 0;i<9;i++)\n\
          {\n\
            b[index] = (float)index;\n\
          }\n\
      }\n"
  };   

  static const char *source3[] = 
  {
    "#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *c)\n\
      {\n\
        int index = get_global_id(0);\n\
        int i;\n\
          for(i = 0;i<9;i++)\n\
          {\n\
            c[index] = (float)index;\n\
          }\n\
      }\n"
  };   


  cl_program program1, program2, program3;
  program1 = clCreateProgramWithSource(context, 1, (const char**)&source1, NULL, &status);
  program2 = clCreateProgramWithSource(context, 1, (const char**)&source2, NULL, &status);
  program3 = clCreateProgramWithSource(context, 1, (const char**)&source3, NULL, &status);
  cout << "program : "<< status <<endl;

  cout << "num_dev : "<< num_device <<endl;

  cout << "device_list : " << device_list <<endl;
  status = clBuildProgram(program1, 4, device_list, NULL, NULL, NULL);
  status = clBuildProgram(program2, 1, &device_list[1], NULL, NULL, NULL);
  status = clBuildProgram(program3, 1, &device_list[2], NULL, NULL, NULL);
  cout << "build : "<< status <<endl;

  ///////////////////////////////////
  size_t logsize;
  status = clGetProgramBuildInfo(program1,device_list[0],CL_PROGRAM_BUILD_LOG,0,NULL,&logsize);
  if(status == CL_SUCCESS)
  {
    char *logbuffer;
    logbuffer = new char[logsize + 1];
    if(logbuffer == NULL)
    {
      cout << "memory allocation failed."<<endl;
      return 0;
    }

    status = clGetProgramBuildInfo(program1,device_list[0],CL_PROGRAM_BUILD_LOG,logsize,logbuffer,NULL);
    if(status == CL_SUCCESS)
    {
      logbuffer[logsize]='\0';
      cout << "build log" << endl;
      cout << logbuffer << endl;
    }
    delete [] logbuffer;
  }
  //////////////////////////////////////////

  cl_kernel kernel1, kernel2, kernel3;
  kernel1 = clCreateKernel(program1, "calc", &status);
  kernel2 = clCreateKernel(program2, "calc", &status);
  kernel3 = clCreateKernel(program3, "calc", &status);

  cout << "creaate kernel : "<<status <<endl;

  cl_mem memIn1, memIn2, memOut;
  memIn1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*3*3, a, &status);
  cout << status << endl;
  memIn2 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*3*3, b, &status);
  cout << status << endl;
  memOut = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*3*3, c, &status);
  cout << status <<endl;

  status = clSetKernelArg(kernel1, 0, sizeof(cl_mem), (void *)&memIn1);
  cout << status << endl;
  status = clSetKernelArg(kernel2, 1, sizeof(cl_mem), (void *)&memIn2);
  cout << status << endl;
  status = clSetKernelArg(kernel3, 2, sizeof(cl_mem), (void *)&memOut);
  cout << status << endl;


  cout << "zikkou"<< endl;
  size_t globalsize[] = {1000};

  for(int x;x < 1000000; x++){
  status = clEnqueueNDRangeKernel(queue, kernel1, 1, NULL, globalsize, NULL, 0, NULL, NULL);
  }

    /*
     * 追加
     */

    //status = clEnqueueNDRangeKernel(queue2, kernel2, 1, NULL, globalsize, NULL, 0, NULL, NULL);
  //  status = clEnqueueNDRangeKernel(queue3, kernel3, 1, NULL, globalsize, NULL, 0, NULL, NULL);
 
  cout << "done." << status << endl;

  status = clEnqueueReadBuffer(queue, memIn1, CL_TRUE, 0, sizeof(float)*3*3, a, 0, NULL, NULL);
  status = clEnqueueReadBuffer(queue, memIn2, CL_TRUE, 0, sizeof(float)*3*3, b, 0, NULL, NULL);
  status = clEnqueueReadBuffer(queue3, memOut, CL_TRUE, 0, sizeof(float)*3*3, c, 0, NULL, NULL);

  for(i = 0 ; i < 3 ; i++)
  {
    for(j = 0; j < 3; j++)
    {
      cout << a[i * 3 + j] << " ";
    }
    cout << endl;
  }

cout << endl;

  for(i = 0 ; i < 3 ; i++)
  {
    for(j = 0; j < 3; j++)
    {
      cout << b[i * 3 + j] << " ";
    }
    cout << endl;
  }

cout << endl;

  for(i = 0 ; i < 3 ; i++)
  {
    for(j = 0; j < 3; j++)
    {
      cout << c[i * 3 + j] << " ";
    }
    cout << endl;
  }

  clReleaseMemObject(memOut);
  clReleaseMemObject(memIn1);
  clReleaseMemObject(memIn2);
  clReleaseKernel(kernel1);
  clReleaseProgram(program1);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

  delete device_list;
}

