/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <Servo.h>
#include <SPI.h>
#include <RFID.h>
#include <SoftwareSerial.h>
#define resetTimeHour 1
#define OPEN_PASSWORD "OP114"

unsigned long resetTime;
unsigned long runTime;

String num = "";
SoftwareSerial mySerial(7, 8); // RX, TX
RFID rfid(10, 5);
bool flag = false;
Servo myservo;


String password[] = {
	"16428253145212",
	"20721851430",
	"111170145143219",
	"473283143211",
  "1642235162239"
};
void(*resetFunc) (void) = 0x0000;
void setup() {
	Serial.begin(9600);
	myservo.attach(9);
	myservo.write(0);
	delay(500);
	Serial.println("START!");
	// Open serial communications and wait for port to open:

	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	SPI.begin();
	rfid.init();
	// set the data rate for the SoftwareSerial port
	mySerial.begin(9600);

}

void loop() { // run over and over
	if (mySerial.available()) {
		String str = mySerial.readString();
		Serial.println(str);
		if (str.equals(OPEN_PASSWORD))
		{
			flag = true;
			Serial.println("Right Password! Open the door");
			mySerial.println("Right Password! Open the door");
		}
	}
	if (rfid.isCard())
	{
		readCard();
		judge();
	}
	if (flag)
	{
		openTheDoor();
	}
	myservo.detach();
	pinMode(9, OUTPUT);
	digitalWrite(9, LOW);
	delay(90);
	num = "";
}

void readCard()
{
	//Serial.println("Find the card!");
	if (rfid.readCardSerial()) {

		num.concat(rfid.serNum[0]);
		num.concat(rfid.serNum[1]);
		num.concat(rfid.serNum[2]);
		num.concat(rfid.serNum[3]);
		num.concat(rfid.serNum[4]);
		//Serial.println(num);
	}
	rfid.selectTag(rfid.serNum);
}
void judge()
{
	Serial.println();
	int member = sizeof(password) / sizeof(password[0]);
	for (member; member > 0; member--)
	{
		if (num == password[member - 1])
		{
			flag = true;
			Serial.print("Welcom Member ");
			Serial.print("No.");
			Serial.print(member);
			Serial.print("  CARD:");
			Serial.println(num);
		}
	}
	if (!flag)
	{
		Serial.print("Unregistered CARD: ");
		Serial.println(num);
	}
}
void openTheDoor()
{
	{
		myservo.attach(9);
		for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		  // in steps of 1 degree
			myservo.write(180);              // tell servo to go to position in variable 'pos'
			delay(5);                       // waits 15ms for the servo to reach the position
		}
		delay(3500);
		for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
			myservo.write(0);              // tell servo to go to position in variable 'pos'
			delay(5);                       // waits 15ms for the servo to reach the position
		}
		num = "";
		Serial.println(num);
		flag = false;
	}
}
void myreset() {
	runTime = millis();
	resetTime = resetTimeHour;
	resetTime *= 60;
	resetTime *= 60;
	resetTime *= 1000;
	if (runTime > (resetTime)) {
		Serial.println(runTime);
		delay(1000);
		resetFunc();
	}
}
