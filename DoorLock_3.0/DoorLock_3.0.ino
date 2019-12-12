//�����ߣ�����
//����ʱ�䣺2017��8��20��

#include <MFRC522.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
//---------�궨�� ��������------------    
//---------RFID-----------------------
#define PIN_RFID_SS 10
#define PIN_RFID_RST 5
#define PIN_RFID_POWER 7//RFID�ĵ�Դ������������

//---------SD-------------------------
#define PIN_SD_SS 4
#define PIN_SD_POWER 8 

//--------���ѹ----------------------
#define PIN_VOLTAGE A0

//--------������----------------------
#define PIN_BEEP	6
//--------��ѹָʾ��------------------
#define PIN_LED_LOWVOLTAGE	3

//---------Servo----------------------
#define Servo_PIN 9

//---------����ģ���л���ʱ���� ����
#define CHANGE_TIME 10


//�궨�峬ʱʱ��
#define resetTimeHour 1
const double setLowVoltage = 6.50; // ���õ�ѹ������
bool hasWarning = false;	//��ѹ���ͱ��� ֻ����һ��
Servo myservo;  //�����������
MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
//MFRC522 myRfid(PIN_RFID_SS, PIN_RFID_RST);
void(*resetFunc) (void) = 0x0000; //������������

void myreset() {
	unsigned long runTime = millis(); //��ȡ��������ʱ��
	unsigned long resetTime;
	resetTime = resetTimeHour; // ת��Ϊ ����(unsigned long����)
	resetTime *= 60;
	resetTime *= 60;
	resetTime *= 1000;
	if (runTime > (resetTime)) {
		Serial.println(runTime);
		delay(1000);//�ӳ�һ��
		resetFunc();//��������ʱ�� ��������
	}
}
void turnOnRFID() {
	//�ص�SD
	digitalWrite(PIN_SD_POWER, LOW);
	delay(CHANGE_TIME);
	//��RFID
	digitalWrite(PIN_RFID_POWER, HIGH);//RFID�ĵ�Դ����
	delay(CHANGE_TIME);//��ʱһС�� �ٳ�ʼ��
	rfid.PCD_Init();
	//rfid.init();//ÿ�������ϵ� ��Ҫ���³�ʼ��
}
void turnOnSD() {
	//�ص�RFID
	digitalWrite(PIN_RFID_POWER, LOW);
	delay(CHANGE_TIME);
	//��SD
	digitalWrite(PIN_SD_POWER, HIGH);//SD�ĵ�Դ����
	delay(CHANGE_TIME);
	SD.begin(PIN_SD_SS);
}
String readCard()
{
	String cardNum = "";
	//Serial.println("Find the card!");
	//��ȡ�����к�
	if (rfid.PICC_ReadCardSerial()) {
		//ƴ���ַ���
		cardNum.concat(rfid.uid.uidByte[0]);
		cardNum.concat(rfid.uid.uidByte[1]);
		cardNum.concat(rfid.uid.uidByte[2]);
		cardNum.concat(rfid.uid.uidByte[3]);
		//Serial.println(num);
	}
	//ѡ�����ɷ��ؿ�������������Ƭ����ֹ������ȡ����ȥ�����н���������
	//rfid.selectTag(rfid.serNum);
	rfid.PICC_Select(&(rfid.uid));
	return cardNum;
}
void openTheDoor()
{
	myservo.attach(Servo_PIN);
	for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
											  // in steps of 1 degree
		myservo.write(pos);              // tell servo to go to position in variable 'pos'
		delay(5);                       // waits 15ms for the servo to reach the position
	}
	delay(3500);
	for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);              // tell servo to go to position in variable 'pos'
		delay(5);                       // waits 15ms for the servo to reach the position
	}
	myservo.detach();//ʹ�ö��������
	pinMode(Servo_PIN, OUTPUT);
	digitalWrite(Servo_PIN, LOW);
	delay(90);
}
/*
��ѹ����
*/
void Warning() {
	for (int i = 0; i < 2; i++)
	{
		//��������
		for (int j = 1; j <= 360; j++)
		{
			//���Ҳ�����
			tone(PIN_BEEP, 3000 + 500 * sin(j * 3.14159 / 180.0));
			delay(10);
		}
	}
	noTone(PIN_BEEP);
	//Serial.println("no beep");
	hasWarning = true;
}
bool judge(String cardNum)
{
	String s;//����������ļ���ȡ��һ���ַ���(��ֹ��'/'ǰ����ַ���)
	Serial.println();
	int n;//����������
	File dataFile;
	dataFile = SD.open("teachers.txt");//���ļ�
	if (dataFile) {//�ж��ļ��Ƿ�򿪳ɹ�
		n = 1;
		while (dataFile.available())//�ж��Ƿ�����������ļ�
		{
			s = dataFile.readStringUntil('\n');//��ȡ�����з� ����(Ҳ���Ƕ�ȡһ�е��ַ���)
											   //substring ��ȡ�ַ��� ��һ����������ʼλ�� �ڶ��������ǽ���λ��
											   //indexOf ����һ���ַ���λ�� ����λ��
			s = s.substring(0, s.indexOf('/'));
			if (s.startsWith(cardNum)) {//�ж��Ƿ��ҵ�����
				//flag = true;//�ҵ����� ��flagΪ��
				Serial.print("Welcome Teacher ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(cardNum);
				return true;
			}
			n++;//û�ҵ�������1
		}
	}
	else {
		Serial.println("error opening file teachers.txt");
		//return false;
	}
	dataFile.close();
	delay(10);
	dataFile = SD.open("students.txt");//���ļ�
	if (dataFile) {//�ж��ļ��Ƿ�򿪳ɹ�
		n = 1;
		while (dataFile.available())//�ж��Ƿ�����������ļ�
		{
			s = dataFile.readStringUntil('\n');//��ȡ�����з� ����(Ҳ���Ƕ�ȡһ�е��ַ���)
											   //substring ��ȡ�ַ��� ��һ����������ʼλ�� �ڶ��������ǽ���λ��
											   //indexOf ����һ���ַ���λ�� ����λ��
			s = s.substring(0, s.indexOf('/'));
			if (s.startsWith(cardNum)) {//�ж��Ƿ��ҵ�����
				//flag = true;//�ҵ����� ��flagΪ��
				Serial.print("Welcome Student ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(cardNum);
				return true;
			}
			n++;//û�ҵ�������1
		}
	}
	else {
		Serial.println("error opening file students.txt");
		//return false;
	}
	//�����δע��� ��д��unexist.txt�ļ��� ������Ա�Լ����뿨
	dataFile.close();
	delay(10);
	Serial.print("Unregistered CARD: ");
	Serial.println(cardNum);
	dataFile = SD.open("unexist.txt", FILE_WRITE);
	if (dataFile) {
		dataFile.println(cardNum + "/");
	}
	//û���ļ�
	else {
		Serial.println("error opening file unexist.txt");
	}
	dataFile.close();
	delay(10);
	return false;
}
//����Ƿ��ѹ ��ѹ����true
bool checkVoltage() {
	int valA0 = analogRead(PIN_VOLTAGE);
	double realV = valA0 * 5 * 5 / 1024.0;
	//Serial.println(realV);
	if (realV < setLowVoltage) {
		return true;
	}
	else
	{
		return false;
	}

}
void setup()
{
	pinMode(PIN_LED_LOWVOLTAGE, OUTPUT);
	pinMode(PIN_BEEP, OUTPUT);
	pinMode(PIN_VOLTAGE, INPUT);
	noTone(PIN_BEEP);
	pinMode(PIN_RFID_POWER, OUTPUT);
	pinMode(PIN_SD_POWER, OUTPUT);
	digitalWrite(PIN_RFID_POWER, LOW);
	digitalWrite(PIN_SD_POWER, LOW);
	delay(100);
	Serial.begin(9600);
	SPI.begin();
	myservo.attach(Servo_PIN);//����ĳ�ʼ��
	myservo.write(0);
	delay(100);
	//   member=sizeof(password);
	Serial.println("START!");
	digitalWrite(PIN_SD_POWER, HIGH);
	if (!SD.begin(PIN_SD_SS)) {
		Serial.println("Card failed, or not present");
		// don't do anything more:
		//return;
	}
	else {
		Serial.println("card initialized.");
	}
	delay(CHANGE_TIME);
	turnOnRFID();//��ʼ����ʱ�� �ȳ�ʼ��RFID ��ΪRFID�ĳ�ʼ�� ����ִֻ��һ�� 
}

void loop()
{
	//turnOnRFID();//��RFID
	if (checkVoltage() == true) {
		Serial.println("low voltage warning!!!");
		digitalWrite(PIN_LED_LOWVOLTAGE, HIGH);
		if (hasWarning == false) {
			Warning();
		}
	}
	else
	{
		hasWarning = false;
		digitalWrite(PIN_LED_LOWVOLTAGE, LOW);
	}
	myreset();//����Ƿ�ִ����λ
	//�ҿ�
	if (rfid.PICC_IsNewCardPresent())
	{
		String carNum = readCard();//����
		//���꿨�� �ѿ��Ŵ浽�� String����num���� Ȼ�����ж�֮ǰ ��SDģ��
		turnOnSD();
		if (judge(carNum) == true) {//�ж�ˢ�뿨���Ƿ�Ǽ�
			openTheDoor();//����
		}
		//�ж���� SD�ص� RFID�����ϵ� ִ��һ��RFID�ĳ�ʼ������
		turnOnRFID();
	}
}
