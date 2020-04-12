#ifndef QUADRATICFIT_H
#define QUADRATICFIT_H

int matrix_trans(double matrix[3][4]);
void to_matrix(double *x, double *y, int count, double matrix_out[3][4]);
bool Quadfit(double *x, double *y, int count, double &a, double &b, double &c);
bool RQuadfit(double *x, double *y, int count, double a, double b, double c, double &R);

bool Linefit(double *x, double *y, int count, double &k, double &b);
void RLinefit(double *x, double *y, int count, double k, double b, double &R);
bool Linefit_PZ(double points[][2], int count, double &k, double &b);

#endif // QUADRATICFIT_H
