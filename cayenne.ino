
////////////////////////////////Cayenne //////////////////////////
CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
  Cayenne.virtualWrite(0, millis());
  Serial.println("escribiendo en out");
 // float valor=sun.dAzimuth-180;
   Cayenne.virtualWrite(1, hour());
   Cayenne.virtualWrite(2, minute());
   Cayenne.virtualWrite(3, second());
 //  Cayenne.virtualWrite(4, valor);
   //Cayenne.virtualWrite(5, sun.dZenithAngle,TYPE_VOLTAGE,UNIT_VOLTS  );
 
  for(j=0;j<Num_reles;j++){
    Cayenne.virtualWrite(j+6, timers[j].Estado_rele);
    Cayenne.virtualWrite(j+10, timers[j].Estado_rele);
  }
  
  
  Cayenne.virtualWrite(15, conf.Latitud);
  Cayenne.virtualWrite(16, conf.Longitud);
  int valor1= WiFi.localIP()[0];
  Cayenne.virtualWrite(17, valor1);
  valor1= WiFi.localIP()[1];
  Cayenne.virtualWrite(18, valor1);
  valor1= WiFi.localIP()[2];
  Cayenne.virtualWrite(19, valor1);
  valor1= WiFi.localIP()[3];
  Cayenne.virtualWrite(20, valor1);
  //WiFi.localIP()

///////////////////////
/*
  // Some examples of other functions you can use to send data.
  //Cayenne.celsiusWrite(1, 22.0);
  //Cayenne.luxWrite(2, 700);
  //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
  */
}

CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());

  Serial.print("request.channel recibido: ");
  Serial.println(request.channel);
  if(request.channel==21){
    Serial.println("D5 ON");
      timers[0].Mode=1;
    Cayenne.virtualWrite(21, timers[0].Mode);
  }
   
 
}

///////////////FIN Cayenne //////////save configuration ////////////////////
