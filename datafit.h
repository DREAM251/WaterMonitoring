#ifndef QUADRATICFIT_H
#define QUADRATICFIT_H

#include "../../export.h"

int WINDLL_EXPORT matrix_trans(double matrix[3][4]);
void WINDLL_EXPORT to_matrix(double *x, double *y, int count, double matrix_out[3][4]);

bool WINDLL_EXPORT Quadfit(double *x, double *y, int count, double &a, double &b, double &c);
bool WINDLL_EXPORT RQuadfit(double *x, double *y, int count, double a, double b, double c, double &R);

bool WINDLL_EXPORT Linefit(double *x, double *y, int count, double &k, double &b);
void WINDLL_EXPORT RLinefit(double *x, double *y, int count, double k, double b, double &R);
bool WINDLL_EXPORT Linefit_PZ(double points[][2], int count, double &k, double &b);

#endif // QUADRATICFIT_H
