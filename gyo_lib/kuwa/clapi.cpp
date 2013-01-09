/*
 * clapi.cpp
 *
 */

#include "clapi.h"

using namespace std;

clapi::clapi() {

}


clapi::clapi(string tmp){
  filename = tmp;
}


clapi::~clapi(){
  clReleaseMemObject(memOut);
  for(int i = 0; i < num_hikisu; i++) clReleaseMemObject(memIn[i]);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}


bool clapi::clauto(int n, ...) {
  num_hikisu = n;
  va_list args;
  va_start(args, n);

  for (int t = 0; t < n ; t++) { //forでループさせ渡された引数１個ずつについて処理する。すべてsに+=していく。
    size[t] = va_arg(args,int);//double型配列の個数
    s[t] = va_arg(args,double*); //可変長引数を取り出す. 第一引数はva_list型の変数。第二引数には取り出す型。
  }
  va_end(args);

  doOpenCL();

  return true;
}


bool clapi::doOpenCL() {
  status = clGetPlatformIDs(4, platforms, &num_platforms);
  if (status != CL_SUCCESS || num_platforms <= 0) {
    fprintf(stdout, "clGetPlatformIDs failed.\n");
    printf("%d\n", status);
    return false;
  }
  // 最初の要素として返されたプラットフォームIDを、プロパティにセットする
  cl_context_properties properties[num_platforms*2];
  for(int i = 0;i < num_platforms ;i++){
    properties[i*2] = CL_CONTEXT_PLATFORM;
    properties[i*2+1] = (cl_context_properties)platforms[i];
  }
  properties[num_platforms*2-1] = 0;

  //1.デバイスの取得
  cout <<"num_platforms : "<<num_platforms<<endl;
  const int tmp = num_platforms;
  
  cl_device_id *device_list= new cl_device_id[10];
  cl_device_id *device_listcpu = new cl_device_id[4];//CPUのリスト
  cl_device_id *device_listgpu = new cl_device_id[4];//GPUのリスト
 
  num_device = 0;
  ndt = 0;
    for(int j = 0;j < num_platforms;j++)
    {
      status = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, 4, &device_list[num_device], &ndt);
      num_device = ndt + num_device ;
      if (status != CL_SUCCESS || num_device <= 0) {
        fprintf(stdout, "clGetDeviceIDs failed.\n");
        printf("%d\n", status);
        return false;
      }
    }
status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 4, device_listcpu, NULL);
status = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_ALL, 4, device_listgpu, NULL);


  ////////////////////
  cl_char buff[1024];
  for(int j=0;j<num_device;j++){
    clGetDeviceInfo(device_list[j],CL_DEVICE_NAME,sizeof(buff),buff,NULL);
    cout<< "Device Name = "<<buff<<endl;
  }
  ////////////////
//for(int j = 0 ;j < num_platforms; j++){
  
  context = clCreateContext(properties, sizeof(device_list)/sizeof(device_list[0]), device_list, NULL,NULL, &status);
  
  if (status != CL_SUCCESS) {
    cout << "clCreateContext failed\nError Code: " << status << endl;
    return false;
  }
//}

  // cl_char buff[1024];
  cout << "num_device : "<< num_device<<endl;
  cl_device_id *getinfo = new cl_device_id[num_device];

  cout << "new getinfo"<<endl;
  for(int i= 0;i< num_device;i++ ){
    cout << getinfo[i] << endl;
  }
  status = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(getinfo), getinfo, NULL);

  cout << "status : "<<status <<endl;
  cout << "num_device of context: "<< num_device << endl;

  for(int i= 0;i< num_device;i++ ){
    //cout <<"getinfo"<< i<<getinfo[i] << endl;
  }

  cl_char buff1[1024];
  for(int i = 0; i < num_device-1 ;i++){
    cout <<"device "<<i;
    //status = clGetDeviceInfo(getinfo[i], CL_DEVICE_NAME, sizeof(buff1),buff1, NULL);

    cout << " : Device Name = "<<buff1 << " status: "<<status<<endl;
  }
  cout << "suru-" << endl;

  //3.コマンドキューの作成
  // queue = clCreateCommandQueue(context, *device_list, 0, &status);
//for(int i=0; i < num_device;i++){
  queue = clCreateCommandQueue(context, device_list[0], 0, &status);
  queue2 = clCreateCommandQueue(context, device_list[1], 0, &status);
  queue3 = clCreateCommandQueue(context, device_list[2], 0, &status);
//}

  if (status != CL_SUCCESS) {
    cout << "clCreateCommandQueue failed\nError Code: " << status << endl;
    return false;
  }

  //4.プログラムオブジェクトの作成
  FILE *fp;
  size_t source_size;
  char *source_str;

  fp = fopen(filename.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "Failed to leas kernel.\n");
    return false;
  }
  source_str = (char *) malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  program = clCreateProgramWithSource(context, 1, (const char**) &(source_str), &source_size, &status);
  if (status != CL_SUCCESS) {
    cout << "clCreateProgramWithSource failed\nError Code: " << status << endl;
    return false;
  }

  cout <<"num_device : "<<num_device<<endl;
  //5.プログラムのビルド
  status = clBuildProgram(program, num_device, device_list, NULL, NULL, NULL);
  if (status != CL_SUCCESS) {
    cout << "clBuildProgram failed \nError Code: "<< status << endl;
    //      builderr();
    return false;
  }
  cout<<"build end"<<endl;


  //6.カーネルの作成
  kernel = clCreateKernel(program, "calc", &status);
  if (status != CL_SUCCESS) {
    cout << "clCreateKernel failed\nError Code: " << status << endl;
    return false;
  }


  //7メモリオブジェクトの作成
  for(int i = 0 ; i<num_hikisu ; i++)
  {
    memIn[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double)*size[i], (void*) s[i], &status);
    if (status != CL_SUCCESS) {
      cout << "clCreateBuffer failed\nError Code: " << status << endl;
      return false;
    }


  }
  memOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double)*size[1] , NULL, &status);
  if (status != CL_SUCCESS) {
    cout << "clCreateBuffer failed\nError Code: " << status << endl;
    return false;
  }


  //8.カーネルに渡す引数の設定
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &memOut);
  if (status != CL_SUCCESS) {
    cout << "clSetKernelArg failed\nError Code: " << status << endl;
    return false;
  }


  for(int i = 0; i< num_hikisu; i++)
  {
    status = clSetKernelArg(kernel,i,sizeof(cl_mem),(void *) &memIn[i]);
    if (status != CL_SUCCESS) {
      cout << "clSetKernelArg failed\nError Code: " << status << endl;
      return false;
    }
  }

  //9.カーネルの実行
  cout <<"zikou"<<endl;
  size_t globalsize[] = { size[0] };
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalsize, NULL, 0, NULL, NULL);





  status = clEnqueueNDRangeKernel(queue2, kernel, 1, NULL, globalsize, NULL, 0, NULL, NULL);


  status = clEnqueueNDRangeKernel(queue3, kernel, 1, NULL, globalsize, NULL, 0, NULL, NULL);



  if (status != CL_SUCCESS) {
    cout << "clEnqueueNDRangeKernel failed\nError Code: " << status << endl;
    return false;
  }

  //10.結果の取得
  Out = (double*) malloc(size[0] * sizeof(double));

  //ここも書きなおすべき
  status = clEnqueueReadBuffer(queue, memOut, CL_TRUE, 0, sizeof(double)*size[1], Out, 0, NULL, NULL);



  if (status != CL_SUCCESS) {
    cout << "clEnqueueReadBuffer failed\nError Code: " << status << endl;
    return false;
  }

  return true;
}


double* clapi::getOut(){
  return Out;
}

/*
   void clapi::builderr() {
   size_t logsize;
   status = clGetProgramBuildInfo(program, device_list[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &logsize);
   if (status == CL_SUCCESS) {
//ログを格納するためのバッファをアロケートする
char *logbuffer;
logbuffer = new char[logsize + 1];
if (logbuffer == NULL) {
printf("memory allocation failed.\n");
return;
}

status = clGetProgramBuildInfo(program, device_list[0], CL_PROGRAM_BUILD_LOG, logsize, logbuffer, NULL);
cout << status << endl;
if (status == CL_SUCCESS) {
logbuffer[logsize] = '\0';
cout << "build log" << endl;
cout << logbuffer << endl;
}
delete[] logbuffer;
}
else {
cout << "clGetProgramBuildInfo failed" << endl;
}
}
*/
