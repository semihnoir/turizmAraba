#include "Movement.h"

// Çizgi takip fonksiyonu
void lineFollow() {
  int sensorValueLeft = digitalRead(sensorPinLeft);
  int sensorValueRight = digitalRead(sensorPinRight);
  int sensorValueMiddle = digitalRead(sensorPinMiddle);

  Serial.print("Sol: "); Serial.print(sensorValueLeft);
  Serial.print("  Orta: "); Serial.print(sensorValueMiddle);
  Serial.print("  Sağ: "); Serial.println(sensorValueRight);

  // 1. Orta sensör aktifken ileri git
  if ((sensorValueMiddle == 0 && sensorValueLeft == 1 && sensorValueRight == 1) 
  || (sensorValueMiddle == 0 && sensorValueLeft == 0 && sensorValueRight == 0)) {
    Serial.println("Düz Git");
    moveForward();
  } 
  // 2. Sol sensör aktifken sağa dön
  else if (sensorValueLeft == 0 && sensorValueMiddle == 1 && sensorValueRight == 1) {
    Serial.println("Sağa Dön");
    turnRight();
  } 
  // 3. Sağ sensör aktifken sola dön
  else if (sensorValueRight == 0 && sensorValueMiddle == 1 && sensorValueLeft == 1) {
    Serial.println("Sola Dön");
    turnLeft();
  } 
  // 4. Sol ve orta sensör aynı anda çizgiyi algılarsa hafif sağa yönel
  else if (sensorValueLeft == 0 && sensorValueMiddle == 0 && sensorValueRight == 1) {
    Serial.println("Hafif Sağa Yönel");
    slightRight();
  } 
  // 5. Sağ ve orta sensör aynı anda çizgiyi algılarsa hafif sola yönel
  else if (sensorValueRight == 0 && sensorValueMiddle == 0 && sensorValueLeft == 1) {
    Serial.println("Hafif Sola Yönel");
    slightLeft();
  } 
  // 6. Üç sensör de çizgiyi algılarsa dur
  else {
    Serial.println("Geri");
    moveBackward();
  }
}