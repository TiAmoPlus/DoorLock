//�����ߣ�����
//����ʱ�䣺2017��8��20��
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <RFID.h>
//---------�궨�� ��������------------    
//---------RFID-----------------------
#define RFID_SS_PIN 10
#define RFID_RST_PIN 5
#define RFID_POWER_PIN 7//RFID�ĵ�Դ������������

//---------SD-------------------------
#define SD_SS_PIN 4
#define SD_POWER_PIN 8 

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

unsigned long resetTime;
unsigned long runTime;//��¼����ʱ��

String num = "";//���ַ��� �洢��ǰ�����Ŀ���
int pos = 0;//����ĽǶ�
bool flag = false;//�Ƿ���


Servo myservo;  //�����������
RFID rfid(RFID_SS_PIN, RFID_RST_PIN);   //D10--������MOSI���š�D5--������RST����
void(*resetFunc) (void) = 0x0000; //������������

void myreset() {
	runTime = millis(); //��ȡ��������ʱ��
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
	digitalWrite(SD_POWER_PIN, LOW);
	delay(CHANGE_TIME);
	//��RFID
	digitalWrite(RFID_POWER_PIN, HIGH);//RFID�ĵ�Դ����
	delay(CHANGE_TIME);//��ʱһС�� �ٳ�ʼ��
	rfid.init();//ÿ�������ϵ� ��Ҫ���³�ʼ��
}
void turnOnSD() {
	//�ص�RFID
	digitalWrite(RFID_POWER_PIN, LOW);
	delay(CHANGE_TIME);
	//��SD
	digitalWrite(SD_POWER_PIN, HIGH);//SD�ĵ�Դ����
	delay(CHANGE_TIME);
	SD.begin(SD_SS_PIN);
}
void readCard()
{
	//Serial.println("Find the card!");
	//��ȡ�����к�
	if (rfid.readCardSerial()) {
		num.concat(rfid.serNum[0]);//ƴ���ַ���
		num.concat(rfid.serNum[1]);
		num.concat(rfid.serNum[2]);
		num.concat(rfid.serNum[3]);
		num.concat(rfid.serNum[4]);
		//Serial.println(num);
	}
	//ѡ�����ɷ��ؿ�������������Ƭ����ֹ������ȡ����ȥ�����н���������
	rfid.selectTag(rfid.serNum);
}
void openTheDoor()
{
	myservo.attach(Servo_PIN);
	for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
											  // in steps of 1 degree
		myservo.write(pos);              // tell servo to go to position in variable 'pos'
		delay(5);                       // waits 15ms for the servo to reach the position
	}
	delay(3500);
	for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);              // tell servo to go to position in variable 'pos'
		delay(5);                       // waits 15ms for the servo to reach the position
	}
	num = "";
	Serial.println(num);
	flag = false;
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
void judge()
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
			if (s == num) {//�ж��Ƿ��ҵ�����
				flag = true;//�ҵ����� ��flagΪ��
				Serial.print("Welcom Teacher ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(s);
				return;
			}
			n++;//û�ҵ�������1
		}
	}
	else {
		Serial.println("error opening file teachers.txt");
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
			if (s == num) {//�ж��Ƿ��ҵ�����
				flag = true;//�ҵ����� ��flagΪ��
				Serial.print("Welcom Student ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(s);
				return;
			}
			n++;//û�ҵ�������1
		}
	}
	else {
		Serial.println("error opening file students.txt");
	}
	if (!flag)
	{//�����δע��� ��д��unexist.txt�ļ��� ������Ա�Լ����뿨
		dataFile.close();
		delay(10);
		Serial.print("Unregistered CARD: ");
		Serial.println(num);
		dataFile = SD.open("unexist.txt", FILE_WRITE);
		if (dataFile) {
			dataFile.println(num + "/");
		}
		//û���ļ�
		else {
			Serial.println("error opening file unexist.txt");
		}
	}
	dataFile.close();
	delay(10);
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
	pinMode(RFID_POWER_PIN, OUTPUT);
	pinMode(SD_POWER_PIN, OUTPUT);
	digitalWrite(RFID_POWER_PIN, LOW);
	digitalWrite(SD_POWER_PIN, LOW);
	delay(100);
	Serial.begin(9600);
	SPI.begin();
	myservo.attach(Servo_PIN);//����ĳ�ʼ��
	myservo.write(0);
	delay(100);
	//   member=sizeof(password);
	Serial.println("START!");
	digitalWrite(SD_POWER_PIN, HIGH);
	if (!SD.begin(SD_SS_PIN)) {
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
	//�ҿ�
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
	if (rfid.isCard())
	{
		readCard();//����
				   //���꿨�� �ѿ��Ŵ浽�� String����num���� Ȼ�����ж�֮ǰ ��SDģ��
		turnOnSD();
		judge();//�ж�ˢ�뿨���Ƿ�Ǽ�
				//�ж���� SD�ص� RFID�����ϵ� ִ��һ��RFID�ĳ�ʼ������
		turnOnRFID();
	}


	if (flag)
	{
		openTheDoor();//����
	}
	myservo.detach();//ʹ�ö��������
	pinMode(Servo_PIN, OUTPUT);
	digitalWrite(Servo_PIN, LOW);
	delay(90);
	num = "";//���
}
