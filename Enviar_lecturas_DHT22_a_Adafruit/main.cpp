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
#include "config.h"
#include "DHT.h"

#define TIEMPO_ESPERA 5000 // Cada cuánto leer del sensor

/*
Nota importante: si usas el pin D8 (como lo recomiendo)
  recuerda desconectar el lector del mismo cada vez que reinicies
  o quieras subir el código, pues el mismo "interfiere" con el
  monitor serial
*/

#define PIN_CONEXION D8   // A cuál pin está conectado el lector
#define TIPO_SENSOR DHT22 // Puede ser DHT11 también
DHT sensor(PIN_CONEXION, TIPO_SENSOR);

// Recuerda que en io.feed() debes indicar el nombre del feed que configuraste en la web

AdafruitIO_Feed *temperaturaAdafruit = io.feed("Temperatura");
AdafruitIO_Feed *humedadAdafruit = io.feed("Humedad");


int ultimaVez = millis(); // Para enviar cada X segundos pero sin usar delay
float humedad, temperaturaEnGradosCelsius = 0;

void setup()
{
  // Iniciar sensor DHT
  sensor.begin();
  // Intentar conectar a Adafruit
  io.connect();
  while (io.status() < AIO_CONNECTED)
  {
    delay(500);
  }
}

// Ahora viene el loop, al cual se entra en caso de que en el setup
// todo haya ido bien

void loop()
{

  // Dejar que Adafruit haga sus cosas...
  io.run();

  // Si han pasado más de 5 segundos desde la última vez, enviar a Adafruit!
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
    temperaturaAdafruit->save(temperaturaEnGradosCelsius);
    humedadAdafruit->save(humedad);
    ultimaVez = millis();
  }
  delay(10);
}
