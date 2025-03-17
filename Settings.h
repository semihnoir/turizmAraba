// Motor pinleri
const unsigned int IN1 = 6;
const unsigned int IN2 = 5;
const unsigned int IN3 = 4;
const unsigned int IN4 = 3;
const unsigned int ENA = 7;
const unsigned int ENB = 2;
// Sensör pinlerini tanımlayalım
const int butonBlack = A0;
const int sensorPinLeft = A2;   // Sol sensör
const int sensorPinRight = A1;  // Sağ sensör
const int sensorPinMiddle = A3; // Orta sensör
#define buzzer 13

// RFID pinleri
#define SS_PIN 10
#define RST_PIN 9

int fSpeed = 100;  // İleri hız
int aSpeed = 95;
int dSpeed = 0;  // Dönüş hızı
// 7.4 V için 100-120 
// 11.1 V için 57-65

int volume = 26;

SoftwareSerial mySerial(0, 1);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

bool isMoving = false;  // Buton durumu için bayrak

int renkSayaci = 1;  // Bölge sırasını takip etmek için sayaç

// Ses dosyalarının çalınma süreleri (milisaniye cinsinden)
int sesSureleriTr[] = { 27000, 25000, 23000, 20000, 24000, 21000, 26000 };  // 1-7.mp3
int sesSureleriEn[] = { 27000, 24000, 22000, 18000, 22000, 21000, 24000 };  // 14-20.mp3
int bolgeSesleriTr[] = {1,2,3,4,5,6,7}; // 1-7.mp3
int bolgeSesleriEn[] = {14,15,16,17,18,19,20}; // 14-20.mp3
int yolculukSesleri[] = {0, 13}; // 0 = Türkçe, 13 = İngilizce

// Son sensör durumlarını saklamak için değişkenler
int lastSensorLeft = 1;   // Son sol sensör durumu
int lastSensorMiddle = 1; // Son orta sensör durumu
int lastSensorRight = 1;  // Son sağ sensör durumu
int lastDirection = 0;    // Son hareket yönü (0: düz, 1: sol, 2: sağ)