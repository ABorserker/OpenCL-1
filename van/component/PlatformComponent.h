#ifndef __PLATFORM_COMPONENT_H_
#define __PLATFORM_COMPONENT_H_

#include <vector>
#include <CL/cl.hpp>

class PlatformComponent
{
 private:
  cl::Platform *platform;

  //! device container
  std::vector<cl::Device> devices;

 public:
  PlatformComponent(cl::Platform & platform)
    : platform(&platform)
  {
    this->platform->getDevices(CL_DEVICE_TYPE_ALL, &devices);
  }

  ~PlatformComponent()
   {

   }

  void printInfo(void)
  {
    std::string platform_name;
    platform->getInfo(CL_PLATFORM_NAME, &platform_name);
    std::cout << "\n" 
	      << "========================================\n"
	      << platform_name << " Device List\n"
	      << "========================================\n";

    for( std::vector<cl::Device>::iterator it = devices.begin();
	 it != devices.end();
	 it++ )
    {
      std::string device_name;
      it->getInfo<std::string>(CL_DEVICE_NAME, &device_name);
      std::cout << "- " << device_name << std::endl;
    }
    std::cout << std::endl;

  }
};

#endif
