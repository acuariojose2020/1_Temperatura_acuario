void setupConfig(){
    for ( int x = 0 ; x < 7 ; x++ ) {
    pinMode(GPIOPIN[x], OUTPUT);
    digitalWrite(GPIOPIN[x],HIGH);
  }
  Serial.begin ( 9600 );
    strcpy(conf.pwd,"121212");
    conf.IP_1            = 129;
    conf.IP_2            = 6;
    conf.IP_3            = 15;
    conf.IP_4            = 28;
    conf. EEPROM_chk     =123;
    conf.Latitud         =40.41;
    conf.Longitud        =-3.731;
    strcpy(conf.DevName,"nombre del dispositivo");
    strcpy(conf.username,username);
    strcpy(conf.password,password);
    strcpy(conf.clientID,clientID);
    
    loadConfig();
   
    Serial.println("Comenzando programa");
    StartWiFi();
    Serial.println("Comenzando programa");
    Serial.println(conf.username);
    Cayenne.begin(conf.username, conf.password, conf.clientID);
    //Cayenne.begin(username, password, clientID);

  Serial.print("DevName :");
  Serial.println(conf.DevName);
  Serial.println("____________________________");
  Serial.print("DevName en conf :");
  Serial.println(conf.DevName);
  Serial.println("____________________________");
 if (ERROR_CAYENNE){
    Serial.println("error en la conexion a cayenne");
  }
  Serial.println ( "" );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
  }


  }

void loadConfig() {
       if(!SPIFFS.begin()){
        Serial.println("Card Mount Failed");
        return;
        }
        File myFile = SPIFFS.open("/conf.txt", "r");
        myFile.read((byte *)&timers, sizeof(timers));
        myFile.read((byte *)&conf, sizeof(conf));
        myFile.close();
        Serial.println("leyendo configuracion");
        Serial.println(conf.DevName);
        //Serial.printf( "read: %s, %.5f\n", myStruct.someString, myStruct.someFloat );
}

void saveConfig() {
  Serial.println("saving config");
   if(!SPIFFS.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    File myFile = SPIFFS.open("/conf.txt", "w");
    myFile.write((byte *)&timers, sizeof(timers));
    myFile.write((byte *)&conf, sizeof(conf));
    myFile.close(); 
    Serial.println("guardando configuracion");
    Serial.println(conf.DevName); 
}
