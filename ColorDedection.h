void readColorValues() {
  digitalWrite(s2, LOW);  //Kırmızıyı filtrelemek için
  digitalWrite(s3, LOW);
  K = pulseIn(sensorOut, LOW);  //OUT pini üzerindeki LOW süresini okur
  Serial.print("Kırmızı= ");
  Serial.print(K);  //Kırmızı için aldığımız değeri serial monitöre yazdır
  Serial.print("  ");
  delay(50);               //50 milisaniye bekle
  digitalWrite(s2, HIGH);  //Yeşili filtrelemek için
  digitalWrite(s3, HIGH);
  Y = pulseIn(sensorOut, LOW);  //OUT pini üzerindeki LOW süresini okur
  Serial.print("Yeşil= ");
  Serial.print(Y);  //Yeşil için aldığımız değeri serial monitöre yazdır
  Serial.print("   ");
  delay(50);              //50 milisaniye bekle
  digitalWrite(s2, LOW);  //Maviyi filtrelemek için
  digitalWrite(s3, HIGH);
  M = pulseIn(sensorOut, LOW);  //OUT pini üzerindeki LOW süresini okur
  Serial.print("Mavi= ");
  Serial.print(M);  //Mavi için aldığımız değeri serial monitöre yazdır
  Serial.println();
  delay(50);  //50 milisaniye bekle
}

// Beyaz yüzeyi kalibre et
void calibrateWhite() {
  readColorValues();
  whiteCalibration[0] = K;
  whiteCalibration[1] = Y;
  whiteCalibration[2] = M;
}

// Siyah yüzeyi kalibre et
void calibrateBlack() {
  readColorValues();
  blackCalibration[0] = K;
  blackCalibration[1] = Y;
  blackCalibration[2] = M;
}

void calibrateRed() {
  readColorValues();
  redCalibration[0] = K;
  redCalibration[1] = Y;
  redCalibration[2] = M;
}


String detectColor(int K, int Y, int M) {
  // Kalibrasyon değerlerini kullanarak renkleri tespit et
  if (K < whiteCalibration[0] && Y < whiteCalibration[1] && M < whiteCalibration[2]) {
    return "Beyaz";
  } else if (K > blackCalibration[0] && Y > blackCalibration[1] && M > blackCalibration[2]) {
    return "Siyah";
  } else if (K > redCalibration[0] && Y < redCalibration[1] && M < redCalibration[2]) {  // Kırmızı için eşik
    return "Kırmızı";
  } else {
    return "Bilinmiyor";
  }
}