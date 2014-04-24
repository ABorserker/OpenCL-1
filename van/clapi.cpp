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


double* clapi::clauto(int n, ...){
  num_hikisu = n;
  va_list args;
  va_start(args, n);

  for(int t = 0; t<n ; t++){
    asize[t] = va_arg(args,int);
    s[t] = va_arg(args, double*);
  }
  va_end(args);

  for(int i = 0;i<num_hikisu-1;i++){
    if(asize[i] == asize[i+1]){
      size = asize[0];
      std::cout << "num_hikisu : " << num_hikisu<< std::endl;
    }
    else if(asize[i] != asize[i+1]){
      std::cout << "入力した配列はすべて同じ大きさにしてください"<< std::endl;
    }
    
  }

  return doOpenCL();
}


double* clapi::doOpenCL(){
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
  std::cout << "num_platforms : " << num_platforms << std::endl;

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
    std::cout << "Platform"<< i <<" num_devices : " << num << std::endl;

    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i], 0};
    context[i] = clCreateContext(properties, num_devices[i], &device_list[count], NULL, NULL, &status);
    std::cout << "create context" << i << " :" << status << std::endl;
    count += num;
  }


  //総デバイスID表示
  for(int i = 0; i < totaldev; i++)
  {
    std::cout << "device_list[" << i << "] : " << device_list[i] << std::endl;
  }

  //デバイス数だけコマンドキューを作成
  cl_command_queue queue[totaldev];
  for(int j=0,count=0;j<num_platforms;j++){
    for(int i=0;i<num_devices[j];i++){

      queue[count]= clCreateCommandQueue(context[j], device_list[count], 0, &status);

      std::cout << "Command Queue" << count << ": " << status << std::endl;
      count++;
    }
  }

  //出力先だけ初期化
  memOut = new double[size];

    for(int j = 0; j < size; j++){
      memOut[j] = 0;
  }

  double* memIn[num_hikisu][totaldev];

  int delta = size / totaldev;//device１つあたりの計算する要素数
  int rest = (size % totaldev) + delta;

  std::cout << "delta = " << delta << ", rest = " << rest << std::endl;

  if(delta == 0){
    std::cout << "エラー : 配列サイズ＜デバイス数" << std::endl;
    return false;
  }

  for(int j = 0; j < num_hikisu; j++) {
    for(int i = 0; i < totaldev; i++) {

      memIn[j][i] = s[j] + (delta * i);
      std::cout << "s[" << j << "] address" << s[j] << " " << *s[j] << std::endl;
      std::cout << "memIn[" << j << "][" << i << "] address" << memIn[j][i] << " " << *memIn[j][i] << std::endl;
    }
  }


  std::string str, new_str="", new_str_last="";
const char *source1, *source_last;
bool flag=true;
char tmp[10],charsize[20];

//ソースコード書き換え
 std::ifstream ifs(filename.c_str());
while(ifs && getline(ifs, str)){
  if(str.find("for")!= std::string::npos && flag){
    if(str.find("SIZE")!= std::string::npos){
      sprintf(tmp,"%d",delta);
      str.replace(str.find("SIZE"), 4, tmp);
      flag = false;
    }
  }
  while(str.find("SIZE")!= std::string::npos){
      sprintf(charsize,"%d", size);
      str.replace(str.find("SIZE"), 4, charsize);
  }

  new_str += str;
  new_str += "\n";
}
source1 = new_str.c_str();
 std::cout << new_str << std::endl;

//最後のデバイスのソースコード書き換え
 std::ifstream ifs_last(filename.c_str());
flag = true;
while(ifs_last && getline(ifs_last, str)){
  if(str.find("for")!= std::string::npos && flag){
    if(str.find("SIZE")!= std::string::npos){
      sprintf(tmp,"%d",rest);
      str.replace(str.find("SIZE"), 4, tmp);
      flag = false;
    }

  }
  while(str.find("SIZE")!= std::string::npos){
      sprintf(charsize,"%d", size);
      str.replace(str.find("SIZE"), 4, charsize);
  }
  new_str_last += str;
  new_str_last += "\n";

}
source_last = new_str_last.c_str();
 std::cout << new_str_last << std::endl;

//build
cl_program program[num_platforms+1];
for(int i =0,count = 0; i < num_platforms;i++)
{
  if(i != num_platforms-1 || rest == delta){
    program[i] = clCreateProgramWithSource(context[i], 1, (const char**)&source1, NULL, &status);
    std::cout << "CreateProgram[" << i << "] : " << status << std::endl;

    status = clBuildProgram(program[i], num_devices[i], &device_list[count],NULL,NULL,NULL);
    count += num_devices[i];

    std::cout << "Program[" << i << "]Build : " << status << std::endl;
  }

  else if(i == num_platforms-1 && rest != delta){
    program[i] = clCreateProgramWithSource(context[i], 1, (const char**)&source1, NULL, &status);
    std::cout << "CreateProgram[" << i << "] : " << status << std::endl;
    status = clBuildProgram(program[i], num_devices[i]-1, &device_list[count],NULL,NULL,NULL);
    count += num_devices[i];
    count--;
    std::cout << "Program[" << i << "]Build : " << status << std::endl;

    program[i+1] = clCreateProgramWithSource(context[i], 1, (const char**)&source_last, NULL, &status);
    std::cout << "last CreateProgram[" << i+1 << "] : " << status << std::endl;
    status = clBuildProgram(program[i+1], 1, &device_list[count],NULL,NULL,NULL);
    count += num_devices[i];
    std::cout << "last Program[" << i+1 << "]Build : " << status << std::endl;
  }
}
cl_kernel kernel[totaldev];
int count3 = 0;
for(int i = 0 ; i < num_platforms;i++){
  for(int j = 0;j < num_devices[i];j++){
    if(count3 != totaldev-1 || rest == delta){
      kernel[count3] = clCreateKernel(program[i], "calc", &status);
      std::cout << "program[" << i << "] create Kernel[" << count3 << "] : " << status<< std::endl;
      count3++;
    }
    else if(count3 == totaldev-1 && rest != delta){
      kernel[count3] = clCreateKernel(program[i+1], "calc", &status);
      std::cout << "program["<< i+1 << "] last create Kernel[" << count3<<"] : " << status << std::endl;
      count3++;
    }
  }
}

int count4 = 0;
cl_mem mem[totaldev*(num_hikisu+1)];
for(int i =0;i<num_platforms;i++){
  for(int j = 0;j<num_devices[i]*(num_hikisu+1);j++){//各プラットホームでのメモリオブジェクトカウント
    if(count4/(num_hikisu+1) != totaldev-1 || rest == delta){
      mem[count4] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(double)*delta, NULL, &status);
      std::cout << "if context[" << i << "] " << "create mem[" << count4 << "] : " << status << std::endl;
      count4++;
    }
     else if(count4/(num_hikisu+1) == totaldev-1 && rest != delta){
      mem[count4] = clCreateBuffer(context[i], CL_MEM_READ_WRITE, sizeof(double)*rest, NULL, &status);
      std::cout << "elseif context[" << i << "] " << "create mem[" << count4 << "] : " << status << std::endl;
      count4++;
    }
  }
}


//入力データ書き込み
for(int i = 0;i<totaldev;i++){
  if(i*(num_hikisu+1) != (totaldev-1)*(num_hikisu+1) || rest == delta){
    for(int j =0;j < num_hikisu;j++){
      status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+j], CL_TRUE, 0, sizeof(double)*delta, memIn[j][i], 0,NULL,NULL);
      std::cout << "if commandQueue[" << i << "]<=mem[" << i*(num_hikisu+1)+j << "] memIn[" << j << "][" << i << "] status:" << status << std::endl;
    }
  }
  else if(i*(num_hikisu+1) == (totaldev-1)*(num_hikisu+1) && rest != delta){
    for(int j = 0;j< num_hikisu;j++){
      status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+j], CL_TRUE, 0, sizeof(double)*rest, memIn[j][i], 0,NULL,NULL);
      std::cout << "elseif commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+j<<"] memIn["<<j<<"]["<<i<<"] status : "<< status << std::endl; 
    }
  }
}

//出力先データ書き込み初期化
for(int i = 0;i<totaldev;i++){
  if(i*(num_hikisu+1) != (totaldev-1)*(num_hikisu+1) || rest == delta){
    status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+num_hikisu], CL_TRUE, 0, sizeof(double)*delta, memOut, 0,NULL,NULL);
    std::cout << "if commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+num_hikisu<<"] : "<< status << std::endl;
  }
  else if(i*(num_hikisu+1) == (totaldev-1)*(num_hikisu+1) && rest != delta){
    status = clEnqueueWriteBuffer(queue[i], mem[i*(num_hikisu+1)+num_hikisu], CL_TRUE, 0, sizeof(double)*rest, memOut, 0,NULL,NULL);
    std::cout << "elseif commandQueue["<<i<<"]<=mem["<<i*(num_hikisu+1)+num_hikisu<<"] : "<< status << std::endl; 
  }
}

count4=0;
for(int i = 0;i<totaldev;i++){
  for(int j = 0;j<(num_hikisu+1);j++){
    status = clSetKernelArg(kernel[i], j, sizeof(cl_mem), (void *)&mem[i*(num_hikisu+1)+j]);
    std::cout <<"kernel["<<i<<"] mem["<<i*(num_hikisu+1)+j<<"] set : "<<status<< std::endl;
  }
}

t3 = gettimeofday_sec();
size_t globalsize[]={1024/totaldev,1024};
//size_t localsize[]={256};
for(int i = 0;i<totaldev;i++){
status= clEnqueueNDRangeKernel(queue[i], kernel[i], 2, NULL,globalsize,NULL,0,NULL,NULL);
 std::cout << "kernel done : "<<status<< std::endl;
}
/*
for(int i =0;i<totaldev;i++){
  status = clEnqueueTask(queue[i], kernel[i],NULL,NULL,NULL);
  cout << "kernel done : "<<status <<endl;
}*/
t4 = gettimeofday_sec();
 std::cout.setf( std::ios::fixed, std::ios::floatfield);
 std::cout <<"calc time = "<<t4 - t3<<" sec."<< std::endl;


for(int i=0;i<totaldev;i++){
  if((num_hikisu+1)*i+num_hikisu != totaldev*(num_hikisu+1)-1 || rest == delta){
    status = clEnqueueReadBuffer(queue[i], mem[(num_hikisu+1)*i+num_hikisu], CL_TRUE, 0, sizeof(double)*delta, &memOut[delta*i], 0, NULL, NULL);
    /*cout << "mem["<<(num_hikisu+1)*i+num_hikisu<<"] result : "<<status<<" >> "<<endl;
    for(int j = i*delta; j<(i+1)*delta;j++){
      cout << memOut[j] << " ";
    }
    cout<<endl;*/
  }
  else if((num_hikisu+1)*i+num_hikisu == totaldev*(num_hikisu+1)-1 && rest != delta){
    status = clEnqueueReadBuffer(queue[i], mem[(num_hikisu+1)*i+num_hikisu], CL_TRUE, 0, sizeof(double)*rest, &memOut[delta*i], 0, NULL, NULL);
    /*cout << "rest :"<<rest<<" mem["<<(num_hikisu+1)*i+num_hikisu<<"] last result : "<<status<<" >> "<<endl;
    for(int j = i*delta; j < i*delta+rest ; j++)
    {
      cout << memOut[j] << " ";
    }
    cout << endl;*/
  }
}
t2 = gettimeofday_sec();
 std::cout.setf(std::ios::fixed, std::ios::floatfield);
 std::cout <<"total time = "<<t2 -t1<<" sec."<< std::endl;


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
 std::cout.setf( std::ios::fixed, std::ios::floatfield);
 std::cout <<"total time = "<<t2 -t1<<" sec."<< std::endl;

return memOut;
}

