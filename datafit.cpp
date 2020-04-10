#include "datafit.h"
#include "math.h"
#include "memory.h"


/*
 *函数名：void to_matrix(double matrix_in[5][2],int count,double matrix_out[3][4])
 *功能：利用最小二乘法计算二次拟合值，输入五组数据matrix_in转换成增广矩阵matrix_out
 *|x1,y1|
 *|x2,y2|       |x11,x12,x13:x14|
 *|x3,y3|  ->   |x21,x22,x23:x24|
 *|x4,y4|       |x31,x32,x33:x34|
 *|x5,y5|
*/
void to_matrix(double *x,double *y,int count,double matrix_out[3][4])
{
    double value[8];
    memset(value , 0 , sizeof(double)*8);

    /*计算累计值*/
    for(int i=0;i<count;i++)
    {
        value[0] += pow(x[i],4);
        value[1] += pow(x[i],3);
        value[2] += pow(x[i],2);
        value[3] += x[i];
        value[4] += 1;

        value[5] += pow(x[i],2)*y[i];
        value[6] += x[i]*y[i];
        value[7] += y[i];
    }
    /*矩阵赋值-对称矩阵*/
    matrix_out[0][0] = value[0];
    matrix_out[1][0] = matrix_out[0][1] = value[1];
    matrix_out[2][0] = matrix_out[1][1] = matrix_out[0][2] = value[2];
    matrix_out[2][1] = matrix_out[1][2] = value[3];
    matrix_out[2][2] = value[4];

    matrix_out[0][3] = value[5];
    matrix_out[1][3] = value[6];
    matrix_out[2][3] = value[7];
}

/*
 *函数名：int matrix_trans(double matrix[3][4])
 *功能：将3*4的增广矩阵变换成最简形式,如下图
 *返回值：返回0代表转换完成，其他值代表异常
 *|x11,x12,x13:x14|     |1,0,0:a|
 *|x21,x22,x23:x24| ->  |0,1,0:b|
 *|x31,x32,x33:x34|     |0,0,1:c|
*/
int matrix_trans(double matrix[3][4])
{
     /*消除x31，x21*/
    double dbtemp1 = matrix[2][0] , dbtemp2 = matrix[0][0] ,    \
            dbtemp3 = matrix[1][0];
    if(dbtemp2==0)
        return -1;
    for(int i=0;i<4;i++)
    {
        matrix[2][i] -= matrix[0][i]*dbtemp1/dbtemp2;
        matrix[1][i] -= matrix[0][i]*dbtemp3/dbtemp2;
    }
    /*消除x32*/
    dbtemp1 = matrix[2][1];
    dbtemp2 = matrix[1][1];
    if(dbtemp2==0)
        return -1;
    for(int i=1;i<4;i++)
    {
        matrix[2][i] -= matrix[1][i]*dbtemp1/dbtemp2;
    }
    /*消除x23，x13*/
    dbtemp1 = matrix[1][2];
    dbtemp2 = matrix[2][2];
    dbtemp3 = matrix[0][2];
    if(dbtemp2==0)
        return -1;
    for(int i=2;i<4;i++)
    {
        matrix[1][i] -= matrix[2][i]*dbtemp1/dbtemp2;
        matrix[0][i] -= matrix[2][i]*dbtemp3/dbtemp2;
    }
    /*消除x12*/
    dbtemp1 = matrix[0][1];
    dbtemp2 = matrix[1][1];
    if(dbtemp2==0)
        return -1;
    for(int i=1;i<4;i++)
    {
        matrix[0][i] -= matrix[1][i]*dbtemp1/dbtemp2;
    }
    /*x11,x22,x33变为1*/
    if(!matrix[0][0] || !matrix[1][1] || !matrix[2][2])
        return 1;
    matrix[0][3] /= matrix[0][0];
    matrix[0][0] /= matrix[0][0];

    matrix[1][3] /= matrix[1][1];
    matrix[1][1] /= matrix[1][1];

    matrix[2][3] /= matrix[2][2];
    matrix[2][2] /= matrix[2][2];
    return 0;
}

/*
 *函数名：bool Quadfit(double points[][2],int count,double &a,double &b,
 *                     double &c)
 *功能：利用最小二乘法计算二次拟合参数
 *返回值：返回true代表转换完成，其他值代表异常
*/
bool Quadfit(double *x,double *y,int count,double &a,double &b,double &c)
{
    double matrix[3][4];

    if(count < 3)
        return false;
    to_matrix(x,y,count,matrix);
    if(matrix_trans(matrix))
        return false;

    a = matrix[0][3];
    b = matrix[1][3];
    c = matrix[2][3];
    return true;
}

/*
 *函数名：bool RQuadfit(double points[][2],int count,double a,double b,
 *                     double c,double &R)
 *功能：计算二次拟合优度R
 *返回值：返回true代表转换完成，其他值代表异常
*/
bool RQuadfit(double *x,double *y,int count,double a,double b,double c,double &R)
{
    double sum0 = 0.0 , sum1 = 0.0,sum2 = 0.0,dy=0.0;
    for(int i=0;i<count;i++){
        sum0 += y[i];
    }
    dy = sum0/count;
    for(int i=0;i<count;i++)
    {
        sum1 += pow(a*pow(x[i],2)+b*x[i]+c - dy, 2);
        sum2 += pow(y[i] - dy ,2);
    }
    if(sum2 == 0.0)
        return false;
    R = sum1/sum2;

    return true;
}

/*
 *函数名：bool Linefit(double points[][2],int count,double &k,double &b)
 *功能：计算一次线性拟合
 *返回值：返回true代表转换完成，其他值代表异常
*/
bool Linefit(double *x ,double *y,int count,double &k, double &b)
{
    double av_x, av_y;
    double L_xx, L_xy, L_yy;               //声明变量
    //变量初始化
    av_x = 0;                             //X的平均值
    av_y = 0;                             //Y的平均值
    L_xx = 0;
    L_yy = 0;
    L_xy = 0;
    for(int i=0; i<count; i++)           //计算XY的平均值
    {
        av_x += x[i]/count;
        av_y += y[i]/count;
    }
    for (int i=0; i<count; i++)               //计算Lxy Lyy和Lxy
    {
        L_xx += (x[i]-av_x) * (x[i]-av_x);
        L_yy += (y[i]-av_y) * (y[i]-av_y);
        L_xy += (x[i]-av_x) * (y[i]-av_y);
    }

    if(L_xx == 0.0)
        return false;
    k =  L_xy / L_xx;
    b =  av_y - L_xy * av_x / L_xx;
    return true;
}

/*
 *函数名：bool RLinefit(double points[][2],int count,double k,double b,
 *                         double &R)
 *功能：计算一次拟合优度R
 *返回值：返回true代表转换完成，其他值代表异常
*/
void RLinefit(double *x,double *y,int count,double k,double b,double &R)
{
    double sum0=0.0,sum1 = 0.0,sum2 = 0.0,dy;

    for(int i=0;i<count;i++){
        sum0 += y[i];
    }
    dy = sum0/count;
    for(int i=0;i<count;i++)
    {
        sum1 += pow(k*x[i]+b - dy,2);
        sum2 += pow(y[i] - dy,2);
    }
    if(sum2 == 0)
        R = 0.0;
    else
        R = sum1/sum2;
}

/*
 *函数名：bool Linefit_PZ(double points[][2],int count,double &k,double &b)
 *功能：计算过原点的一次拟合
 *返回值：返回true代表转换完成，其他值代表异常
*/
bool Linefit_PZ(double points[][2],int count,double &k, double &b)
{
    b = 0.0;k = 0.0;
    double sumxx = 0.0 , sumxy = 0.0;
    for(int i=0;i<count;i++)
    {
        sumxx += points[i][0]*points[i][0];
        sumxy += points[i][0]*points[i][1];
    }
    if(sumxx == 0.0)
        return false;
    k = sumxy/sumxx;
    return true;
}
