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

#define NOMBRE_RED "parzibyte"
#define PASSWORD_RED "hunter2"
#define DIRECCION_SERVIDOR_MQTT "192.168.1.79"
#define PUERTO_SERVIDOR_MQTT 1883
#define USUARIO_MQTT ""    // En mi caso es vacío
#define PASSWORD_MQTT ""   // En mi caso es vacío
#define TIEMPO_ESPERA 3000 // Cada cuánto enviar el mensaje

int ultimaVez = millis(); // Para enviar cada X segundos pero sin usar delay

WiFiClient clienteWiFi;
PubSubClient clienteMqtt(clienteWiFi);
// El mensaje que se envía al servidor. Debe ser lo suficientemente
// grande
char mensaje[500] = "";

// La función que será llamada cuando se reciba un
// mensaje en el tópico al que nos vamos a suscribir
void callback(char *topico, byte *cargaUtil, unsigned int longitudDeDatos)
{

  Serial.print("Se ha recibido un mensaje en: ");
  Serial.println(topico);

  Serial.print("Contenido: ");
  for (unsigned int i = 0; i < longitudDeDatos; i++)
  {
    Serial.print((char)cargaUtil[i]);
  }
  Serial.println();
}

void setup()
{
  Serial.begin(9600);

  // Conexión WiFi
  WiFi.begin(NOMBRE_RED, PASSWORD_RED);
  Serial.print("\nConectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Conectado");
  // Y ahora intentamos conectarnos al servidor MQTT
  clienteMqtt.setServer(DIRECCION_SERVIDOR_MQTT, PUERTO_SERVIDOR_MQTT);
  // Establecer la función que se invoca al recibir mensajes
  clienteMqtt.setCallback(callback);
  Serial.print("Conectando a servidor MQTT...");
  while (!clienteMqtt.connected())
  {
    if (clienteMqtt.connect("ClienteESP8266", USUARIO_MQTT, PASSWORD_MQTT))
    {
      Serial.println("Conectado");
    }
    else
    {
      Serial.print("Error conectando. Estado: ");
      Serial.println(clienteMqtt.state());
      delay(2000);
    }
  }
  // Después de habernos conectado al servidor MQTT nos suscribimos
  clienteMqtt.subscribe("casa/sala/iluminacion");
}

// Ahora viene el loop, al cual se entra en caso de que en el setup
// todo haya ido bien
int contador = 0;

void loop()
{
  // Procesar los mensajes entrantes en caso de que existan
  clienteMqtt.loop();

  // Si han pasado más de 3 segundos desde la última vez, enviar un mensaje
  if (millis() - ultimaVez > TIEMPO_ESPERA)
  {

    // Formatear una cadena dentro de mensaje
    sprintf(mensaje, "Contador: %d", contador);
    // Y publicar un simple contador
    Serial.print("Publicando mensaje...");
    clienteMqtt.publish("jardin/temperatura", mensaje);
    Serial.println("OK");
    contador++;
    ultimaVez = millis();
  }
  delay(10);
}
