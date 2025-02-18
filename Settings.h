#define s0 8
#define s1 9
#define s2 10
#define s3 11
#define sensorOut 12

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


int fSpeed = 60;  // İleri hız
int dSpeed = 20;  // Dönüş hızı

SoftwareSerial mySerial(0, 1);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

bool isMoving = false;  // Buton durumu için bayrak

int K, Y, M = 0;                                                                           //3 ana renk için değişken tanımlıyoruz
int renkSayaci = 1;                                                                        // Renk sırasını takip etmek için sayaç
String renkler[] = { "Kırmızı", "Yeşil", "Mavi", "Kırmızı", "Yeşil", "Mavi", "Kırmızı" };  // Renk sırası (7 renk için)

// Ses dosyalarının çalınma süreleri (milisaniye cinsinden)
int sesSureleri[] = { 22000, 24000, 25000, 21000, 22000, 26000, 30000 };  // 1-7.mp3 için süreler (0.mp3 süresi kullanılmıyor)

// Kalibrasyon değerleri
int whiteCalibration[3] = { 0, 0, 0 };  // Beyaz kalibrasyon değerleri (Kırmızı, Yeşil, Mavi)
int blackCalibration[3] = { 0, 0, 0 };  // Siyah kalibrasyon değerleri (Kırmızı, Yeşil, Mavi)
int redCalibration[3] = { 0, 0, 0 };  // Kırmızı kalibrasyon değerleri (Kırmızı, Yeşil, Mavi)

int redThreshold = 60;