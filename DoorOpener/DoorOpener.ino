/*

It connects to an MQTT server then:
- on "UNLOCK" uses to servo to push the unlock button


It will reconnect to the server if the connection is lost using a blocking
reconnect function. 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

// WiFi settings
const char* ssid = "";
const char* password = "";

// MQTT settings
const char* mqtt_server = "";
const int mqtt_server_port = 1883;

const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_deviceId = "dooropener-4j2jh9a";

const char* stateTopic = "home/frontdoor/";
const char* commandTopic = "home/frontdoor/set/";

// Init WiFi client and PubSubClient
WiFiClient espClient;
PubSubClient client(espClient);

// Servo settings
int servo_pin = D1;
int servo_power_pin = D2;
Servo servo;

#define POWER_SERVO_ON digitalWrite(servo_power_pin, HIGH); delay(250);
#define POWER_SERVO_OFF delay(250); digitalWrite(servo_power_pin, LOW);

void setup_wifi() {

	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void pressOpenButton() {
	client.publish(stateTopic, "UNLOCK");
	POWER_SERVO_ON
	servo.write(130);
	delay(250);
	servo.write(90);
	POWER_SERVO_OFF
	client.publish(stateTopic, "LOCK");
}

void callback(char* topic, byte* payload, unsigned int length) {
	char* message = new char[length + 1];
	message[length] = '\0';
	
	for (int i = 0; i < length; i++)
	{
		message[i] = (char)payload[i];
	}
	
	if(strcmp(message, "UNLOCK") == 0) {
		pressOpenButton();
	}

	delete[] message;
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect(mqtt_deviceId, mqtt_username, mqtt_password)) {
			Serial.println("connected");
			// Publish state
			client.publish(stateTopic, "LOCK");
			// Subscribe to command topic
			client.subscribe(commandTopic);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void setup() {
	//Servo
	pinMode(servo_power_pin, OUTPUT);
	digitalWrite(servo_power_pin, LOW);

	servo.attach(servo_pin);

	// Set serial speed
	Serial.begin(115200);
	
	// Connect to wifi 
	setup_wifi();      

	// Set up client
	client.setServer(mqtt_server, mqtt_server_port);
	client.setCallback(callback);
}

void loop() {

	if (!client.connected()) {
		reconnect();
	}
	client.loop();
}
