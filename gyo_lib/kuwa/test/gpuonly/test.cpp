#include<iostream>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

#define SIZE 10
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
  cl_command_queue queue[totaldev];
  for(int j=0,count=0;j<num_platforms;j++){
    for(int i=0;i<num_devices[j];i++){

      queue[count]= clCreateCommandQueue(context[j], device_list[count], 0, &status);

      cout <<"Context["<<j<<"] Command Queue"<<count<<": "<<status <<endl;
      count++;
    }
  }

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

  int delta = SIZE / totaldev;
  int rest = SIZE % totaldev;
  //余りの処理がまだ終わっていない

  for(y = 0;y<totaldev;y++){
    a_[y] = a + delta * SIZE * y;
    b_[y] = b + delta * SIZE * y;
  }

  static const char *source[] =
  {
    "#define SIZE 1025\n\
     #pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *a,\n\
          __global float *b,\n\
          __global float *c)\n\
    {\n\
      for(int i=0;i<SIZE/5;i++){;\n\
        for(int j=0;j<SIZE;j++){\n\
          for(int k = 0 ; k <SIZE ;k++){\n\
            c[i*j] += a[j*SIZE+k] * b[k*SIZE+j];\n\
          }\n\
        }\n\
      }\n\
    }\n"
  };

  cl_program program[num_platforms];

  program[1] = clCreateProgramWithSource(context[1], 1, (const char**)&source, NULL, &status);
  cout<< "CreateProgram[1] : "<<status<<endl;

  status = clBuildProgram(program[1], num_devices[1], &device_list[1],NULL,NULL,NULL);

  cout << "Program[1]Build : "<<status<< endl;

  cl_kernel kernel[totaldev];
  for(int j = 1;j < totaldev;j++){
    kernel[j] = clCreateKernel(program[1], "calc", &status);
    cout << "create Kernel["<<j<<"] : "<< status<<endl; 
  }

  cl_mem mem[totaldev*3];
  mem[3] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[4] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[5] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[6] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[7] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[8] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[9] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[10] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[11] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[12] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[13] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;
  mem[14] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
  cout<<"create mem : "<<status<<endl;


  status = clEnqueueWriteBuffer(queue[1], mem[3], CL_TRUE, 0, sizeof(float)*delta, a_[1], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 
  status = clEnqueueWriteBuffer(queue[1], mem[4], CL_TRUE, 0, sizeof(float)*delta, b_[1], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 
  status = clEnqueueWriteBuffer(queue[2], mem[6], CL_TRUE, 0, sizeof(float)*delta, a_[2], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 
  status = clEnqueueWriteBuffer(queue[2], mem[7], CL_TRUE, 0, sizeof(float)*delta, b_[2], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl;
  status = clEnqueueWriteBuffer(queue[3], mem[9], CL_TRUE, 0, sizeof(float)*delta, a_[3], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 
  status = clEnqueueWriteBuffer(queue[3], mem[10], CL_TRUE, 0, sizeof(float)*delta, b_[3], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 
  status = clEnqueueWriteBuffer(queue[4], mem[12], CL_TRUE, 0, sizeof(float)*delta, a_[4], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl;
  status = clEnqueueWriteBuffer(queue[4], mem[13], CL_TRUE, 0, sizeof(float)*delta, b_[4], 0,NULL,NULL);
  cout << "commandQueue : "<< status <<endl; 


  status = clSetKernelArg(kernel[1], 0, sizeof(cl_mem), (void *)&mem[3]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[1], 1, sizeof(cl_mem), (void *)&mem[4]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[1], 2, sizeof(cl_mem), (void *)&mem[5]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[2], 0, sizeof(cl_mem), (void *)&mem[6]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[2], 1, sizeof(cl_mem), (void *)&mem[7]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[2], 2, sizeof(cl_mem), (void *)&mem[8]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[3], 0, sizeof(cl_mem), (void *)&mem[9]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[3], 1, sizeof(cl_mem), (void *)&mem[10]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[3], 2, sizeof(cl_mem), (void *)&mem[11]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[4], 0, sizeof(cl_mem), (void *)&mem[12]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[4], 1, sizeof(cl_mem), (void *)&mem[13]);
  cout <<"kernel mem set : "<<status<<endl;
  status = clSetKernelArg(kernel[4], 2, sizeof(cl_mem), (void *)&mem[14]);
  cout <<"kernel mem set : "<<status<<endl;


  size_t globalsize[] = {10};
  status = clEnqueueNDRangeKernel(queue[1], kernel[1], 1, NULL, globalsize, NULL, 0, NULL, NULL);
  cout << "kernel done : " <<status << endl;
  status = clEnqueueNDRangeKernel(queue[2], kernel[2], 1, NULL, globalsize, NULL, 0, NULL, NULL);
  cout << "kernel done : " <<status << endl;
  status = clEnqueueNDRangeKernel(queue[3], kernel[3], 1, NULL, globalsize, NULL, 0, NULL, NULL);
  cout << "kernel done : " <<status << endl;
  status = clEnqueueNDRangeKernel(queue[4], kernel[4], 1, NULL, globalsize, NULL, 0, NULL, NULL);
  cout << "kernel done : " <<status << endl; 

  status = clEnqueueReadBuffer(queue[1], mem[5], CL_TRUE, 0, sizeof(float)*delta, c, 0, NULL, NULL);
  cout << "result : "<<status<<" >> ";
  for(int i = 0; i<9;i++)
  {
    cout << c[i] << " ";
  }
  cout << endl;

  status = clEnqueueReadBuffer(queue[2], mem[8], CL_TRUE, 0, sizeof(float)*delta, c, 0, NULL, NULL);
  cout << "result : "<<status<<" >> ";
  for(int i = 0; i<9;i++)
  {
    cout << c[i] << " ";
  }
  cout << endl;

  status = clEnqueueReadBuffer(queue[3], mem[11], CL_TRUE, 0, sizeof(float)*delta, c, 0, NULL, NULL);
  cout << "result : "<<status<<" >> ";
  for(int i = 0; i<9;i++)
  {
    cout << c[i] << " ";
  }
  cout << endl;

  status = clEnqueueReadBuffer(queue[4], mem[14], CL_TRUE, 0, sizeof(float)*delta, c, 0, NULL, NULL);
  cout << "result : "<<status<<" >> ";
  for(int i = 0; i<9;i++)
  {
    cout << c[i] << " ";
  }
  cout << endl;
}
