#include <Timer.h>
#include <Button.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(19, 18, 15, 17, 14, 16);



// # Timers
Timer *hiddenTimer;
Timer *visibleTimer;

// # IO

#define RELAY 7

Button stopButton = Button(2, PULLUP);
Button startButton = Button(4, PULLUP);
Button switchButton = Button(8, PULLUP);


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
	lcd.begin(16, 2);
	// Init LCD
	lcd.setCursor(0, 0);
	// Print a message to the LCD.
	lcd.print("VELG PROGRAM : ");
	lcd.setCursor(0, 1);
	lcd.print("STANDARD");

}

void loop()
{

	if (startButton.isPressed() && startProgram) {
		
		Serial.println("Start detected.");


		switch (currentProgram) {
		case 0:    // STANDARD
			runProgram(0, 150000, 1);
			break;
		case 1:    // VM FIN / GROV
			runProgram(7000, 300000, 1);
			break;
		case 2:    // VMFIN / GROV DUELL
			runProgram(7000, 3000, 5);
			break;
		case 3:    // 7-20
			runProgram(7000, 20000, 1);
			break;
		case 4:    // 7-10
			runProgram(7000, 10000, 1);
			break;
		case 5:    // 7-8
			runProgram(7000, 8000, 1);
			break;
		case 6:    // 7-6
			runProgram(7000, 6000, 1);
			break;
		case 7:    // 7-4
			runProgram(7000, 4000, 3);
			break;
		case 8:    // CUSTOM
			runProgram(7000, 10000, 1);
			break;
		}

		delay(300);
	}



	if (switchButton.isPressed() && startProgram) {
		
		switchProgram();
		delay(300);
	}


}

void runProgram(int hidden, int visible, int repeat) {



	if (startProgram) {
		startProgram = false;

		Serial.println("We are in program start...");
		Serial.println(finished);

		while (!finished) {

			if (notifyUser) {
				// Notify user of progress.
				Serial.print("Run : ");
				Serial.print(runTimes + 1);
				Serial.print(" of ");
				Serial.println(repeat);
				notifyUser = false;



				lcd.setCursor(0, 0);
				lcd.print("KJORER PROGRAM : ");
				lcd.setCursor(0, 1);
				lcd.print("OMGANG : ");
				lcd.print(runTimes + 1);
				lcd.print("/");
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
				Serial.println("Program finished");

				stopProgram();
			}


		}
	}

	finished = false;
	hasStopped = false;
}

void stopProgram() {
	
	visibleTimer->Stop();
	hiddenTimer->Stop();

	// Free alloc mem.
	delete(visibleTimer);
	delete(hiddenTimer);
	
	hasStopped = true;
	initial = true;
	finished = true;
	startProgram = true;
	notifyUser = true;
	runTimes = 0;
	showTarget();
	Serial.println("Hard stop detected.");
	
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("KJORER PROGRAM : ");
	lcd.setCursor(0, 1);
	lcd.print("STOPPET!");

	

}

void switchProgram() {
	Serial.println("Switching.");

	if (currentProgram < numberOfPrograms) {
		currentProgram++;
	}

	else if (currentProgram = numberOfPrograms) {
		currentProgram = 0;
	}

	Serial.print("Current program is : ");
	Serial.println(currentProgram);

	// Update LCD.
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("VELG PROGRAM : ");
	lcd.setCursor(0, 1);
	switch (currentProgram) {
	case 0:    // STANDARD
		lcd.print("STANDARD");
		break;
	case 1:    // VM FIN / GROV
		lcd.print("VM FIN / GROV");

		break;
	case 2:    // VMFIN / GROV DUELL
		lcd.print("VM FIN / GROV DUELL");
		break;
	case 3:    // 7-20
		lcd.print("7-20");
		break;
	case 4:    // 7-10
		lcd.print("7-10");
		break;
	case 5:    // 7-8
		lcd.print("7-8");
		break;
	case 6:    // 7-6
		lcd.print("7-6");
		break;
	case 7:    // 7-4		
		lcd.print("7-4");
		break;
	case 8:    // CUSTOM
		lcd.print("CUSTOM");

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
	Serial.println("Hiding target..");

}

void showTarget() {
	digitalWrite(RELAY, 0);
	Serial.println("Showing target..");

}

