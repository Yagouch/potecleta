// Esta es la librería para utilizar las funciones de red del ESP8266
#include <ESP8266WiFi.h>

// Datos acceso AP Terrassa
const char* ssid = "Projectes";
const char* password = "Petrux10";

const char* host = "147.83.83.21";
const int httpPort = 8080;
// Token Provider
const char* token = "*******";


void setup() {
  Serial.begin(115200);
  delay(10);

  // Generamos una semilla de para la generación de la secuencia
  // de los nums aleatorios, leyendo el 'ruido' del ADC que está 'al aire'
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
}


void loop() {
  // log en consola
  Serial.print("connecting to ");
  Serial.println(host);

  // Creación de un objeto cliente para realizar la conexión al servidor
  WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // Simula lectura de sensor
  long randNumber = random(800);
  float temp_value = randNumber/10.0;
  
  // Construcción 'parcial' del mensaje HTTP para realizar la publicación de la medida
  String url = "http://147.83.83.21:8080/data/grup_5-101@prov";
  url += temp_value;
  
  // log en consola
  Serial.println("Sending HTTP resource:\r" + url);

  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               token + "\r\n\r\n");

  // Esperamos hasta 5seg la respuesta del servidor
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Superado el tiempo de espera !");
      client.stop();
      return;
    }
  }
  // Leemos la respuesta del servidor
  String HTTPResponse="";
  while (client.available()) {
    HTTPResponse += client.readStringUntil('\r');
  }
  // Cerramos connexión
  client.stop();
  
  // Verificamos el éxito/error de la publicación 
  if (HTTPResponse.startsWith("200 OK", 9) == false) {
    Serial.println("Got an Error from the server");
    Serial.print(HTTPResponse);
  }

  Serial.println("closing connection");
  Serial.println();
  Serial.println();
  // Tiempo entre publicación de las medidas
  delay(30000);
}
