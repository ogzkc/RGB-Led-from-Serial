#include <FastLED.h>

#define LED_PIN     7
#define NUM_LEDS    60
#define BRIGHTNESS  48
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define LED_GROUP 3
#define DELAY_LED 100

CRGB leds[NUM_LEDS];
String receivedText;

void setup() {

	delay(200);
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);
	delay(100);

	setColor(CRGB::DarkCyan);

	delay(10);

	Serial.begin(9600);
}

void loop() {

	if (Serial.available() != 0)
	{
		receivedText = Serial.readString();

		int red = getValue(receivedText, ',', 0).toInt();
		int green = getValue(receivedText, ',', 1).toInt();
		int blue = getValue(receivedText, ',', 2).toInt();

		setColor(CRGB(red, green, blue));

		Serial.println("Deneme: " + receivedText);
	}

	delay(20);

}

// Expecting data -> "255,255,255", "192,240,0" etc.
String getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setColor(CRGB color)
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = color;
	}
	FastLED.show();
}
