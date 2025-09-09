#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>


#define i2c_Address 0x3c 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1   

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 




const int pinLM35 = A0;            
const unsigned long intervalo = 1000; // intervalo de actualización en ms
unsigned long ultimaMillis = 0;

void setup() {
  Serial.begin(9600);            
  while (!Serial) { /* espera en algunas placas */ }
  delay(50);

  Serial.println("LM35 - Lectura simple");
  display.begin(i2c_Address, true); // Address 0x3C default
  display.setContrast (255); // dim display
 
  display.clearDisplay();

  display.display();
}

void loop() {
  display.setTextSize(1); 
  //Set text color define el color del texto de la pantalla
  display.setTextColor(SH110X_WHITE); 
  


  unsigned long ahora = millis();
  if (ahora - ultimaMillis >= intervalo) {
    ultimaMillis = ahora;

    int lectura = analogRead(pinLM35);              // 0..1023
    const float Vref = 5.0;                         // ajustar si tu referencia ADC es distinta
    float voltaje = lectura * (Vref / 1023.0);      // voltaje en V
    float tempC = voltaje * 100.0;                  // LM35: 10 mV/°C -> factor 100


    Serial.print("Temperatura: ");
    Serial.print(tempC, 2);   
    Serial.println(" °C");
    display.setCursor(20, 0);
    display.println("Temperatura");
    display.setCursor(20,11);
    display.print(tempC, 2);
    display.println(" °C");

   display.display();
  

  display.clearDisplay();
  }

}
 
