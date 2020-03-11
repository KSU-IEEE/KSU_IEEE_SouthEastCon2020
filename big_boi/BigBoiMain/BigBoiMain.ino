// includes
/**********************************************************************/
// #none

// struct definitions
/**********************************************************************/

// global variable definitions
/**********************************************************************/
struct motor {
  int pinA;
  int pinB;
  int pwmPin;
} ;
motor frontR, backR, frontL, backL;
motor grabL, grabR;

enum stateMachine {next, moveToBin, pickUp, moveToEnd, plop, quit} state;

struct location {
		int bottomLeftX;
		int bottomLeftY;
		int legosTaken;
		int binIndex;
		bool onTop;
    bool bin;
	};
location one, two, three, four, five, six, seven, eight, nine, zero, end;
location currTarget;

float currX, currY;		// best effor estimations
float percentX, percentY;		// set by diff function

float pi = 3.14159265359;
int currDigit;

float startTime;
// Piston Functions
/**********************************************************************/
void pushSolenoid(int pin) {
	digitalWrite(pin, LOW);
}

void pullSolenoid(int pin) {
	digitalWrite(pin, HIGH);
}

//void grab() {
//	pushSolenoid(pistonR);
//	pushSolenoid(pistonL);
//}
//
//void yeet() {
//	pushSolenoid(pistonR);
//	pushSolenoid(pistonL);
//}
// Low Level Motor Functions
/**********************************************************************/
void motor_clockwise(motor target, uint8_t pwm) {
  digitalWrite(target.pinA, HIGH);
  digitalWrite(target.pinB, LOW);
  analogWrite(target.pwmPin, pwm);  // for now just full speed
}

void motor_cclockwise(motor target, uint8_t pwm) {
  digitalWrite(target.pinA, LOW);
  digitalWrite(target.pinB, HIGH);
  analogWrite(target.pwmPin, pwm);  // for now just full speed
}

void halt(motor target) {
  digitalWrite(target.pinA, LOW);
  digitalWrite(target.pinB, HIGH);
  analogWrite(target.pwmPin, 0); // should stop if no PWM
}

void allHalt() {
  halt(frontR);
  halt(backR);
  halt(frontL);
  halt(backL);
}

void pin_init(motor target) {
  pinMode(target.pinA, OUTPUT);
  pinMode(target.pinB, OUTPUT);
  pinMode(target.pwmPin, OUTPUT);
}

// Higher Level Wheel Movement Functions
/**********************************************************************/
void move_right(uint8_t pwm) {
  motor_clockwise(frontR, pwm);
  motor_clockwise(backR, pwm);
  motor_cclockwise(frontL, pwm);
  motor_cclockwise(backL, pwm);
}

void move_left(uint8_t pwm) {
  motor_cclockwise(frontR, pwm);
  motor_cclockwise(backR, pwm);
  motor_clockwise(frontL, pwm);
  motor_clockwise(backL, pwm);
}

void move_up(uint8_t pwm) {
  motor_clockwise(frontR, pwm);
  motor_cclockwise(backR, pwm);
  motor_cclockwise(frontL, pwm);
  motor_clockwise(backL, pwm);
}

void move_down(uint8_t pwm) {
  motor_cclockwise(frontR, pwm);
  motor_clockwise(backR, pwm);
  motor_clockwise(frontL, pwm);
  motor_cclockwise(backL, pwm);
}

// drive functions
/**********************************************************************/
void percent_difference(location target) {
	// handle end location
	int endX = target.bottomLeftX;
	int endY = target.bottomLeftY;
	if(target.bin) {
		// X the same for Top and Bottom
		if (target.legosTaken%2==0) endX += 2;
		else endX += 7;

		// hand Y -- different for Top and Bottom
		// assuming that no more than 4 being taken
		if (target.onTop) {
			 if (target.legosTaken>2) endY += 2;
		} else {
			if (target.legosTaken<3) endY += 12;
			else endY += 10;
		}
	}

	// diffs
	int deltaX = endX - currX;
	int deltaY = endY - currY;

	// averages
	int xBar = (endX + currX)/2;
	int yBar = (endY + currY)/2;

	// set global variables 
	percentX = deltaX/xBar;
	percentY = deltaY/yBar;
}

// Grabber Functions
/**********************************************************************/

// misc
/**********************************************************************/
location getNext() {
	location returner;
	switch(currDigit) {
		case 0: returner = zero;
			break;
		case 1: returner = one;
			break;
		case 2: returner = two;
			break;
		case 3: returner = three;
			break;
		case 4: returner = four;
			break;
		case 5: returner = five;
			break;
		case 6: returner = six;
			break;
		case 7: returner = seven;
			break;
		case 8: returner = eight;
			break;
		case 9: returner = nine;
			break;
	}
	return returner;
}

// main functions
/**********************************************************************/
void setup() {
	state = next;

	/*
	Pins being Used:
	2,3,4,5
	14,15,16,17,18,19,20,21
	*/
	// pin inits 
	frontR.pwmPin = 2;
  	backR.pwmPin = 3;
  	frontL.pwmPin = 4;
  	backL.pwmPin = 5;

  	frontR.pinA = 14;
  	frontR.pinB = 15;

  	backR.pinA = 16;
  	backR.pinB = 17;

  	frontL.pinA = 18;
  	frontL.pinB = 19;

  	backL.pinA = 20;
  	backL.pinB = 21;

  	pin_init(frontR);
  	pin_init(backR);
  	pin_init(frontL);
  	pin_init(backL);

 	// start time
 	// startTime = 

}

void loop() {

	// major differences between moveToBin and moveToEnd is:
	// moveToBin moves in the X direction then Y direction
	// moveToEnd moves in the Y direction then X direction
	switch(state) {
		case next:
			currDigit = (int)(pi/10);
			pi = (pi-currDigit)*10;
			break;
		case moveToBin:
			percent_difference(currTarget);
			state = pickUp;
			break;
		case pickUp:
			// Pick dat boi up
			state = moveToEnd;
			break;
		case moveToEnd: 
			// perform movement to end
			state = plop;
			break;
		case plop:
			// plop
			state = moveToBin;
			break;
	}
}
