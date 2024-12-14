// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// Variables para los cálculos
float roll = 0, pitch = 0;       // Ángulos calculados (en grados)
float angleZ = 0;               // Ángulo acumulado en el eje Z (en grados)
float alpha = 0.98;             // Peso del filtro complementario
unsigned long lastTime = 0;     // Para calcular el intervalo de tiempo entre lecturas


void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
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
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
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
  lastTime = millis(); // Inicializa el tiempo inicial
}

void loop() {

  // **CÁLCULO DEL TIEMPO TRANSCURRIDO (dt)**
  unsigned long currentTime = millis(); // Obtiene el tiempo actual
  float dt = (currentTime - lastTime) / 1000.0; // Intervalo de tiempo en segundos
  lastTime = currentTime; // Actualiza el tiempo de la última lectura

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Lecturas del acelerómetro (en m/s^2)
  float accX = a.acceleration.x;
  float accY = a.acceleration.y;
  float accZ = a.acceleration.z;

  // Lecturas del giroscopio (en rad/s, se convierten a grados/s)
  float gyroX = g.gyro.x * 180 / PI;
  float gyroY = g.gyro.y * 180 / PI;
  float gyroZ = g.gyro.z * 180 / PI;

  // Cálculo de ángulos con el acelerómetro
  float accPitch = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * 180 / PI;
  float accRoll = atan2(accY, sqrt(accX * accX + accZ * accZ)) * 180 / PI;

  // **USO DE dt EN EL FILTRO COMPLEMENTARIO**
  pitch = alpha * (pitch + gyroY * dt) + (1 - alpha) * accPitch; // Filtro para el eje Pitch
  roll = alpha * (roll + gyroX * dt) + (1 - alpha) * accRoll;    // Filtro para el eje Roll

  // **USO DE dt EN EL ÁNGULO ACUMULADO Z**
  angleZ += gyroZ * dt; // Acumula el ángulo en el eje Z

  // Imprimir los resultados calculados
  Serial.print("Pitch: "); Serial.print(pitch);
  Serial.print(" | Roll: "); Serial.print(roll);
  Serial.print(" | Ángulo acumulado Z: "); Serial.print(angleZ);
  Serial.println(" grados");

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

  Serial.println("");
  delay(500);
}
