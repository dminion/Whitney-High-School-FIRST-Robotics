#include "WPILib.h"

DigitalInput *lefti;
DigitalInput *middle;
DigitalInput *righti;
float lFRVel=0.f;
float lFLVel=0.f;
int canHasLine=0;

/** 
 * Sets up the Line Tracking Interface, or something to that effect.
 * @Return - Returns -1 if a class could not be setup or some other shit broke
 */
int InitLines()
{
  lefti=new DigitalInput(1);
  middle=new DigitalInput(2);
  righti=new DigitalInput(3);
  if (left!=NULL&&middle!=NULL&&right!=NULL)
  {
    _DEBPRINT("Line Sensors Setup...\n");
    return 1;
  }
  else
  {
      _DEBPRINT("Oh Balls. Something Blew up in the Line Sensors\n");
      return -1;	 
  }
}

/**
 * Returns if the Robot is currently Roaming or it has a line its locked on to
 * @Return - Returns 1 if true, 0 if it is roaming
 */
int GetRoaming()
{
  return canHasLine;
}

/**
 * Calculates the velocity we want for line tracking
 * @Return - Returns an int describing which direction needs to be taken
 * 1 - On Track
 * 2 - Right of line, go left
 * 3 - Left of line, go right
 * 4 - Off Line - Oh noes D:
 * 5 - Fork - Choose which direction to go and adjust
 */
int CalcVel()
{
  int r,m,l;
  r=righti->Get();
  m=middle->Get();
  l=lefti->Get();
  
  //Need to check all possibilities on where we are
  //On line - Middle low, all others high
  //Right of line - Left low, all others high
  //Left of line - Right low, all others high
  //Off line - All High 
  //Fork - Left & Right low, middle high
  //Fun fun fun.
  
  //On line check. 
  if ((m==0) && (l==1) && (r==1))
    {
      canHasLine=1;
      return 1;
    }

  //Right of line check
  else if ((l==0) && (m==1) && (r==1))
    {
      canHasLine=1;
      return 2;
    }

  //Left of line check
  else if ((r==0) && (l==1)&& (m==1))
    {
      canHasLine=1;
      return 3;
    }

  //Off line check
  else if ((l==1) && (m==1) && (r==1))
    {
      canHasLine=0;
      return 4;
    }

  //Fork line check
  else if ((l==0) && (m==1) && (r==0))
    {
      canHasLine=1;
      return 5;
    }

  _DEBPRINT("Should not reach here Something died..\n");
  return -1;
}



