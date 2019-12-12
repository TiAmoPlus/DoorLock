//贵州大学航模社
//
#include <SPI.h>
#include <RFID.h>

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

String num;
int pos = 0;

String password2[] = {
  "175338133254",
  "77022819182",
  "22474024197",
  "18112723082126",
  "13922924119790"
};//登记教师卡号

String password1[] = { "13018329186146","2072276216","175525718220","82294124126","24218029186225",
"24217729186228","791636618127","175239109182155","1117055182168","792135518227","207295518283",
"2391677673","476520017723","13013634186146","24218529186236","792939175","11419029186107","98430186194",
"16284118657","162155118636","472192365733","22451503364","1142542918643","23912919917724","14314155182131",
"11127133143126","148193615457","1006325414552","1117029650","877615276","18062254145229","14322881143181",
"20722714614349","1481033415475","151160795765","20721851430"/*周松*/,"111170145143219"/*陈昊*/
};//登记会员卡号

int member = 0;
bool flag = false;
RFID rfid(11, 9);   //D10--读卡器MOSI引脚、D5--读卡器RST引脚

void setup()
{
	Serial.begin(9600);
	SPI.begin();
	rfid.init();
	myservo.attach(9);
	myservo.write(0);
	delay(500);
	//   member=sizeof(password);
	Serial.println("START!");
}

void loop()
{
	//myservo.write(0);
   // num==0;
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
	pinMode(1, OUTPUT);
	digitalWrite(1, LOW);

	delay(90);
}


void readCard()
{
	//Serial.println("Find the card!");
	//读取卡序列号
	if (rfid.readCardSerial()) {
		num.concat((String)rfid.serNum[0]);//拼接字符串
		num.concat((String)rfid.serNum[1]);
		num.concat((String)rfid.serNum[2]);
		num.concat((String)rfid.serNum[3]);
		num.concat((String)rfid.serNum[4]);
		Serial.println("123");
		Serial.println(num);

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
