/********************************** (C) COPYRIGHT *******************************
* File Name          : EXAM11.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        : CH579 C���Ե�U��Ŀ¼�ļ�ö�ٳ���
 ֧��: FAT12/FAT16/FAT32
 ע����� CH579UFI.LIB/USBHOST.C/DEBUG.C
*******************************************************************************/

/** ��ʹ��U���ļ�ϵͳ�����U�̹���USBhub���棬��Ҫ�رն��� #define	FOR_ROOT_UDISK_ONLY  */
/** ʹ��U���ļ�ϵͳ�⣬��Ҫ�������涨��, ��ʹ����ر� #define DISK_BASE_BUF_LEN		512	      */


#include "CH57x_common.h"
#include "CH579UFI.H"

__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ];  // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ];  // OUT, must even address


/* ������״̬,�����������ʾ������벢ͣ�� */
void    mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* �����ɹ� */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */
    /* ���������,Ӧ�÷����������Լ�CH579DiskStatus״̬,�������CH579DiskReady��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
       ���������Ĵ�������:
       1������һ��CH579DiskReady,�ɹ����������,����Open,Read/Write��
       2�����CH579DiskReady���ɹ�,��ôǿ�н���ͷ��ʼ����(�ȴ�U�����ӣ�CH579DiskReady��) */
    while ( 1 )
    {   }
}


int main( )
{
    UINT8   s, i;
    PUINT8  pCodeStr;
    UINT16  j;

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
    printf( "Wait Device In\n" );
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( R8_USB_INT_FG & RB_UIF_DETECT )   // �����USB��������ж�����
        {
            R8_USB_INT_FG = RB_UIF_DETECT ;                // �������жϱ�־
            s = AnalyzeRootHub( );                                          // ����ROOT-HUB״̬
            if ( s == ERR_USB_CONNECT ) 		FoundNewDev = 1;
        }
        
        if ( FoundNewDev || s == ERR_USB_CONNECT )
        {
            // ���µ�USB�豸����
            FoundNewDev = 0;
            mDelaymS( 200 );                                                    // ����USB�豸�ղ�����δ�ȶ�,�ʵȴ�USB�豸���ٺ���,������ζ���
            s = InitRootDevice( );                                              // ��ʼ��USB�豸
            if ( s == ERR_SUCCESS )
            {
                printf( "Start UDISK_demo @CH579UFI library\n" );
                // U�̲������̣�USB���߸�λ��U�����ӡ���ȡ�豸������������USB��ַ����ѡ�Ļ�ȡ������������֮�󵽴�˴�����CH579�ӳ���������ɺ�������
                CH579DiskStatus = DISK_USB_ADDR;
                for ( i = 0; i != 10; i ++ )
                {
                    printf( "Wait DiskReady\n" );
                    s = CH579DiskReady( );
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    mDelaymS( 50 );
                }
                if ( CH579DiskStatus >= DISK_MOUNTED )                           //U��׼����
                {                    
                    /* ���ļ� */
                    printf( "Open\n" );
                    strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/CH579HFT.C" );         //����Ҫ�������ļ�����·��
                    s = CH579FileOpen( );                                          //���ļ�
                    if ( s == ERR_MISS_DIR )
                    {
                        printf("�����ڸ��ļ������г���Ŀ¼�����ļ�\n");
                        pCodeStr = (PUINT8)"/*";
                    }
                    else
                    {
                        pCodeStr = (PUINT8)"/C51/*";    //�г�\C51��Ŀ¼�µĵ��ļ�
                    }
                    
                    printf( "List file %s\n", pCodeStr );
                    for ( j = 0; j < 10000; j ++ )                                 //�޶�10000���ļ�,ʵ����û������
                    {
                        strcpy( (PCHAR)mCmdParam.Open.mPathName, (PCCHAR)pCodeStr );              //�����ļ���,*Ϊͨ���,�����������ļ�������Ŀ¼
                        i = strlen( (PCHAR)mCmdParam.Open.mPathName );
                        mCmdParam.Open.mPathName[ i ] = 0xFF;                      //�����ַ������Ƚ��������滻Ϊ���������,��0��254,�����0xFF��255��˵�����������CH579vFileSize������
                        CH579vFileSize = j;                                        //ָ������/ö�ٵ����
                        i = CH579FileOpen( );                                      //���ļ�,����ļ����к���ͨ���*,��Ϊ�����ļ�������
                        /* CH579FileEnum �� CH579FileOpen ��Ψһ�����ǵ����߷���ERR_FOUND_NAMEʱ��ô��Ӧ��ǰ�߷���ERR_SUCCESS */
                        if ( i == ERR_MISS_FILE )
                        {
                            break;    //��Ҳ��������ƥ����ļ�,�Ѿ�û��ƥ����ļ���
                        }
                        if ( i == ERR_FOUND_NAME )                                 //��������ͨ�����ƥ����ļ���,�ļ�����������·�������������
                        {
                            printf( "  match file %04d#: %s\n", (unsigned int)j, mCmdParam.Open.mPathName );  /* ��ʾ��ź���������ƥ���ļ���������Ŀ¼�� */
                            continue;                                                /* ����������һ��ƥ����ļ���,�´�����ʱ��Ż��1 */
                        }
                        else                                                       //����
                        {
                            mStopIfError( i );
                            break;
                        }
                    }
                    i = CH579FileClose( );                                          //�ر��ļ�
                    printf( "U����ʾ���\n" );                     
                }
                else
                {
                    printf( "U��û��׼���� ERR =%02X\n", (UINT16)s );
                }
            }
            else
            {
                printf("��ʼ��U��ʧ�ܣ������U������\n");
            }
        }
        mDelaymS( 100 );  // ģ�ⵥƬ����������
        SetUsbSpeed( 1 );  // Ĭ��Ϊȫ��
    }
}


