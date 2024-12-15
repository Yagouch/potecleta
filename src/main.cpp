// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>

Adafruit_MPU6050 mpu;

int zeroCrossings = 0;
int fullRotations = 0;
bool wasPositive = false;

// Datos acceso AP Terrassa
const char* ssid = "MOVISTAR_D84E";
const char* password = ";9o2a3ei5RY#!:";

const char* host = "147.83.83.21";
const int httpPort = 8081;

const char* token = "56aaec441eb7e77f5668c6cff7455251a4bb96572423b385ff20beeeeab0d872";

String readHTTPResponse(WiFiClient &client) {
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Timeout waiting for response!");
      client.stop();
      return "ERROR: Timeout";
    }
  }
  String response = "";
  while (client.available()) {
    response += client.readStringUntil('\r');
  }
  return response;
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  randomSeed(analogRead(0));
  
  // Conectamos a la red WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Modo cliente WiFi
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  Serial.println("Adafruit MPU6050 test!");
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {

  Serial.print("connecting to ");
  Serial.println(host);

  // Creación de un objeto cliente para realizar la conexión al servidor
  WiFiClient client1;
  WiFiClient client2;
  
  if (!client1.connect(host, httpPort) || !client2.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");


  float xAccel = a.acceleration.x;
  if ((wasPositive && xAccel <= 0) || (!wasPositive && xAccel > 0)) {
    zeroCrossings++;
    wasPositive = xAccel > 0; // Actualiza el estado previo
    Serial.print("Cruce por cero detectado. Total: ");
    Serial.println(zeroCrossings);

    if (zeroCrossings % 2 == 0) {
      fullRotations++;
      Serial.print("Vueltas completas: ");
      Serial.println(fullRotations);
    }
  }

  Serial.println("");

  String url1 = "http://147.83.83.21:8081/data/grup_5-101@id_PEgrup5/temperature/";
  url1 += String(temp.temperature);

  String url2 = "http://147.83.83.21:8081/data/grup_5-101@id_PEgrup5/Gravedad/";
  url2 += String(a.acceleration.z);

  // log en consola
  Serial.println("Sending HTTP request to URL1:\r" + url1);
  Serial.println("Sending HTTP request to URL2:\r" + url2);

  client1.print(String("PUT ") + url1 + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "IDENTITY_KEY: " + token + 
                "\r\n\r\n");

  client2.print(String("PUT ") + url2 + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "IDENTITY_KEY: " + token + 
                "\r\n\r\n");


  /* Leer las respuestas de URL1 y URL2 */
  String HTTPResponse1 = readHTTPResponse(client1);
  String HTTPResponse2 = readHTTPResponse(client2);

  /* Mostrar las respuestas en la consola */
  Serial.println("Response from URL1:");
  Serial.println(HTTPResponse1);
  Serial.println("Response from URL2:");
  Serial.println(HTTPResponse2);

  /* Verificar éxito/error en ambas respuestas */
  if (!HTTPResponse1.startsWith("HTTP/1.1 200")) {
    Serial.println("Error from server on URL1");
    Serial.println(HTTPResponse1);
  }
  if (!HTTPResponse2.startsWith("HTTP/1.1 200")) {
    Serial.println("Error from server on URL2");
    Serial.println(HTTPResponse2);
  }

  Serial.println("Closing connections");
  client1.stop();
  client2.stop();
  Serial.println("");
  Serial.println("");
  delay(1000);
}



// Cuando la aceleracion de X es 0, es que el sensor esta en vertical,
// por lo que en una vuelta completa pasa dos veces por 0