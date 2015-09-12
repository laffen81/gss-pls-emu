#include <Timer.h>
#include <Button.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(19, 18, 15, 17, 14, 16);





// # IO

#define RELAY 6

Button stopButton = Button(2, PULLUP);
Button startButton = Button(4, PULLUP);
Button switchButton = Button(7, PULLUP);


// # Global program variables...
bool hasStopped = false;
bool initial = true;
bool finished = false;
bool notifyUser = true;
bool startProgram = true;

// # Keeps track of repeating programs.
int runTimes = 0;

// # Programs
int currentProgram = 0;
int numberOfPrograms = 8;


void setup()
{

	Serial.begin(9600);
	
	runTimes = 0;
	
	// Init LCD
	lcd.begin(16, 2);
	lcd.setCursor(0, 0);
	
	// Print a message to the LCD.
	lcd.print(F("VELG PROGRAM : "));
	lcd.setCursor(0, 1);
	lcd.print(F("STANDARD"));

	pinMode(RELAY, OUTPUT);


}  


void loop()
{

	// Program start
	if (startButton.isPressed() && startProgram) {
		
		Serial.println(F("Start detected."));
		switchProgram(true);
		delay(300);
	}


	// Program picker
	if (switchButton.isPressed() && startProgram) {
		
		switchProgram(false);
		delay(300);
	}


}



void runProgram(int hidden, int visible, int repeat) {

	// # Timers
	Timer *hiddenTimer;
	Timer *visibleTimer;

	if (startProgram) {
		
		startProgram = false;

		while (!finished) {


			if (notifyUser) {
				
				// Notify user of progress.
				Serial.print(F("Run : "));
				Serial.print(runTimes + 1);
				Serial.print(F(" of "));
				Serial.println(repeat);
				notifyUser = false;

				lcd.setCursor(0, 0);
				lcd.print(F("KJORER PROGRAM : "));
				lcd.setCursor(0, 1);
				lcd.print(F("OMGANG : "));
				lcd.print(runTimes + 1);
				lcd.print(F("/"));
				lcd.print(repeat);
			}

			if (initial) {
				
				hideTarget();

				hiddenTimer = new Timer(hidden, &showTarget);
				visibleTimer = new Timer(hidden + visible, &finalizeRun);

				hiddenTimer->setSingleShot(true);
				hiddenTimer->Start();
				visibleTimer->setSingleShot(true);
				visibleTimer->Start();

				initial = false;
			}

			if (stopButton.isPressed() && !hasStopped) {
				stopProgram();
			}

			hiddenTimer->Update();
			visibleTimer->Update();


			if (runTimes >= repeat) {
				Serial.println(F("Program finished"));
				visibleTimer->Stop();
				hiddenTimer->Stop();
				stopProgram();
			}


		}
	}

	finished = false;
	hasStopped = false;
}

void stopProgram() {

	hasStopped = true;
	initial = true;
	finished = true;
	startProgram = true;
	notifyUser = true;
	runTimes = 0;
	showTarget();
	Serial.println(F("Hard stop detected."));
	
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("KJORER PROGRAM : "));
	lcd.setCursor(0, 1);
	lcd.print(F("STOPPET!"));

	

}

void switchProgram(bool run) {
	
	Serial.println(F("Switching."));

	if(!run) {
		if (currentProgram < numberOfPrograms) {
			currentProgram++;
		}

		else if (currentProgram = numberOfPrograms) {
			currentProgram = 0;
		}
	}

	Serial.print(F("Current program is : "));
	Serial.println(currentProgram);

	// Update LCD.
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("VELG PROGRAM : "));
	lcd.setCursor(0, 1);

	switch (currentProgram) {
	case 0:    // STANDARD
		lcd.print(F("STANDARD"));
		if(run) { runProgram(0, 150000, 1); }
		break;
	case 1:    // VM FIN / GROV
		lcd.print(F("VM FIN / GROV"));
		if (run) { runProgram(7000, 300000, 1); }
		break;
	case 2:    // VMFIN / GROV DUELL
		lcd.print(F("VM FIN / GROV DUELL"));
		if (run) { runProgram(7000, 3000, 5); }
		break;
	case 3:    // 7-20
		lcd.print(F("7-20"));
		if (run) { runProgram(7000, 20000, 1); }
		break;
	case 4:    // 7-10
		lcd.print(F("7-10"));
		if (run) { runProgram(7000, 10000, 1); }
		break;
	case 5:    // 7-8
		lcd.print(F("7-8"));
		if (run) { runProgram(7000, 8000, 1); }
		break;
	case 6:    // 7-6
		lcd.print(F("7-6"));
		if (run) { runProgram(7000, 6000, 1); }
		break;
	case 7:    // 7-4		
		lcd.print(F("7-4"));
		if (run) { runProgram(7000, 4000, 3); }
		break;
	case 8:    // CUSTOM
		lcd.print(F("CUSTOM"));
		if (run) { runProgram(7000, 10000, 1); }

		break;
	}

}


void finalizeRun() {
	hideTarget();
	runTimes++;
	initial = true;
	notifyUser = true;
}

void hideTarget() {
	digitalWrite(RELAY, 1);
	Serial.println(F("Hiding target.."));

}

void showTarget() {
	digitalWrite(RELAY, 0);
	Serial.println(F("Showing target.."));

}

