#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "com_inside.h"
#include "hardware.h"


Adafruit_PCD8544 display(D0, D5, D6, D7, D4);
void setup()
{

	display.begin();
	// init done

	// you can change the contrast around to adapt the display
	// for the best viewing!
	display.setContrast(50);

	display.display(); // show splashscreen
	delay(2000);
	display.clearDisplay();   // clears the screen and buffer

							  // draw a single pixel
	display.drawPixel(10, 10, BLACK);
	display.display();
	delay(2000);
	display.clearDisplay();

	// draw a circle, 10 pixel radius
	display.fillCircle(display.width() / 2, display.height() / 2, 10, BLACK);
	display.display();
	delay(2000);
	display.clearDisplay();

	// text display tests
	display.setTextSize(1);
	display.setTextColor(BLACK);
	display.setCursor(0, 0);
	display.println("Hello, world!");
	display.setTextColor(WHITE, BLACK); // 'inverted' text
	display.println(3.141592);
	display.setTextSize(2);
	display.setTextColor(BLACK);
	display.print("0x"); display.println(0xDEADBEEF, HEX);
	display.display();
	delay(2000);

	// rotation example
	display.clearDisplay();
	display.setRotation(1);  // rotate 90 degrees counter clockwise, can also use values of 2 and 3 to go further.
	display.setTextSize(1);
	display.setTextColor(BLACK);
	display.setCursor(0, 0);
	display.println("Rotation");
	display.setTextSize(2);
	display.println("Example!");
	display.display();
	delay(2000);

	// revert back to no rotation
	display.setRotation(0);


}

void loop()
{

  /* add main program code here */

}
