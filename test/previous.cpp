#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Configurar G's máximos para el acelerómetro
// 

Adafruit_MPU6050 mpu;

float angleZ = 0;      // Para acumular el ángulo de rotación en el eje Z
unsigned long lastTime = 0;  // Para medir el tiempo entre lecturas
float deltaTime = 0;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pausa para Zero, Leonardo, etc hasta que se abra la consola serial

  Serial.println("Adafruit MPU6050 test!");

  // Intentamos inicializar el sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Configuración del sensor
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);  // Mantener latente la interrupción.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);


  lastTime = millis();  // Inicializamos el tiempo
}

void loop() {
  if (mpu.getMotionInterruptStatus()) {
    /* Obtener nuevos eventos del sensor con las lecturas */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Medimos el tiempo desde la última lectura
    unsigned long currentTime = millis();
    deltaTime = (currentTime - lastTime) / 1000.0;  // Convertimos a segundos
    lastTime = currentTime;

    // Calculamos el cambio de ángulo en el eje Z
    angleZ += g.gyro.z * deltaTime;  // g.gyro.z está en rad/s, multiplicamos por el tiempo para obtener el ángulo

    // Convertimos a grados
    angleZ *= 180 / PI;

    // Normalizamos el ángulo para que esté entre 0 y 360 grados
    if (angleZ >= 360) {
      angleZ = 0;
      Serial.println("¡Giro completo detectado en el plano XY!");
    } else if (angleZ < 0) {
      angleZ = 0;
    }

    // Mostrar la lectura del giroscopio y el ángulo acumulado
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);
    Serial.print(", Ángulo Z acumulado: ");
    Serial.println(angleZ);

    // Opcional: Imprimir también acelerómetro y temperatura si lo necesitas
    Serial.println("");
    Serial.print("Temperature: ");
    Serial.println(temp.temperature);
  }

  delay(10);  // Pequeño retraso para evitar sobrecargar el bus I2C
}
