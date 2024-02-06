/* This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <Servo.h>
#include <HCSR04.h>
#include <LiquidCrystal_I2C.h>
//#include <JC_Button.h>
//#include <ezButton.h>
//#include <QuadratureEncoder.h>


#define rMotorPin 11
#define encoderA 19
#define encoderB 18
volatile int countA = 0;
volatile int countB = 0;
int Dir = 0;  // 1 = CW
			  // 0 = Stationary
			  // -1 = CCW

#define lMotorPin 12


// ultrasonic sensors pins
#define ULTRA_CENTER_TRIG 22
//#define ULTRA_CENTER_ECHO 23
byte NUM_ECHO = 1;
byte* echoPins = new byte[NUM_ECHO] { 23 };

// Start Button
#define START_BUTTON 47 // change later
#define GND2 48

// LEDs
#define RED_LED 15
#define BLUE_LED 16
#define GREEN_LED 17

const float WHEEL_DIAMETER = 0.073025f;
const int TICKS_PER_REVOLUTION = 48;
int percent = 0;  //between -100 and 100, indicates how fast the motor goes



Servo driveMotorL;
Servo driveMotorR;
LiquidCrystal_I2C lcd(0x3F,16,2);
//ezButton myButton(47);
long ticks = 0l;

double* distances = HCSR04.measureDistanceIn();

void setup() {
	Serial.begin(9600);
	setupPins();
	HCSR04.begin(ULTRA_CENTER_TRIG, echoPins, NUM_ECHO);
	// digitalWrite(ULTRA_CENTER_TRIG, LOW);
	initLcd();
	//myButton.setDebounceTime(50); // set debounce time to 50 milliseconds
								  //attach
	fallbackMethod();
}

void setupPins() {
	driveMotorL.attach(rMotorPin);
	driveMotorR.attach(lMotorPin);
	pinMode(ULTRA_CENTER_TRIG, OUTPUT);
	//pinMode(ULTRA_CENTER_ECHO, INPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(START_BUTTON, INPUT);
	pinMode(GND2,OUTPUT);//define a digital pin as output
	digitalWrite(GND2, LOW);// set the above pin as LOW so it acts as Ground
	pinMode(encoderA, INPUT);
	pinMode(encoderB, INPUT);
	//attachInterrupt(digitalPinToInterrupt(encoderA), pulseA, RISING);
	//attachInterrupt(digitalPinToInterrupt(encoderB), pulseB, RISING);
}

// returns Distance 
float getDistance(int trigPin, int echoPin){
	// Clear the trigPin by setting it LOW:
	digitalWrite(trigPin, LOW);
	delayMicroseconds(5);

	// Trigger the sensor by setting the trigPin high for 10 microseconds:
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	// Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
	float duration = pulseIn(echoPin, HIGH);
	// Calculate the distance:
	float distance = ((int)((((duration * 0.034) / 2)*2)+.5)/2.0);

	return distance;
}

void printDistance() {
	for (int i = 0; i < NUM_ECHO; i++) {
		if (i > 0) Serial.print(" | ");
		lcd.setCursor(0,0);
		lcd.print(i + 1);
		lcd.print(": ");
		lcd.setCursor(0,1);
		lcd.print(distances[i]);
		lcd.print(" in");
	}
}

void initLcd() {
	lcd.init();
	lcd.backlight();
}

int getPWMval(int value) {
	return (value*5+1500);
}
void loop() {
	/*myButton.loop();
	  pulseA();
	 */
	int PWMvalue = percent * 5 + 1500; //scale up to 1000-2000
	/*
	   do {

	   driveMotorL.writeMicroseconds(getPWMval(100));
	   driveMotorR.writeMicroseconds(getPWMval(-100));

	//printDistance();

	delay(10);
	ticks = countA;
	} while(ticks)
	driveMotorL.writeMicroseconds(getPWMval(0));
	driveMotorR.writeMicroseconds(getPWMval(0));
	/*
	Serial.print(digitalRead(encoderA));      // uncomment to see squarewave output
	Serial.print('\t');
	//Serial.print(digitalRead(encoderB)+2);    // +2 shifts output B up so both A and B are visible 
	Serial.print('\t');
	//Serial.print(Dir);
	Serial.print('\t');
	Serial.print(countA);
	Serial.print('\t');
	Serial.print(countB);
	Serial.println();  
	 */
	//int btnState = myButton.getState();
	//Serial.println(btnState);
	/*
	   int state = myButton.getState();
	   if (state == HIGH)
	   Serial.println("The switch: OFF");
	   else
	   Serial.println("The switch: ON");
	 */
	fallbackMethod();
  //if(distances[0] <= 6){digitalWrite(13, HIGH);}
  //digitalWrite(13, HIGH);
}

void checkDirection(){
	if(digitalRead(encoderB) ==  HIGH){                             
		Dir = 1;  
	}
	else{
		Dir = -1;
	}
}

void pulseA(){  
	checkDirection();
	countA += Dir;
}

void pulseB(){  
	countB += Dir;
}


int cmToTicks(float cm) {
	return cm / (M_PI * WHEEL_DIAMETER) * TICKS_PER_REVOLUTION;
}

float ticksToCm(int ticks) {
	return (float)ticks / TICKS_PER_REVOLUTION * M_PI * WHEEL_DIAMETER;
}

void fallbackMethod(){
	//driveMotorL.writeMicroseconds(getPWMval(0));
	//driveMotorR.writeMicroseconds(getPWMval(0));
	// delay(50);
	driveMotorL.writeMicroseconds(getPWMval(40));
	driveMotorR.writeMicroseconds(getPWMval(-40));
	delay(10000);
	driveMotorL.writeMicroseconds(getPWMval(0));
	driveMotorR.writeMicroseconds(getPWMval(0));
	delay(100);
	driveMotorL.writeMicroseconds(getPWMval(20));
	driveMotorR.writeMicroseconds(getPWMval(20));
	delay(7000);
	driveMotorL.writeMicroseconds(getPWMval(0));
	driveMotorR.writeMicroseconds(getPWMval(0));
	delay(100);
	driveMotorL.writeMicroseconds(getPWMval(-20));
	driveMotorR.writeMicroseconds(getPWMval(-20));

}

void move(char direction, int time, int speed) {
	if (direction == "f") {
		driveMotorL.writeMicroseconds(getPWMval(speed));
		driveMotorR.writeMicroseconds(getPWMval(-speed));
		Serial.println("Going forward");
	}
	else if (direction == "b") {
		driveMotorL.writeMicroseconds(getPWMval(speed));
		driveMotorR.writeMicroseconds(getPWMval(-speed));
		Serial.println("Going forward");
	}
}
