/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef JAGUAR_H
#define JAGUAR_H

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

/**
 * Luminary Micro Jaguar Speed Control
 */
class Jaguar : public SafePWM, public SpeedController, public PIDOutput
{
public:
	explicit Jaguar(UINT32 channel);
	Jaguar(UINT32 slot, UINT32 channel);
	virtual ~Jaguar();
	virtual void Set(float value, UINT8 syncGroup=0);
	virtual float Get();
	virtual void Disable();

	virtual void PIDWrite(float output);

private:
	void InitJaguar();
};

#endif

