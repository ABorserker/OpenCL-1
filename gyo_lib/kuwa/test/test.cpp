#include<iostream>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

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
  for(int i = 0,count=0 ; i< num_platforms; i++)
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

      cout <<"Command Queue"<<count<<": "<<status <<endl;
      count++;
    }
  }

  float a[3*3] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  float b[3*3] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
  float c[3*3];

  static const char *source[] =
  {
    "#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *a,\n\
          __global float *b,\n\
          __global float *c)\n\
      {\n\
        int index = get_global_id(0);\n\
          *c = (float)index;\n\
      }\n"
  };

  cl_program program[num_platforms];
  cl_kernel kernel[num_platforms];
  for(int i =0,count = 0; i < num_platforms; i++)
  {
  program[i] = clCreateProgramWithSource(context[i], 1, (const char**)&source, NULL, &status);
  cout<< status<<endl;

  status = clBuildProgram(program[i], num_devices[i], &device_list[count],NULL,NULL,NULL);
  count += num_devices[i];
 
  cout << status<< endl;
  
  kernel[i] = clCreateKernel(program[i], "calc", &status);
  }
  
/*  cl_mem mem[num_platforms*3];
  for(int i =0,count1=0,count2=0,count =0;i<num_platforms;i++)
  {
    for(int j = 0;j<3;j++){
      mem[count1] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem"<<count1<<" : "<<status<<endl;
      count1++;
    }
    
    for(int j =0;j<num_devices[i];j++){
      status = clEnqueueWriteBuffer(queue[count], mem[i*3], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue"<<count<<" mem"<<i*3<<" write : "<<status <<endl; 
      status = clEnqueueWriteBuffer(queue[count], mem[i*3+1], CL_TRUE, 0, sizeof(float)*3*3, b, 0,NULL,NULL);

      cout << "commandQueue"<<count<<" mem"<<i*3+1<<" write : "<<status <<endl;
count++;
    }

    for(int j=0;j<2;j++){
    status = clSetKernelArg(kernel[i], j, sizeof(cl_mem), (void *)mem[count2]);
    cout <<"kernel"<<i<<" mem"<<count2<<" set : "<<status<<endl;
   count2++;
    }
  }
*/


cl_mem mem[num_platforms*3];
      mem[0] = clCreateBuffer(context[0], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem : "<<status<<endl;
      mem[1] = clCreateBuffer(context[0], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem : "<<status<<endl;
      mem[2] = clCreateBuffer(context[0], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem : "<<status<<endl;
      mem[3] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem : "<<status<<endl;
      mem[4] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem : "<<status<<endl;
      mem[5] = clCreateBuffer(context[1], CL_MEM_READ_WRITE, sizeof(float)*3*3, NULL, &status);
      cout<<"create mem : "<<status<<endl;
      
      status = clEnqueueWriteBuffer(queue[0], mem[0], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      status = clEnqueueWriteBuffer(queue[0], mem[1], CL_TRUE, 0, sizeof(float)*3*3, b, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl;     
      status = clEnqueueWriteBuffer(queue[1], mem[3], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      status = clEnqueueWriteBuffer(queue[1], mem[4], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      status = clEnqueueWriteBuffer(queue[2], mem[3], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      status = clEnqueueWriteBuffer(queue[2], mem[4], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl;
      status = clEnqueueWriteBuffer(queue[3], mem[3], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      status = clEnqueueWriteBuffer(queue[3], mem[4], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      status = clEnqueueWriteBuffer(queue[4], mem[3], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl;
      status = clEnqueueWriteBuffer(queue[4], mem[4], CL_TRUE, 0, sizeof(float)*3*3, a, 0,NULL,NULL);
      cout << "commandQueue : "<< status <<endl; 
      

      status = clSetKernelArg(kernel[0], 0, sizeof(cl_mem), (void *)&mem[0]);
      cout <<"kernel mem set : "<<status<<endl;
      status = clSetKernelArg(kernel[0], 1, sizeof(cl_mem), (void *)&mem[1]);
      cout <<"kernel mem set : "<<status<<endl;
      status = clSetKernelArg(kernel[0], 2, sizeof(cl_mem), (void *)&mem[2]);
      cout <<"kernel mem set : "<<status<<endl;
      status = clSetKernelArg(kernel[1], 0, sizeof(cl_mem), (void *)&mem[3]);
      cout <<"kernel mem set : "<<status<<endl;
      status = clSetKernelArg(kernel[1], 1, sizeof(cl_mem), (void *)&mem[4]);
      cout <<"kernel mem set : "<<status<<endl;
      status = clSetKernelArg(kernel[1], 2, sizeof(cl_mem), (void *)&mem[5]);
      cout <<"kernel mem set : "<<status<<endl;

      size_t globalsize = 100;
      status = clEnqueueNDRangeKernel(queue[0], kernel[0], 1, NULL, &globalsize, NULL, 0, NULL, NULL);
      cout << "kernel done : " <<status << endl;
      status = clEnqueueNDRangeKernel(queue[1], kernel[1], 1, NULL, &globalsize, NULL, 0, NULL, NULL);
      cout << "kernel done : " <<status << endl;
      status = clEnqueueNDRangeKernel(queue[1], kernel[1], 1, NULL, &globalsize, NULL, 0, NULL, NULL);
      cout << "kernel done : " <<status << endl;
      status = clEnqueueNDRangeKernel(queue[1], kernel[1], 1, NULL, &globalsize, NULL, 0, NULL, NULL);
      cout << "kernel done : " <<status << endl;
      status = clEnqueueNDRangeKernel(queue[1], kernel[1], 1, NULL, &globalsize, NULL, 0, NULL, NULL);
      cout << "kernel done : " <<status << endl;

      status = clEnqueueReadBuffer(queue[0], mem[2], CL_TRUE, 0, 3*3*sizeof(float), c, 0, NULL, NULL);
      cout << "result : "<<status<<" >> ";
      for(int i = 0; i<9;i++)
      {
        cout << c[i] << " ";
      }
      cout << endl;

      status = clEnqueueReadBuffer(queue[1], mem[5], CL_TRUE, 0, 3*3*sizeof(float), c, 0, NULL, NULL);
      cout << "result : "<<status<<" >> ";
      for(int i = 0; i<9;i++)
      {
        cout << c[i] << " ";
      }
      cout << endl;
}
