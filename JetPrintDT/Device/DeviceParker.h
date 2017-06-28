#ifndef __DEVICE_PARKER_H
#define __DEVICE_PARKER_H

#include <afx.h>
#include "../SingleTon/SingletonTemplate.h"

#import "ComACRServer.tlb" no_namespace named_guids
#define L_COMACRSERVER_PROGID L"ComACRServer.Channel.1"

typedef struct _STATUS_DATA{
	float m_fXPos;
	float m_fXSpeed;
	float m_fYPos;
	float m_fYSpeed;
}STATUS_DATA;


class CDeviceParker :public CSingletonTemplate<CDeviceParker>
{
	//public methods
	friend CSingletonTemplate<CDeviceParker>;
public:
	CDeviceParker();
	~CDeviceParker(void);

	CComQIPtr<IChannel> GetControlChannel()const;
	

	//Enable Motors and Disable Motors
	void JetParkerKillMotor();
	void JetParkerEnableMotor();
	void JetParkerDisableMotor();

	//Parker Jog Control
	void JetParkerJogVelocitySet(const UINT &n_motor,const float& fSpeed);
	float JetParkerJogVelocityGet(const UINT &n_motor);

	float JetParkerPositionGet(const UINT &motor);
	float JetParkerSpeedGet(const UINT &motor);
	float JetParkerMasterSpeedGet();
	void JetParkerJogAccSet(const UINT &n_motor,const float& fSpeed);
	void JetParkerJogAccGet();

	void JetParkerXJogPlus();
	void JetParkerYJogPlus();
	void JetParkerXJogMinus();
	void JetParkerYJogMinum();
	void JetParkerJogHome() ;
	void JetParkerJogStop(const UINT & n_motor);

	void JetParkerSetLimit(float plusPos,float minumPos);

	//Init WatchStatus
	bool JetParkerWatchStatusInit();
	void JetParkerSuspendThread();
	void JetParkerResumeThread();
	void JetParkerInitWnd(CWnd *pWnd);
	//增量运动,1表示0轴，2表示1轴
	void JetParkerMove(float *pTargetPosition,bool bMoveAbsolute,int nAxis);
	void JetParkerSetAglient(bool bStart);
	CString JetParkerGetIPAddr()const ;
	void JetParkerJogStep(double dbStep,bool bPositive,int nAxis);
	bool JetParkerIsMotorEnabled() ;
	void JetParkerRunProgram();
	bool JetParkerSetProg();

	bool JetParkerIsXJogPlus() const;
	bool JetParkerIsXJogMinus() const;
	bool JetParkerIsYJogPlus() const;
	bool JetParkerIsYJogMinus() const;
	void JetParkerMoveVelSet(double Speed);

	bool JetParkerIsBitChecked(int nGroup,int nIndex);
	bool JetParkerIsConnected();
	void JetParkerGetCustomParamLong(int nParam,long &result);
	void JetParkerGetCustomParamFloat(int nParam,float &result);
	bool JetParkerDownloadFile(CString fileName);
	bool JetParkerIsInitialized();


	typedef struct _THREAD_PARAM{
		CDeviceParker *pJetParkerControl;
		bool m_bIsExitThread;
	}THREAD_PARAM;

	void JetParkerTerminateWatchThread();

	//private methods
public:
	bool JetParkerInitialize();
	void JetParkerUnInitialize();
	bool JetParkerConnect();
	bool JetParkerDisConnect();
	static UINT StatusInit(void *param);
	void JetParkerUpdateState(CComVariant & varItem);
	//public members
public:
	enum {OFFLINE=0,SERIAL = 2,ETHERNET = 3,USB = 4};
	enum {JOG_X_PLUS = 796              ,JOG_X_MINUS = 797             ,JOG_X_STOP = 795,
		  JOG_Y_PLUS = 828              ,JOG_Y_MINUS = 829             ,JOG_Y_STOP = 827,
		  ENABLE_MOTOR_ZERO = 8465      ,KILL_MOTOR_ZERO = 8467        ,ENABLE_MOTOR_ONE = 8497,
		  KILL_MOTOR_ONE = 8499         ,JOG_SPEED_ZERO_SET = 12348    ,JOG_SPEED_ONE_SET=12604,
		  JOG_SPEED_ZERO_GET = 12346    ,JOG_SPEED_ONE_GET = 12602     ,JOG_ACC_ZERO_SET = 12347,
		  JOG_ACC_ONE_SET = 12603       ,JOG_X_LIMIT_PLUS = 12334      ,JOG_X_LIMIT_MINUM = 12335,
		  JOG_Y_LIMIT_PLUS = 12590      ,JOG_Y_LIMIT_MINUS=12591       ,MOVE_X_LIMIT_PLUS = 12324,
		  MOVE_X_LIMIT_MINUS = 12325    ,MOVE_Y_LIMIT_PLUS = 12580     ,MOVE_Y_LIMIT_MINUS = 12590};
	bool m_bSpeedParamUpdated;
	bool m_bIsJogMode;
	THREAD_PARAM m_ThreadParam;
	CWinThread * m_watchStatusThread;
	CComQIPtr<IChannel> m_controlChannel;
	//private members
private:
	CWnd * m_pWnd;
	bool m_bIsParkerInitialized;
	bool m_bIsParkerConnected;
	bool m_bIsMotorKilled;
	bool m_bWatchStatusIntial;
	CString m_strIpAddr;
	STATUS_DATA status_data;
	long statusMsg;
	float m_fAxisXSpeed;		//Axis X Current Speed, AXIS 0
	float m_fAxisYSpeed;		// AXis Y Current Speed,AXIS 1
	long m_lAxisXPosition;		// AXis X Current Position, AXIS 0
	long m_lAxisYPosition;		// AXis Y Current Position,AXIS 1
	bool m_bIsMotorEnable;      // Indicate If Motors Has Been Enabled

	bool m_bXJogPlus;           //Indicate If Motor 0 Jog Plus
	bool m_bXJogMinus;          //Indicate If Motor 0 Jog Minus 
	bool m_bYJogPlus;           //Indicate If Motor 1 Jog Plus
	bool m_bYjogMinus;          //Indicate If Motor 1 Jog Minus 

	bool m_bProgWrite;
	bool m_bSuspendThread;		//To Suspend A Thread
	HANDLE m_WatchStatusEvent;

};
#endif



