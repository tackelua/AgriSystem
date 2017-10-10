#include <SoftwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "com_inside.h"
#include "hardware.h"

#define DEBUG	Serial
#define DB(x)	DEBUG.println(x)
#define Db(x)	DEBUG.print(x)

SoftwareSerial CORE_SERIAL(CORE_SERIAL_RX, CORE_SERIAL_TX);
Adafruit_PCD8544 display(LCD_CLK, LCD_DIN, LCD_DC, LCD_CE, LCD_RST);

String core_serial_received;
void setup()
{
	delay(10);
	DEBUG.begin(115200);
	DEBUG.setTimeout(10);
	DEBUG.println(F("\r\n### E S P ###"));

	CORE_SERIAL.begin(9600);
	CORE_SERIAL.setTimeout(20);
	core_serial_received.reserve(100);

	hardware_init();

	display.begin();
	display.clearDisplay();

	display.setTextColor(BLACK);
	display.setTextSize(1);
	display.print("Hi Nidhi");
	display.display();

	//display.setTextColor(WHITE, BLACK);
	display.setCursor(0, 17);
	display.print("TEMP:");
	display.setCursor(0, 33);
	display.print("HUMI:");
	display.display();

	//display.setTextColor(WHITE, BLACK); // 'inverted' text

	//display.setTextColor(BLACK);
	//display.print("0x"); display.println(0xDEADBEEF, HEX);
	//display.display();
	//delay(2000);
}

void loop()
{

	CORE_SERIAL_handle();

}
