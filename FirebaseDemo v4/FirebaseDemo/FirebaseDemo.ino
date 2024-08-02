
#include <ESP8266WiFi.h>
#include <ESP8266Firebase.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define _SSID "Fall"          // SSID WiFi 
#define _PASSWORD "12345678"   // Kata sandi WiFi 
#define REFERENCE_URL "https://hidroponik-b8394-default-rtdb.firebaseio.com/"  // URL referensi proyek Firebase 

Firebase firebase(REFERENCE_URL);

// Setup untuk LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat I2C 0x27, ukuran LCD 16x2

// Definisikan pin untuk sensor ultrasonik
const int trigPin_murni = D5;
const int echoPin_murni = D6;
const int trigPin_vitamin = D7;
const int echoPin_vitamin = D8;
const int trigPin_campuran = D7;
const int echoPin_campuran = D8;

// Definisikan pin untuk sensor TDS
const int tdsPinAnalog = A0; // Pin analog A0 untuk sensor TDS

// Definisikan pin untuk relay (pompa)
const int relayPin_Air = D4; // Pin digital D4 untuk relay
const int relayPin_Tds = D1; // Pin digital D1 untuk relay

// Definisikan variabel
long duration1, duration2, duration3;
int distance1, distance2, distance3;
int tdsValue;
int ketinggianAirMurni, ketinggianVitamin, ketinggianCampuran;

void setup() {
  Serial.begin(9600);                                        
  // Inisialisasi I2C dengan pin yang diinginkan
  Wire.begin(D3, D2);  // SDA ke D0, SCL ke D5

  lcd.init(); // Inisialisasi LCD
  lcd.backlight();
  lcd.clear();

  pinMode(trigPin_murni, OUTPUT);
  pinMode(echoPin_murni, INPUT);
  pinMode(trigPin_vitamin, OUTPUT);  
  pinMode(echoPin_vitamin, INPUT);
  pinMode(trigPin_campuran, OUTPUT);
  pinMode(echoPin_campuran, INPUT);

  pinMode(relayPin_Air, OUTPUT);  // Inisialisasi pin relay sebagai output
  digitalWrite(relayPin_Air, LOW); // relay mati awalnya

  pinMode(relayPin_Tds, OUTPUT);  // Inisialisasi pin relay sebagai output
  digitalWrite(relayPin_Tds, LOW); //  relay mati awalnya

  digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Hubungkan ke WiFi
  Serial.println();
  Serial.println();
  Serial.print("Menghubungkan ke: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  // Cetak alamat IP
  Serial.print("Alamat IP: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // Pemicu sensor ultrasonik pertama (ketinggian air murni)
  digitalWrite(trigPin_murni, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_murni, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_murni, LOW);

  // Ukur durasi echo untuk sensor pertama
  duration1 = pulseIn(echoPin_murni, HIGH);

  // Hitung jarak untuk sensor pertama dalam sentimeter
  distance1 = duration1 * 0.034 / 2;
  ketinggianAirMurni = 80 - distance1;
  // Cetak jarak untuk sensor pertama ke Serial Monitor
  Serial.print("Jarak (Air Murni): ");
  Serial.print(ketinggianAirMurni); //ini
  Serial.println(" cm");

  firebase.setInt("hidro/air_murni", ketinggianAirMurni);

  // Pemicu sensor ultrasonik kedua (ketinggian vitamin)
  digitalWrite(trigPin_vitamin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_vitamin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_vitamin, LOW);

  // Ukur durasi echo untuk sensor kedua
  duration2 = pulseIn(echoPin_vitamin, HIGH);

  // Hitung jarak untuk sensor kedua dalam sentimeter
  distance2 = duration2 * 0.034 / 2;
  ketinggianVitamin = 30 - distance2;
  // Cetak jarak untuk sensor kedua ke Serial Monitor
  Serial.print("Jarak (Vitamin): ");
  Serial.print(ketinggianVitamin);// ini
  Serial.println(" cm");

  firebase.setInt("hidro/vitamin", ketinggianVitamin);

  // Pemicu sensor ultrasonik ketiga (air campuran)
  digitalWrite(trigPin_campuran, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_campuran, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_campuran, LOW);

  // Ukur durasi echo untuk sensor ketiga
  duration3 = pulseIn(echoPin_campuran, HIGH);

  // Hitung jarak untuk sensor ketiga dalam sentimeter
  distance3 = duration3 * 0.034 / 2;
  ketinggianCampuran = 40 - distance3;
  // Cetak jarak untuk sensor ketiga ke Serial Monitor
  Serial.print("Jarak (Air Campuran): ");
  Serial.print(ketinggianCampuran);// ini
  Serial.println(" cm");

  firebase.setInt("hidro/air_campuran", ketinggianCampuran);

  // Baca nilai TDS
  tdsValue = analogRead(tdsPinAnalog);

  // Cetak nilai TDS ke Serial Monitor
  Serial.print("Nilai TDS: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  firebase.setInt("hidro/tds", tdsValue);

  // Tampilkan data pada LCD
  lcd.setCursor(0, 0);
  lcd.print("Air: ");
  lcd.print(distance1);
  lcd.print(" cm");

  lcd.setCursor(0, 1);
  lcd.print("TDS: ");
  lcd.print(tdsValue);
  lcd.print(" ppm");

 //Kontrol relay (pompa) berdasarkan kondisi  
      if (distance1 > 40) {
        digitalWrite(relayPin_Air, HIGH); // Nyalakan pompa
        Serial.println("Pompa Air OFF");
      }else if (distance1 < 30){
        digitalWrite(relayPin_Air, LOW); // Matikan pompa
        Serial.println("Pompa Air ON");
      }
      if (tdsValue < 1350){
        digitalWrite(relayPin_Tds, HIGH);
        Serial.println("Pompa Tds ON");
      }else if(tdsValue > 1500){
        digitalWrite(relayPin_Tds, LOW);
        Serial.println("Pompa Tds OFF");
      }

  delay(1000); // Tunggu 1 detik sebelum pengukuran berikutnya
}
//distance1 = Air Murni  ------ Ketinggian Air Murni(80) - distance1 = ketinggianAirMurni
//distance2 = Ketinggian Vitamin ------- Ketinggian Vitamin(30) - distance2 = ketinggianVitamin
//distance3 = Campuran -------- Ketinggian Campuran(40) - distance3 = ketinggianCampuran



