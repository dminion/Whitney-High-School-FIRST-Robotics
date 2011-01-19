cd C:\WindRiver\vxworks-6.3\target

mkdir h\WPILib
mkdir h\WPILib\CAN
mkdir h\WPILib\ChipObject
mkdir h\WPILib\CInterfaces
mkdir h\WPILib\NetworkCommunication
mkdir h\WPILib\visa
mkdir h\WPILib\Vision
mkdir h\WPILib\Vision2009

del h\WPILib\*.h
del h\WPILib\CAN\*.h
del h\WPILib\ChipObject\*.h
del h\WPILib\CInterfaces\*.h
del h\WPILib\NetworkCommunication\*.h
del h\WPILib\visa\*.h
del h\WPILib\Vision\*.h
del h\WPILib\Vision2009\*.h

copy c:\WindRiver\workspace\WPILib\*.h h\WPILib
copy c:\WindRiver\workspace\WPILib\CAN\*.h h\WPILib\CAN
copy c:\WindRiver\workspace\WPILib\ChipObject\*.h h\WPILib\ChipObject
copy C:\WindRiver\workspace\WPILib\CInterfaces\*.h h\WPILib\CInterfaces
copy C:\WindRiver\workspace\WPILib\NetworkCommunication\*.h h\WPILib\NetworkCommunication
copy c:\WindRiver\workspace\WPILib\visa\*.h h\WPILib\visa
copy c:\WindRiver\workspace\WPILib\Vision\*.h h\WPILib\Vision
copy c:\WindRiver\workspace\WPILib\Vision2009\*.h h\WPILib\Vision2009

copy C:\WindRiver\workspace\WPILib\PPC603gnu\WPILib\Debug\WPILib.a lib

copy c:\WindRiver\workspace\WorkbenchUpdate\frc_20*.zip c:\WindRiver\WPILib\cRIO_Images

cd \WindRiver\workspace\WPILib\Scripts
