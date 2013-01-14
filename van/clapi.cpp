#include"clapi.h"
#include<iostream>
#include<string>
#include<fstream>
#include<time.h>
#include<sys/time.h>

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.h>
#endif

#define SIZE 10
using namespace std;

double gettimeofday_sec()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

clapi::clapi(){
}

clapi::clapi(string tmp){
  filename = tmp;
}

clapi::~clapi(){
}

bool clapi::clauto(int n, ...){
  num_hikisu = n;
  va_list args;
  va_start(args, n);

  for(int t = 0; t<n ; t++){
    size[t] = va_arg(args,int);
    s[t] = va_arg(args, double*);
  }
  va_end(args);

  doOpenCL();

  return true;
}

bool clapi::doOpenCL(){
  double t1, t2, t3, t4, t5, t6;
  t1 = gettimeofday_sec();

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

  //////////////////////////////////
  /*
     unsigned int x, y;
     double *a = new double[SIZE*SIZE];
     double *b = new double[SIZE*SIZE];
     double *c = new double[SIZE*SIZE];

     for(y = 0;y < SIZE;y++){
     for(x = 0;x < SIZE ;x++){
     a[y * SIZE + x] = 1;
     b[y * SIZE + x] = 2;
     c[y * SIZE + x] = 0;
     }
     }
     *////////////////////////////////////

  //出力先だけ初期化
  memOut = new double[SIZE*SIZE];

  unsigned int x, y;
  for(y = 0; y < SIZE; y++){
    for(x = 0; x <SIZE; x++){
      memOut[y * SIZE+ x] = 0;
    }
  }

  //double *a_[totaldev];
  //double *b_[totaldev];
  double* memIn[num_hikisu][totaldev];

  int delta = (SIZE / totaldev)* SIZE;//device１つあたりの計算する要素数
  int rest = (SIZE % totaldev)* SIZE + delta;

  cout <<"delta = "<<delta<<", rest = "<<rest<<endl;

  //cout<<"a address : "<<a<<endl;
  for(y = 0;y<totaldev;y++){
    for(int i = 0;i<num_hikisu;i++){
      //memIn[i][y] = s[delta * y];

      memIn[i][y] = s[i] + /*sizeof(double)**/(delta * y);
      cout<<"s["<<i<<"] address"<<s[i]<<" "<<*s[i]<<endl;
      cout<<"memIn["<<i<<"]["<<y<<"] address"<<memIn[i][y]<<" "<<*memIn[i][y]<<endl;
    }
  }

  /*
     static const char *source[] =
     {
     "#define SIZE 6\n\
#pragma OPENCL EXTENSION cl_khr_fp64:enable\n\
__kernel void\n\
calc(__global double *a,\n\
__global double *b,\n\
__global double *c)\n\
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
char tmp[10],size[20];

//ソースコード書き換え
ifstream ifs(filename.c_str());
while(ifs && getline(ifs, str)){
  //cout << str <<endl;
  if(str.find("#define SIZE")!=string::npos){
    sprintf(size,"#define SIZE %d",SIZE);
    str.replace(str.find("#define SIZE"), 12, size);
  }
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

//最後のデバイスのソースコード書き換え
ifstream ifs_last(filename.c_str());
flag = true;
while(ifs_last && getline(ifs_last, str)){
  //cout << str <<endl;
  if(str.find("#define SIZE")!=string::npos){
    sprintf(size,"#define SIZE %d",SIZE);
    str.replace(str.find("#define SIZE"), 12, size);
  }
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
  if(i != num_platforms-1 || rest == delta){
    program[i] = clCreateProgramWithSource(context[i], 1, (const char**)&source1, NULL, &status);
    cout<< "CreateProgram["<<i<<"] : "<<status<<endl;

    status = clBuildProgram(program[i], num_devices[i], &device_list[count],NULL,NULL,NULL);
    count += num_devices[i];

    cout << "Program["<<i<<"]Build : "<<status<< endl;
  }

  else if(i == num_platforms-1 && rest != delta){
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
    if(count3 != totaldev-1 || rest == delta){
      kernel[count3] = clCreateKernel(program[i], "calc", &status);
      cout << "program["<< i <<"] create Kernel["<<count3<<"] : "<< status<<endl;
      count3++;
    }
    else if(count3 == totaldev-1 && rest != delta){
      kernel[count3] = clCreateKernel(program[i+1], "calc", &status);
      cout << "program["<< i+1 << "] last create Kernel["<<count3<<"] : "<< status<<endl;
      count3++;
    }
  }
}

cl_mem mem[totaldev*(num_hikisu+1)];
int count4 = 0;
for(int i =0;i<num_platforms;i++){
  for(int j = 0;j<num_devices[i]*(num_hikisu+1);j++){
    if(count4 != totaldev*(num_hikisu+1)-1 || rest == delta){
      mem[count4] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(double)*delta*2, NULL, &status);
      cout<<"context["<<i<<"] "<<"create mem["<<count4<<"] : "<<status<<endl;
      count4++;
    }
    else if(count4 == totaldev*(num_hikisu+1)-1 && rest != delta){
      mem[count4] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(double)*rest, NULL, &status);
      cout<<"context["<<i<<"] "<<"create mem["<<count4<<"] : "<<status<<endl;
    }
  }
}

//入力データ書き込み
for(int i = 0;i<totaldev;i++){
  if(i*(num_hikisu+1) != (totaldev-1)*(num_hikisu+1) || rest == delta){
    for(int j =0;j < num_hikisu;j++){

      //cout <<"eeeeeeeeeee "<<i*(num_hikisu+1)+j<< " j : " << j <<", i : "<<i<<endl;
      //cout << "commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+j<<"] : "<< status <<endl;

      status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+j], CL_TRUE, 0, sizeof(double)*delta, memIn[j][i], 0,NULL,NULL);
      cout << "if commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+j<<"] memIn["<<j<<"]["<<i<<"] status:"<< status <<endl;
    }
  }
  else if(i*(num_hikisu+1) == (totaldev-1)*(num_hikisu+1) && rest != delta){
    for(int j = 0;j< num_hikisu;j++){
      status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+j], CL_TRUE, 0, sizeof(double)*rest, memIn[j][i], 0,NULL,NULL);
      cout << "elseif commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+j<<"] : "<< status <<endl; 
    }
  }
}

//出力先データ書き込み初期化
for(int i = 0;i<totaldev;i++){
  if(i*(num_hikisu+1) != (totaldev-1)*(num_hikisu+1) || rest == delta){
    status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+num_hikisu], CL_TRUE, 0, sizeof(double)*delta, memOut, 0,NULL,NULL);
    cout << "commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+num_hikisu<<"] : "<< status <<endl;
  }
  else if(i*(num_hikisu+1) == (totaldev-1)*(num_hikisu+1) && rest != delta){
    status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+num_hikisu], CL_TRUE, 0, sizeof(double)*rest, memOut, 0,NULL,NULL);
    cout << "commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+num_hikisu<<"] : "<< status <<endl; 
  }
}

count4=0;
for(int i = 0;i<totaldev;i++){
  for(int j = 0;j<(num_hikisu+1);j++){
    status = clSetKernelArg(kernel[i], j, sizeof(cl_mem), (void *)&mem[i*(num_hikisu+1)+j]);
    cout <<"kernel["<<i<<"] mem["<<i*(num_hikisu+1)+j<<"] set : "<<status<<endl;
  }
}

t3 = gettimeofday_sec();
size_t globalsize[] = {1,1,1,1,1};
for(int i =0; i<totaldev;i++){
  status = clEnqueueNDRangeKernel(queue[i], kernel[i], 1, NULL, globalsize, NULL, 0, NULL, NULL);
  cout << "kernel done : " <<status << endl;
}



for(int i=0;i<totaldev;i++){
  if((num_hikisu+1)*i+num_hikisu != totaldev*(num_hikisu+1)-1 || rest == delta){
    status = clEnqueueReadBuffer(queue[i], mem[(num_hikisu+1)*i+num_hikisu], CL_TRUE, 0, sizeof(double)*delta, &memOut[delta*i], 0, NULL, NULL);
    cout << "mem["<<(num_hikisu+1)*i+num_hikisu<<"] result : "<<status<<" >> "<<endl;
    for(int j = i*(SIZE/totaldev); j<(i+1)*(SIZE/totaldev);j++)
    {
    for(int k = 0;k<SIZE;k++){
        cout << memOut[j*SIZE+k] << " ";
      }
      cout << endl;
    }
    cout << endl;
  }
  else if((num_hikisu+1)*i+num_hikisu == totaldev*(num_hikisu+1)-1 && rest != delta){
    status = clEnqueueReadBuffer(queue[i], mem[(num_hikisu+1)*i+num_hikisu], CL_TRUE, 0, sizeof(double)*rest, memOut, 0, NULL, NULL);
    cout << "rest :"<<rest<<" mem["<<(num_hikisu+1)*i+num_hikisu<<"] last result : "<<status<<" >> "<<endl;
    for(int j = 0; j < (SIZE/totaldev) + (SIZE%totaldev); j++)
    {
      for(int k = 0;k<SIZE;k++){
        cout << memOut[j*SIZE+k] << " ";
      }
      cout << endl;
    }
    cout << endl;
  }
}


t4 = gettimeofday_sec();

for(int i=0;i<totaldev;i++){
  status = clFlush(queue[i]);
  status = clFinish(queue[i]);
}
for(int i=0;i<totaldev;i++){ 
  clReleaseKernel(kernel[i]);
}
for(int i=0;i<num_platforms+1;i++){ 
  clReleaseProgram(program[i]);
}
for(int i=0;i<totaldev*(num_hikisu+1);i++){ 
  clReleaseMemObject(mem[i]);
}
for(int i=0;i<totaldev;i++){ 
  clReleaseCommandQueue(queue[i]);
}
for(int i=0;i<num_platforms;i++){ 
  clReleaseContext(context[i]);
}

t2 = gettimeofday_sec();
cout.setf(ios::fixed, ios::floatfield);
cout <<"calc time = "<<t4 - t3<<" sec."<< endl;
cout <<"total time = "<<t2 -t1<<" sec."<< endl;
return true;
}

double* clapi::getOut(){
  //return new double[SIZE*SIZE];
  return memOut;
}
