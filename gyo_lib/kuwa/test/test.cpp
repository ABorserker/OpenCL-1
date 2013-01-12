#include<iostream>
#include<string>
#include<fstream>
#include<time.h>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

#define SIZE 6
using namespace std;

int main()
{
  clock_t t1, t2, t3, t4, t5, t6;
  t1 = clock();

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

  ///////////////////////////////////
  unsigned int x, y;
  float *a = new float[SIZE*SIZE];
  float *b = new float[SIZE*SIZE];
  float *c = new float[SIZE*SIZE];

  for(y = 0;y < SIZE;y++){
    for(x = 0;x < SIZE ;x++){
      a[y * SIZE + x] = 3;
      b[y * SIZE + x] = 8;
      c[y * SIZE + x] = 0;
    }
  }
  ////////////////////////////////////

  float *a_[totaldev];
  float *b_[totaldev];

  int delta = (SIZE / totaldev)* SIZE;
  int rest = (SIZE % totaldev)* SIZE + delta;

  cout <<"delta = "<<delta<<", rest = "<<rest<<endl;

  cout<<"a address : "<<a<<endl;
  for(y = 0;y<totaldev;y++){
    a_[y] = &a[delta * y];
    cout<<"a["<<y<<"] address"<<a_[y]<<" "<<*a_[y]<<endl;
    b_[y] = &b[delta * y];
    cout<<"b["<<y<<"] address"<<b_[y]<<" "<<*b_[y]<<endl;
  }

  /*
  static const char *source[] =
  {
    "#define SIZE 6\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
      __kernel void\n\
      calc(__global float *a,\n\
          __global float *b,\n\
          __global float *c)\n\
    {\n\
      for(int i=0;i<SIZE;i++){;\n\
        for(int j=0;j<SIZE;j++){\n\
            c[i*SIZE+j] = a[i*SIZE+j] + b[i*SIZE+j];\n\
        }\n\
      }\n\
    }\n"
  };
*/

  string str, new_str="", new_str_last="";
  const char *source1, *source_last;
  bool flag=true;
  char tmp[10];

  //ソースコード書き換え
  ifstream ifs("before.cl");
  while(ifs && getline(ifs, str)){
    cout << str <<endl;
    if(str.find("for")!=string::npos && flag){
      if(str.find("SIZE")!=string::npos){
        sprintf(tmp,"SIZE/%d",totaldev);
        str.replace(str.find("SIZE"), 4, tmp);
        flag = false;
      }

    }
    new_str += str;
    new_str += "\n";
  }
  source1 = new_str.c_str();
  cout<< new_str <<endl;


cout<< "oooooooooooooooooo"<<endl;

  //最後のデバイスのソースコード書き換え
  ifstream ifs_last("before.cl");
  flag = true;
  while(ifs_last && getline(ifs_last, str)){
    cout << str <<endl;
    if(str.find("for")!=string::npos && flag){
      if(str.find("SIZE")!=string::npos){
        sprintf(tmp,"%d",SIZE/totaldev+(SIZE%totaldev));
        str.replace(str.find("SIZE"), 4, tmp);
        flag = false;
      }

    }
    new_str_last += str;
    new_str_last += "\n";
  }
  source_last = new_str_last.c_str();
  cout<< new_str_last <<endl;

  //build
  cl_program program[num_platforms+1];
  for(int i =0,count = 0; i < num_platforms;i++)
  {
    if(i != num_platforms-1 || rest == 0){
      program[i] = clCreateProgramWithSource(context[i], 1, (const char**)&source1, NULL, &status);
      cout<< "CreateProgram["<<i<<"] : "<<status<<endl;

      status = clBuildProgram(program[i], num_devices[i], &device_list[count],NULL,NULL,NULL);
      count += num_devices[i];

      cout << "Program["<<i<<"]Build : "<<status<< endl;
    }

    else if(i == num_platforms-1 && rest != 0){
      program[i] = clCreateProgramWithSource(context[i], 1, (const char**)&source1, NULL, &status);
      cout<< "CreateProgram["<<i<<"] : "<<status<<endl;
      status = clBuildProgram(program[i], num_devices[i]-1, &device_list[count],NULL,NULL,NULL);
      count += num_devices[i];
      count--;
      cout << "Program["<<i<<"]Build : "<<status<< endl;
 
      program[i+1] = clCreateProgramWithSource(context[i], 1, (const char**)&source_last, NULL, &status);
      cout<< "last CreateProgram["<<i+1<<"] : "<<status<<endl;
      status = clBuildProgram(program[i+1], 1, &device_list[count],NULL,NULL,NULL);
      count += num_devices[i];
      cout << "last Program["<<i+1<<"]Build : "<<status<< endl;
    }
  }
  cl_kernel kernel[totaldev];
  int count3 = 0;
  for(int i = 0 ; i < num_platforms;i++){
    for(int j = 0;j < num_devices[i];j++){
      if(count3 != totaldev-1 || rest == 0){
        kernel[count3] = clCreateKernel(program[i], "calc", &status);
        cout << "program["<< i <<"] create Kernel["<<count3<<"] : "<< status<<endl;
        count3++;
      }
      else if(count3 == totaldev-1 && rest != 0){
        kernel[count3] = clCreateKernel(program[i+1], "calc", &status);
        cout << "program["<< i+1 << "] last create Kernel["<<count3<<"] : "<< status<<endl;
        count3++;
      }
    }
  }

  cl_mem mem[totaldev*3];
  int count4 = 0;
  for(int i =0;i<num_platforms;i++){
    for(int j = 0;j<num_devices[i];j++){
      if(count4+2 != totaldev*3-1 || rest == 0){
      mem[count4] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
      cout<<"context["<<i<<"] "<<"create mem["<<count4<<"] : "<<status<<endl;
      mem[count4+1] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
      cout<<"context["<<i<<"] "<<"create mem["<<count4+1<<"] : "<<status<<endl; 
        mem[count4+2] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*delta, NULL, &status);
        cout<<"context["<<i<<"] "<<"create mem["<<count4+2<<"] : "<<status<<endl;
      }
      else if(count4+2 == totaldev*3-1 && rest != 0){
       mem[count4] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*rest, NULL, &status);
      cout<<"context["<<i<<"] "<<"create mem["<<count4<<"] : "<<status<<endl;
      mem[count4+1] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*rest, NULL, &status);
      cout<<"context["<<i<<"] "<<"create mem["<<count4+1<<"] : "<<status<<endl; 
        mem[count4+2] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(float)*rest, NULL, &status);
        cout<<"context["<<i<<"] "<<"create mem["<<count4+2<<"] : "<<status<<endl;
      }
      count4+=3;
    }
  }

  count4=0;
  for(int i = 0;i<totaldev;i++,count4+=3){
    if(count4+2 != totaldev*3-1 || rest == 0){
    status = clEnqueueWriteBuffer(queue[i], mem[count4], CL_TRUE, 0, sizeof(float)*delta, a_[i], 0,NULL,NULL);
    cout << "commandQueue["<<i<<"]<=mem["<<count4<<"] : "<< status <<endl; 
    status = clEnqueueWriteBuffer(queue[i], mem[count4+1], CL_TRUE, 0, sizeof(float)*delta, b_[i], 0,NULL,NULL);
    cout << "commandQueue["<<i<<"]<=mem["<<count4+1<<"] : "<< status <<endl;  
      status = clEnqueueWriteBuffer(queue[i], mem[count4+2], CL_TRUE, 0, sizeof(float)*delta, c, 0,NULL,NULL);
      cout << "commandQueue["<<i<<"]<=mem["<<count4+2<<"] : "<< status <<endl; 
    }
    else if(count4+2 == totaldev*3-1 && rest != 0){
     status = clEnqueueWriteBuffer(queue[i], mem[count4], CL_TRUE, 0, sizeof(float)*rest, a_[i], 0,NULL,NULL);
    cout << "commandQueue["<<i<<"]<=mem["<<count4<<"] : "<< status <<endl; 
    status = clEnqueueWriteBuffer(queue[i], mem[count4+1], CL_TRUE, 0, sizeof(float)*rest, b_[i], 0,NULL,NULL);
    cout << "commandQueue["<<i<<"]<=mem["<<count4+1<<"] : "<< status <<endl;  
      status = clEnqueueWriteBuffer(queue[i], mem[count4+2], CL_TRUE, 0, sizeof(float)*rest, c, 0,NULL,NULL);
      cout << "commandQueue["<<i<<"]<=mem["<<count4+2<<"] : "<< status <<endl;  
    }
  }
  //status = clEnqueueWriteBuffer(queue[i], mem)

  count4=0;
  for(int i = 0;i<totaldev;i++){
    for(int j = 0;j<3;j++){
      status = clSetKernelArg(kernel[i], j, sizeof(cl_mem), (void *)&mem[i*3+j]);
      cout <<"kernel["<<i<<"] mem["<<i*3+j<<"] set : "<<status<<endl;
    }
  }

  t3 = clock();
  size_t globalsize[] = {1,1000,1000,100,1024};
  for(int i =0; i<totaldev;i++){
    status = clEnqueueNDRangeKernel(queue[i], kernel[i], 1, NULL, globalsize, NULL, 0, NULL, NULL);
    cout << "kernel done : " <<status << endl;
  }

  for(int i=0;i<totaldev;i++){
    if(3*i+2 != totaldev*3-1 || rest == 0){
      status = clEnqueueReadBuffer(queue[i], mem[3*i+2], CL_TRUE, 0, sizeof(float)*delta, c, 0, NULL, NULL);
      cout << "result : "<<status<<" >> "<<endl;
      for(int j = 0; j<SIZE/totaldev;j++)
      {
        for(int k = 0;k<SIZE;k++){
          cout << c[j*SIZE+k] << " ";
        }
        cout << endl;
      }
      cout << endl;
    }
    else if(3*i+2 == totaldev*3-1 && rest != 0){
      status = clEnqueueReadBuffer(queue[i], mem[3*i+2], CL_TRUE, 0, sizeof(float)*rest, c, 0, NULL, NULL);
      cout << "rest :"<<rest<<" mem["<<3*i+2<<"] last result : "<<status<<" >> "<<endl;
      for(int j = 0; j < (SIZE/totaldev) + (SIZE%totaldev); j++)
      {
        for(int k = 0;k<SIZE;k++){
          cout << c[j*SIZE+k] << " ";
        }
        cout << endl;
      }
      cout << endl;
    }
  }

  t4 = clock();

  delete a, b, c;

  for(int i=0;i<totaldev;i++){
    status = clFlush(queue[i]);
    status = clFinish(queue[i]);
  }
  for(int i=0;i<totaldev;i++){ 
    clReleaseKernel(kernel[i]);
  }
  for(int i=0;i<num_platforms;i++){ 
    clReleaseProgram(program[i]);
  }
  for(int i=0;i<totaldev*3;i++){ 
    clReleaseMemObject(mem[i]);
  }
  for(int i=0;i<totaldev;i++){ 
    clReleaseCommandQueue(queue[i]);
  }
  for(int i=0;i<num_platforms;i++){ 
    clReleaseContext(context[i]);
  }

  t2 = clock();
  cout <<"calc time = "<<(double)(t4 - t3) / CLOCKS_PER_SEC <<" sec."<< endl;
  cout <<"total time = "<<(double)(t2 -t1) / CLOCKS_PER_SEC <<" sec."<< endl;
}
