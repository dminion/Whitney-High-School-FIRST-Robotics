#include "WPILib.h"

DigitalInput *left;
DigitalInput *middle;
DigitalInput *right;
float lFRVel=0.f;
float lFLVel=0.f;

typedef struct 
{
	float frVel;
	float flVel;
}lineVels;

/** 
 * Sets up the Line Tracking Interface, or something to that effect.
 * @Return - Returns -1 if a class could not be setup or some other shit broke
 */
int InitLines()
{
	left=new DigitalInput(1);
	middle=new DigitalInput(2);
	right=new DigitalInput(3);
	if (left!=NULL&&middle!=NULL&&right!=NULL)
	{
		printf("Line Sensors Setup...\n");
		return 1;
	else
	{
		printf("Oh Balls. Something Blew up in the Line Sensors\n");
		return -1;	 
	}
}

/**
 * Calculates the velocity we want for line tracking
 * @Return - Returns a populated lineVel struct that does magical things
 */
lineVels CalcVel()
{
	
}



