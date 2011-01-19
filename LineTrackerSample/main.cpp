#include "WPILib.h"
#include "ADXL345_I2C.h"
#include <math.h>

float pidstuff = 0;
Encoder *lolcoder;
void DoStuff();
Joystick *leftStick;
Joystick *rightStick;
RobotDrive *myRobot;
DriverStation *ds;
PIDController *con;
PWM *motor;
Joystick *leftJoystick;
extern DriverStation *ds;
PWM *flMotor;
char flVel=0;
PWM *blMotor;
char blVel=0;
PWM *frMotor;
char frVel=0;
PWM *brMotor;
char brVel=0;
//volatile int i;
void TimerThread();
void MecanumThread();

#define KP 5 / 1000

unsigned char PCntrl(signed int target, signed int current)
{
	signed int error = 0;
	signed int p_out = 0;
	error = target - current;
	p_out = error * KP;
	if (p_out > 127)
		p_out=127;
	if (p_out < -127)
		p_out = -127;
	
	return (unsigned char)(p_out);
}

class LineTest : public SimpleRobot
{
  ADXL345_I2C *accel;
  int tid,i;
  
public:
  LineTest()
  {
    lolcoder=new Encoder(4,14,4,13);
    ds=DriverStation::GetInstance();
    printf("Online\n");
    leftStick=new Joystick(1);
    rightStick=new Joystick(2);
    //myRobot=new RobotDrive(1,1);
    //myRobot->SetExpiration(0.001);
    motor=new PWM(4,1);
  }
  void Autonomous()
  {
	  while (ds->IsEnabled())
	  {
	  }
	  printf("All your base are belong to us.\n");
  }
  
  void Disabled()
  {
  }
  void OperatorControl()
  {
	  //unsigned char c;
	  tid=taskCreate("Do Stuff Related To Work",100,0,0x1000,(FUNCPTR)MecanumThread,4,0,0,0,0,0,0,0,0,0);
	  taskActivate(tid);
	  lolcoder->Start();
	  lolcoder->Reset();
	  /*while (!leftStick->GetButton(Joystick::kTopButton))
	  {
		  bar++;
		  if (bar>=100000)
		  {
			  if (leftStick->GetButton(Joystick::kTriggerButton))
			  {
				  target+=10;
				  printf("Increasing target to: %d\n", target);
			  }
			  bar=0;
		  }
	  }*/
	  /*do
	  {
		  i=lolcoder->Get();
		  c=PCntrl(target,i);
		  j=c/255.0;
		  if (bar>=3000)
		  {
			j=c/255.0;
		  	printf("%f\n",j);
		  	bar=0;
		  }
		  myRobot->Drive(j,0);
		  bar++;
	  } while(i<=target&&ds->IsEnabled());*/
	  //lolcoder->Stop();
	  //myRobot->Drive(0,0);
	  /*lolcoder->Reset();
	  do
	  {
		  bar++;
		  error=((float)target-(float)lolcoder->Get())*KP; 
		  
		  if (error>1)
			  error=1;
		  if (error<-1)
			  error=-1;
		  if (bar>=100000)
		  {
			  printf("%f\n",error);
			  bar=0;
			  if (leftStick->GetButton(Joystick::kTriggerButton))
			  {
				  target+=10;
				  printf("Increasing target to: %d\n", target); 
			  }
		  }
		  speed=(char)((error*128)+128);
		  motor->SetRaw(speed);
	  } while (ds->IsEnabled());*/
  }
};

void TimerThread()
{
	int target=0;
	float error=0;
	char speed=0;
	printf("---Tim the Second has entered this world---\n");
	while (ds->IsEnabled())
	{
		Wait(0.01);
		error=((float)target-(float)lolcoder->Get())*KP; 
		  
		if (error>1)
			error=1;
		if (error<-1)
			error=-1;
		printf("%f\n",error);
		target+=(int)(leftStick->GetY()*-20);
		printf("Increasing target to: %d\n", target); 
		speed=(char)((error*128)+128);
		motor->SetRaw(speed);
	}
	printf("The cake is a lie.\n");
}

void MecanumThread()
{
	float x,y;
	printf("---Mecanum Thread Alive---\n");
	flMotor=new PWM(4,1);
	frMotor=new PWM(4,2);
	blMotor=new PWM(4,3);
	brMotor=new PWM(4,4);
	leftJoystick=new Joystick(1);
	
	while (ds->IsEnabled())
	{
		x=leftJoystick->GetX();
		y=leftJoystick->GetY();
		ds->SetDigitalOut(4,0);
		
		if (fabs(x) < fabs(y))
		{
			//forward and reverse
			flVel=(char)((y*64)+128);
			blVel=(char)((y*64)+128);
			frVel=(char)((y*64)+128);
			brVel=(char)((y*64)+128);
			flMotor->SetRaw(flVel);
			blMotor->SetRaw(blVel);
			frMotor->SetRaw(frVel);
			brMotor->SetRaw(blVel);
		}
		
		else if (fabs(x) > fabs(y))
		{
			if (x > 0.1)
			{
				if (leftJoystick->GetButton(Joystick::kTriggerButton))
				{
					//cw turn
					flVel=(char)((x*64)+128);
					blVel=(char)((x*64)+128);
					frVel=(char)((-x*64)+128);
					brVel=(char)((-x*64)+128);
				}
			
				else
				{
					//right shift
					flVel=(char)((x*64)+128);
					blVel=(char)((-x*64)+128);
					frVel=(char)((-x*64)+128);
					brVel=(char)((x*64)+128);
				}
			}
			
			else if (x < -0.1)
			{
				if (leftJoystick->GetButton(Joystick::kTriggerButton))
					{
						//ccw turn
						flVel=(char)((-x*64)+128);
						blVel=(char)((-x*64)+128);
						frVel=(char)((x*64)+128);
						brVel=(char)((x*64)+128);
					}
							
					else
					{
						//left shift
						flVel=(char)((-x*64)+128);
						blVel=(char)((x*64)+128);
						frVel=(char)((x*64)+128);
						brVel=(char)((-x*64)+128);
					}
			}
		}
		flMotor->SetRaw(flVel);
		blMotor->SetRaw(blVel);
		frMotor->SetRaw(frVel);
		brMotor->SetRaw(blVel);
		printf("\nX is %f\n", x);
		printf("Y is %f\n", y);
		printf("flVel is %d\n", flVel);
		printf("blVel is %d\n", blVel);
		printf("frVel is %d\n", frVel);
		printf("brVel is %d\n", brVel);
		/*//forward
		if (y>=0.1)
		{
			flVel=(char)((y*128)+128);
			blVel=(char)((y*128)+128);
			frVel=(char)((y*128)+128);
			brVel=(char)((y*128)+128);
		}
		
		//reverse
		else if (y<=-0.1)
		{
			flVel=(char)(-1 * (y*128)+128);
			blVel=(char)(-1 * (y*128)+128);
			frVel=(char)(-1 * (y*128)+128);
			brVel=(char)(-1 * (y*128)+128);
		}
		
		//right shift
		else if (x>=0.1)
		{
			if (leftJoystick->GetButton(Joystick::kTriggerButton))
			{
				//trigger is down, cw turn
				flVel=(char)((y*128)+128);
				blVel=(char)((y*128)+128);
				frVel=(char)(-1 * (y*128)+128);
				brVel=(char)(-1 * (y*128)+128);
			}
			
			else {
				flVel=(char)(-1 * (y*128)+128);
				blVel=(char)((y*128)+128);
				frVel=(char)((y*128)+128);
				brVel=(char)(-1 * (y*128)+128);
			}
		}
		
		//left shift
		else if (x<-0.1)
		{
			if (leftJoystick->GetButton(Joystick::kTriggerButton))
			{
				//trigger is down, ccw turn
				flVel=(char)(-1 * (y*128)+128);
				blVel=(char)(-1 * (y*128)+128);
				frVel=(char)((y*128)+128);
				brVel=(char)((y*128)+128);
			}
			
			else {
				flVel=(char)((y*128)+128);
				blVel=(char)((y*128)+128);
				frVel=(char)((y*128)+128);
				brVel=(char)((y*128)+128);
			}
		}*/
		
		Wait(0.1);
	}
}

START_ROBOT_CLASS(LineTest);
  
