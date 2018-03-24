/*
 Name:		NexaRemote.ino
 Created:	3/23/2018 5:44:33 PM
 Author:	sindr
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "NexaTransmitter.h"


// WiFi settings
const char* ssid = "";
const char* password = "";

// MQTT settings
const char* mqtt_server = "";
const int mqtt_server_port = 1883;

const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_deviceId = "nexabridge-4j2jh9a";


const char* stateTopic = "home/lights/nexa/";
const char* commandTopic = "home/lights/nexa/+/set/";
const char* availabilityTopic = "home/lights/nexa/available/";

// Init WiFi client and PubSubClient
WiFiClient espClient;
PubSubClient client(espClient);

NexaTransmitter remote(D4, 9728630); // Create the nexa remote on pin2 with remote id

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

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Received message with topic: ");
	Serial.println(topic);
	// Get topic and use as sender id
	int id = strtol(topic + 17, nullptr, 10);

	char* message = new char[length + 1];
	message[length] = '\0';

	for (int i = 0; i < length; i++)
	{
		message[i] = (char)payload[i];
	}

	bool turnOn = strcmp((char*)message, "ON") == 0;

	delete[] message;


	
	Serial.print("Issuing: ");
	Serial.print(id);
	Serial.print(" ");
	Serial.println(turnOn);

	remote.setTransmiterId(id);
	remote.setSwitch(turnOn, 2);
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Status: ");
		Serial.println(client.state());
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect(mqtt_deviceId, mqtt_username, mqtt_password)) {
			Serial.println("connected");
			// Publish state
			// Subscribe to command topic
			client.subscribe(commandTopic);
			// Subscribe to availability topic. 
			client.subscribe(availabilityTopic);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
		Serial.print("Status: ");
		Serial.println(client.state());
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("start");

	// Connect to wifi 
	setup_wifi();

	// Set up client
	client.setServer(mqtt_server, mqtt_server_port);
	client.setCallback(callback);
}

void loop() {

	if (!WiFi.isConnected())
	{
		setup_wifi();
	}

	if (!client.connected()) {
		Serial.println("loop reconnect");
		reconnect();
	}

	client.loop();
}