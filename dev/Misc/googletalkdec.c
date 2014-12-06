#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincrypt.h>

#define SEED_CONSTANT 0xba0da71d

unsigned char secretKey[16]={ 0xa3,0x1e,0xf3,0x69,
                              0x07,0x62,0xd9,0x1f,
                              0x1e,0xe9,0x35,0x7d,
                              0x4f,0xd2,0x7d,0x48 };

int Decode(char output[], char passEntry[], DWORD entryLen)
{
    int ret = -1;
    HANDLE hToken;
    char sid[512], name[512],domain[512];
    DWORD SidSize = 0, i, j;
    DWORD cchName,cchDomain;
    SID_NAME_USE peUse;
    TOKEN_USER *SidUser = (TOKEN_USER*)&sid;

    unsigned char staticKey[16];
    unsigned int seed;
    unsigned char *a,*b;

    memcpy(staticKey,secretKey,sizeof(staticKey));

    if((OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken)))
    {
        if((GetTokenInformation(hToken,TokenUser,SidUser,sizeof(sid),&SidSize)))
        {
            cchName = cchDomain = sizeof(name);

            if((LookupAccountSid(NULL,SidUser->User.Sid,
                    name,&cchName,domain,&cchDomain,&peUse)))
            {
                seed = SEED_CONSTANT;

                // mix username with key

                for(i = 0;i < cchName;i++)
                {
                    ((unsigned int*)staticKey)[ i % 4 ] ^= name[i] * seed;
                    seed *= 48271;
                }

                // mix domain name with key

                for(j = 0;j < cchDomain;i++,j++)
                {
                    ((unsigned int*)staticKey)[ i % 4 ] ^= domain[j] * seed;
                    seed *= 48271;
                }

                // decode  string

                seed = (((unsigned int*)staticKey)[0] | 1);
                a = (unsigned char*)&passEntry[4];
                b = (unsigned char*)&passEntry[5];

                for(i = 0;i < entryLen;i += 2)
                {
                    passEntry[ i / 2 ] = (((a[i]-1)*16) | (b[i]-33)) - (seed & 0xff);
                    seed *= 69621;
                }

                // use protected storage to decrypt data

                DATA_BLOB   DataIn, DataEntropy, DataOut;

                DataEntropy.cbData = sizeof(staticKey);
                DataEntropy.pbData = (BYTE*)&staticKey;

                DataIn.cbData = (i/2);
                DataIn.pbData = (BYTE*)passEntry;

                //passEntry[(i/2)+4]=0;

                if(CryptUnprotectData(&DataIn,
                                   NULL,
                                   &DataEntropy,
                                   NULL,
                                   NULL,
                                   1,
                                   &DataOut)) {
                    memcpy(output,DataOut.pbData,DataOut.cbData);
                    output[DataOut.cbData] = 0;
                    LocalFree(DataOut.pbData);
                    ret = 0;
                }
            }
        }
        CloseHandle(hToken);
    }
    return(ret);
}

int main(void) {
    char    pwd[1024],
            out[1024],
            *p;

    printf(
        "paste your encrypted password here\n"
        "(\"pw\" from HKEY_CURRENT_USER\\Software\\Google\\Google Talk\\Accounts):\n");
    fgets(pwd, sizeof(pwd), stdin);
    for(p = pwd; *p && (*p != '\n') && (*p != '\r'); p++);
    *p = 0;

    if(!Decode(out, pwd, strlen(pwd))) {
        printf("\nPASSWORD: %s\n", out);
    } else {
        printf("\nthe password cannot be decrypted on this account/machine\n");
    }
    printf("\npress return to quit\n");
    fgetc(stdin);
    return(0);
}
