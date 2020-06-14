#include "smooth.h"
#include "math.h"
#include "profile.h"
#include <QDebug>

smooth::smooth(int c)
{
    DatabaseProfile profile;
    if (profile.beginSection("smooth")){
        rs1 = profile.value(QString("%1/rs1").arg(c),-1.0).toFloat();
        rs2 = profile.value(QString("%1/rs2").arg(c),-1.0).toFloat();
        rs3 = profile.value(QString("%1/rs3").arg(c),-1.0).toFloat();
    }
    threshold = 0.0;
    current = c;
}

float smooth::calc(float rs)
{
    float result = rs;
    if( rs3>0.0 )
    {
        if(fabs(rs3-rs)<rs3*threshold)
        {
            result = (rs2+rs3+rs)/3.0;
            rs1 = rs2;rs2 = rs3;rs3 = result;
        }
        else
            rs1 =  rs2 =  rs3 = -1.0;
    }else if (rs2 > 0.0){
        if(fabs(rs2-rs)<rs2*threshold)
        {
            result = (rs2+rs1+rs)/3.0;
            rs3 = result;
        }
        else
            rs1 = rs2 = rs3 = -1.0;
    }else if( rs1 > 0.0){
        if(fabs(rs1-rs)<rs1*threshold)
        {
            result = (rs1+rs)/2.0;
            rs2 = result;
        }
        else
            rs1 = rs2 = rs3 = -1.0;
    }else{
        rs1 = rs ;
    }

    DatabaseProfile profile;
    if (profile.beginSection("smooth")){
        profile.setValue(QString("%1/rs1").arg(current),rs1);
        profile.setValue(QString("%1/rs2").arg(current),rs2);
        profile.setValue(QString("%1/rs3").arg(current),rs3);
    }
    return result;
}

void smooth::setThreshold(float v)
{
    if(v<0.0)
        threshold = 0.0;
    else if(v>1.0)
        threshold = 1.0;
    else
        threshold = v;
}
