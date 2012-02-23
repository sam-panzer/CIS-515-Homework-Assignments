#include "Gauss.h"
#include <iostream>

using std::cout;
int main() {

  double backing[4][4] = {{1.0,2.0,3.0,4.0},
                          {5.0,6.0,8.0,8.0},
                          {7.0,2.0,5.0,1.0},
                          {1.0,2.0,0.0,6.0}};

  double tridiagonal[10][10] = {
    {1, 86, 0, 0, 0, 0, 0, 0, 0, 0},
    {736842, 2, 26, 0, 0, 0, 0, 0, 0, 0},
    {0, -248637, 3, 42, 0, 0, 0, 0, 0, 0},
    {0, 0, 9, 4, 3.14159, 0, 0, 0, 0, 0},
    {0, 0, 0, -32, 5, 20.25, 0, 0, 0, 0},
    {0, 0, 0, 0, -0x32, 6, 12.137, 0, 0, 0},
    {0, 0, 0, 0, 0, 032, 7, 107432.7, 0, 0},
    {0, 0, 0, 0, 0, 0, 3.2, 8, -2643, 0},
    {0, 0, 0, 0, 0, 0, 0, 63, 9, 4.6e-4},
    {0, 0, 0, 0, 0, 0, 0, 0, -6291993, 10}
  };
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

  Matrix<10,10> tridiag(tridiagonal);

  cout << "\nNow to test tridiagonal solving. Here's the GaussElim solution\n";
  tridiag.printMatrix();
  double resTen[10];
  double targetTen[10] = {10,9,8,7,6,5,4,3,2,1};

  tridiag.gaussElim();
  tridiag.backSubstitute(resTen, targetTen);
  cout << "Got back:\n";
  (tridiag.id() * resTen).printMatrix();
  cout << "\nResult:\n";
  (tridiag * resTen).printMatrix();

  // And using the tridiagonal solution...
  cout << "\nAnd the tridiagonal solution\n";
  tridiag = Matrix<10,10>(tridiagonal);
  tridiag.luSolve(resTen, targetTen);
  cout << "Got back:\n";
  (tridiag.id() * resTen).printMatrix();
  cout << "\nResult:\n";
  (tridiag * resTen).printMatrix();

  return 0;
}
