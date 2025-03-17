// Motor fonksiyonları
void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, fSpeed);
  analogWrite(ENB, fSpeed);
}

void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, fSpeed - 5);
  analogWrite(ENB, fSpeed - 5);
}


void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, dSpeed);
  analogWrite(ENB, aSpeed);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, aSpeed);
  analogWrite(ENB, dSpeed);
}

void slightRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, fSpeed / 2);
  analogWrite(ENB, fSpeed - 10);
}

void slightLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, fSpeed - 10);
  analogWrite(ENB, fSpeed / 2);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// Kademeli yavaşlama fonksiyonu
void gradualSlowdown() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  
  // Mevcut hızdan (fSpeed) sıfıra kadar kademeli azalt
  for (int currentSpeed = fSpeed; currentSpeed >= 0; currentSpeed -= 5) {
    analogWrite(ENA, currentSpeed);
    analogWrite(ENB, currentSpeed);
    delay(50); // Her kademe için 50ms bekle
  }
}