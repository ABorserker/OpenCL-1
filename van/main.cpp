#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include"clapi.h"

#ifdef __APPLE__
#include<OpenCL/opencl.h>
#else
#include<CL/cl.hpp>
#endif //__APPLE

#define MAX_SOURCE_SIZE (0x100000)
#define SIZE 1024


int main(int argc, char **argv){

  unsigned int x, y;
  double *input1 = new double[SIZE*SIZE];
  double *input2 = new double[SIZE*SIZE];
  double *output;

  for(y = 0; y < SIZE; y++) {
    for(x = 0;x < SIZE;x++){
      input1[y * SIZE + x] = y*SIZE+x; 
      input2[y * SIZE + x] = y*SIZE+x; 
    }
  }

  //1.カーネルプログラム指定
  std::string filename="calc.cl";
  //2.オブジェクト生成
  clapi cl(filename);
  //3.メンバ関数実行
  //cl.auto(入力数, データ１のdouble型配列の個数, データ１の配列のアドレス, 
  //データ２の配列の個数, データ２の配列のアドレス, ....)

  cl.doOpenCL_classify();

  output = cl.clauto(2, SIZE*SIZE, input1, SIZE*SIZE, input2);

  //結果表示
  std::cout << "加算結果" << std::endl;
  for(int i = 0; i < SIZE; i++) {
    for(int j = 0; j < SIZE; j++) {
      std::cout<< output[i*SIZE+j] << " ";
    }
    std::cout << std::endl;
  }

  delete[] input2;
}
