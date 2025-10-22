//sensor DHT
#include "DHT.h" // Library DHT
#define DHTPIN 2 // deklarasi pin yang digunakan untuk sensor DHT
#define DHTTYPE DHT22 // mendefinisikan tipe DHT yang digunakan
DHT dht(DHTPIN, DHTTYPE); // deklarasi untuk mengenalkan pin DHT dan tipe DHT yang digunakan 

//module relay
#define RELAY_ON 1 // relay on high
#define RELAY_OFF 0 // relay off low
#define RELAY_1  13  // deklarasi pin 13 untuk relay 1
const int RELAY_2 = 8;  // deklarasi pin 8
#define RELAY_3  12 // deklarasi pin 12 untuk relay 3

//module LCD I2C
#include <LiquidCrystal_I2C.h> //Library LCD
LiquidCrystal_I2C lcd(0x27,16,2); //pin analog A4, A5

//RTC
#include "RTClib.h" // Library RTC
RTC_DS3231 rtc;
bool relayState = false;

//jadwal rak telur itu bergeser (bisa ditambah sesuai kebutuhan)
int scheduleHours[]   = {24, 4, 21, 21, 21, 20};   // jam
int scheduleMinutes[] = {0, 0, 3, 5, 7, 59};    // menit
int scheduleSecond[] = {0, 0, 0, 0, 0, 0};    // detik
int numSchedules = 6;              // jumlah jadwal

unsigned long RELAY_2_OnTime =0; // menyimpan waktu saat relay ON
const unsigned long RELAY_2_Duration = 12000; // menyala selama 12 detik

//#include <Servo.h> // Library Servo
//Servo motorServo; // membuat variabel servo untuk dikendalikan

void setup() {
  Serial.begin(9600); 
  Serial.println("DHT22 test!"); 
  dht.begin();
  
 // Sett pin keluaran.
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
   
 // Inisialisasi RELAY_1 mati sehingga saat reset akan mati secara default.
  digitalWrite(RELAY_1, RELAY_OFF);
 // Inisialisai RELAY_2 hidup secara default.
  digitalWrite(RELAY_2, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0); 
  lcd.print("INKUBATOR TELUR"); 
  lcd.setCursor(1,1); 
  lcd.print("TEDI MHE"); 
  delay(2000); //delay selama 2 detik / 2000m/s
  lcd.clear(); 
    
 // Set pin keluaran.
  //motorServo.attach(8); // motor servo Pada Pin digital 8 
  //Serial.print("Waktu telah di atur ke: ");
 // Clock.printTo(Serial);
 // Serial.println();

   if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi!");
    while (1);
  }

  // Jika RTC belum di-set, jalankan sekali:
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  

}

void loop() {

  // Baca humidity (kelembaban) dan temperature (suhu)
  float h = dht.readHumidity(); // pembaca Humidity/Kelembaban
  float t = dht.readTemperature(); // pembaca Temperatur/Suhu

  // Cek hasil pembacaan, dan tampilkan bila ok
  if (isnan(t) || isnan(h)) // isnan adalah type data yang digunakan oleh dht
  {
    Serial.println("Failed to read from DHT"); 
    return;
  }

  if (t<37.77) // inisialisasi jika suhu kurang dari 37.77 Relay 1 akan ON
{
  digitalWrite(RELAY_1, RELAY_ON);
}
 else if (t>38.33) // inisialisasi jika suhu lebih dari 38.33 Relay 1 akan OFF
{
  digitalWrite(RELAY_1, RELAY_OFF);
}

  if (h<50.00) // inisialisasi jika kelembaban kurang dari 50 Relay 3 akan ON
{
  digitalWrite(RELAY_3, RELAY_ON);
}
 else if (h>60.00) // inisialisasi jika kelembaban lebih dari 60 Relay 3 akan OFF
{
  digitalWrite(RELAY_3, RELAY_OFF);
}
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %t");
    
    Serial.print("Temperature: "); 
    Serial.print(t); 
    Serial.println(" *C");

  lcd.setCursor(0,0);
  lcd.print("Humi: "); 
  lcd.print(h);
  lcd.print(" %      ");
  lcd.setCursor(0,1);
  lcd.print("Temp: "); 
  lcd.print(t);
  lcd.print(" C      ");
        
   //motorServo.write(180);  // Memutar Servo ke kiri 180 derajat
   //delay(5000);   // waktu tunda dalam satuan second
   //motorServo.write(0);   // Memutar Servo ke kanan kembali ke 0 derajat
   //delay(5000);

  DateTime now = rtc.now();

  Serial.print("Waktu: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // Cek apakah waktu saat ini sama dengan jadwal
  for (int i = 0; i < numSchedules; i++) {
    if (now.hour() == scheduleHours[i] && now.minute() == scheduleMinutes[i] && now.second() == scheduleSecond[i] && !relayState) {
      digitalWrite(RELAY_2, LOW); // nyalakan RELAY_2 
      relayState = true;
      RELAY_2_OnTime = millis();       // simpan waktu mulai
      Serial.println("Geser RAK telur dimulai, RELAY_3 ON");
    }
  }

  // Jika relay ON, cek durasi
  if (relayState) {
    if (millis() - RELAY_2_OnTime >= RELAY_2_Duration) {
      digitalWrite(RELAY_2, HIGH); // matikan RELAY_2
      relayState = false;
      Serial.println("Geser RAK telur selesai, RELAY_2 OFF.");
    }
  }

  delay(1000);
}
