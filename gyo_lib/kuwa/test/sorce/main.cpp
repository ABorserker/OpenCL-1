#include<iostream>
#include<string>
#include<fstream>

using namespace std;

int main(){
  ifstream ifs("before.cl");
  string str;

  int totaldev = 5;
  char tmp;
  while(ifs && getline(ifs, str)){
    if(str.find("for")!=string::npos){
      if(str.find("SIZE")!=string::npos){
        sprintf(tmp,"SIZE/%d",totaldev);
        str.replace(str.find("SIZE"), 4, tmp);
        //break;
      }
    }
    cout << str << endl;
  }



}
