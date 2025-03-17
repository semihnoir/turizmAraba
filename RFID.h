#define SS_PIN 10  // RC522 SS pini
#define RST_PIN 9  // RC522 RST pini

MFRC522 rfid(SS_PIN, RST_PIN);

// Türkiye bölgeleri ve UID eşleşmeleri
struct Bolge {
  byte ID[7]; 
  String isim;
};

// UID'leri kendi kartlarının ID'leri ile değiştir!
Bolge bolgeler[7] = {
  {{0x1D, 0x08, 0xD7, 0x6C, 0x08, 0x10, 0x80}, "İç Anadolu"},
  {{0x1D, 0x4B, 0x2A, 0x6C, 0x08, 0x10, 0x80}, "Karadeniz"},
  {{0x1D, 0xB8, 0xDA, 0x6D, 0x08, 0x10, 0x80}, "Doğu Anadolu"},
  {{0x1D, 0x53, 0xDA, 0x6D, 0x08, 0x10, 0x80}, "Güneydoğu Anadolu"},
  {{0x1D, 0x06, 0xFD, 0x6D, 0x08, 0x10, 0x80}, "Akdeniz"},
  {{0x1D, 0xF0, 0xEB, 0x6D, 0x08, 0x10, 0x80}, "Ege"},
  {{0x1D, 0xCD, 0x4C, 0x6E, 0x08, 0x10, 0x80}, "Marmara"}
};

void rfidSetup() {
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Kartı okutun...");
}

void rfidLoop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID:");
  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  bool eslesme = false;

  for (int i = 0; i < 7; i++) {
    if (memcmp(rfid.uid.uidByte, bolgeler[i].ID, 4) == 0) {
      Serial.print("Kart ");
      Serial.print(bolgeler[i].isim);
      Serial.println(" ile eşleşti!");
      eslesme = true;
      break;
    }
  }

  if (!eslesme) {
    Serial.println("Bölge bulunamadı, tanımlı değil.");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}