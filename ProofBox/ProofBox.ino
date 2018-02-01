#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <DallasTemperature.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "e9181935ebf748b092567b7f19577de9";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TP-LINK";
char pass[] = "PandaBjorn";

#define DHTPIN D2          // What digital pin we're connected to
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

#define HEATERPIN D4

BlynkTimer ambientTempTimer;
BlynkTimer heaterTempTimer;
BlynkTimer connectionTimer;
BlynkTimer heaterTimer;
BlynkTimer dataTimer;

int heaterPower = LOW;
float ambientTemp = 50;
float heaterTemp = 50;

void readAmbientSensor()
{
	ambientTemp = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

	if (isnan(ambientTemp)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}
	Serial.print("Temperature is: ");
	Serial.println(ambientTemp);
}

void readDigitalThermometerTemp() {
	sensors.requestTemperatures();
	heaterTemp = sensors.getTempCByIndex(0);
	Serial.print("Heatertemp is: ");
	Serial.println(heaterTemp);
}

void checkConnection()
{
	if (WiFi.status() != WL_CONNECTED) {
		Blynk.begin(auth, ssid, pass);
	}
}

void controlHeaterElement() {
	if (ambientTemp < 24 || heaterTemp < 26)
	{
		heaterPower = HIGH;
	}
	else if (heaterTemp > 40) 
	{
		heaterPower = LOW;
	}
	else {
		heaterPower = LOW;
	}
	digitalWrite(HEATERPIN, heaterPower);
	Serial.print("Heater is: ");
	Serial.println(heaterPower);
}

void sendDataToBlynk() {
	Blynk.virtualWrite(V1, ambientTemp);
	Blynk.virtualWrite(V2, heaterPower);
	Blynk.virtualWrite(V0, heaterTemp);
}

void setup() {
	// Debug console
	Serial.begin(9600);

	Blynk.begin(auth, ssid, pass);

	dht.begin();

	pinMode(HEATERPIN, OUTPUT);

	ambientTempTimer.setInterval(5000L, readAmbientSensor);
	heaterTempTimer.setInterval(1000L, readDigitalThermometerTemp);
	heaterTimer.setInterval(1000L, controlHeaterElement);
	connectionTimer.setInterval(10000L, checkConnection);
	dataTimer.setInterval(200L, sendDataToBlynk);
}

void loop()
{
	Blynk.run();

	ambientTempTimer.run();
	heaterTempTimer.run();
	heaterTimer.run();
	connectionTimer.run();
	dataTimer.run();
}