/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef VICTOR_H
#define VICTOR_H

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

/**
 * IFI Victor Speed Controller
 */
class Victor : public SafePWM, public SpeedController, public PIDOutput
{
public:
	explicit Victor(UINT32 channel);
	Victor(UINT32 slot, UINT32 channel);
	virtual ~Victor();
	virtual void Set(float value, UINT8 syncGroup=0);
	virtual float Get();
	virtual void Disable();

	virtual void PIDWrite(float output);

private:
	void InitVictor();
};

#endif

