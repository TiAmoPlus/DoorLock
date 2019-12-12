//贵州大学航模社
//
#include <SoftwareSerial.h>
#include <SPI.h>
#include <RFID.h>

#include <Servo.h>
#define PIN_BT_RX 7
#define PIN_BT_TX 8
#define BT_PWD "OP114"

SoftwareSerial BTSerial(PIN_BT_RX, PIN_BT_TX);

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

String num = "";
int pos = 0;
#define resetTimeHour 1//宏定义超时时间
unsigned long resetTime;
unsigned long runTime;//记录运行时间


String password2[] = { "175338133254", "77022819182", "22474024197", "18112723082126", "13922924119790" }; //登记教师卡号

String password1[] = { "13018329186146", "2072276216", "175525718220", "82294124126", "24218029186225",
					  "24217729186228", "791636618127", "175239109182155", "1117055182168", "792135518227", "207295518283",
					  "2391677673", "476520017723", "13013634186146", "24218529186236", "792939175", "11419029186107", "98430186194",
					  "16284118657", "162155118636", "472192365733", "22451503364", "1142542918643", "23912919917724", "14314155182131",
					  "20722714614349", "1481033415475", "151160795765", "20721851430"/*周松*/, "111170145143219"/*陈昊*/, "473283143211" /*赵林玉*/
};//登记会员卡号

int member = 0;
bool flag = false;
RFID rfid(10, 5);  //D10--读卡器MOSI引1脚、D5--读卡器RST引脚
void(*resetFunc) (void) = 0x0000; //制造重启命令
void setup()
{
	BTSerial.begin(9600);
	Serial.begin(9600);
	SPI.begin();
	rfid.init();
	myservo.attach(9);
	myservo.write(0);
	delay(500);
	//   member=sizeof(password);
	Serial.println("START!");
}
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
void loop()
{
	//myservo.write(0);
	// num==0;
	if (BTSerial.available())
	{

		String BT_String = BTSerial.readString();
		Serial.println(BT_String);
		if (BT_String == BT_PWD)
		{
			flag = true;
			Serial.println("Right Password! Open the door");
		}
		else
		{
			Serial.println("Wrong Password");
		}
	}
	BTSerial.println("hello");
	delay(1000);
	myreset();
	if (rfid.isCard())
	{
		readCard();//读卡
		judge();//判断刷入卡号是否登记
	}



	if (flag)
	{
		openTheDoor();//开门
	}
	myservo.detach();//使得舵机不加力
	pinMode(9, OUTPUT);
	digitalWrite(9, LOW);
	delay(90);
	num = "";//清空
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
	{
		myservo.attach(9);
		for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		  // in steps of 1 degree
			myservo.write(180);              // tell servo to go to position in variable 'pos'
			delay(5);                       // waits 15ms for the servo to reach the position
		}
		delay(3500);
		for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
			myservo.write(0);              // tell servo to go to position in variable 'pos'
			delay(5);                       // waits 15ms for the servo to reach the position
		}
		num = "";
		Serial.println(num);
		flag = false;
	}
}



void judge()
{
	Serial.println();

	member = sizeof(password1) / sizeof(password1[0]);
	for (member; member > 0; member--)
	{
		if (num == password1[member - 1])
		{
			flag = true;
			Serial.print("Welcom Member ");
			Serial.print("No.");
			Serial.print(member);
			Serial.print("  CARD:");
			Serial.println(num);
		}
	}

	int member2 = sizeof(password2) / sizeof(password2[0]);
	for (member2; member2 > 0; member2 = member2 - 1)
	{
		if (num == password2[member2 - 1])
		{
			flag = true;
			Serial.print("Welcom Teacher ");
			Serial.print("No.");
			Serial.print(member2);
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
