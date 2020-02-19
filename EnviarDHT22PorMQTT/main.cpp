/*

  ____          _____               _ _           _       
 |  _ \        |  __ \             (_) |         | |      
 | |_) |_   _  | |__) |_ _ _ __ _____| |__  _   _| |_ ___ 
 |  _ <| | | | |  ___/ _` | '__|_  / | '_ \| | | | __/ _ \
 | |_) | |_| | | |  | (_| | |   / /| | |_) | |_| | ||  __/
 |____/ \__, | |_|   \__,_|_|  /___|_|_.__/ \__, |\__\___|
         __/ |                               __/ |        
        |___/                               |___/         
    
    Blog:       https://parzibyte.me/blog
    Ayuda:      https://parzibyte.me/blog/contrataciones-ayuda/
    Contacto:   https://parzibyte.me/blog/contacto/
    
    Copyright (c) 2020 Luis Cabrera Benito
    Licenciado bajo la licencia MIT
    
    El texto de arriba debe ser incluido en cualquier redistribucion
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define NOMBRE_RED "parzibyte"
#define PASSWORD_RED "hunter2"
#define DIRECCION_SERVIDOR_MQTT "192.168.1.79"
#define PUERTO_SERVIDOR_MQTT 1883
#define USUARIO_MQTT ""    // En mi caso es vacío
#define PASSWORD_MQTT ""   // En mi caso es vacío
#define TIEMPO_ESPERA 2000 // Cada cuánto leer del sensor

/*
Nota importante: si usas el pin D8 (como lo recomiendo)
  recuerda desconectar el lector del mismo cada vez que reinicies
  o quieras subir el código, pues el mismo "interfiere" con el
  monitor serial
*/

#define PIN_CONEXION D8   // A cuál pin está conectado el lector
#define TIPO_SENSOR DHT22 // Puede ser DHT11 también

#define PIN_LED LED_BUILTIN

DHT sensor(PIN_CONEXION, TIPO_SENSOR);
WiFiClient clienteWiFi;
PubSubClient clienteMqtt(clienteWiFi);
// El mensaje que se envía al servidor. Debe ser lo suficientemente
// grande
char mensaje[500] = "";
int ultimaVez = millis(); // Para enviar cada X segundos pero sin usar delay
float humedad, temperaturaEnGradosCelsius = 0;

/*
No tengo idea, pero parece que está invertido y al escribir un HIGH
se apaga
Relacionado: https://github.com/nodemcu/nodemcu-devkit-v1.0/issues/16
*/
void apagarLed()
{
  digitalWrite(PIN_LED, HIGH);
}

void encenderLed()
{
  digitalWrite(PIN_LED, LOW);
}

// La función que será llamada cuando se reciba un
// mensaje en el tópico al que nos vamos a suscribir
void callback(char *topico, byte *cargaUtil, unsigned int longitudDeDatos)
{
  if (longitudDeDatos <= 0)
    return;

  char primerCaracter = (char)cargaUtil[0];
  if (primerCaracter == '0')
    apagarLed();
  else if (primerCaracter == '1')
    encenderLed();
}

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  apagarLed();

  sensor.begin();

  // Conexión WiFi
  WiFi.begin(NOMBRE_RED, PASSWORD_RED);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  // Y ahora intentamos conectarnos al servidor MQTT
  clienteMqtt.setServer(DIRECCION_SERVIDOR_MQTT, PUERTO_SERVIDOR_MQTT);
  // Establecer la función que se invoca al recibir mensajes
  clienteMqtt.setCallback(callback);
  while (!clienteMqtt.connected())
  {
    if (!clienteMqtt.connect("ClienteESP8266", USUARIO_MQTT, PASSWORD_MQTT))
    {
      delay(2000);
    }
  }
  // Después de habernos conectado al servidor MQTT nos suscribimos
  clienteMqtt.subscribe("casa/sala/iluminacion");
}

// Ahora viene el loop, al cual se entra en caso de que en el setup
// todo haya ido bien

void loop()
{
  // Procesar los mensajes entrantes en caso de que existan
  clienteMqtt.loop();

  // Si han pasado más de 2 segundos desde la última vez, enviar un mensaje
  if (millis() - ultimaVez > TIEMPO_ESPERA)
  {
    humedad = sensor.readHumidity();
    temperaturaEnGradosCelsius = sensor.readTemperature();
    // En ocasiones puede devolver datos erróneos; por eso lo comprobamos
    if (isnan(temperaturaEnGradosCelsius) || isnan(humedad))
    {
      ultimaVez = millis();
      return;
    }

    // Formatear la temperatura y humedad
    sprintf(mensaje, "Temperatura: %.2f. Humedad: %.2f", temperaturaEnGradosCelsius, humedad);
    clienteMqtt.publish("jardin/temperatura", mensaje);
    ultimaVez = millis();
  }
  delay(10);
}
