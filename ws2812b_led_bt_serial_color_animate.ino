#include <FastLED.h>
#include <SoftwareSerial.h>

#pragma region Defined and Constants

#define SERIAL_RX_PIN 5
#define SERIAL_TX_PIN 4

#define LED_PIN 8

#define WS2812B_PIN 10
#define NUM_LEDS    60  
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define DELAY_LED 20
#define LED_GROUP 3

#pragma endregion


#pragma region Variables

char data;
String receivingText = "";
String mode = "solid";
bool colorSet = false;
CRGB selectedColor = CRGB::Black;

#pragma endregion


#pragma region Components

SoftwareSerial BTSerial(SERIAL_RX_PIN, SERIAL_TX_PIN); // RX,TX
CRGB leds[NUM_LEDS];

#pragma endregion


void setup()
{
	delay(200);
	FastLED.addLeds<LED_TYPE, WS2812B_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);
	delay(100);

	Serial.begin(9600);
	BTSerial.begin(9600);

	delay(100);

	Serial.println("Ready");

	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH);
}

void loop()
{
	checkBT();

	if (mode == "animate")
		animateColor();
	else if (!colorSet)
		setColor();
}

bool checkBT()
{
	if (BTSerial.available())
	{
		data = (char)BTSerial.read();

		if (data == '#')
		{
			if (receivingText == "88")
			{
				mode = "animate";
				setColor(CRGB::Black);
				BTSerial.println("ok");
			}
			else if (receivingText == "99")
			{
				mode = "solid";
				setColor();
				BTSerial.println("ok");
			}
			else
			{
				int red = getValue(receivingText, ',', 0).toInt();
				int green = getValue(receivingText, ',', 1).toInt();
				int blue = getValue(receivingText, ',', 2).toInt();

				if (red >= 0 && red <= 255 && green >= 0 && green <= 255 && blue >= 0 && blue <= 255)
				{
					selectedColor = CRGB(red, green, blue);
					if (mode == "solid")
						colorSet = false;
					BTSerial.println("ok");
				}
			}

			Serial.println("Received " + receivingText);
			receivingText = "";
			delay(20);
		}
		else if (isdigit(data) || data == ',')
			receivingText.concat(data);

		return true;
	}
	else
		return false;
}

void animateColor()
{
	bool hasData = false;

	for (int i = 0; i < NUM_LEDS; i++)
	{
		if (i > 0)
			leds[i - 1] = CRGB::Black;

		for (int j = 0; j < LED_GROUP; j++)
		{
			if (i + j <= NUM_LEDS)
			{
				leds[i + j] = selectedColor;
			}
		}


		while (checkBT())
			hasData = true;

		if (hasData && mode != "animate")
			return;

		FastLED.show();

		delay(DELAY_LED);
	}

	for (int i = NUM_LEDS - 1; i >= 0; i--)
	{
		if (i < NUM_LEDS - 1)
			leds[i + 1] = CRGB::Black;

		for (int j = 0; j < LED_GROUP; j++)
		{
			if (i - j >= 0)
			{
				leds[i - j] = selectedColor;
			}
		}

		while (checkBT())
			hasData = true;

		if (hasData && mode != "animate")
			return;

		FastLED.show();

		delay(DELAY_LED);
	}

}

void setColor()
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = selectedColor;
	}
	FastLED.show();

	colorSet = true;
}

void setColor(CRGB color)
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = color;
	}
	FastLED.show();
}

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
