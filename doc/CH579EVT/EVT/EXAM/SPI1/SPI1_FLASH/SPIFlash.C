/********************************** (C) COPYRIGHT *******************************
* File Name          : SPIFlash.C
* Author             : WCH
* Version            : V1.0
* Date               : 2019/4/29
* Description        : SPI Flash?????ӿں???(W25QXX)
*******************************************************************************/

#include "CH57x_common.h"
#include "SPIFlash.H"

/********************************* ???Ŷ??? ************************************
*    PA3  <===========>  SCS
*    PA0  <===========>  SCK
*    PA1  <===========>  DI/MOSI
*    PA2  <===========>  DO/MISO
*******************************************************************************/

#define  SPI1_CS_LOW()        GPIOA_ResetBits( GPIO_Pin_3 )
#define  SPI1_CS_HIGH()       GPIOA_SetBits( GPIO_Pin_3 )

/*******************************************************************************
* Function Name  : ReadExternalFlashStatusReg_SPI
* Description    : ??????ȡ״̬?Ĵ???,??????״̬?Ĵ?????ֵ
* Input          : None
* Output         : None
* Return         : ExFlashRegStatus
*******************************************************************************/
UINT8 ReadExternalFlashStatusReg_SPI( void )
{
    UINT8 ExFlashRegStatus;
    
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte( CMD_STATUS1 );                                          //???Ͷ?״̬?Ĵ?????????
    ExFlashRegStatus = SPI1_MasterRecvByte();                                    //??ȡ״̬?Ĵ???
    SPI1_CS_HIGH();
    
    return ExFlashRegStatus;
}

/*******************************************************************************
* Function Name  : WaitExternalFlashIfBusy
* Description    : ?ȴ?оƬ????(??ִ??Byte-Program, Sector-Erase, Block-Erase, Chip-Erase??????)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WaitExternalFlashIfBusy( void )
{
    while ((ReadExternalFlashStatusReg_SPI())&0x01 == 0x01 )
    {
        ;    //?ȴ?ֱ??Flash????
    }
}
/*******************************************************************************
* Function Name  : WriteExternalFlashEnable_SPI
* Description    : дʹ??,ͬ??????????ʹ??д״̬?Ĵ???
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteExternalFlashEnable_SPI( void )
{
    SPI1_CS_LOW();
    SPI1_MasterSendByte( CMD_WR_ENABLE );                                        //????дʹ??????
    SPI1_CS_HIGH();
}
/*******************************************************************************
* Function Name  : EraseExternal4KFlash_SPI
* Description    : ????4K Flash  ????һ??????
* Input          : Dst_Addr 0-1 ffff ffff ,??????????ַ???ڵ???????
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternal4KFlash_SPI( UINT32 Dst_Addr )
{
    WriteExternalFlashEnable_SPI();
    WaitExternalFlashIfBusy();
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_ERASE_4KBYTE);                                    //????????????
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFFFF) >> 16));                       //????3?ֽڵ?ַ
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(Dst_Addr & 0xFF);
    SPI1_CS_HIGH();
    
    WaitExternalFlashIfBusy();
}
/*******************************************************************************
* Function Name  : EraseExternalFlash_SPI
* Description    : ????32K Flash  ????һ??????
* Input          : Dst_Addr 0-1 ffff ffff ,??????????ַ???ڵ???????
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternal32KFlash_SPI( UINT32 Dst_Addr )
{
    WriteExternalFlashEnable_SPI();
    WaitExternalFlashIfBusy();
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_ERASE_32KBYTE);                                    //32K????????
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFFFF) >> 16));                        //????3?ֽڵ?ַ
    SPI1_MasterSendByte(((Dst_Addr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(Dst_Addr & 0xFF);
    SPI1_CS_HIGH();
    
    WaitExternalFlashIfBusy();
}
/*******************************************************************************
* Function Name  : PageWriteExternalFlash_SPI
* Description    : ҳд??SPI??һҳ(0~65535)??д??????256???ֽڵ?????
* Input          : RcvBuffer:???ݴ洢??
*                  StarAddr:??ʼд???ĵ?ַ
*                  Len:Ҫд?????ֽ???(????256),??????Ӧ?ó?????ҳ??ʣ???ֽ???!!!
* Output         : None
* Return         : None
*******************************************************************************/
void PageWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer)
{
    UINT16 i;
    
    WriteExternalFlashEnable_SPI();                                            //SET WEL
    
    SPI1_CS_LOW();                                                            
    SPI1_MasterSendByte(CMD_PAGE_PROG);                                        //????дҳ????
    SPI1_MasterSendByte(((StarAddr & 0xFFFFFF) >> 16));                        //????24bit??ַ
    SPI1_MasterSendByte(((StarAddr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(StarAddr & 0xFF);
    for(i=0; i!=Len; i++)
    {
        SPI1_MasterSendByte(RcvBuffer[i]);    //ѭ??д??
    }
    SPI1_CS_HIGH();
    
    WaitExternalFlashIfBusy();                                                 //?ȴ?д??????
}
/*******************************************************************************
* Function Name  : BlukWriteExternalFlash_SPI
* Description    : ?޼???дSPI FLASH
*                  ????ȷ????д?ĵ?ַ??Χ?ڵ?????ȫ??Ϊ0XFF,?????ڷ?0XFF??д???????ݽ?ʧ??!
*                  ?????Զ???ҳ????
*                  ??ָ????ַ??ʼд??ָ?????ȵ?????,????Ҫȷ????ַ??Խ??!
* Input          : SendBuffer:???ݴ洢??
*                  StarAddr:??ʼд???ĵ?ַ
*                  Len:Ҫд?????ֽ???(????65535)
* Output         : None
* Return         : None
*******************************************************************************/
void BlukWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 SendBuffer)
{
    UINT16  pageremain;
    
    pageremain = 256-StarAddr%256;                                               //??ҳʣ?????ֽ???
    if(Len<=pageremain)
    {
        pageremain=Len;    //??????256???ֽ?
    }
    while(1)
    {
        PageWriteExternalFlash_SPI(StarAddr,pageremain,SendBuffer);
        if(Len==pageremain)
        {
            break;    //д????????
        }
        else
        {
            SendBuffer+=pageremain;
            StarAddr+=pageremain;
            Len-=pageremain;                                                   //??ȥ?Ѿ?д???˵??ֽ???
            if(Len>256)
            {
                pageremain=256;    //һ?ο???д??256???ֽ?
            }
            else
            {
                pageremain=Len;    //????256???ֽ???
            }
        }
    }
}
/*******************************************************************************
* Function Name  : ReadExternalFlash_SPI
* Description    : ??ȡ??ַ??????.
*******************************************************************************/
void ReadExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 RcvBuffer )
{    
    SPI1_CS_LOW();                                                      
    SPI1_MasterSendByte(CMD_READ_DATA);                                 //??????
    SPI1_MasterSendByte(((StarAddr & 0xFFFFFF) >> 16));                 //????3?ֽڵ?ַ
    SPI1_MasterSendByte(((StarAddr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(StarAddr & 0xFF);
    SPI1_MasterRecv( RcvBuffer, Len );
    SPI1_CS_HIGH();
}
/*******************************************************************************
* Function Name  : BlukReadExternalFlash_SPI
* Description    : ??ȡ??ʼ??ַ(StarAddr)?ڶ????ֽ?(Len)??????.???뻺????RcvBuffer??
* Input          : StarAddr -Destination Address 000000H - 1FFFFFH
                   Len ??ȡ???ݳ???
                   RcvBuffer ???ջ???????ʼ??ַ
* Output         : None
* Return         : None
*******************************************************************************/
void BlukReadExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 RcvBuffer )
{    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_FAST_READ);                                 //???ٶ?
    SPI1_MasterSendByte(((StarAddr & 0xFFFFFF) >> 16));                 //????3?ֽڵ?ַ
    SPI1_MasterSendByte(((StarAddr & 0xFFFF) >> 8));
    SPI1_MasterSendByte(StarAddr & 0xFF);
    SPI1_MasterSendByte(0x00);    
    SPI1_MasterRecv( RcvBuffer, Len );
    SPI1_CS_HIGH();
}
/*******************************************************************************
* Function Name  : SPIFlash_ReadID
* Description    : SPI Flash??ȡоƬID
* Input          : None
* Output         : None
* Return         : 0XEF13,??ʾоƬ?ͺ?ΪW25Q80
*                  0XEF14,??ʾоƬ?ͺ?ΪW25Q16
*                  0XEF15,??ʾоƬ?ͺ?ΪW25Q32
*                  0XEF16,??ʾоƬ?ͺ?ΪW25Q64
*                  0XEF17,??ʾоƬ?ͺ?ΪW25Q128
*******************************************************************************/
UINT16 SPIFlash_ReadID(void)
{
    UINT16  temp = 0;
    
    SPI1_CS_LOW();
    SPI1_MasterSendByte(CMD_DEVICE_ID);                    //??ȡID????
    SPI1_MasterSendByte(0x00);
    SPI1_MasterSendByte(0x00);
    SPI1_MasterSendByte(0x00);
    temp = SPI1_MasterRecvByte();
    temp = temp<<8;
    temp |= SPI1_MasterRecvByte();
    SPI1_CS_HIGH();
    
    return temp;
}
/*******************************************************************************
* Function Name  : SPI1Flash_Init
* Description    : SPI Flash??ʼ??
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPIFlash_Init(void)
{
	GPIOA_SetBits( GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3 );
	GPIOA_ModeCfg( GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3, GPIO_ModeOut_PP_5mA );	// MOSI/SCK/CS
	GPIOA_SetBits( GPIO_Pin_2 );
	GPIOA_ModeCfg( GPIO_Pin_2, GPIO_ModeIN_PU );	// MISO	
	
    SPI1_MasterDefInit();
    SPI1_DataMode( Mode3_HighBitINFront );
    SPI1_CLKCfg( 4 );
    printf("id:0x%04x\n", SPIFlash_ReadID() );
}


