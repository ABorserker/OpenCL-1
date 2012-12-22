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
  status = clGetPlatformIDs(0, NULL, &num_platforms);
  platforms = new cl_platform_id[num_platforms];
  status = clGetPlatformIDs(num_platforms, platforms, NULL);
  cout << "num_platforms : " << num_platforms << endl;

  cl_uint num_devices[num_platforms];
  cl_device_id *device_list;
  
  for(int i = 0 ; i< num_platforms; i++)
  {
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices[i]);
  }

  cl_uint totaldev = 0;
  for(int i= 0; i < num_platforms;i++)
  {
    totaldev += num_devices[i];
  }

  device_list = new cl_device_id[totaldev];
  cl_uint num;
  for(int i = 0,count=0 ; i< num_platforms; i++)
  {
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices[i], &device_list[count], &num);
    count+=num;
    cout << "Platform"<<i<<" num_devices : "<<num <<endl;
  }

  for(int i =0;i<totaldev;i++)
  {
    cout <<"device_list["<<i<<"] : "<<device_list[i]<<endl;
  }

  //cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],0};
  //cl_context_properties properties2[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[1],0};

  //cout << "totaldev : "<<totaldev<<endl;



  cl_context context[num_platforms];//プラットフォームの数だけコンテキスト作成
  for(int i=0;i<num_platforms;i++){
    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i],0};
    context[i] = clCreateContext(properties,num_devices[i],& device_list[i], NULL, NULL, &status); 
    cout<<"create context"<<i<<" :"<<status<<endl;
  }

  //context = clCreateContext(properties,1, device_list, NULL, NULL, &status); 
  //cout<<status<<endl;

  //context2 = clCreateContext(properties2,4,& device_list[1], NULL, NULL, &status);

  //cout << status <<endl;


  //for(int i=0;i<totaldev;i++)
  //  cout<<"device_list: "<<sizeof(device_list)<<"  "<<sizeof(device_list[i])<<endl;



  // clCreateCommandQueue(context, device_list[0], 0, &status);
  //
  //cout <<"CPU context: "<< status <<endl;

  cl_command_queue queue[totaldev];
  for(int j=0,count=0;j<num_platforms;j++){
    for(int i=0;i<num_devices[j];i++){

      queue[count]= clCreateCommandQueue(context[j], device_list[count], 0, &status);

      cout <<"Command Queue"<<count<<": "<<status <<endl;
      count++;
    }
  }
}

