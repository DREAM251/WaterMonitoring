#ifndef _LOGIN_MANAGE_H_
#define _LOGIN_MANAGE_H_

#define MAX_USERNAME_LEN   16
#define PASSWORD_LEN    64

static const char *pwfilename = "pw.dat";

class LoginManage
{
public:
    LoginManage();
    ~LoginManage();

private:
    int initCode , idcode;
    int calcID(void);

    bool findUser(const char *user, char *password);
    bool changePassword(const char *user, const char *password);
    bool changeDePassword(const char *user, const char *password);
    void Hex2String(char *src,int len,char *dest);
public:
    int check(const char *user, const char *password);
    int modify(const char *user,const char *password,const char *newpassword);
    int getMacCode();
    bool checkVerificationCode(int code);

    friend class UserDlg;
};

#endif
