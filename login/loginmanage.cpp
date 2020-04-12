#include "loginmanage.h"
#include "md5.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


LoginManage::LoginManage()
{
    initCode = idcode = 0;

    //creat file if it's not existed;
    FILE *fl = fopen(pwfilename,"r");
    if(fl == NULL){
        fl = fopen(pwfilename,"w");
        changeDePassword("admin","123456");
        changeDePassword("guest","111111");
    }
    if(fl != NULL)
        fclose(fl);
}

LoginManage::~LoginManage()
{
}

//校验密码
int LoginManage::check(const char* user,const char* password)
{
    char stgPW[PASSWORD_LEN]={0},dePW[PASSWORD_LEN]={0};
    if(!findUser(user,stgPW))
        return 0;

    unsigned char temp1[PASSWORD_LEN],temp2[PASSWORD_LEN];
    strcpy((char*)temp1,password);
    MD5Test16(temp1,strlen(password),temp2);
    Hex2String((char*)temp2,16,dePW);

    if(strcmp(stgPW,dePW)==0)
        return 1;
    return 0;
}

//修改密码
int LoginManage::modify(const char *user, const char *password, const char *newpassword)
{
    if(check(user,password)==0)
        return 0;
    changeDePassword(user,newpassword);
    return 1;
}

/*
 *函数名：int calcID(void)
 *功能：根据随机码（initCode）计算验证码（idcode）
 */
int LoginManage::calcID(void)
{
    int dat[4],code[4];

    dat[3] = initCode%10;
    dat[2] = (initCode/10)%10;
    dat[1] = (initCode/100)%10;
    dat[0] = (initCode/1000)%10;

    code[0] = (dat[0]*dat[1]+dat[2]+dat[3])%10;
    code[1] = (dat[1]*dat[2]+dat[3]+dat[0])%10;
    code[2] = (dat[2]*dat[3]+dat[0]+dat[1])%10;
    code[3] = (dat[3]*dat[0]+dat[1]+dat[2])%10;

    return code[0]*1000+code[1]*100+code[2]*10+code[3];

}

/*
 *函数名：int getMacCode(void)
 *功能：产生新的随机码（initCode）
 */
int LoginManage::getMacCode(void)
{
    srand((int)time(0));
    initCode = rand()%10000;
    idcode = calcID();
    return initCode;
}

//根据用户名从文件中读取密码
bool LoginManage::findUser(const char *user, char *password)
{
    bool ret = 0;
    FILE *fl = fopen(pwfilename,"r");
    if(fl == NULL)
        return 0;

    char *strFile = new char[1024];
    char storeUser[MAX_USERNAME_LEN]={0};
    char storePassword[PASSWORD_LEN]={0};
    char *pl = strFile,*pr = NULL;

    size_t rc = fread(strFile,sizeof(char),1024,fl);
    if(rc==0)
        goto _FU_END_;
    while(pl<strFile+rc)
    {
        memset(storeUser,0,sizeof(storeUser));
        memset(storePassword,0,sizeof(storePassword));
        //find :
        pr = strchr(pl,':');
        if(pr == NULL)
            break;
        strncpy(storeUser,pl,pr-pl>MAX_USERNAME_LEN?MAX_USERNAME_LEN:pr-pl);

        //find \n
        pl = pr+1;
        pr = strchr(pl,'\n');
        if(pr == NULL)
            break;
        strncpy(storePassword,pl,pr-pl>PASSWORD_LEN?PASSWORD_LEN:pr-pl);
        pl = pr+1;

        //compare
        if(strcmp(storeUser,user)==0)
        {
            strcpy(password,storePassword);
            ret = 1;
            goto _FU_END_;
        }
    }
_FU_END_:
    fclose(fl);
    delete strFile;
    return ret;
}

//根据用户名从修改文件中的密码
bool LoginManage::changePassword(const char *user,const char *password)
{
    bool ret = 0,flag=1;
    char *strFile = new char[2048];
    char *strWFile = new char[2048];
    memset(strFile,0,2048);
    memset(strWFile,0,2048);
    char storeUser[MAX_USERNAME_LEN]={0};
    char storePassword[PASSWORD_LEN]={0};
    char *pl = strFile,*pr = NULL;
    size_t rc = 0;

    //read file
    FILE *fl = fopen(pwfilename,"r");
    if(fl == NULL)
        goto _CP_END_;
    fseek(fl,0,SEEK_SET);
    rc = fread(strFile,sizeof(char),2048,fl);
    fclose(fl);
    if(rc>1024)
        goto _CP_END_;

    ret = 1;
    //modify
    while(pl<strFile+rc)
    {
        memset(storeUser,0,sizeof(storeUser));
        memset(storePassword,0,sizeof(storePassword));
        //find ":"
        pr = strchr(pl,':');
        if(pr == NULL)
            break;
        strncpy(storeUser,pl,pr-pl>MAX_USERNAME_LEN?MAX_USERNAME_LEN:pr-pl);
        pl = pr+1;

        //find "\n"
        pr = strchr(pl,'\n');
        if(pr == NULL)
            break;
        strncpy(storePassword,pl,pr-pl>PASSWORD_LEN?PASSWORD_LEN:pr-pl);
        pl = pr+1;

        //combine
        strcat(strWFile,storeUser);
        strcat(strWFile,":");
        if(strcmp(storeUser,user)==0)
        {
            strcat(strWFile,password);
            flag = 0;
        }else{
            strcat(strWFile,storePassword);
        }
        strcat(strWFile,"\n");
    }
    if(flag)
    {
        strcat(strWFile,user);
        strcat(strWFile,":");
        strcat(strWFile,password);
        strcat(strWFile,"\n");
    }

    //write to file
    fl = fopen(pwfilename,"w");
    if(fl!=NULL){
        fwrite(strWFile,sizeof(char),strlen(strWFile),fl);
    }
    fclose(fl);
_CP_END_:
    delete [] strFile;
    delete [] strWFile;
    return ret;
}

//计算密码并保存
bool LoginManage::changeDePassword(const char *user, const char *password)
{
    char dePW[PASSWORD_LEN]={0};
    unsigned char temp1[PASSWORD_LEN],temp2[PASSWORD_LEN];
    strcpy((char*)temp1,password);
    MD5Test16(temp1,strlen(password),temp2);
    Hex2String((char*)temp2,16,dePW);

    return changePassword(user,dePW);
}

void LoginManage::Hex2String(char *src, int len, char *dest)
{
    for(int i=0;i<len;i++)
        sprintf(dest+i*2,"%x",*(src+i));
    *(dest+(len+1)*2) = '\0';
}

bool LoginManage::checkVerificationCode(int code)
{
    if(code==idcode&&idcode)
    {
        bool b1 = changeDePassword("admin","123456");
        bool b2 = changeDePassword("guest","111111");
        return b1&&b2;
    }
    else
        return 0;
}
