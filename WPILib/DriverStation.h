/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __DRIVER_STATION_H__
#define __DRIVER_STATION_H__

#include "Dashboard.h"
#include "DriverStationEnhancedIO.h"
#include "SensorBase.h"
#include "Task.h"

struct FRCCommonControlData;
class AnalogChannel;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
class DriverStation : public SensorBase
{
public:
	enum Alliance {kRed, kBlue, kInvalid};

	virtual ~DriverStation();
	static DriverStation *GetInstance();

	static const UINT32 kBatterySlot = 1;
	static const UINT32 kBatteryChannel = 8;
	static const UINT32 kJoystickPorts = 4;
	static const UINT32 kJoystickAxes = 6;

	float GetStickAxis(UINT32 stick, UINT32 axis);
	short GetStickButtons(UINT32 stick);

	float GetAnalogIn(UINT32 channel);
	bool GetDigitalIn(UINT32 channel);
	void SetDigitalOut(UINT32 channel, bool value);
	bool GetDigitalOut(UINT32 channel);

	bool IsEnabled();
	bool IsDisabled();
	bool IsAutonomous();
	bool IsOperatorControl();
	bool IsNewControlData();
	bool IsFMSAttached();

	UINT32 GetPacketNumber();
	Alliance GetAlliance();
	UINT32 GetLocation();

	float GetBatteryVoltage();

	// Get the default dashboard packers. These instances stay around even after
	// a call to SetHigh|LowPriorityDashboardPackerToUse() changes which packer
	// is in use. You can restore the default high priority packer by calling
	// SetHighPriorityDashboardPackerToUse(&GetHighPriorityDashboardPacker()).
	Dashboard& GetHighPriorityDashboardPacker() { return m_dashboardHigh; }
	Dashboard& GetLowPriorityDashboardPacker() { return m_dashboardLow; }

	// Get/set the dashboard packers to use. This can sideline or restore the
	// default packers. Initializing SmartDashboard changes the high priority
	// packer in use so beware that the default packer will then be idle. These
	// methods support any kind of DashboardBase, e.g. a Dashboard or a
	// SmartDashboard.
	DashboardBase* GetHighPriorityDashboardPackerInUse() { return m_dashboardInUseHigh; }
	DashboardBase* GetLowPriorityDashboardPackerInUse() { return m_dashboardInUseLow; }
	void SetHighPriorityDashboardPackerToUse(DashboardBase* db) { m_dashboardInUseHigh = db; }
	void SetLowPriorityDashboardPackerToUse(DashboardBase* db) { m_dashboardInUseLow = db; }

	DriverStationEnhancedIO& GetEnhancedIO() { return m_enhancedIO; }

	void IncrementUpdateNumber() { m_updateNumber++; }
	SEM_ID GetUserStatusDataSem() { return m_statusDataSemaphore; }

protected:
	DriverStation();

	void GetData();
	void SetData();

private:
	static void InitTask(DriverStation *ds);
	static DriverStation *m_instance;
	///< TODO: Get rid of this and use the semaphore signaling
	static const float kUpdatePeriod = 0.02;

	void Run();

	struct FRCCommonControlData *m_controlData;
	UINT8 m_digitalOut;
	AnalogChannel *m_batteryChannel;
	SEM_ID m_statusDataSemaphore;
	Task m_task;
	Dashboard m_dashboardHigh;  // the default dashboard packers
	Dashboard m_dashboardLow;
	DashboardBase* m_dashboardInUseHigh;  // the current dashboard packers in use
	DashboardBase* m_dashboardInUseLow;
	bool m_newControlData;
	SEM_ID m_packetDataAvailableSem;
	DriverStationEnhancedIO m_enhancedIO;
	static UINT8 m_updateNumber;
};

#endif

