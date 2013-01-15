#include"main.h"
#define SIZE  10000

using namespace std;
int main(){

  unsigned int x, y;
  double *a = new double[SIZE*SIZE];
  double *b = new double[SIZE*SIZE];
  double *c;// = new double[SIZE*SIZE];

  for(y = 0;y <SIZE;y++){
    for(x = 0;x < SIZE;x++){
      a[y * SIZE + x] = y*SIZE+x; 
      b[y * SIZE + x] = y*SIZE+x;
      //c[y * SIZE + x] = 0;
    }
  }

  //1.カーネルプログラム指定
  string filename="before.cl";
  //2.オブジェクト生成？？？
  clapi cl(filename);
  //3.メンバ関数実行
  //cl.auto(入力数, データ１のdouble型配列の個数, データ１の配列のアドレス, データ２の配列の個数, データ２の配列のアドレス, ....)

  c = cl.clauto(2, SIZE*SIZE, a, SIZE*SIZE, b);

  //結果表示
  cout<<"加算結果"<<endl;
  for(int i = 0 ; i < SIZE ; i++){
    for(int j = 0 ; j < SIZE ; j++){
      cout<< c[i*SIZE+j] << " " ;
    }
    cout << endl;
  }

//  cout << c[SIZE*SIZE-1] << endl;

  delete[] a;
  delete[] b;
  delete[] c;
}
