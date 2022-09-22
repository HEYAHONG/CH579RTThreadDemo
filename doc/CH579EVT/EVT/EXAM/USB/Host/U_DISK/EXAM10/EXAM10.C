/********************************** (C) COPYRIGHT *******************************
* File Name          : EXAM1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        :
 C���Ե�U���ļ��������޸��ļ����ԣ�ɾ���ļ��Ȳ���
 ֧��: FAT12/FAT16/FAT32
 ע����� CH579UFI.LIB/USBHOST.C/DEBUG.C
*******************************************************************************/

/** ��ʹ��U���ļ�ϵͳ�����U�̹���USBhub���棬��Ҫ�رն��� #define	FOR_ROOT_UDISK_ONLY  */
/** ʹ��U���ļ�ϵͳ�⣬��Ҫ�������涨��, ��ʹ����ر� #define DISK_BASE_BUF_LEN		512	      */

#include "CH57x_common.h"
#include "CH579UFI.H"

__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ];  // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ];  // OUT, must even address

UINT8  buf[100];                                  //���ȿ��Ը���Ӧ���Լ�ָ��


/* ������״̬,�����������ʾ������벢ͣ�� */
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* �����ɹ� */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */
    /* ���������,Ӧ�÷����������Լ�CH554DiskStatus״̬,�������CH579DiskReady��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
       ���������Ĵ�������:
       1������һ��CH579DiskReady,�ɹ����������,����Open,Read/Write��
       2�����CH579DiskReady���ɹ�,��ôǿ�н���ͷ��ʼ����(�ȴ�U�����ӣ�CH554DiskReady��) */
    while ( 1 )
    {  }
}

int main( )
{
    UINT8   s, c, i;
    
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PWR_UnitModCfg( ENABLE, UNIT_SYS_PLL );		// ��PLL
    DelayMs(5);
    
	GPIOA_SetBits(GPIO_Pin_9);
	GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
	GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
	UART1_DefInit();
	PRINT( "Start @ChipID=%02X %s \n", R8_CHIP_ID, __TIME__);

    pHOST_RX_RAM_Addr = RxBuffer;
    pHOST_TX_RAM_Addr = TxBuffer;
	USB_HostInit();    
    CH579LibInit( );                           //��ʼ��U�̳������֧��U���ļ�
    
    FoundNewDev = 0;
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( R8_USB_INT_FG & RB_UIF_DETECT )   // �����USB��������ж�����
        {
            R8_USB_INT_FG = RB_UIF_DETECT ;                // �������жϱ�־
            s = AnalyzeRootHub( );                                          // ����ROOT-HUB״̬
            if ( s == ERR_USB_CONNECT ) 		FoundNewDev = 1;
        }
        
        if ( FoundNewDev || s == ERR_USB_CONNECT )                         // ���µ�USB�豸����
        {
            FoundNewDev = 0;
            mDelaymS( 200 );                                               // ����USB�豸�ղ�����δ�ȶ�,�ʵȴ�USB�豸���ٺ���,������ζ���
            s = InitRootDevice();  // ��ʼ��USB�豸
            if ( s == ERR_SUCCESS )
            {
                // U�̲������̣�USB���߸�λ��U�����ӡ���ȡ�豸������������USB��ַ����ѡ�Ļ�ȡ������������֮�󵽴�˴�����CH579�ӳ���������ɺ�������
                CH579DiskStatus = DISK_USB_ADDR;
                for ( i = 0; i != 10; i ++ )
                {
                    printf( "Wait DiskReady\n" );
                    s = CH579DiskReady( );                                 //�ȴ�U��׼����
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    else
                    {
                        printf("%02x\n",(UINT16)s);
                    }
                    mDelaymS( 50 );
                }
                
                if ( CH579DiskStatus >= DISK_MOUNTED )
                {                    
                    //�����ļ���ʾ
                    printf( "Create\n" );
                    strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* ���ļ���,�ڸ�Ŀ¼��,�����ļ��� */
                    s = CH579FileCreate( );                                        /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
                    mStopIfError( s );
                    printf( "ByteWrite\n" );
                    //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
                    i = sprintf( (PCHAR)buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,579����ʾ���ܡ�\xd\xa");  /*��ʾ */
                    for(c=0; c<10; c++)
                    {
                        mCmdParam.ByteWrite.mByteCount = i;                          /* ָ������д����ֽ��� */
                        mCmdParam.ByteWrite.mByteBuffer = buf;                       /* ָ�򻺳��� */
                        s = CH579ByteWrite( );                                       /* ���ֽ�Ϊ��λ���ļ�д������ */
                        mStopIfError( s );
                        printf("�ɹ�д�� %02X��\n",(UINT16)c);
                    }
                    //��ʾ�޸��ļ�����
										printf( "Modify\n" );
										mCmdParam.Modify.mFileAttr = 0xff;   //�������: �µ��ļ�����,Ϊ0FFH���޸�
										mCmdParam.Modify.mFileTime = 0xffff;   //�������: �µ��ļ�ʱ��,Ϊ0FFFFH���޸�,ʹ���½��ļ�������Ĭ��ʱ��
										mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //�������: �µ��ļ�����: 2015.05.18
										mCmdParam.Modify.mFileSize = 0xffffffff;  // �������: �µ��ļ�����,���ֽ�Ϊ��λд�ļ�Ӧ���ɳ����ر��ļ�ʱ�Զ����³���,���Դ˴����޸�
										i = CH579FileModify( );   //�޸ĵ�ǰ�ļ�����Ϣ,�޸�����
										mStopIfError( i );
                    printf( "Close\n" );
                    mCmdParam.Close.mUpdateLen = 1;                                  /* �Զ������ļ�����,���ֽ�Ϊ��λд�ļ�,�����ó����ر��ļ��Ա��Զ������ļ����� */
                    i = CH579FileClose( );
                    mStopIfError( i );
                    
//                  strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* ���ļ���,�ڸ�Ŀ¼��,�����ļ��� */
//                  s = CH579FileOpen( );                                        /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
//                  mStopIfError( s );
                   
                    
                  /* ɾ��ĳ�ļ� */
                  printf( "Erase\n" );
                  strcpy( (PCHAR)mCmdParam.Create.mPathName, "/OLD" );  //����ɾ�����ļ���,�ڸ�Ŀ¼��
                  i = CH579FileErase( );  //ɾ���ļ����ر�
                  if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //��ʾ����
                }
            }
        }
        mDelaymS( 100 );  // ģ�ⵥƬ����������
        SetUsbSpeed( 1 );  // Ĭ��Ϊȫ��
    }
}

