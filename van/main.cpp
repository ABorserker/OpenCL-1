#include"main.h"

using namespace std;
int main(){
  int fvar=4096, fwid=4096, lvar=4096, lwid=4096;
  double *mtrx1,*mtrx2,*Out;
  FILE *fp;
  if ((fp = fopen("inputD.txt","r")) == NULL)
  {
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
  }
  int size;
  fscanf(fp, "%d", &size);

  mtrx1 = (double*) malloc(size * size * sizeof(double));
  mtrx2 = (double*) malloc(size * size * sizeof(double));
  Out = (double*) malloc(size * size * sizeof(double));

  if(mtrx1==NULL) return 1;
  if(mtrx2==NULL) return 1;
  if(Out==NULL) return 1;

  int i, j;
  for(i = 0; i < size; i++)
  {
    for(j = 0; j < size; j++)
    {
      Out[i * size + j] = 0;
      fscanf(fp, "%lf", &mtrx1[i * size + j]);
    }
  }

  for(i = 0; i < size; i++)
  {
    for(j = 0; j < size; j++)
    {
      fscanf(fp,"%lf", &mtrx2[i * size + j]);
    }
  }

  fclose(fp);

  kotake
    //1.カーネルプログラム指定
    string filename="test.cl";
  //2.オブジェクト生成？？？
  clapi cl(filename);
  //3.メンバ関数実行
  //cl.auto(入力数, データ１のdouble型配列の個数, データ１の配列のアドレス, データ２の配列の個数, データ２の配列のアドレス, ....)

  if(!cl.clauto(2, size*size, mtrx1, size*size, mtrx2))return -1;
  //cl.doOpenCL();
  //  iif(cl.hikisu != true || doOpenCL != true) return 0;

  Out = cl.getOut();

  //結果表示
  cout<<"加算結果"<<endl;
  for(int i = 0 ; i < fvar ; i++){
    for(int j = 0 ; j < fwid ; j++){
      cout<< Out[i*fwid+j] << " " ;
    }
    cout << endl;
  }

  free(mtrx1);
  free(mtrx2);
  free(Out);
}
