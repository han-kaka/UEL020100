#include "bsp_var.h"

static void SYS_Mess_Ver_Init(void)
{
//    uint8_t i=0;
//    uint8_t software_var[4] = TERMINAL_SOFTWARE_VER;
//    uint8_t hardware_var[4] = TERMINAL_HARDWARE_VER;
//    for(i=0;i<4;i++)
//    {
//         SYS_HardVerMess[i] = hardware_var[i];
//    }
//    for(i=0;i<4;i++)
//    {
//         SYS_SoftVerMess[i] = software_var[i];
//    }
//    SYS_HardVerMessLen = 4;
//    SYS_SoftVerMessLen = 4;
}


static void Net_Par_Config(void)
{
    strcpy((char*)NB_NetPar.ServerIp, SERVER_IP_DEFAULT);
    strcpy((char*)NB_NetPar.ServerApn, SERVER_APN_DEFAULT);
    NB_NetPar.ServerPort     = SERVER_PORT_DEFAULT;
    NB_NetPar.ulReportPeriod = REPORT_PERIOD_DEFAULT;
		NB_NetPar.NetProtocol    = NET_PROTOCOL_DEFAULT;
    NB_NetPar.NetType        = NET_NO_NET;
}

static void NB86_Var_Init(void)
{
    Var_Fill(g_stNB_Handler,0);
    Var_Fill(PacketHead,0);
    Net_Par_Config();
}

static void Charge_Var_Init(void)
{
//    Var_Fill(g_stCharge_Handler,0);
//    Charge_ThresValue.ChargeTime  = 0;  //���ڳ��״̬,�����쳣�ϵ���´��ϵ��������´򿪳�繦��
//    Charge_ThresValue.PulloutTime = 3600;//�γ���ֵ��ʼֵΪ3600S
//    Charge_ThresValue.DeviationValue=12;//ƫ����ֵ���Ϊ12

//    g_stCharge_Handler.ChargeStatusExePeriod = EXE_PERIOD_MAX;
//    g_stCharge_Handler.StateOld = STATE_IDLE;
//    g_stCharge_Handler.State = STATE_IDLE;      //��ʼΪIDLE̬
}

void Bsp_Var_Init(void)
{
    NB86_Var_Init();
    Charge_Var_Init();
    SYS_Mess_Ver_Init();
    for(uint8_t i=0;i<8;i++)
    {
       ga_Subtask[i] = 0;  //��ϵͳ�������֧   SYS_SubTask[0]��Ӧ�������Ȩ        
    }  
    g_Maintask = 0; 
}




