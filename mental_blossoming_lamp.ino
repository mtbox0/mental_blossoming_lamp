/**
 * macro-toolbox Mental Blossoming Lamp
 * Copyright (c) 2010-2015, macro-toolbox.com and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Pulic License (GPL) version 3
 * which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Some parts of this code was written by neurosky or puzzlebox. These parts
 * have been marked like so.
 */
 
//Configure HC-05 as follow
//AT+NAME=BLOOM
//AT+UART=57600,0,0
//AT+ROLE=1
//AT+PSWD=0000
//AT+CMODE=0
//AT+BIND=2068,9D,79D551 (Mindwave Unique Number)
//AT+BIND=2068,9D,79D5DA (Laurent)
//AT+IAC=9E8B33
//AT+CLASS=0
//AT+INQM=1,9,48

//number of plots in our remap curves
//Nota : must be defined before any code as it's used in function definition
#define NBPLOTS 10

#include <Servo.h>
Servo myservo ; // Create the Servo object

//pins definition
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6
#define LEDGND_PIN 7
#define SERVO_PIN 2

#define TERMBAUDRATE 57600
#define BTBAUDRATE 57600

//uncomment to allow usb serail monitoring
//Leonardo use Serial1 for TTL and Serial for USB
#define MONITORING
#ifdef MONITORING
//if you want to use other Serial port then adapt line bellow
#define BT_BEGIN(x)			Serial1.begin(x);while (!Serial1) {}
#define BT_AVAILABLE		Serial1.available
#define BT_READ				Serial1.read
#define TERM_BEGIN(x)		Serial.begin(x)
#define TERM_PRINT(x)		Serial.print (x)
#define TERM_PRINTLN(x)		Serial.println (x)
#define TERM_PRINTDEC(x)	Serial.print (x, DEC)
#define TERM_PRINTDECLN(x)	Serial.println (x, DEC)
//uncomment if you want to log every byte received from BT
//Nota : if you uncomment, you have to adapt TERMBAUDRATE to be at 
//least equal to BTBAUDRATE
//#define DEBUGOUTPUT
#else
#define BT_BEGIN(x)			Serial1.begin(x);while (!Serial1) {}
#define BT_AVAILABLE		Serial1.available
#define BT_READ				Serial1.read
#define TERM_BEGIN(x)		
#define TERM_PRINT(x)
#define TERM_PRINTLN(x)
#define TERM_PRINTDEC(x)
#define TERM_PRINTDECLN(x)
#endif

//#define DEMO
#ifdef DEMO
byte attention = 100;
#else
byte attention = 0;
#endif

byte meditation = 0;
unsigned int rgbColour[3];
const int servoOpenedAngle = 10;
const int servoClosedAngle = 90;

void setup(){
	TERM_BEGIN(TERMBAUDRATE);			// USB
	BT_BEGIN(BTBAUDRATE);
	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);
        pinMode(LEDGND_PIN, OUTPUT);
        digitalWrite(LEDGND_PIN, LOW);
	// Start off with red.
	rgbColour[0] = 255;
	rgbColour[1] = 0;
	rgbColour[2] = 0;

	myservo.attach(SERVO_PIN);
	myservo.write(servoClosedAngle);//servoClosedAngle);
	randomSeed(analogRead(0));
}

void remapAndRamp(float input, float &target, float &result, float increment, float decrement, const float plots[NBPLOTS][2]){
	if(target>result) {
		result+=increment;
		if(result>target) result=target;
	}
	else if(target<result) {
		result-=decrement;	
		if(result<target) result=target;
	}
	
	for(int i=0;i<NBPLOTS;i++) {
		if(input<=plots[i][0]) {
			target=plots[i][1];
			break;
		}
	}
}

//eeg manager
float eegResult = 0;
//change below if you want a slower/faster reactivity of EEG 
//lower : slower response
//highe : faster response
float difficultyCoef=0.2;
void eegManager(){
	static float attentionTarget = 0;
	static float attentionResult = 0;
	static float meditationTarget = 0;
	static float meditationResult = 0;
	static unsigned long lastEegManagerMillis=0;
	
	if((millis()-lastEegManagerMillis) > 10) {
		lastEegManagerMillis=millis();
		
		//eeg result transformation curve
		const float eegPlots [NBPLOTS][2] = {
			{10,0.0}, {20,2.0}, {30,5.0}, {40,15.0}, {50,30.0}, 
			{60,50.0}, {70,75.0}, {80,95.0}, {90,100.0}, {100,100.0}};
		//process eeg headset values
		remapAndRamp(attention, attentionTarget, attentionResult, .1*difficultyCoef, .1*difficultyCoef, eegPlots);
		remapAndRamp(meditation, meditationTarget, meditationResult, .1*difficultyCoef, .1*difficultyCoef, eegPlots);
		eegResult=max(attentionResult, meditationResult);
		#ifdef DEMO
		static unsigned long lastDemoMaxResultMillis=0;
		if((millis()-lastDemoMaxResultMillis) > 1000) {
                  TERM_PRINT(" Attention: ");
		  TERM_PRINTDEC(attention);
		  TERM_PRINT(" Meditation: ");
		  TERM_PRINTDEC(meditation);
		  TERM_PRINT(" eegResult: ");
		  TERM_PRINTDECLN(eegResult);
			lastDemoMaxResultMillis=millis();
			static byte count=0;
			if(attentionResult>=100.0) count+=1;
			if(count>30) {
				attention=0;	
				difficultyCoef=0.5;	
			}
		}
		#endif
	}
}

//candle manager
float candlePowerResult=0;
void candleManager(){
	static float candlePowerTarget=0;
	static unsigned long candleDelay=0;
	static unsigned long lastCandleManagerMillis=0;

	if((millis()-lastCandleManagerMillis) > candleDelay) {
		lastCandleManagerMillis = millis();
		candleDelay = random(50)+10;
		
		//candle power curve
		const float candlePlots [NBPLOTS][2] = {
			{10,0.0}, {20,1.0/100}, {30,2.0/100}, {40,4.0/100}, {50,7.0/100}, 
			{60,10.0/100}, {70,6.0/100}, {80,2.0/100}, {90,1.0/100}, {100,0.0}};
		//process candle power
		remapAndRamp(eegResult, candlePowerTarget, candlePowerResult, .01, .02, candlePlots);
		//candle blink
		candlePowerTarget = candlePowerTarget*((float)random(3)+1);
	}
}

//rgbLoop manager
float rgbLoopPowerResult=0;
void rgbLoopManager() {
	static float rgbLoopPowerTarget=0;
	static int decColour = 0;
	static int colorIncrement=0;
	static unsigned long lastRgbLoopMillis=0;

	if((millis()-lastRgbLoopMillis) > 40) {
		lastRgbLoopMillis = millis();
		
		//start of puzzlebox code
		//please refer to https://github.com/PuzzleboxIO/bloom-design/tree/master/firmware/Bloom for the original
		// Choose the colours to increment and decrement.
		int incColour = decColour == 2 ? 0 : decColour + 1;
		// cross-fade the two colours.
		rgbColour[decColour] -= 1;
		rgbColour[incColour] += 1;
		colorIncrement+=1;
		if(colorIncrement==255) {
			colorIncrement=0;
			decColour+=1;
			if(decColour==3) decColour=0;
		}
		//end of puzzlebox code
		
		//rgbLoop power curve
		const float rgbLoopPlots [NBPLOTS][2] = {
			{10,0.0}, {20,0.0}, {30,0.0}, {40,0.0}, {50,0.0}, 
			{60,5.0/100}, {70,10.0/100}, {80,50.0/100}, {90,95.0/100}, {100,1.0}};
		//process rgbLoop power
		remapAndRamp(eegResult, rgbLoopPowerTarget, rgbLoopPowerResult, .002, .002, rgbLoopPlots);
	}
}

//light manager
void lightManager(){
	candleManager();
	rgbLoopManager();
	float rval = rgbLoopPowerResult*rgbColour[0];
	rval += candlePowerResult*0xFF; //orange
	if(rval>255) rval=255;
	float gval = rgbLoopPowerResult*rgbColour[1];
	gval += candlePowerResult*0x99; //orange
	if(gval>255) gval=255;
	float bval = rgbLoopPowerResult*rgbColour[2];
	bval += 0; //orange

	analogWrite(RED_PIN, (int)rval);
	analogWrite(GREEN_PIN, (int)gval);
	analogWrite(BLUE_PIN, (int)bval);
}

//servo amplitude manager
float servoAmplitudeResult=0;
void servoAmplitudeManager(){
	static float servoAmplitudeTarget=0;
	static unsigned long lastServoAmplitudeManagerMillis=0;

	if((millis()-lastServoAmplitudeManagerMillis) > 100) {
		lastServoAmplitudeManagerMillis = millis();
		
		//amplitude curve
		const float servoAmplitudePlots [NBPLOTS][2] = {
			{10,0.0}, {20,0.0}, {30,10.0}, {40,20.0}, {50,32.0}, 
			{60,45.0}, {70,62.0}, {80,80.0}, {90,90.0}, {100,100.0}};
		//process servo amplitude
		remapAndRamp(eegResult, servoAmplitudeTarget, servoAmplitudeResult, .5, .5, servoAmplitudePlots);
	}
}

//motor manager
const int servoPauseTime = 3000;
void servoManager(){
	static float servoAngleTarget=0;
	static float servoAngleResult=0;
	const int OPENING=0;
	const int CLOSING=1;
	const int WILLOPEN=2;
	const int WILLCLOSE=3;
	static int servoState = OPENING;
	static unsigned long lastServoManagerMillis=0;
	static unsigned long lastServoStateChangeMillis=0;

	servoAmplitudeManager();

	if((millis()-lastServoStateChangeMillis) > servoPauseTime) {
		lastServoStateChangeMillis = millis();
		if(servoState==WILLOPEN) servoState=OPENING;
		if(servoState==WILLCLOSE) servoState=CLOSING;
	}
	
	if((millis()-lastServoManagerMillis) > 50){
		lastServoManagerMillis = millis();
		//angle opening curve
		const float servoOpeningAnglePlots [NBPLOTS][2] = {
			{10,0.0}, {20,20.0}, {30,30.0}, {40,40.0}, {50,50.0}, 
			{60,60.0}, {70,70.0}, {80,80.0}, {90,90.0}, {100,100.0}};
		const float servoClosingAnglePlots [NBPLOTS][2] = {
			{10,0.0}, {20,0.0}, {30,0.0}, {40,0.0}, {50,0.0}, 
			{60,0.0}, {70,0.0}, {80,0.0}, {90,0.0}, {100,0.0}};
		//process servo angle
		if(servoState==OPENING) {
			remapAndRamp(servoAmplitudeResult, servoAngleTarget, servoAngleResult, 1, 0.8, servoOpeningAnglePlots);
			if(servoAngleResult>=servoAngleTarget) servoState=WILLCLOSE;
		}
			
		if(servoState==CLOSING){
			remapAndRamp(servoAmplitudeResult, servoAngleTarget, servoAngleResult, 1, 1.2, servoClosingAnglePlots);
			if(servoAngleResult<=servoAngleTarget) servoState=WILLOPEN;
		}	
		
		int angle = map(servoAngleResult, 0, 100, servoClosedAngle, servoOpenedAngle);
		//TERM_PRINT(" angle: ");
		//TERM_PRINTDECLN(angle);
		myservo.write(angle);
	}
}




//start of neurosky code
//please refer to http://developer.neurosky.com/docs/doku.php?id=arduino_tutorial for the original
////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
unsigned long lastReceivedPacketMillis = 0;
byte ReadOneByte(){
	int ByteRead;
	while(!BT_AVAILABLE()){
		eegManager();
		lightManager();
		servoManager();
		
	};

	ByteRead = BT_READ();
#ifdef DEBUGOUTPUT
	TERM_PRINT((char)ByteRead);	  // echo the same byte out the USB serial (for debug purposes)
#endif
	lastReceivedPacketMillis = millis();
	return ByteRead;
}
/////////////
//MAIN LOOP//
/////////////
void loop(){
	static byte generatedChecksum = 0;
	static byte checksum = 0;
	static int payloadLength = 0;
	static byte payloadData[64] = {0};
	static byte poorQuality = 200;

	static boolean bigPacket = false;
	
	
	// Look for sync bytes
  	if(ReadOneByte() == 170){
		if(ReadOneByte() == 170){
			//TERM_PRINTLN("nada");
			payloadLength = ReadOneByte();
			if(payloadLength > 169)			 //Payload length can not be greater than 169
				return;
			generatedChecksum = 0;
			for(int i = 0; i < payloadLength; i++){
				payloadData[i] = ReadOneByte();			   //Read payload into memory
				generatedChecksum += payloadData[i];
			}
			checksum = ReadOneByte();		  //Read checksum byte from stream
			generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum
			if(checksum == generatedChecksum){
				for(int i = 0; i < payloadLength; i++) {
					// Parse the payload
					switch (payloadData[i]){
						case 2:
							i++;
							poorQuality = payloadData[i];
							bigPacket = true;
							break;
						case 4:
							i++;
							attention = payloadData[i];
							break;
						case 5:
							i++;
							meditation = payloadData[i];
							break;
						case 0x80:
							i = i + 3;
							break;
						case 0x83:
							i = i + 25;
							break;
						default:
							break;
					} // switch
				} // for loop
#ifndef DEBUGOUTPUT
				//*** Add your code here ***
				if(poorQuality!=0) digitalWrite(13, HIGH);
				else digitalWrite(13, LOW);
#ifndef DEMO
				if(bigPacket){
					TERM_PRINT("PoorQuality: ");
					TERM_PRINTDEC(poorQuality);
					TERM_PRINT(" Attention: ");
					TERM_PRINTDEC(attention);
					TERM_PRINT(" Meditation: ");
					TERM_PRINTDEC(meditation);
					TERM_PRINT(" eegResult: ");
					TERM_PRINTDECLN(eegResult);
				}
#endif
#endif
				bigPacket = false;
			} else {
				// Checksum Error
			}	 // end if else for checksum
		} // end if read 0xAA byte
	} // end if read 0xAA byte
}
//end of neurosky code
