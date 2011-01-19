/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DigitalOutput.h"
#include "DigitalModule.h"
#include "Resource.h"
#include "Utility.h"
#include "WPIStatus.h"

extern Resource *interruptsResource;

/**
 * Create an instance of a DigitalOutput.
 * Creates a digital output given a slot and channel. Common creation routine
 * for all constructors.
 */
void DigitalOutput::InitDigitalOutput(UINT32 slot, UINT32 channel)
{
	m_channel = channel;
	m_pwmGenerator = ~0ul;
	m_module = DigitalModule::GetInstance(slot);
	m_module->AllocateDIO(m_channel, false);
}

/**
 * Create an instance of a digital output.
 * Create a digital output given a channel. The default module is used.
 */
DigitalOutput::DigitalOutput(UINT32 channel)
{
	InitDigitalOutput(GetDefaultDigitalModule(), channel);
}

/**
 * Create an instance of a digital output.
 * Create an instance of a digital output given a slot and channel.
 */
DigitalOutput::DigitalOutput(UINT32 slot, UINT32 channel)
{
	InitDigitalOutput(slot, channel);
}

/**
 * Free the resources associated with a digital output.
 */
DigitalOutput::~DigitalOutput()
{
	// Disable the PWM in case it was running.
	DisablePWM();
	m_module->FreeDIO(m_channel);
}

/**
 * Set the value of a digital output.
 * Set the value of a digital output to either one (true) or zero (false).
 */
void DigitalOutput::Set(UINT32 value)
{
	m_module->SetDIO(m_channel, value);
}

/**
 * @return The GPIO channel number that this object represents.
 */
UINT32 DigitalOutput::GetChannel()
{
	return m_channel;
}

/**
 * Output a single pulse on the digital output line.
 * Send a single pulse on the digital output line where the pulse diration is specified in seconds.
 * Maximum pulse length is 0.0016 seconds.
 * @param length The pulselength in seconds
 */
void DigitalOutput::Pulse(float length)
{
	m_module->Pulse(m_channel, length);
}

/**
 * Determine if the pulse is still going.
 * Determine if a previously started pulse is still going.
 */
bool DigitalOutput::IsPulsing()
{
	return m_module->IsPulsing(m_channel);
}

/**
 * Change the PWM frequency of the PWM output on a Digital Output line.
 * 
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is logarithmic.
 * 
 * There is only one PWM frequency per digital module.
 * 
 * @param rate The frequency to output all digital output PWM signals on this module.
 */
void DigitalOutput::SetPWMRate(float rate)
{
	m_module->SetDO_PWMRate(rate);
}

/**
 * Enable a PWM Output on this line.
 * 
 * Allocate one of the 4 DO PWM generator resources from this module.
 * 
 * Supply the initial duty-cycle to output so as to avoid a glitch when first starting.
 * 
 * The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less)
 * but is reduced the higher the frequency of the PWM signal is.
 * 
 * @param initialDutyCycle The duty-cycle to start generating. [0..1]
 */
void DigitalOutput::EnablePWM(float initialDutyCycle)
{
	if (m_pwmGenerator != ~0ul) return;
	m_pwmGenerator = m_module->AllocateDO_PWM();
	m_module->SetDO_PWMDutyCycle(m_pwmGenerator, initialDutyCycle);
	m_module->SetDO_PWMOutputChannel(m_pwmGenerator, m_channel);
}

/**
 * Change this line from a PWM output back to a static Digital Output line.
 * 
 * Free up one of the 4 DO PWM generator resources that were in use.
 */
void DigitalOutput::DisablePWM()
{
	// Disable the output by routing to a dead bit.
	m_module->SetDO_PWMOutputChannel(m_pwmGenerator, kDigitalChannels);
	m_module->FreeDO_PWM(m_pwmGenerator);
	m_pwmGenerator = ~0ul;
}

/**
 * Change the duty-cycle that is being generated on the line.
 * 
 * The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less)
 * but is reduced the higher the frequency of the PWM signal is.
 * 
 * @param dutyCycle The duty-cycle to change to. [0..1]
 */
void DigitalOutput::UpdateDutyCycle(float dutyCycle)
{
	m_module->SetDO_PWMDutyCycle(m_pwmGenerator, dutyCycle);
}

/**
 * @return The value to be written to the channel field of a routing mux.
 */
UINT32 DigitalOutput::GetChannelForRouting()
{
	return DigitalModule::RemapDigitalChannel(GetChannel() - 1);
}

/**
 * @return The value to be written to the module field of a routing mux.
 */
UINT32 DigitalOutput::GetModuleForRouting()
{
	return DigitalModule::SlotToIndex(m_module->GetSlot());
}

/**
 * @return The value to be written to the analog trigger field of a routing mux.
 */
bool DigitalOutput::GetAnalogTriggerForRouting()
{
	return false;
}

/**
 * Request interrupts asynchronously on this digital output.
 * @param handler The address of the interrupt handler function of type tInterruptHandler that
 * will be called whenever there is an interrupt on the digitial output port.
 * Request interrupts in synchronus mode where the user program interrupt handler will be
 * called when an interrupt occurs.
 * The default is interrupt on rising edges only.
 */
void DigitalOutput::RequestInterrupts(tInterruptHandler handler, void *param)
{
	m_interruptIndex = interruptsResource->Allocate();
//TODO: check for error on allocation

	// Creates a manager too
	AllocateInterrupts(false);

	m_interrupt->writeConfig_WaitForAck(false, &status);
	m_interrupt->writeConfig_Source_AnalogTrigger(GetAnalogTriggerForRouting(), &status);
	m_interrupt->writeConfig_Source_Channel(GetChannelForRouting(), &status);
	m_interrupt->writeConfig_Source_Module(GetModuleForRouting(), &status);
	SetUpSourceEdge(true, false);

	m_manager->registerHandler(handler, param, &status);
	wpi_assertCleanStatus(status);
}

/**
 * Request interrupts synchronously on this digital output.
 * Request interrupts in synchronus mode where the user program will have to explicitly
 * wait for the interrupt to occur.
 * The default is interrupt on rising edges only.
 */
void DigitalOutput::RequestInterrupts()
{
	m_interruptIndex = interruptsResource->Allocate();
//TODO: check for errors

	AllocateInterrupts(true);

	m_interrupt->writeConfig_Source_AnalogTrigger(GetAnalogTriggerForRouting(), &status);
	m_interrupt->writeConfig_Source_Channel(GetChannelForRouting(), &status);
	m_interrupt->writeConfig_Source_Module(GetModuleForRouting(), &status);
	SetUpSourceEdge(true, false);
	wpi_assertCleanStatus(status);
}

void DigitalOutput::SetUpSourceEdge(bool risingEdge, bool fallingEdge)
{
	wpi_assert(m_interrupt != NULL);
	if (m_interrupt != NULL)
	{
		m_interrupt->writeConfig_RisingEdge(risingEdge, &status);
		m_interrupt->writeConfig_FallingEdge(fallingEdge, &status);
	}
	wpi_assertCleanStatus(status);
}

