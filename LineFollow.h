#include "Movement.h"

const int siyah = 0;
const int beyaz = 1;

// Çizgi takip fonksiyonu
void lineFollow() {
  int sensorValueLeft = digitalRead(sensorPinLeft);
  int sensorValueRight = digitalRead(sensorPinRight);
  int sensorValueMiddle = digitalRead(sensorPinMiddle);

  Serial.print("Sol: "); Serial.print(sensorValueLeft);
  Serial.print("  Orta: "); Serial.print(sensorValueMiddle);
  Serial.print("  Sağ: "); Serial.println(sensorValueRight);

  // Tüm sensörler beyaz görüyorsa (1,1,1) son yöne göre hareket et
  if (sensorValueLeft == siyah && sensorValueMiddle == siyah && sensorValueRight == siyah) {
    if (lastDirection == 1) {
      // Son hareket sola doğruysa, sola dönmeye devam et
      turnRight();
    } else if (lastDirection == 2) {
      // Son hareket sağa doğruysa, sağa dönmeye devam et
      turnLeft();
    } else {
      // Son hareket düz ise, düz gitmeye devam et
      moveForward();
    }
    return;
  }

  // Normal çizgi takip mantığı
  if ((sensorValueMiddle == beyaz && sensorValueLeft == siyah && sensorValueRight == siyah) 
      || (sensorValueMiddle == beyaz && sensorValueLeft == beyaz && sensorValueRight == beyaz)) {
    moveForward();
    lastDirection = 0;
  } 
  else if (sensorValueLeft == beyaz && sensorValueMiddle == siyah && sensorValueRight == siyah) {
    turnRight();
    lastDirection = 1;
  } 
  else if (sensorValueRight == beyaz && sensorValueMiddle == siyah && sensorValueLeft == siyah) {
    turnLeft();
    lastDirection = 2;
  } 
  else if (sensorValueLeft == beyaz && sensorValueMiddle == beyaz && sensorValueRight == siyah) {
    slightRight();
    lastDirection = 1;
  } 
  else if (sensorValueRight == beyaz && sensorValueMiddle == beyaz && sensorValueLeft == siyah) {
    slightLeft();
    lastDirection = 2;
  }

  // Son sensör durumlarını kaydet
  lastSensorLeft = sensorValueLeft;
  lastSensorMiddle = sensorValueMiddle;
  lastSensorRight = sensorValueRight;
}