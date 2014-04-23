#include"main.h"
#define SIZE 1024 

using namespace std;
int main(){

  unsigned int x, y;
  double *input1 = new double[SIZE*SIZE];
  double *input2 = new double[SIZE*SIZE];
  double *output;

  for(y = 0;y <SIZE;y++){
    for(x = 0;x < SIZE;x++){
      input1[y * SIZE + x] = y*SIZE+x; 
      input2[y * SIZE + x] = y*SIZE+x; 
    }
  }

  //1.カーネルプログラム指定
  string filename="calc.cl";
  //2.オブジェクト生成
  clapi cl(filename);
  //3.メンバ関数実行
  //cl.auto(入力数, データ１のdouble型配列の個数, データ１の配列のアドレス, 
  //データ２の配列の個数, データ２の配列のアドレス, ....)

  output = cl.clauto(2, SIZE*SIZE, a, SIZE*SIZE, b);

  //結果表示
  cout<<"加算結果"<<endl;
  for(int i = 0 ; i < SIZE ; i++){
    for(int j = 0 ; j < SIZE ; j++){
      cout<< output[i*SIZE+j] << " " ;
    }
    cout << endl;
  }

  delete[] input2;
}
