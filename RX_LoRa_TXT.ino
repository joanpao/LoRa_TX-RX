// Programa original de Club Robotica Granada
// https://clubroboticagranada.github.io/democratizandoCansat/comunicaciones_lora.html
// Modificado por EA5JTT 
// 20250303 Mofificados pinout y reboot continuo para LilyGo T-Display (Lilygo ESP32 LoRa433 T3V161)
// 20250303 Recepcion letras aleatorias

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED 25

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
//#define RST 23
#define RST 12
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
//#define BAND 866E6
#define BAND 433E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

int counter=0;
String LoRaData;

void setup() {

  //reset OLED display via software
  //pinMode(OLED_RST, OUTPUT);
  //digitalWrite(OLED_RST, LOW);
  //delay(20);
  //digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {  // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA RECEIVER ");
  display.display();

  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("Prueba de recepcion LoRa");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 10);
  display.println("LoRa Inicializacion OK!");
  display.display();
}

void loop() {

  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    //Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      counter++;
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      Serial.print("\r\n");
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    //Serial.print(" with RSSI ");
    //Serial.println(rssi);

    // Display information
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" RX LoRa - EA5JTT ");
    display.setCursor(0, 14);
    display.setTextSize(1);
    display.print("Paquetes recibidos: ");

    display.setCursor(0,26);
    display.print("  Contador: ");
    display.setCursor(65,26);
    display.print(counter); 

    display.setCursor(0,38);
    display.print("  Letra: ");
    display.setCursor(65,38);
    display.print(LoRaData);
    display.setCursor(0, 50);
    display.print("  RSSI:");
    display.setCursor(65, 50);
    display.print(rssi);
    display.display();
  }
}