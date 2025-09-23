#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define temp 32
/************ Global State (you don't need to change this!) ******************/
//  Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
// WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds for Publishing***************************************/
// Setup a feed called 'photocell' for publishing.
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/prueba");

/****************************** Feeds for Subscribing***************************************/
// Setup a feed called 'slider' for subscribing to changes on the slider
Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/slider", MQTT_QOS_1);


/* ****************Declaración de variables globales, para pines, datos, etc. *******************/
// Ej: int ledRojo = D1;


/*************************** Sketch Code ************************************/
// Callbacks, funciones para el manejo de la información obtenida en adafruit (subscribed)
void slidercallback(double x)
{
  Serial.print("Hey we're in a slider callback, the slider value is: ");
  Serial.println(x);
}



/*********************************************** SetUp *****************************/
void setup()
{
  
  // Definicion del tipo de pines o cosas necesarias para el funcionamiento de sensores
  // Ej: pinMode(<pin>, <INPUT | OUTPUT>);
  pinMode(temp, INPUT);
  // Iniciar la comunición Serial
  Serial.begin(115200);

  // Delay para comenzar la conexion WIFI
  delay(10);

  /****************************************** Connect to WiFi access point *************/
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /********************************************* Callback Functions for Subscribed Feeds *************/
  // Se setean los callbacks para las variables los cuales se ejecutaran cuando se reciba información
  // Sintaxis: <variableSubscribed>.setCallback(<callBack a usar>);
  

  // Configurar que MQTT se suscriba a los feed creados para subscribed
  // Sintaxis: mqtt.subscribe(&<variableSuscribed>);
 
}

uint32_t x = 0;
float v;
//***************************************************** Loop ********************************/
void loop()
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(10000);

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }

  /* Aquí se coloca el código de nuestro programa */
  x = analogRead(32);
  v = (x/4095)*3.33;
  v*=100;
  // Ejemplo de estructura de publicación
  Serial.print(F("\nEnviando valor a feed temperatura "));
  Serial.print(v);
  Serial.print(" ");
  Serial.print(x);
  Serial.print("...");
  if (!photocell.publish(x))
  {
    Serial.println(F("Fallido!"));
  }
  else
  {
    Serial.println(F("Éxito!"));
  }
  
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000); // wait 10 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}