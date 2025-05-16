#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SERVICE_UUID        "a0b1c2d3-e4f5-a6b7-c8d9-e0f1a2b3c4d5"
#define TEMP_CHAR_UUID      "f000aa01-0451-4000-b000-000000000000"
#define HUM_CHAR_UUID       "f000aa02-0451-4000-b000-000000000000"

BLECharacteristic *tempCharacteristic;
BLECharacteristic *humCharacteristic;

void setup() {
  Serial.begin(115200);
  dht.begin();

  BLEDevice::init("ESP32_DHT11");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  tempCharacteristic = pService->createCharacteristic(
    TEMP_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  tempCharacteristic->addDescriptor(new BLE2902());

  humCharacteristic = pService->createCharacteristic(
    HUM_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  humCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
  Serial.println("Servidor BLE DHT11 iniciado...");
}

void loop() {
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  if (!isnan(temperatura) && !isnan(humedad)) {
    char tempStr[8], humStr[8];
    dtostrf(temperatura, 4, 2, tempStr);
    dtostrf(humedad, 4, 2, humStr);

    tempCharacteristic->setValue((uint8_t*)tempStr, strlen(tempStr));
    tempCharacteristic->notify();

    humCharacteristic->setValue((uint8_t*)humStr, strlen(humStr));
    humCharacteristic->notify();

    Serial.print("Temp: "); Serial.print(tempStr); Serial.print(" C\t");
    Serial.print("Hum: "); Serial.println(humStr);
  } else {
    Serial.println("Error al leer DHT11");
  }

  delay(2000);
}






