#include<iostream>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

#define SIZE 1000
using namespace std;

int main()
{
  cl_platform_id *platforms;
  cl_uint num_platforms;
  cl_int status;
  //`プラットフォームの数を取得
  status = clGetPlatformIDs(0, NULL, &num_platforms);
  platforms = new cl_platform_id[num_platforms];
  //プラットフォームIDを取得
  status = clGetPlatformIDs(num_platforms, platforms, NULL);
  cout << "num_platforms : " << num_platforms << endl;

  cl_uint num_devices[num_platforms];
  cl_device_id *device_list;

  //各プラットフォームからデバイス数を取得
  //すべてのプラットフォームのデバイス数の総和を計算
  cl_uint totaldev = 0;
  for(int i = 0 ; i< num_platforms; i++)
  {
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices[i]);
    totaldev += num_devices[i];
  }

  //デバイス数の総和分のメモリ確保
  device_list = new cl_device_id[totaldev];
  //各プラットフォームのデバイスID, デバイス数を取得、表示
  //プラットフォームの数だけコンテキスト作成
  cl_uint num;
  cl_context context[num_platforms];
  for(int i = 0, count = 0; i< num_platforms; i++)
  {
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices[i], &device_list[count], &num);
    cout << "Platform"<<i<<" num_devices : "<<num <<endl;

    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i], 0};
    context[i] = clCreateContext(properties, num_devices[i], &device_list[count], NULL, NULL, &status);
    cout << "create context" << i<<" :"<<status<<endl;
    count += num;
  }


  //総デバイスID表示
  for(int i =0;i<totaldev;i++)
  {
    cout <<"device_list["<<i<<"] : "<<device_list[i]<<endl;
  }

  //デバイス数だけコマンドキューを作成
  cl_command_queue queue;
  queue= clCreateCommandQueue(context[0], device_list[0], 0, &status);

  cout <<"Context[0] CommandQueue: "<<status <<endl;

  unsigned int x, y;
  float *a = new float[SIZE*SIZE];
  float *b = new float[SIZE*SIZE];
  float *c = new float[SIZE*SIZE];

  for(y =0; y < SIZE;y++){
    for(x = 0;x <SIZE ;x++){
      a[y * SIZE + x] = 1;
      b[y * SIZE + x] = 2;
      c[y * SIZE + x] = 0;
    }
  }  

  float *a_[totaldev], *b_[totaldev];
  
  int delta = SIZE * SIZE / totaldev;
  int rest = SIZE % totaldev;
  //余りの処理がまだ終わっていない

  for(y = 0;y<totaldev;y++){
    a_[y] = a + delta * y;
    b_[y] = b + delta * y;
  }

  static const char *source[] =
  {
    "#define SIZE 1000\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *a,\n\
          __global float *b,\n\
          __global float *c)\n\
    {\n\
      for(int i=0;i<SIZE/5;i++){;\n\
        for(int j=0;j<SIZE/5;j++){\n\
          for(int k = 0 ; k <SIZE ;k++){\n\
            c[i*j] += a[i*SIZE+k] * b[j*SIZE+k];\n\
          }\n\
        }\n\
      }\n\
    }\n"
  };

  cl_program program[num_platforms];
  program[0] = clCreateProgramWithSource(context[0], 1, (const char**)&source, NULL, &status);
  cout<< "CreateProgram[0] : "<<status<<endl;

  status = clBuildProgram(program[0], num_devices[0], &device_list[0],NULL,NULL,NULL);
  cout << "Program[0]Build : "<<status<< endl;

  cl_kernel kernel[totaldev];
  kernel[0] = clCreateKernel(program[0], "calc", &status);
  cout << "create Kernel[0] : "<< status<<endl; 

  //delta = 1000 * 1000;
  cl_mem mem[totaldev*3];
  mem[0] = clCreateBuffer(context[0], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[1] = clCreateBuffer(context[0], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[2] = clCreateBuffer(context[0], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;

  status = clEnqueueWriteBuffer(queue, mem[0], CL_TRUE, 0, sizeof(float)*delta, a_[0], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 
  status = clEnqueueWriteBuffer(queue, mem[1], CL_TRUE, 0, sizeof(float)*delta, b_[0], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 


  status = clSetKernelArg(kernel[0], 0, sizeof(cl_mem), (void *)&mem[0]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[0], 1, sizeof(cl_mem), (void *)&mem[1]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[0], 2, sizeof(cl_mem), (void *)&mem[2]);
  cout <<"kernel mem set : "<<status<<endl;

  size_t globalsize[] = {10};
  status = clEnqueueNDRangeKernel(queue, kernel[0], 1, NULL, globalsize, NULL, 0, NULL, NULL);
  cout << "kernel done : " <<status << endl;

  status = clEnqueueReadBuffer(queue, mem[2], CL_TRUE, 0, sizeof(float)*delta, c, 0, NULL, NULL);
  cout << "result : "<<status<<" >> ";
  for(int i = 0; i<9;i++)
  {
    cout << c[i] << " ";
  }
  cout << endl;
}
