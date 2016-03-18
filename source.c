
#include "stdio.h"
#include "stdlib.h"

#define HEADER  0xFE2F00
#define TOKEN1  0x020001FD //Start
#define TOKEN14 0x040001FB //End

#define TOKEN4  0x03040BFC//0xFC0B0403

#define TOKEN6  0x030004FC


#define TOKEN2  0x010000FC
#define TOKEN3  0x020000FC
#define TOKEN5  0x040000FC
#define TOKEN7  0x050000FC


#define TOKEN13 0x06001EFC  //



#define TOKEN20 0x06001eFC
#define TOKEN15 0xFFFFFFFF  //Flag
#define TOKEN16 0x00000000  //Flag

unsigned char ReadBuffer[1024] ={0};

unsigned long Buffer3 = 0;
unsigned long Buffer4 = 0;
int main()
{
    FILE *fp;
    unsigned int uiStartCount = 0;
    unsigned int uiEndCount = 0;
    unsigned int uiErrCount = 0;
    fp = fopen("UV4_504.cdb","rb");

    if(NULL == fp)
    {
        printf("No File\r\n");
        return -1;
    }

    fread(ReadBuffer,3,1,fp);
    Buffer3 = 0;
    memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));
    printf("Header:%04x\r\n",Buffer3);
    fread(ReadBuffer,2,1,fp);   //Length
    Buffer3 = 0;
    memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));
    memset(ReadBuffer,0,sizeof(ReadBuffer));

    fread(ReadBuffer,Buffer3+1,1,fp);   //Value Checksum
    printf("%s\r\n",ReadBuffer);

    while(!feof(fp))
    {
        fread(ReadBuffer,4,1,fp);   //Start
        Buffer3 = 0;
        memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));
        if(((ReadBuffer[3] == 0x05)||(ReadBuffer[3] == 0x04)||\
            (ReadBuffer[3] == 0x02)||(ReadBuffer[3] == 0x01))&&(ReadBuffer[0] == 0xFC))
        {
            Buffer3 = Buffer3&0xFF0000FF;
        }

		if((ReadBuffer[3] == 0x00)&&(ReadBuffer[2] == 0x00))
		{
			Buffer3 = 0;
		}

        memset(ReadBuffer,0,sizeof(ReadBuffer));

        switch(Buffer3)
        {
            case TOKEN1:    //Start
                {
                    uiStartCount++;
                    break;
                }
            case TOKEN2:    //Vendor
            case TOKEN3:
            case TOKEN5:
                {
                    fread(ReadBuffer,2,1,fp);   //Vendor
                    Buffer3 = 0;
                    memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));    //Length
                    memset(ReadBuffer,0,sizeof(ReadBuffer));
                    fread(ReadBuffer,Buffer3+1,1,fp);
                    printf("%s\r\n",ReadBuffer);
                    break;
                }
            case TOKEN6:
                {
                    fread(ReadBuffer,3,1,fp);   //Vendor
                    Buffer3 = 0;
                    memset(ReadBuffer,0,sizeof(ReadBuffer));
                    break;
                }
            case TOKEN7:
                {
                    fread(ReadBuffer,2,1,fp);   //Vendor
                    Buffer3 = 0;
                    memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));    //Length
                    memset(ReadBuffer,0,sizeof(ReadBuffer));
                    fread(ReadBuffer,Buffer3,1,fp);
                    printf("%s:",ReadBuffer);
                    memset(ReadBuffer,0,sizeof(ReadBuffer));
                    fread(ReadBuffer,2,1,fp);   //Vendor
                    Buffer3 = 0;
                    memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));    //Length
                    memset(ReadBuffer,0,sizeof(ReadBuffer));
                    fread(ReadBuffer,Buffer3+1,1,fp);
                    printf("%s\r\n",ReadBuffer);
                    break;
                }

            case TOKEN15:
                {
                    while(1)
                    {
                        fread(ReadBuffer,1,1,fp);
                        if((ReadBuffer[0] != 0xFF)&&1)
                        {
                            break;
                        }
                    }
                    break;
                }
            case TOKEN14:
                {
                    uiEndCount++;
                    break;
                }   
            case TOKEN16:
            case TOKEN20:
                {
                    break;
                }
            default:
				Buffer4 = ftell(fp);
                uiErrCount++;
                break;
        }
    }
    printf("Start%08x End%08x\r\n",uiStartCount,uiEndCount);
}
