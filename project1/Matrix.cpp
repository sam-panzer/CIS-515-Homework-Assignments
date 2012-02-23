#include "Gauss.h"
#include <iostream>

using std::cout;
int main() {

  double backing[4][4] = {{1.0,2.0,3.0,4.0},
                        {5.0,6.0,8.0,8.0},
                        {7.0,2.0,5.0,1.0},
                        {1.0,2.0,0.0,6.0}};
  Matrix<4,4> mat(backing);
  Matrix<4,4> id = Matrix<4,4>::id();
  id.printMatrix();
  cout << "\n";
  mat.gaussElim();
  mat.printMatrix();
  cout << "\n";
  double res[4];
  double ts[4] = {4,3,2,1};
  mat.backSubstitute(res,ts);

  cout << "Got back:\n";
  (id * res).printMatrix();
  cout << "\nResult:\n";
  Matrix<4,1> m2 = mat * res;
  m2.printMatrix();

  return 0;
}
