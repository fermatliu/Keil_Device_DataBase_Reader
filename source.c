
#include "stdio.h"
#include "stdlib.h"

#define HEADER  0xFE2F00
#define TOKENSTART  0x020001FD //Start
#define TOKENEND    0x040001FB //End

#define TOKENMODEM  0x030004FC

#define MCS80251        0xFC0001      
#define MCS8051         0xFD0000
#define MCS80166        0xFB0002
#define ARM             0xFA0003

#define TOKENVENDOR  0x010000FC
#define TOKENDEVICE  0x020000FC
#define TOKEN5  0x040000FC
#define TOKEN7  0x050000FC


#define TOKEN13 0x06001EFC  //


#define TOKEN15 0xFFFFFFFF  //Flag
#define TOKEN16 0x00000000  //Flag

unsigned char ReadBuffer[0x400] ={0};

unsigned long Buffer3 = 0;
int main()
{
    FILE *fp;
    unsigned int uiStartCount = 0;
    unsigned int ui00Count = 0;
    unsigned int ui01Count = 0;
    unsigned int ui02Count = 0;
    unsigned int ui03Count = 0;
    unsigned int ui04Count = 0;
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
            case TOKENVENDOR:    //Vendor
            case TOKENDEVICE:    //Device
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
            case TOKENMODEM:
                {
                    memset(ReadBuffer,0,sizeof(ReadBuffer));
                    fread(ReadBuffer,3,1,fp);   //Vendor
                    Buffer3 = 0;
                    memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));
                    if(Buffer3 == MCS80166)
                    {
                        ui00Count++;
                    }
                    else if(Buffer3 == MCS80251)
                    {
                        ui01Count++;
                    }
                    else if(Buffer3 == MCS8051)
                    {
                        ui02Count++;
                    }
                    else if(Buffer3 == ARM)
                    {
                        ui03Count++;
                    }
                    else
                    {
                        ui04Count++;
                    }
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

            case TOKEN15:       //End Flag
                {
                    while(1)
                    {
                        fread(ReadBuffer,1,1,fp);

                        if(ReadBuffer[0] != 0xFF)   //Checksum or FB
                        {
                            fread(ReadBuffer,4,1,fp);
                            Buffer3 = 0;
                            memcpy(&Buffer3,ReadBuffer,sizeof(Buffer3));
                            if(Buffer3 == TOKENEND)
                            {
                                break;              // New Device
                            }
                            else if((Buffer3&0x00FFFFFF) == (TOKENEND>>8))
                            {
                                fseek(fp,-1,SEEK_CUR);
                                break;
                            }
                        }
                    }
                    break;
                }
            case TOKENSTART:    //Start
                uiStartCount++;
            case TOKENEND:
            case TOKEN16:
            case TOKEN13:
            default:
                Buffer3 = ftell(fp);
                break;
        }
    }
    printf("Start%08x End%08x\r\n",uiStartCount,0);
}
