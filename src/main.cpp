#include <SPI.h>
#include <LoRa.h>
#include <OneButton.h>
#include <DHTesp.h>

// WIFI_LoRa_32 ports
// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26

OneButton Button4(4, true, true);

DHTesp dht;

int counter = 0;
int lastHelloMs = 0;
long helloInterval = 5000;

void handleButtonClick() {
  static int buttonCounter = 0;
  buttonCounter++;

  Serial.print("BUTTON clicked #");
  Serial.println(buttonCounter);

  LoRa.beginPacket();
  LoRa.print("BUTTON #");
  LoRa.print(buttonCounter);
  LoRa.endPacket();
}

void handleButtonLongClick() {
  static int buttonCounter = 0;
  buttonCounter++;

  Serial.print("BUTTON long clicked #");
  Serial.println(buttonCounter);

  LoRa.beginPacket();
  LoRa.print("BUTTON LONG #");
  LoRa.print(buttonCounter);
  LoRa.endPacket();
}

void handleButtonDoublelick() {
  static int buttonCounter = 0;
  buttonCounter++;

  Serial.print("BUTTON double clicked #");
  Serial.println(buttonCounter);

  LoRa.beginPacket();
  LoRa.print("BUTTON DOUBLE #");
  LoRa.print(buttonCounter);
  LoRa.endPacket();
}

String getTemperature() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
	// Check if any reads failed and exit early (to try again).
	if (dht.getStatus() != 0) {
		return "DHT error status: " + String(dht.getStatusString());
	}

	float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
  float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);

	return "T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + "\nI:" + String(heatIndex) + " D:" + String(dewPoint);
}

void setup() {
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Transmitter");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Button4.attachClick(handleButtonClick);
  Button4.attachLongPressStop(handleButtonLongClick);
  Button4.attachDoubleClick(handleButtonDoublelick);

  dht.setup(0, DHTesp::DHT22);
}

void loop() {
  unsigned long ms = millis();

  Button4.tick();
  
  if (ms - lastHelloMs > helloInterval) {
    Serial.print("Sending packet: ");
    Serial.println(counter);

    // send  packet
    // LoRa.beginPacket();
    // LoRa.print("Ping ");
    // LoRa.print(counter);
    // LoRa.endPacket();

    LoRa.beginPacket();
    LoRa.print(getTemperature());
    LoRa.endPacket();

    counter++;
    lastHelloMs = ms;

  }
}
