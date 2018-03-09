#ifndef COMMUNICATEDEFINE_H
#define COMMUNICATEDEFINE_H

//ins communicate with downServr

//save ins
#define BufSize 1024*16

//servo max num by default
#define ServoNum 36

//ServoType
#define DigitalServoType 0x00
#define AnalogServoType 0x01
#define MixtureServoType 0x02

//instype--request
#define         GetServoPosition                0x00
#define         EnableServo                     0x01
#define         DisableServo                    0x02
#define         DeleteMiniAction                0x04
#define         ExecuteInstruction              0x05 //carry out ins
#define         UploadFileName                  0x06
#define         GetServoConnection              0x07
#define         ExcuteMiniAction                0x08
#define         DownloadMiniAction              0x10
#define         MoveServoPosition               0x12
#define         MoveServoPosWithSpeed           0x13
#define         UpLoadFile                      0x14
#define         DownSequence                    0x15 //save ins sequence
#define         ExectueSequence                 0x16 // carry out ins sequence

//instype-reply
#define         OK                      0x00
#define         ResServoPosInfo         0x01
#define         CheckError              0x02
#define         NoSuchServo             0x04
#define         NoSuchType              0x05
#define         NoSuchInsType           0x06
#define         NoSuchFile              0x07
#define         HaveActionFrame         0x09
#define         ConnectServo            0x0a
#define         UpLoadActionsNameType   0x0b
#define         Error                   0x0c
#define         OVERTIME                0x0d
#define         ResSensorData           0x0e// return Sensor Data

//sensorType
#define         SensorType              0x03

//instype--sensorType--request
#define 		CAMERA 			    0x01
#define 		ANGULAR 		    0x02
#define 		ACCERLERATION 		0x03
#define 		GYROSCOPE 		    0x04

//instye--sensorType--reply
#define			SensorOK		    0x01
#define			NoSuchSensorType	0x02

///////////////////////////////////////////////////////
#define         ResendData              0x00// check error and send again
#define         RequestPacket           0x01// request information
#define         ReplyPacket             0x02// reply information
#define         LogPacket               0x03// log information


#endif // COMMUNICATEDEFINE_H
