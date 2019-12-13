//整理者：仰望
//整理时间：2017年8月20日
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <RFID.h>
//---------宏定义 各个引脚------------    
//---------RFID-----------------------
#define RFID_SS_PIN 10
#define RFID_RST_PIN 5
#define RFID_POWER_PIN 7//RFID的电源用引脚来控制

//---------SD-------------------------
#define SD_SS_PIN 4
#define SD_POWER_PIN 8 

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

unsigned long resetTime;
unsigned long runTime;//记录运行时间

String num = "";//用字符串 存储当前读到的卡号
int pos = 0;//舵机的角度
bool flag = false;//是否开门


Servo myservo;  //声明舵机对象
RFID rfid(RFID_SS_PIN, RFID_RST_PIN);   //D10--读卡器MOSI引脚、D5--读卡器RST引脚
void(*resetFunc) (void) = 0x0000; //制造重启命令

void myreset() {
	runTime = millis(); //获取板子运行时间
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
	digitalWrite(SD_POWER_PIN, LOW);
	delay(CHANGE_TIME);
	//打开RFID
	digitalWrite(RFID_POWER_PIN, HIGH);//RFID的电源给电
	delay(CHANGE_TIME);//延时一小会 再初始化
	rfid.init();//每次重新上电 都要重新初始化
}
void turnOnSD() {
	//关掉RFID
	digitalWrite(RFID_POWER_PIN, LOW);
	delay(CHANGE_TIME);
	//打开SD
	digitalWrite(SD_POWER_PIN, HIGH);//SD的电源给电
	delay(CHANGE_TIME);
	SD.begin(SD_SS_PIN);
}
void readCard()
{
	//Serial.println("Find the card!");
	//读取卡序列号
	if (rfid.readCardSerial()) {
		num.concat(rfid.serNum[0]);//拼接字符串
		num.concat(rfid.serNum[1]);
		num.concat(rfid.serNum[2]);
		num.concat(rfid.serNum[3]);
		num.concat(rfid.serNum[4]);
		//Serial.println(num);
	}
	//选卡，可返回卡容量（锁定卡片，防止多数读取），去掉本行将连续读卡
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
void judge()
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
			if (s == num) {//判断是否找到了人
				flag = true;//找到了人 令flag为真
				Serial.print("Welcom Teacher ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(s);
				return;
			}
			n++;//没找到就自增1
		}
	}
	else {
		Serial.println("error opening file teachers.txt");
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
			if (s == num) {//判断是否找到了人
				flag = true;//找到了人 令flag为真
				Serial.print("Welcom Student ");
				Serial.print("No.");
				Serial.print(n);
				Serial.print("  CARD:");
				Serial.println(s);
				return;
			}
			n++;//没找到就自增1
		}
	}
	else {
		Serial.println("error opening file students.txt");
	}
	if (!flag)
	{//如果是未注册的 就写到unexist.txt文件里 方便社员自己加入卡
		dataFile.close();
		delay(10);
		Serial.print("Unregistered CARD: ");
		Serial.println(num);
		dataFile = SD.open("unexist.txt", FILE_WRITE);
		if (dataFile) {
			dataFile.println(num + "/");
		}
		//没打开文件
		else {
			Serial.println("error opening file unexist.txt");
		}
	}
	dataFile.close();
	delay(10);
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
	pinMode(RFID_POWER_PIN, OUTPUT);
	pinMode(SD_POWER_PIN, OUTPUT);
	digitalWrite(RFID_POWER_PIN, LOW);
	digitalWrite(SD_POWER_PIN, LOW);
	delay(100);
	Serial.begin(9600);
	SPI.begin();
	myservo.attach(Servo_PIN);//舵机的初始化
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
	turnOnRFID();//初始化的时候 先初始化RFID 因为RFID的初始化 必须只执行一次 
}

void loop()
{
	//turnOnRFID();//打开RFID
	//找卡
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
	if (rfid.isCard())
	{
		readCard();//读卡
				   //读完卡后 把卡号存到了 String变量num里面 然后在判断之前 打开SD模块
		turnOnSD();
		judge();//判断刷入卡号是否登记
				//判断完后 SD关电 RFID重新上电 执行一次RFID的初始化函数
		turnOnRFID();
	}


	if (flag)
	{
		openTheDoor();//开门
	}
	myservo.detach();//使得舵机不加力
	pinMode(Servo_PIN, OUTPUT);
	digitalWrite(Servo_PIN, LOW);
	delay(90);
	num = "";//清空
}
