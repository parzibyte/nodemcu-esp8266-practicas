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
#include "DHT.h"

/*
Nota importante: si usas el pin D8 (como lo recomiendo)
  recuerda desconectar el lector del mismo cada vez que reinicies
  o quieras subir el código, pues el mismo "interfiere" con el
  monitor serial
*/

#define PIN_CONEXION D8// A cuál pin está conectado el lector
#define TIPO_SENSOR DHT22 // Puede ser DHT11 también

DHT sensor(PIN_CONEXION, TIPO_SENSOR);


float humedad, temperaturaEnGradosCelsius = 0;

int ultimaVezLeido = 0;
long milisegundosDeEsperaParaLeer = 2000; // 2 segundos

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);
  // Muy importante la siguiente línea, pues configura el pin del sensor como INPUT_PULLUP
  sensor.begin();

  while(!Serial) { }// Esperar que haya conexión serial

  Serial.println("Iniciando el dispositivo");
  Serial.println("Practica de IoT");
}

void loop() {
  // Debemos leer cada 2 segundos
  if(ultimaVezLeido > milisegundosDeEsperaParaLeer) {
    humedad = sensor.readHumidity();
    temperaturaEnGradosCelsius = sensor.readTemperature();
    // En ocasiones puede devolver datos erróneos; por eso lo comprobamos
    if (isnan(temperaturaEnGradosCelsius) || isnan(humedad)) {
      Serial.println("Error leyendo valores");
      ultimaVezLeido = 0;
      return;
    }
    // En caso de que todo esté correcto, imprimimos los valores
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(temperaturaEnGradosCelsius);
    Serial.println(" *C");

    ultimaVezLeido = 0;
  }
  // Aquí podemos hacer otras cosas...


  // Esperamos 100 milisegundos y también los aumentamos al contador, de este
  // modo evitamos un delay de 2000 milisegundos y podemos hacer otras cosas
  // por aquí
  delay(100);
  ultimaVezLeido += 100;
}
