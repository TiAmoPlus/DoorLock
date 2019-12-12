//���ݴ�ѧ��ģ��
//
#include <SPI.h>
#include <RFID.h>

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

String num = String(13);
int pos = 0;

String password2[] = { "175338133254","77022819182","22474024197","18112723082126","13922924119790" };//�Ǽǽ�ʦ����

String password1[] = { "13018329186146","16428253145212"/*���*/,"151438114394"/*������*/,"1111498414333"/*����Զ*/,"52251252145162"/*�ŷ�ʢ*/
};//�Ǽǻ�Ա����

int member = 0;
bool flag = false;
RFID rfid(10, 5);   //D10--������MOSI���š�D5--������RST����

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
		readCard();//����
		judge();//�ж�ˢ�뿨���Ƿ�Ǽ�
	}


	if (flag)
	{
		openTheDoor();//����
	}
	myservo.detach();//ʹ�ö��������
	pinMode(9, OUTPUT);
	digitalWrite(9, LOW);

	delay(90);
}


void readCard()
{
	//Serial.println("Find the card!");
	//��ȡ�����к�
	if (rfid.readCardSerial()) {
		num = (String)rfid.serNum[0] + (String)rfid.serNum[1] + (String)rfid.serNum[2] + (String)rfid.serNum[3] + (String)rfid.serNum[4];

		//Serial.println(num);

	}
	//ѡ�����ɷ��ؿ�������������Ƭ����ֹ������ȡ����ȥ�����н���������
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
		num = "0";
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
