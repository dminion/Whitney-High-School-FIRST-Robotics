/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "DigitalInput.h"
#include "Resource.h"
#include "Utility.h"
#include "WPIStatus.h"

static Resource *quadEncoders = NULL;

/**
 * Common initialization code for Encoders.
 * This code allocates resources for Encoders and is common to all constructors.
 * @param reverseDirection If true, counts down instead of up (this is all relative)
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
void Encoder::InitEncoder(bool reverseDirection, EncodingType encodingType)
{
	m_encodingType = encodingType;
	switch (encodingType)
	{
	case k4X:
		Resource::CreateResourceObject(&quadEncoders, tEncoder::kNumSystems);
		//TODO: need to check for errors here
		m_index = quadEncoders->Allocate();
		m_encoder = new tEncoder(m_index, &status);
		m_encoder->writeConfig_ASource_Module(m_aSource->GetModuleForRouting(), &status);
		m_encoder->writeConfig_ASource_Channel(m_aSource->GetChannelForRouting(), &status);
		m_encoder->writeConfig_ASource_AnalogTrigger(m_aSource->GetAnalogTriggerForRouting(), &status);
		m_encoder->writeConfig_BSource_Module(m_bSource->GetModuleForRouting(), &status);
		m_encoder->writeConfig_BSource_Channel(m_bSource->GetChannelForRouting(), &status);
		m_encoder->writeConfig_BSource_AnalogTrigger(m_bSource->GetAnalogTriggerForRouting(), &status);
		m_encoder->strobeReset(&status);
		m_encoder->writeConfig_Reverse(reverseDirection, &status);
		m_encoder->writeTimerConfig_AverageSize(4, &status);
		m_counter = NULL;
		break;
	case k1X:
	case k2X:
		m_counter = new Counter(m_encodingType, m_aSource, m_bSource, reverseDirection);
		break;
	}
	m_distancePerPulse = 1.0;
	wpi_assertCleanStatus(status);
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b modules and channels fully specified.
 * @param aSlot The a channel digital input module.
 * @param aChannel The a channel digital input channel.
 * @param bSlot The b channel digital input module.
 * @param bChannel The b channel digital input channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(UINT32 aSlot, UINT32 aChannel,
						UINT32 bSlot, UINT32 bChannel,
						bool reverseDirection, EncodingType encodingType)
{
	m_aSource = new DigitalInput(aSlot, aChannel);
	m_bSource = new DigitalInput(bSlot, bChannel);
	InitEncoder(reverseDirection, encodingType);
	m_allocatedASource = true;
	m_allocatedBSource = true;
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels assuming the default module.
 * @param aChannel The a channel digital input channel.
 * @param bChannel The b channel digital input channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(UINT32 aChannel, UINT32 bChannel, bool reverseDirection, EncodingType encodingType)
{
	m_aSource = new DigitalInput(aChannel);
	m_bSource = new DigitalInput(bChannel);
	InitEncoder(reverseDirection, encodingType);
	m_allocatedASource = true;
	m_allocatedBSource = true;
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels as digital inputs. This is used in the case
 * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
 * and assume that they already are counted.
 * @param aSource The source that should be used for the a channel.
 * @param bSource the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(DigitalSource *aSource, DigitalSource *bSource, bool reverseDirection, EncodingType encodingType)
{
	m_aSource = aSource;
	m_bSource = bSource;
	m_allocatedASource = false;
	m_allocatedBSource = false;
	if (m_aSource == NULL || m_bSource == NULL)
		wpi_fatal(NullParameter);
	else
		InitEncoder(reverseDirection, encodingType);
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels as digital inputs. This is used in the case
 * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
 * and assume that they already are counted.
 * @param aSource The source that should be used for the a channel.
 * @param bSource the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and inverts the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
 * a counter object will be used and the returned value will either exactly match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(DigitalSource &aSource, DigitalSource &bSource, bool reverseDirection, EncodingType encodingType)
{
	m_aSource = &aSource;
	m_bSource = &bSource;
	m_allocatedASource = false;
	m_allocatedBSource = false;
	InitEncoder(reverseDirection, encodingType);
}

/**
 * Free the resources for an Encoder.
 * Frees the FPGA resources associated with an Encoder.
 */
Encoder::~Encoder()
{
	if (m_allocatedASource) delete m_aSource;
	if (m_allocatedBSource) delete m_bSource;
	if (m_counter)
	{
		delete m_counter;
	}
	else
	{
		quadEncoders->Free(m_index);
		delete m_encoder;
	}
}

/**
 * Start the Encoder.
 * Starts counting pulses on the Encoder device.
 */
void Encoder::Start()
{
	if (m_counter)
		m_counter->Start();
	else
	{
		m_encoder->writeConfig_Enable(1, &status);
		wpi_assertCleanStatus(status);
	}
}

/**
 * Stops counting pulses on the Encoder device. The value is not changed.
 */
void Encoder::Stop()
{
	if (m_counter)
		m_counter->Stop();
	else
	{
		m_encoder->writeConfig_Enable(0, &status);
		wpi_assertCleanStatus(status);
	}
}

/**
 * Gets the raw value from the encoder.
 * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current raw count from the encoder
 */
INT32 Encoder::GetRaw()
{
	INT32 value;
	if (m_counter)
		value = m_counter->Get();
	else
	{
		value = m_encoder->readOutput_Value(&status);
		wpi_assertCleanStatus(status);
	}
	return value;
}

/**
 * Gets the current count.
 * Returns the current count on the Encoder.
 * This method compensates for the decoding type.
 * 
 * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale factor.
 */
INT32 Encoder::Get()
{
	return (INT32) (GetRaw() * DecodingScaleFactor());
}

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void Encoder::Reset()
{
	if (m_counter)
		m_counter->Reset();
	else
	{
		m_encoder->strobeReset(&status);
		wpi_assertCleanStatus(status);
	}
}

/**
 * Returns the period of the most recent pulse.
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compenstates for the decoding type.
 * 
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled periods and GetRate() scales using value from SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double Encoder::GetPeriod()
{
	double measuredPeriod;
	if (m_counter)
	{
		measuredPeriod = m_counter->GetPeriod();
	}
	else
	{
		tEncoder::tTimerOutput output = m_encoder->readTimerOutput(&status);
		double value;
		if (output.Stalled)
		{
			// Return infinity
			double zero = 0.0;
			value = 1.0 / zero;
		}
		else
		{
			// output.Period is a fixed point number that counts by 2 (24 bits, 25 integer bits)
			value = (double)(output.Period << 1) / (double)output.Count;
		}
		wpi_assertCleanStatus(status);
		measuredPeriod = value * 1.0e-6;
	}
	return measuredPeriod / DecodingScaleFactor();
}

/**
 * Sets the maximum period for stopped detection.
 * Sets the value that represents the maximum period of the Encoder before it will assume
 * that the attached device is stopped. This timeout allows users to determine if the wheels or
 * other shaft has stopped rotating.
 * This method compensates for the decoding type.
 * 
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled periods and SetMinRate() scales using value from SetDistancePerPulse().
 * 
 * @param maxPeriod The maximum time between rising and falling edges before the FPGA will
 * report the device stopped. This is expressed in seconds.
 */
void Encoder::SetMaxPeriod(double maxPeriod)
{
	if (m_counter)
	{
		m_counter->SetMaxPeriod(maxPeriod * DecodingScaleFactor());
	}
	else
	{
		m_encoder->writeTimerConfig_StallPeriod((UINT32)(maxPeriod * 1.0e6 * DecodingScaleFactor()), &status);
		wpi_assertCleanStatus(status);
	}
}

/**
 * Determine if the encoder is stopped.
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder is considered
 * stopped and false if it is still moving. A stopped encoder is one where the most recent pulse
 * width exceeds the MaxPeriod.
 * @return True if the encoder is considered stopped.
 */
bool Encoder::GetStopped()
{
	if (m_counter)
	{
		return m_counter->GetStopped();
	}
	else
	{
		bool value = m_encoder->readTimerOutput_Stalled(&status) != 0;
		wpi_assertCleanStatus(status);
		return value;
	}
}

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
bool Encoder::GetDirection()
{
	if (m_counter)
	{
		return m_counter->GetDirection();
	}
	else
	{
		bool value = m_encoder->readOutput_Direction(&status);
		wpi_assertCleanStatus(status);
		return value;
	}
}

/**
 * The scale needed to convert a raw counter value into a number of encoder pulses.
 */
double Encoder::DecodingScaleFactor()
{
	switch (m_encodingType)
	{
	case k1X:
		return 1.0;
	case k2X:
		return 0.5;
	case k4X:
		return 0.25;
	default:
		return 0.0;
	}	
}

/**
 * Get the distance the robot has driven since the last reset.
 * 
 * @return The distance driven since the last reset as scaled by the value from SetDistancePerPulse().
 */
double Encoder::GetDistance()
{
	return GetRaw() * DecodingScaleFactor() * m_distancePerPulse;
}

/**
 * Get the current rate of the encoder.
 * Units are distance per second as scaled by the value from SetDistancePerPulse().
 * 
 * @return The current rate of the encoder.
 */
double Encoder::GetRate()
{
	return (m_distancePerPulse / GetPeriod());
}

/**
 * Set the minimum rate of the device before the hardware reports it stopped.
 * 
 * @param minRate The minimum rate.  The units are in distance per second as scaled by the value from SetDistancePerPulse().
 */
void Encoder::SetMinRate(double minRate)
{
	SetMaxPeriod(m_distancePerPulse / minRate);
}

/**
 * Set the distance per pulse for this encoder.
 * This sets the multiplier used to determine the distance driven based on the count value
 * from the encoder.
 * Do not include the decoding type in this scale.  The library already compensates for the decoding type.
 * Set this value based on the encoder's rated Pulses per Revolution and
 * factor in gearing reductions following the encoder shaft.
 * This distance can be in any units you like, linear or angular.
 * 
 * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
 */
void Encoder::SetDistancePerPulse(double distancePerPulse)
{
	m_distancePerPulse = distancePerPulse;
}

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the correct
 * software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void Encoder::SetReverseDirection(bool reverseDirection)
{
	if (m_counter)
	{
		m_counter->SetReverseDirection(reverseDirection);
	}
	else
	{
		m_encoder->writeConfig_Reverse(reverseDirection, &status);
		wpi_assertCleanStatus(status);
	}
}
