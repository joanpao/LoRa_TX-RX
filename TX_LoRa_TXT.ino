// Programa original de Club Robotica Granada
// https://clubroboticagranada.github.io/democratizandoCansat/comunicaciones_lora.html
// Modificado por EA5JTT 
// 20250303 Mofificados pinout y reboot continuo para LilyGo T-Display (Lilygo ESP32 LoRa433 T3V161)
// 20250303 Envio letras aleatorias
// 
/*
 * Copyright (C) 2025 Juan Pablo Sanchez EA5JTT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
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

// LoRa frecuency
//
// EU433 (433.05 - 434.79 MHz) (Free transmision)
// ETSI has defined 433 to 434 MHz frequency band for LoRa application.
// It uses 433.175 MHz, 433.375 MHz and 433.575 MHz frequency channels.
//
// EU863 (863 - 870 MHz) (Free transmision)
//
// REST WORLD:
//433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
//
// HAM band (only with license)
// 2m 144 - 146 MHz Beacon in 144.400 - 144.491
// 70 cm 430 - 440 MHz BEacon in 432.400 - 432.490
//
// PMR band (Free transmision)
// PMR 446 446.400 - 446.490


//#define BAND 866E6
#define BAND 433E6
// Max TX Power in Lyligo ESP32 LoRa is 20 dBm = 100 mW
// Tested:  11dBm 12,5 mW, 14 25 mW, 17dBm 50 mW, 20 dBm 100 mW
#define TX_DBM 20

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//contador de tiempo
int counter=0;
String letra=" ";

// for random characters generation 
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,;:/?=";
const int alphabetLength = sizeof(alphabet) - 1;  // -1 para evitar el carácter nulo

//creacion de pantalla (objeto display)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);


void setup() {

  Serial.begin(115200);
  Serial.println("Prueba de envio LoRa");

  //reset OLED display via software
  //pinMode(OLED_RST, OUTPUT);
  //digitalWrite(OLED_RST, LOW);
  //delay(20);
  //digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(" LORA TX ");
  display.display();
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    display.setCursor(0,10);
    display.print("Starting LoRa failed!");
    display.display();
    Serial.println("Starting LoRa failed!");
    while (1);
    randomSeed(analogRead(0));  
  }
  //Serial.println("LoRa Inicializacion OK!");
  LoRa.setTxPower(TX_DBM);
  display.setCursor(0,10);
  display.print("Inicializacion OK!");
  display.display();
  delay(2000);
}

void loop() {
  digitalWrite(LED, HIGH);  // LED on
  envia_lora();
  envia_oled();
  counter++;
  digitalWrite(LED, LOW);  // LED off
  delay(1000);
}
//funciones

void envia_lora(){//Send LoRa packet to receiver
  LoRa.beginPacket();
  int randomIndex1 = random(0, alphabetLength);
  letra = alphabet[randomIndex1];  // Array for character and terminal sign
  LoRa.print(letra);
  LoRa.endPacket();
}

void envia_oled(){
  display.clearDisplay();

  display.setCursor(0,0);
  display.println("  TX LoRa - EA5JTT");

  display.setCursor(0,14);
  display.setTextSize(1);
  display.print("Paquetes enviados: ");

  display.setCursor(0,26);
  display.print("  Contador: ");
  display.setCursor(67,26);
  display.print(counter); 

  display.setCursor(0,38);
  display.print("  Letra: ");
  display.setCursor(67,38);
  display.print(letra);   

     display.setCursor(0, 50);
    display.print("  dBm/MHz:");
    display.setCursor(65, 50);
    display.print(TX_DBM);  
    display.setCursor(85, 50);
    //display.print(BAND); 
    // Formatear BAND para mostrarlo como "433.00"
  char bandStr[10]; // Suficiente espacio para "433.00" + terminador nulo
  dtostrf(BAND / 1000000.0, 3, 2, bandStr); // 3 dígitos en total, 2 después del punto decimal
  display.print(bandStr);



  display.display();
}
