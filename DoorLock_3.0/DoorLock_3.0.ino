//整理者：仰望
//整理时间：2017年8月20日

#include <MFRC522.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
//---------宏定义 各个引脚------------    
//---------RFID-----------------------
#define PIN_RFID_SS 10
#define PIN_RFID_RST 5
#define PIN_RFID_POWER 7//RFID的电源用引脚来控制

//---------SD-------------------------
#define PIN_SD_SS 4
#define PIN_SD_POWER 8 

//--------测电压----------------------
#define PIN_VOLTAGE A0

//--------蜂鸣器----------------------
#define PIN_BEEP	6
//--------低压指示灯------------------
#define PIN_LED_LOWVOLTAGE	3

//---------Servo----------------------
#define Servo_PIN 9

//---------两个模块切换的时间间隔 毫秒
#define CHANGE_TIME 10


//宏定义超时时间
#define resetTimeHour 1
const double setLowVoltage = 6.50; // 设置低压警戒线
bool hasWarning = false;	//电压过低报警 只报警一次
Servo myservo;  //声明舵机对象
MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
//MFRC522 myRfid(PIN_RFID_SS, PIN_RFID_RST);
void(*resetFunc) (void) = 0x0000; //制造重启命令

void myreset() {
	unsigned long runTime = millis(); //获取板子运行时间
	unsigned long resetTime;
	resetTime = resetTimeHour; // 转换为 毫秒(unsigned long类型)
	resetTime *= 60;
	resetTime *= 60;
	resetTime *= 1000;
	if (runTime > (resetTime)) {
		Serial.println(runTime);
		delay(1000);//延迟一会
		resetFunc();//超出运行时间 就软重启
	}
}
void turnOnRFID() {
	//关掉SD
	digitalWrite(PIN_SD_POWER, LOW);
	delay(CHANGE_TIME);
	//打开RFID
	digitalWrite(PIN_RFID_POWER, HIGH);//RFID的电源给电
	delay(CHANGE_TIME);//延时一小会 再初始化
	rfid.PCD_Init();
	//rfid.init();//每次重新上电 都要重新初始化
}
void turnOnSD() {
	//关掉RFID
	digitalWrite(PIN_RFID_POWER, LOW);
	delay(CHANGE_TIME);
	//打开SD
	digitalWrite(PIN_SD_POWER, HIGH);//SD的电源给电
	delay(CHANGE_TIME);
	SD.begin(PIN_SD_SS);
}
String readCard()
{
	String cardNum = "";
	//Serial.println("Find the card!");
	//读取卡序列号
	if (rfid.PICC_ReadCardSerial()) {
		//拼接字符串
		cardNum.concat(rfid.uid.uidByte[0]);
		cardNum.concat(rfid.uid.uidByte[1]);
		cardNum.concat(rfid.uid.uidByte[2]);
		cardNum.concat(rfid.uid.uidByte[3]);
		//Serial.println(num);
	}
	//选卡，可返回卡容量（锁定卡片，防止多数读取），去掉本行将连续读卡
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
	myservo.detach();//使得舵机不加力
	pinMode(Servo_PIN, OUTPUT);
	digitalWrite(Servo_PIN, LOW);
	delay(90);
}
/*
低压报警
*/
void Warning() {
	for (int i = 0; i < 2; i++)
	{
		//鸣笛两次
		for (int j = 1; j <= 360; j++)
		{
			//正弦波警报
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
	String s;//用来保存从文件读取的一行字符串(截止符'/'前面的字符串)
	Serial.println();
	int n;//用来计数的
	File dataFile;
	dataFile = SD.open("teachers.txt");//打开文件
	if (dataFile) {//判断文件是否打开成功
		n = 1;
		while (dataFile.available())//判断是否读完了整个文件
		{
			s = dataFile.readStringUntil('\n');//读取到换行符 结束(也就是读取一行的字符串)
											   //substring 截取字符串 第一个参数是起始位置 第二个参数是结束位置
											   //indexOf 查找一个字符的位置 返回位置
			s = s.substring(0, s.indexOf('/'));
			if (s.startsWith(cardNum)) {//判断是否找到了人
				//flag = true;//找到了人 令flag为真
				Serial.print("Welcome Teacher ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(cardNum);
				return true;
			}
			n++;//没找到就自增1
		}
	}
	else {
		Serial.println("error opening file teachers.txt");
		//return false;
	}
	dataFile.close();
	delay(10);
	dataFile = SD.open("students.txt");//打开文件
	if (dataFile) {//判断文件是否打开成功
		n = 1;
		while (dataFile.available())//判断是否读完了整个文件
		{
			s = dataFile.readStringUntil('\n');//读取到换行符 结束(也就是读取一行的字符串)
											   //substring 截取字符串 第一个参数是起始位置 第二个参数是结束位置
											   //indexOf 查找一个字符的位置 返回位置
			s = s.substring(0, s.indexOf('/'));
			if (s.startsWith(cardNum)) {//判断是否找到了人
				//flag = true;//找到了人 令flag为真
				Serial.print("Welcome Student ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(cardNum);
				return true;
			}
			n++;//没找到就自增1
		}
	}
	else {
		Serial.println("error opening file students.txt");
		//return false;
	}
	//如果是未注册的 就写到unexist.txt文件里 方便社员自己加入卡
	dataFile.close();
	delay(10);
	Serial.print("Unregistered CARD: ");
	Serial.println(cardNum);
	dataFile = SD.open("unexist.txt", FILE_WRITE);
	if (dataFile) {
		dataFile.println(cardNum + "/");
	}
	//没打开文件
	else {
		Serial.println("error opening file unexist.txt");
	}
	dataFile.close();
	delay(10);
	return false;
}
//检测是否低压 低压返回true
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
	myservo.attach(Servo_PIN);//舵机的初始化
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
	turnOnRFID();//初始化的时候 先初始化RFID 因为RFID的初始化 必须只执行一次 
}

void loop()
{
	//turnOnRFID();//打开RFID
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
	myreset();//检测是否执行软复位
	//找卡
	if (rfid.PICC_IsNewCardPresent())
	{
		String carNum = readCard();//读卡
		//读完卡后 把卡号存到了 String变量num里面 然后在判断之前 打开SD模块
		turnOnSD();
		if (judge(carNum) == true) {//判断刷入卡号是否登记
			openTheDoor();//开门
		}
		//判断完后 SD关电 RFID重新上电 执行一次RFID的初始化函数
		turnOnRFID();
	}
}
