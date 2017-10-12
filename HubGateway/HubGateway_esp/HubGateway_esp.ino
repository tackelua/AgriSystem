#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "com_inside.h"
#include "hardware.h"
#include "mqtt_helper.h"

SoftwareSerial CORE_SERIAL(CORE_SERIAL_RX, CORE_SERIAL_TX);
Adafruit_PCD8544 display(LCD_CLK, LCD_DIN, LCD_DC, LCD_CE, LCD_RST);
#define display_contrast_level	50

String core_serial_received;

void setup()
{
	delay(10);
	DEBUG.begin(115200);
	DEBUG.setTimeout(5);
	DEBUG.println(F("\r\n### E S P ###"));

	CORE_SERIAL.begin(9600);
	CORE_SERIAL.setTimeout(5);
	core_serial_received.reserve(100);

	hardware_init();

	display.begin();
	display.clearDisplay();
	display.setContrast(display_contrast_level);

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

	//
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
