/*
 * Adaptado por Miguel Alonso Abella en junio de 2020
 * 
 * ESP8266 + DHT22 + BMP180 + BOOTSTRAP + SPIFFS + GOOGLE CHARTS
 * Copyright (C) 2017 http://www.projetsdiy.fr - http://www.diyprojects.io
 * https://randomnerdtutorials.com/esp8266-ds18b20-temperature-sensor-web-server-with-arduino-ide/
 * https://roboindia.com/tutorials/ds18b20-temp-sensor-nodemcu/
 * Modificado por Miguel Alonso Abella Mayo 2020
 * 
 * Acuario sistema 1
 * TSD y DS18B20 con ADS1115
 * 
 * D3 para del DS18B20 (tres pines, +, gnd y data.
 * 
 * ADS1115 para la lectura del voltaje de salida del sensor TDS gravity 0-2.3V / 0-1000ppm
 * https://i0.wp.com/www.esp8266learning.com/wp-content/uploads/2018/09/esp8266-and-ads1115_bb.jpg?w=642
 * 
 * D1 --> SCL
 * D2 --> SDA
 * 
 * D0 --> Calentador
 * D4 --> ventilador
 * Quedan los 4 reles 
 * 
 * D5 D6 D7 y D8
 */


   
int ERROR_CAYENNE=0;

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

#define CONFIG_VERSION "v01"
#define CONFIG_START 0
//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial

#include <CayenneMQTTESP8266.h>


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <FS.h>
#include <ArduinoJson.h>  
#include <DNSServer.h>
#include <WiFiManager.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <math.h>
//#include <SunPos.h> //Sunpos de la plataforma solar de Almería
#include <spa.h>    //spa del NREL

#include "variables.h" 
#include "TDS_DS18B20.h"  //para el sistema 1, TDS DS18B20 sensor de temperatura con control calentador y ventilador

void setup() {
  setupConfig();
  
  server.on("/tabmesures.json", sendTabMesures);
  server.on("/mesures.json", sendMesures);
  server.on("/gpio", updateGpio);
  server.on("/readgpio.json", sendGPIO);
  server.on("/hora.json", sendTime);
  server.on("/sendDataTimers.json", sendDataTimers);
  server.on("/sendDataConf.json", sendDataConf);
  server.on("/save", save);
  server.on("/savemode", savemode);
  server.on("/saveconfig", saveconfig_http);

  server.serveStatic("/js", SPIFFS, "/js");
  server.serveStatic("/css", SPIFFS, "/css");
  server.serveStatic("/img", SPIFFS, "/img");
  server.serveStatic("/html", SPIFFS, "/html");
  server.serveStatic("/", SPIFFS, "/index.html");
  server.begin();
  Serial.println ( "HTTP server started" );

  setup_TDS_DS18B20();

}

void loop() {
  server.handleClient();
  t=temperatureC;     //DS18B20 temperatura;
  h=mV_TDS;           //mV del TSD ADS1115 A0_A1;
  pa=tdsValue;        //TDS value en ppm;
  if (!ERROR_CAYENNE){Cayenne.loop();} //libreria cayenne modificada para detectar error
  TDS_DS18B20();
  CheckNTPtime();
  server_read();
  horasolar();
  for (j=0;j<Num_reles;j++){  controlTimers(j);}
  
}
