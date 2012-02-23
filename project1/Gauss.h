#ifndef __GAUSS_H__
#define __GAUSS_H__

#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cmath>

const double tolerance = 1e-10;
using std::cout;

double round(double d) {
  const double e = fabs(d);
  if (e - int(e) < tolerance) {
    return (d >= 0) ? int(d) : int(d) + 1;
  }
  return d;
}

double dot(double *v1, double *v2, unsigned int len) {
  double d = 0;
  for (unsigned int i = 0; i < len; i++) {
    d += v1[i] * v2[i];
  }
  return d;
}

template<int m, int n>
class Matrix {
  private:
    double mat [m][n];

  public:
    double rows() const;
    double columns() const;

    double * operator[](int index);
    const double *operator[](int index) const;
    template<int p> Matrix<m,p> operator*(const Matrix<n,p>) const;
    Matrix<m,1> operator*(const double vec[n]) const;

    void swapRows(unsigned int a, unsigned int b);
    void gaussElim();

    // id isn't available for non-square matrices!
    static Matrix<m,n> id();

    void printMatrix();
    double *backSubstitute(double dest[m], double target[m]);
    void luSolve(double dest[m], double target[m]);

    Matrix();
    Matrix(const double src[m][n]);
    ~Matrix();
};


template<int m,int n>
Matrix<m,n> Matrix<m,n>::id() {
  Matrix<m,n> mat;
  const int l = m < n ? m : n;
  for (int i = 0; i < l; i++) {
    mat[i][i] = 1;
  }
  return mat;
}

template<int m, int n>
template<int p>
Matrix<m,p> Matrix<m,n>::operator*(const Matrix<n,p> m2) const {
  double res[m][p];
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < p; j++) {
      for (int k = 0; k < n; k++) {
        res[i][j] += mat[i][k] * m2[k][j];
      }
      res[i][j] = res[i][j];
    }
  }
  return Matrix<m,p>(res);
}

template<int m, int n>
Matrix<m,1> Matrix<m,n>::operator*(const double vec[n]) const {
  double res[m][1];
  for (int i = 0; i < m; i++) {
    for (int k = 0; k < n; k++) {
      res[i][0] += mat[i][k] * vec[k];
    }
    res[i][0] = res[i][0];
  }
  return Matrix<m,1>(res);
}


template<int m, int n>
double Matrix<m,n>::rows() const {
  return m;
}
template<int m, int n>
double Matrix<m,n>::columns() const {
  return n;
}

template<int m, int n>
double* Matrix<m,n>::operator[](int index) {
  if (index < 0 || index > m) {
    return NULL;
  }
  return mat[index];
}

template<int m, int n>
const double* Matrix<m,n>::operator[](int index) const {
  if (index < 0 || index > m) {
    return NULL;
  }
  return mat[index];
}

template<int m, int n>
Matrix<m,n>::Matrix() {
  memset(mat, 0, m* n * sizeof(double));
}

template<int m, int n>
Matrix<m,n>::Matrix(const double src[m][n]) {
  memcpy(mat, src, m* n * sizeof(double));
}

template<int m, int n>
Matrix<m,n>::~Matrix() {

}


// Swaps rows a and b in matrix mat
template<int m, int n>
void Matrix<m,n>::swapRows(unsigned int a, unsigned int b) {
  for (int i = 0; i < n; i++) {
    double d = mat[a][i];
    mat[a][i] = mat[b][i];
    mat[b][i] = d;
  }
}
// Gaussian elimination
template<int m, int n>
void Matrix<m,n>::gaussElim() {
  for (int c = 0; c < n; c++) {
    int iMax = 0;
    double vMax = 0.0;
    for (int r = c; r < n; r++) {
      int v = fabs(mat[r][c]);
      if(v > vMax) {
        vMax = v;
        iMax = r;
      }
    }
    if (vMax == 0)
      continue; 
    swapRows(c, iMax);

    for (int i = c + 1; i < m; i++) {
      for (int j = c  + 1; j < n; j++) {
          mat[i][j] -= mat[c][j] * (mat[i][c] / mat[c][c]);
      }
      mat[i][c] = 0;
    }
  }
}

// Prints the matrix mat. Not the prettiest printer, but better than just
// writing to stdout
template<int m, int n>
void Matrix<m,n>::printMatrix() {
  cout << std::setiosflags(std::ios :: left);
  cout << std::setprecision(4);
  for (int i = 0; i < m; i++) {
    cout << "[ ";
    for (int j = 0; j < n; j++) {
      cout << std::setw(8) << round(mat[i][j]);
    }
    cout << " ]\n";
  }
}

// Assumes that mat is a triangular matrix with a nonzero trace
template<int m, int n>
double *Matrix<m,n>::backSubstitute(double dest[m], double target[m]) {
  for (int j = m - 1; j >= 0; j--) {
    double d = dot(dest + (j + 1), mat[j] + (j + 1), m - (j + 1));
    dest[j] = (target[j] - d) / mat[j][j];
  }
  return dest;
}

// Assumes that mat is a tridiagonal square matrix
template<int m, int n>
void Matrix<m,n>::luSolve(double dest[m], double target[m]) {
  double w[m];
  double z[m];

  z[0] = mat[0][0 + 1] / mat[0][0];
  w[0] = target[0] / mat[0][0];
  for (int i = 1; i < m - 1; i++) {
    z[i] = mat[i][i + 1] / (mat[i][i] - mat[i][i - 1] * z[i-1]);
    double num   = (target[i] - mat[i][i-1] * w[i-1]);
    double denom = (mat[i][i] - mat[i][i-1] * z[i-1]);
    w[i] =  num / denom;
  }
  z[m - 1] = mat[m - 1][m-1] - mat[m-1][m-2] * z[m-2];
  w[m - 1] = (target[m-1]   - mat[m-1][m-2] * w[m-2]) / 
             (mat[m-1][m-1] - mat[m-1][m-2] * z[m-2]);

  dest[m - 1] = w[m - 1];
  for (int i = m - 2; i >= 0; i--) {
    dest[i] = w[i] - z[i] * dest[i + 1];
  }
}

#endif
