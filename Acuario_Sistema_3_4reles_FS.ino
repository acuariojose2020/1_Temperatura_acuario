
/**************************************************************************
 * 
 * ESP8266 NodeMCU Acuario_Sistema3 .
 * Lo he adaptado de un control de rele IoT Timer https://www.instructables.com/id/ESP8266-01-IoT-Smart-Timer-for-Home-Automation/
 *
 *incluye envio de datos a Cayenne
  * modificado en CayenneArduinoMQTTClient.h
  * if(millis()-t_inicio>20000){
        ERROR_CAYENNE=1;
        break;
      }
      
      *************************************************************************/

   
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

#include <FS.h>  
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>       
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <EEPROM.h>


#include <math.h>
#include <SunPos.h>

//#include <OneWire.h>
//#include <DallasTemperature.h>


// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "xx";
char password[] = "xx";
char clientID[] = "xx";

int Num_reles=4;
int j;

struct StoreStruct {
  char version[4];
  int         EEPROM_chk;        // EEPROM Check
  int         Mode;              // Mode
  boolean     estado_rele;
  int           TimeZone;
  int          IP_1;
  int          IP_2;
  int          IP_3;
  int          IP_4;
  char         DevName[30];           // Device name
  int          On_Time[7];
  int          Off_Time[7];
  boolean      On_Days[7][8];
  char        username[50] ;
  char        password[50] ;
  char        clientID[50];
} storage[4] ;

struct StoreStruct2 {
  char version[4];
  int           EEPROM_chk;        // EEPROM Check
  char         DevName[30];           // Device name
  float         Latitud;
  float         Longitud;
  int           TimeZone;
  int          IP_1;
  int          IP_2;
  int          IP_3;
  int          IP_4;
  char        username[50] ;
  char        password[50] ;
  char        clientID[50];
} storage2 ;



unsigned long lastMillis = 0;
int led=LOW;

// definimos el lugar, sun y time de acuerdo con sunpos.h
 cLocation lugar;
 cSunCoordinates sun;
 cTime fecha_hora_PSA;
 

// Defines
#define      DefaultName       "Acuario_Sistema3"  // Default device name
#define      NTPfastReq        10                 // NTP time request in seconds when  time not set
#define      NTPslowReq        3600               // NTP time request in seconds after time is  set
#define      Version           "1.00"             // Firmware version

// NTP Server details
//-------------------
IPAddress timeServer(129, 6, 15, 28);              // time.nist.gov NTP server
WiFiUDP Udp;
unsigned int localPort       = 3232;               // Local port to listen for UDP packets
WiFiServer server(80);

//define your default values here, if there are different values in config.json, they are overwritten.
//length should be max size + 1 
//-----------------------------
//char static_ip[16] = "192.168.4.1";
//char static_gw[16] = "192.168.4.1";
//char static_sn[16] = "255.255.255.0";

//flag for saving configuration data
//----------------------------------
bool shouldSaveConfig = false;

// I/O config
//-----------
#define      Relay_D5        D4   // (D5) 
#define      Relay_D6        D6   // (D6) 
#define      Relay_D7        D7   // (D7) 
#define      Relay_D8        D8   // (D8) 

#define      Button        D0   // (D8) 

// Variables
//----------
#define      ErrMsg            "<font color=\"red\"> < < Error !</font>"
boolean      ResetWiFi       = false;
long         timeNow         = 0;
long         timeOld         = 300000;
long         timeOld2         = 0;
boolean      TimeOk          = false;
boolean      NTPtimeOk       = false;
String       request         = "";
byte         Page            = 1;
boolean      TimeCheck       = false;
int          NewHH           = 0;
int          NewMM           = 0;
int          Newdd           = 0;
int          Newmm           = 0;
int          Newyy           = 0;
int          PgmNr           = 0;
int          LastHH          = 0;
int          LastMM          = 0;
int          Lastdd          = 0;
int          Lastmm          = 0;
int          Lastyy          = 0;
byte         old_sec         = 0;
long         old_ms          = 0;
boolean      WebButton       = false;
boolean      PgmPrev         = false;
boolean      PgmNext         = false;
boolean      PgmSave         = false;
boolean      Error1          = false;
boolean      Error2          = false;
boolean      Error3          = false;
boolean      Error3b          = false;
boolean      Error4          = false;
boolean      Error5          = false;
boolean      Error6          = false;
boolean      Error7          = false;
boolean      Error8          = false;
boolean      D[8]            = {false, false, false, false, false, false, false, false};
int          PgmNrG           = 0;
int          OnHour          = 0;
int          OnMinute        = 0;
int          OffHour         = 0;
int          OffMinute       = 0;


//###############################################################################################
// Setup
//
//###############################################################################################
 
void setup(void)
{
storage[0].IP_1            = 129;
storage[0].IP_2            = 6;
storage[0].IP_3            = 15;
storage[0].IP_4            = 28;
storage[0]. EEPROM_chk     =123;
storage2.Latitud=40.41;
storage2.Longitud=-3.731;

storage2.Latitud=40.41;
storage2.Longitud=-3.731;

  
strcpy(storage2.DevName,"nombre del dispositivo");

strcpy(storage[0].username,username);
strcpy(storage[0].password,password);
strcpy(storage[0].clientID,clientID);

   // For debug only - REMOVE
  //----------------------------------------------------------------------
  Serial.begin(9600);
  
  pinMode(Relay_D5,OUTPUT);
  pinMode(Relay_D7 ,OUTPUT);
  pinMode(Relay_D6,OUTPUT);
  pinMode(Relay_D8,OUTPUT);

  pinMode(Button,INPUT_PULLUP);
  
  
  digitalWrite(Relay_D5,HIGH);
  digitalWrite(Relay_D6,HIGH);
  digitalWrite(Relay_D7 ,HIGH); 
  digitalWrite(Relay_D8,HIGH);
 
  Serial.println("Comenzando programa");
  StartWiFi();
  Serial.println("Comenzando programa");
  Serial.println(storage[0].username);
  Cayenne.begin(storage[0].username, storage[0].password, storage[0].clientID);


 //Cayenne.begin(username, password, clientID);

  Serial.print("DevName :");
  Serial.println(storage[0].DevName);
  Serial.println("____________________________");

Serial.print("DevName en storage2 :");
  Serial.println(storage2.DevName);
  Serial.println("____________________________");


  if (ERROR_CAYENNE){
    Serial.println("error en la conexion a cayenne");
  }
}
 


//###############################################################################################
// Main program loop
//
//###############################################################################################
void loop() {
     if (!ERROR_CAYENNE){
        Cayenne.loop();
     }
 
 lugar.dLatitude= storage2.Latitud; //Norte (positiva)
 lugar.dLongitude= storage2.Longitud;// Este (positiva)//
  
   // fecha de sunpos PSA
  fecha_hora_PSA.iYear=year();
  fecha_hora_PSA.iMonth=month();
  fecha_hora_PSA.iDay=day();
  fecha_hora_PSA.dHours=hour();
  fecha_hora_PSA.dMinutes=minute();
  fecha_hora_PSA.dSeconds=second();
  sunpos(fecha_hora_PSA, lugar, &sun);


   
  // Scan for NTP time changes
  //----------------------------------------------------------------------
  CheckNTPtime();
  // See if time has changed
  //----------------------------------------------------------------------
  for(j=0;j<Num_reles;j++){DoTimeCheck(j);}
 
  // Handle Web page
  //----------------------------------------------------------------------
   WiFiClient client = server.available();
  // Read requests from web page
  //----------------------------------------------------------------------
  request = client.readStringUntil('\r');
  client.flush();
  // See if data was received
  //----------------------------------------------------------------------
  if (request.indexOf("/") != -1)  {
    //Serial.println(request);
    if (request.indexOf("Link=1")     != -1) Page = 1;
    if (request.indexOf("Link=2")     != -1) Page = 2;
    if (request.indexOf("Link=3")     != -1) Page = 3;
    if (request.indexOf("Link=4")     != -1) Page = 4;
    if (request.indexOf("Link=5")     != -1) Page = 5;
    if (request.indexOf("Link=6")     != -1) Page = 6;
    if (request.indexOf("Link=30")    != -1) Page = 30;
    if (request.indexOf("Link=31")    != -1) Page = 31;
    if (request.indexOf("Link=32")    != -1) Page = 32;
    if (request.indexOf("Link=33")    != -1) Page = 33;
    
    if (request.indexOf("Link=20")     != -1) Page = 20;
    if (request.indexOf("Link=21")     != -1) Page = 21;
    if (request.indexOf("Link=22")     != -1) Page = 22;
    if (request.indexOf("Link=23")     != -1) Page = 23;
 
    if (request.indexOf("GET / HTTP") != -1) Page = 1;
    Error1 = false;
    Error2 = false;
    Error3 = false;
    Error4 = false;
    Error5 = false;
    Error6 = false;
    Error7 = false;


    // Respond to Buttons
    //==================================

    // PAGE 1 - STATUS
    //----------------
    // See if Save Button was presed
    //----------------------------------------------------------------------
    if (request.indexOf("TodosONBtn=") != -1) {
      Page = 1;
      WebButton = true;
     
    }
    // See if Refresh Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("TodosOFFBtn=") != -1) {
      Page = 1;
    }

    // PAGE 2 - PROGRAMS ///////////////////////////////////////////////////////////////////////////////////////////
    //------------------
    // See if Previous Buttomn was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtnPrev=") != -1) {
      //Page = 2;
      PgmPrev = true;
      PgmNext = false;
      PgmSave = true;
    }
    // See if Next Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtnNext=") != -1) {
      //Page = 2;
      PgmPrev = false;
      PgmNext = true;
      PgmSave = true;
    }
    // See if Save Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtn2=") != -1) {
      //Page = 2;
      PgmSave = true;
      PgmPrev = false;
      PgmNext = false;
    }
    // See if Clear Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("ClearBtn1=") != -1) {
      //Page = 2;
      storage[Page-30].On_Time[PgmNr]  = 0;
      storage[Page-30].Off_Time[PgmNr] = 0;
      for (byte i = 0; i < 7; i++ ) {
        storage[Page-30].On_Days[PgmNr][i] = false;
      }
      PgmPrev = false;
      PgmNext = false;
      // Save program data
      saveConfig();
    }
    // Get program data if any button was pressed
    //----------------------------------------------------------------------
    if (PgmSave == true) {
      PgmSave = false;
      // On Hour
      if (request.indexOf("OnH=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("OnH=");
        Tmp.remove(0,t1+4);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        OnHour = Tmp.toInt();
        if ( (OnHour < 0) or (OnHour > 23) ) Error1 = true;
      }
      // On Minute
      if (request.indexOf("OnM=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("OnM=");
        Tmp.remove(0,t1+4);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        OnMinute = Tmp.toInt();
        if ( (OnMinute < 0) or (OnMinute > 59) ) Error1 = true;
      }
      // Off Hour
      if (request.indexOf("OffH=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("OffH=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        OffHour = Tmp.toInt();
        if ( (OffHour < 0) or (OffHour > 23) ) Error2 = true;
      }
      // Off Minute
      if (request.indexOf("OffM=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("OffM=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        if (t1 == -1) {
          t1 = Tmp.indexOf(" ");
        }
        Tmp.remove(t1);
        OffMinute = Tmp.toInt();
        if ( (OffMinute < 0) or (OffMinute > 59) ) Error2 = true;
      }
      // Reset day flags
      D[0] = false;
      D[1] = false;
      D[2] = false;
      D[3] = false;
      D[4] = false;
      D[5] = false;
      D[6] = false;
      D[7] = false;
      // Day 1
      if (request.indexOf("D1=on") != -1) D[0] = true;
      // Day 2
      if (request.indexOf("D2=on") != -1) D[1] = true;
      // Day 3
      if (request.indexOf("D3=on") != -1) D[2] = true;
      // Day 4
      if (request.indexOf("D4=on") != -1) D[3] = true;
      // Day 5
      if (request.indexOf("D5=on") != -1) D[4] = true;
      // Day 6
      if (request.indexOf("D6=on") != -1) D[5] = true;
      // Day 7
      if (request.indexOf("D7=on") != -1) D[6] = true;
      // Update program if no errors
      if ( (Error1 == false) and (Error2 == false) ) {
        storage[Page-30].On_Time[PgmNr]  = (OnHour  * 100) + OnMinute;
        storage[Page-30].Off_Time[PgmNr] = (OffHour * 100) + OffMinute;        
        for (byte i = 0; i < 7; i++) {
          if (D[i] == true) storage[Page-30].On_Days[PgmNr][i] = true; else storage[Page-30].On_Days[PgmNr][i] = false;
        }  
        // Save program data
        
        saveConfig();
        timeOld = 0;
      }
      else {
        PgmPrev = false;
        PgmNext = false;
      }
    }
    // Change to Prev/Next Program
    if (PgmPrev == true) {
      PgmPrev = false;
      PgmNr = PgmNr - 1;
      if (PgmNr <0) PgmNr = 6;
    }
    if (PgmNext == true) {
      PgmNext = false;
      PgmNr = PgmNr + 1;
      if (PgmNr > 6) PgmNr = 0;        
    }

    // PAGE 3 - CONFIG
    //----------------
    // See if Save Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtn3=") != -1) {
      //Page = 3;
      // Device Name
      if (request.indexOf("Dev=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("Dev=");
        Tmp.remove(0,t1+4);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        Tmp.replace("+"," ");
        //storage[0].DevName=Tmp;
        Tmp.toCharArray(storage[Page-20].DevName,30);
      }
      // Mode select
      if (request.indexOf("mode=Auto") != -1)storage[Page-20].Mode = 2;
      if (request.indexOf("mode=On") != -1)storage[Page-20].Mode = 1;
      if (request.indexOf("mode=Off") != -1)storage[Page-20].Mode = 0;
      

      if ((Error3 == false) and (Error3b== false) ){
        saveConfig();
        timeOld = 0;
      }
      
    }

    // PAGE 4 - NTP Setup
    //-------------------
    // See if Save Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtn4=") != -1) {
      Page = 4;

      //DevGlobal Name
      if (request.indexOf("DevGlobal=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("DevGlobal=");
        Tmp.remove(0,t1+10);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        Tmp.replace("+"," ");
        //storage[0].DevName=Tmp;
        Tmp.toCharArray(storage2.DevName,30);
      }
      //Latitud y longitud
      if (request.indexOf("Latitud=") != -1) {
        String Tmp = request;
        int t1 = Tmp.indexOf("Latitud=");
        Tmp.remove(0,t1+8);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        storage2.Latitud = Tmp.toFloat();
        }
      if (request.indexOf("Longitud=") != -1) {
        String Tmp = request;
        int t1 = Tmp.indexOf("Longitud=");
        Tmp.remove(0,t1+9);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        storage2.Longitud = Tmp.toFloat();
        }
      // Time Zone
      if (request.indexOf("TZH=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("TZH=");
        Tmp.remove(0,t1+4);
        t1 = Tmp.indexOf("&");
        if (t1 == -1) {
          t1 = Tmp.indexOf(" ");
        }
        Tmp.remove(t1);
        storage[0].TimeZone = Tmp.toFloat();
        if ( (storage[0].TimeZone < -12) or (storage[0].TimeZone > 12) ) {
          Error4 = true;
          storage[0].TimeZone = 0;
        }
      }
      // Get NTP IP address
      //--------------------------------
      if (request.indexOf("IP_1=") != -1) {
        String Tmp = request;
        int t1 = Tmp.indexOf("IP_1=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        storage[0].IP_1 = Tmp.toInt();
        if ( (storage[0].IP_1 < 0) or (storage[0].IP_1 > 255) ) Error5 = true;
      }
      if (request.indexOf("IP_2=") != -1) {
        String Tmp = request;
        int t1 = Tmp.indexOf("IP_2=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        storage[0].IP_2 = Tmp.toInt();
        if ( (storage[0].IP_2 < 0) or (storage[0].IP_2 > 255) ) Error5 = true;
      }
      if (request.indexOf("IP_3=") != -1) {
        String Tmp = request;
        int t1 = Tmp.indexOf("IP_3=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        storage[0].IP_3 = Tmp.toInt();
        if ( (storage[0].IP_3 < 0) or (storage[0].IP_3 > 255) ) Error5 = true;
      }
      if (request.indexOf("IP_4=") != -1) {
        String Tmp = request;
        int t1 = Tmp.indexOf("IP_4=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        storage[0].IP_4 = Tmp.toInt();
        if ( (storage[0].IP_4 < 0) or (storage[0].IP_4 > 255) ) Error5 = true;
      }
      if ( (Error4 == false) and (Error5 == false) ) {
        // Set new NTP IP
        timeServer[0] = storage[0].IP_1;
        timeServer[1] = storage[0].IP_2;
        timeServer[2] = storage[0].IP_3;
        timeServer[3] = storage[0].IP_4;
            
        //Save Time Server Settings
        saveConfig();
        NTPtimeOk = false;
        setSyncInterval(NTPfastReq);
      }
    }
    // See if Save Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtn5=") != -1) {
      Page = 4;
      //Get new hour
      String Tmp = request;
      int t1 = Tmp.indexOf("TimeHour=");
      Tmp.remove(0,t1+9);
      t1 = Tmp.indexOf("&");
      Tmp.remove(t1);
      NewHH = Tmp.toInt();
      if ( (NewHH < 0) or (NewHH > 23) ) Error6 = true;
      //Get new minute
      Tmp = request;
      t1 = Tmp.indexOf("TimeMinute=");
      Tmp.remove(0,t1+11);
      t1 = Tmp.indexOf("&");
      Tmp.remove(t1);
      NewMM = Tmp.toInt();
      if ( (NewMM < 0) or (NewMM > 59) ) Error6 = true;
      //Get new date
      Tmp = request;
      t1 = Tmp.indexOf("TimeDate=");
      Tmp.remove(0,t1+9);
      t1 = Tmp.indexOf("&");
      Tmp.remove(t1);
      Newdd = Tmp.toInt();
      if ( (Newdd < 1) or (Newdd > 31) ) Error7 = true;
      //Get new month
      Tmp = request;
      t1 = Tmp.indexOf("TimeMonth=");
      Tmp.remove(0,t1+10);
      t1 = Tmp.indexOf("&");
      Tmp.remove(t1);
      Newmm = Tmp.toInt();
      if ( (Newmm < 1) or (Newmm > 12) ) Error7 = true;
      //Get new year
      Tmp = request;
      t1 = Tmp.indexOf("TimeYear=");
      Tmp.remove(0,t1+9);
      t1 = Tmp.indexOf("&");
      if (t1 == -1) {
        t1 = Tmp.indexOf(" ");
      }
      Tmp.remove(t1);
      Newyy = Tmp.toInt();
      if ( (Newyy < 2000) or (Newyy > 2069) ) Error7 = true;
      // Update time
      //------------
      setTime(NewHH, NewMM, 0, Newdd, Newmm, Newyy);
      LastHH = NewHH;
      LastMM = NewMM;
      Lastdd = Newdd;
      Lastmm = Newmm;
      Lastyy = Newyy;
      TimeOk = true;
      
      timeOld = 0;
      setSyncInterval(NTPslowReq);
    }
    // See if Refresh Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("RefreshBtn5=") != -1) {
      Page = 4;
    }

    // Check time before updating web page
    //----------------------------------------------------------------------
   for (j=0;j<Num_reles;j++){  DoTimeCheck(j);}
   
//////////////////////////////////////////////////////////////////////////////////////////////////////////



   

//////////////////////////////////////////////////// Cayenne

 // PAGE 6 - CAYENNE
// char username[] = "xx";
//char password[] = "xx";
//char clientID[] = "xx";
   //----------------
    // See if Save Button was pressed
    //----------------------------------------------------------------------
    if (request.indexOf("SaveBtn6=") != -1) {
      Page = 6;
      // Device Name
      if (request.indexOf("user=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("user=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        Tmp.replace("+"," ");
         Tmp.toCharArray(storage[0].username,50);
      }
      if (request.indexOf("pass=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("pass=");
        Tmp.remove(0,t1+5);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        Tmp.replace("+"," ");
        Tmp.toCharArray(storage[0].password,50);
      }
      if (request.indexOf("cli=") != -1)  {
        String Tmp = request;
        int t1 = Tmp.indexOf("cli=");
        Tmp.remove(0,t1+4);
        t1 = Tmp.indexOf("&");
        Tmp.remove(t1);
        Tmp.replace("+"," ");
        Tmp.toCharArray(storage[0].clientID,50);
      }

     
        saveConfig();
    }
    
    }


//////////////////////////////////////////////////////////////////////


    // Web Page HTML Code
    //==================================
    client.println("<!doctype html>");
    client.println("<html lang='en'>");
    client.println("<head>");
    // Refresh home page every 60 sec
    //WiFi.localIP()
    //client.println("<META HTTP-EQUIV=""refresh"" CONTENT=""30;url=http://example.com"">");
    client.print("<META HTTP-EQUIV=""refresh"" CONTENT=""30;url=http://");
    client.print(WiFi.localIP());
    client.print("/Link=");
    client.print(Page);
    client.println(">");
    
    
    
    client.print("<style> body {background-color: #C3FCF7;Color: #2B276E;}</style>");
    client.println("<title>");
    client.println(storage[0].DevName);
    client.println("</title>");
    client.println("</head>");
    client.print("<body>");
    client.print("<font size = \"5\"><b>");
    client.println(storage[0].DevName);
    client.print("</font></b><br>"); 
    
   
    // Show time
    //----------------------------------------------------------------------
    client.print("<p style=\"color:#180BF4;\";>");  
    client.print("<font size = \"5\"><b>");
    if (hour() < 10) client.print(" ");
    client.print(hour());
    client.print(":");
    if (minute() < 10) client.print("0");
    client.print(minute());
    client.print(":");
    if (second() < 10) client.print("0");
    client.print(second());
    client.print("</font></p>"); 
    // Day of the week
    //----------------------------------------------------------------------
    switch (weekday()) {
      case 1: client.print("Sunday, ");
              break;
      case 2: client.print("Monday, ");
              break;
      case 3: client.print("Tuesday, ");
              break;  
      case 4: client.print("Wednesday, ");
              break;  
      case 5: client.print("Thursday, ");
              break;  
      case 6: client.print("Friday, ");
              break;  
      case 7: client.print("Saturday, ");
              break;  
      default: client.print("");
              break;        
    }
    // Date
    //----------------------------------------------------------------------
    client.print(day());
    // Month
    //----------------------------------------------------------------------
    switch (month()) {
      case  1: client.print(" January ");
               break;
      case  2: client.print(" February ");
               break;
      case  3: client.print(" March ");
               break;
      case  4: client.print(" April ");
               break;
      case  5: client.print(" May ");
               break;
      case  6: client.print(" June ");
               break;
      case  7: client.print(" July ");
               break;
      case  8: client.print(" August ");
               break;
      case  9: client.print(" September ");
               break;
      case 10: client.print(" October ");
               break;
      case 11: client.print(" November ");
               break;
      case 12: client.print(" December ");
               break;
      default: client.print(" ");
               break;        
    }
    // Year
    //----------------------------------------------------------------------
    client.print(year());
    client.print("<br>");
    client.print(F("AcimutSol: ")); float valor=sun.dAzimuth-180;  client.print(valor);
    client.print(F("  CenitSol: ")); valor=sun.dZenithAngle;  client.print(valor);
        
    client.print("<br>");
   
      
      
       
      
    client.println("<br><br>");
    //Menu
    //----------------------------------------------------------------------
    client.print("<form action= method=\"get\"><b>");
    client.print("<a href=\"Link=1\">Home</a>&emsp;");
    client.print("<a href=\"Link=4\">Time</a><br>");
    client.print("<a href=\"Link=6\">Cayenne Iot</a><br>"); 
     
    client.print("<a href=\"Link=30\">Programs_1(D5)</a>&emsp;");
    client.print("<a href=\"Link=31\">Programs_2 (D6)</a>&emsp;"); 
    client.print("<a href=\"Link=32\">Programs_3 (D7)</a>&emsp;"); 
    client.print("<a href=\"Link=33\">Programs_4 (D8)</a><br>"); 
    
    client.print("<a href=\"Link=20\">Config_1(D5)</a>&emsp;");
    client.print("<a href=\"Link=21\">Config_2(D6)</a>&emsp;");
    client.print("<a href=\"Link=22\">Config_3(D7)</a>&emsp;");
    client.print("<a href=\"Link=23\">Config_4(D8)</a><br>");

        
      
    // Draw line
    //----------------------------------------------------------------------
     client.print("</b><hr />");  
     
    // Status PAGE
    //============
    if (Page == 1) {
       // Draw line
      client.print("<hr />");  
      client.print("Control Puerta: ");
    
      
       // Draw line
      client.print("<hr />");   
      for(j=0;j<Num_reles;j++){
          client.print("<font size = \"4\"><b>Estado Rele");
          client.print(storage[j].DevName);
          client.print("(D4)</font></b><br>"); 
          client.print("<b>Modo Rele D4 : </b>");
          switch (storage[j].Mode) {
            case 0 : client.print("Off");
                     break;
            case 1 : client.print("On");
                     break;
            case 2 : client.print("Auto");
                     break;
          }
      }
      client.print("<br>");
      //Button 1
      client.println("<br>");
      client.println("<input type=\"submit\" name =\"TodosONBtn\" value=\"Manual Override\">");
      //Button 2
      client.println("&emsp;");
      client.println("<input type=\"submit\" name =\"TodosOFFBtn\" value=\"Refresh\">");
      client.println("<br>");
      // Draw line
      client.print("<hr />");        
      // Show last time synch
      client.print("<font size = \"2\">"); 
      client.print("</b>Version ");
      client.print(Version);
      client.print("<br>");
      client.print("Time was last updated on ");
      client.print(Lastdd);
      client.print("/");
      if (Lastmm < 10) client.print("0");
      client.print(Lastmm);
      client.print("/");
      client.print(Lastyy);
      client.print(" at ");
      if (LastHH < 10) client.print("0");
      client.print(LastHH);
      client.print(":");
      if (LastMM < 10) client.print("0");
      client.print(LastMM);
      client.print("</font>");
    }
  
    //Program  PAGE
    //============
    if (Page == 30 ||Page == 31 ||Page == 32 ||Page == 33 ) {
      int k=Page-30;
      
      // Program number
      client.print("<font size = \"4\"><b>"); 
      client.print("Program Rele D");
      client.print(k+5);
       client.print(" - ");
      client.print(PgmNr + 1);
      client.print(" of 7");
      //Previous Button
      client.println("&emsp;<input type=\"submit\" name =\"SaveBtnPrev\" value=\" << \">");
      client.println("&emsp;");
      //Next Button
      client.println("<input type=\"submit\" name =\"SaveBtnNext\" value=\" >> \"></font></b><br><br>");
      //On time
      client.print("On  Time: <input type=\"text\"<input maxlength=\"2\" size=\"2\" name=\"OnH\"value =\"");
//      if ( (storage[k].On_Time[PgmNr] / 100) < 10) client.print("0");
      client.print(storage[k].On_Time[PgmNr]/100);    
      client.print("\"> : <input type=\"text\"<input maxlength=\"2\" size=\"2\" name=\"OnM\"value =\"");
      if ( (storage[k].On_Time[PgmNr] % 100) < 10) client.print("0");
      client.print(storage[k].On_Time[PgmNr]%100);    
      client.print("\">");
      if (Error1 == true) client.print(ErrMsg);
      client.print("<br><br>");
      //Off time
      client.print("Off Time: <input type=\"text\"<input maxlength=\"2\" size=\"2\" name=\"OffH\"value =\"");
//      if ( (storage[k].Off_Time[PgmNr] / 100) < 10) client.print("0");
      client.print(storage[k].Off_Time[PgmNr]/100);    
      client.print("\"> : <input type=\"text\"<input maxlength=\"2\" size=\"2\" name=\"OffM\"value =\"");
      if ( (storage[k].Off_Time[PgmNr] % 100) < 10) client.print("0");
      client.print(storage[k].Off_Time[PgmNr]%100);    
      client.print("\">");
      if (Error2 == true) client.print(ErrMsg);
      client.print("<br><br>");
      //Day 1
      client.print("<input type=\"Checkbox\" name=\"D1\"");
      if (storage[k].On_Days[PgmNr][0]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Sun<br>");
      //Day 2
      client.print("<input type=\"Checkbox\" name=\"D2\"");
      if (storage[k].On_Days[PgmNr][1]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Mon<br>");
      //Day 3
      client.print("<input type=\"Checkbox\" name=\"D3\"");
      if (storage[k].On_Days[PgmNr][2]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Tue<br>");
      //Day 4
      client.print("<input type=\"Checkbox\" name=\"D4\"");
      if (storage[k].On_Days[PgmNr][3]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Wed<br>");
      //Day 5
      client.print("<input type=\"Checkbox\" name=\"D5\"");
      if (storage[k].On_Days[PgmNr][4]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Thu<br>");
      //Day 6
      client.print("<input type=\"Checkbox\" name=\"D6\"");
      if (storage[k].On_Days[PgmNr][5]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Fri<br>");
      //Day 7
      client.print("<input type=\"Checkbox\" name=\"D7\"");
      if (storage[k].On_Days[PgmNr][6]==true) client.print("checked"); else client.print("unchecked");
      client.print("> Sat<br>");
      //Button
      client.println("<br>");
      client.println("<input type=\"submit\" name =\"SaveBtn2\" value=\"Save\">");
      client.print("&emsp;");
      client.println("<input type=\"submit\" name =\"ClearBtn1\" value=\"Clear\">");
    }
  
    // Config PAGE
    //============
    if ((Page == 20) || (Page == 21) || (Page == 22) || (Page == 23))  {
      int k=Page-20;
      client.print("<font size = \"4\"><b>Configuracion D ");
      client.print(k+5);
      client.print(" </font></b><br><br>"); 
       
      
      // Device Name
      client.print("<b>Device Name: D");
      client.print(k+5);
      client.print(" </b>");
      
      client.print("<input type=\"text\"<input maxlength=\"30\" size=\"35\" name=\"Dev\" value =\"");
      client.print(storage[k].DevName);
      client.println("\"><br><br>");
      //Mode Select
      client.println("<b>Mode: D");
      client.print(k+5);
      client.print(" </b>");
      
      client.print("<input type=\"radio\" name=\"mode\" value=\"Auto\"");
      if (storage[k].Mode == 2) client.print("checked");
      client.print("/> Auto ");
      client.print("<input type=\"radio\" name=\"mode\" value=\"On\"");
      if (storage[k].Mode == 1) client.print("checked");
      client.print("/> On ");
      client.print("<input type=\"radio\" name=\"mode\" value=\"Off\"");
      if (storage[k].Mode == 0) client.print("checked");
      client.print("/> Off <br><br>");

      //Button
      client.println("<br><br>");
      client.println("<input type=\"submit\" name =\"SaveBtn3\" value=\"Save\">");
    }
  
    // Time Server PAGE
    //=================
    if (Page == 4) {
      //Latitud y Longitud
      client.print("Latitud ");
      client.print("<input type=\"text\"<input maxlength=\"5\" size=\"5\" name=\"Latitud\" value =\"");
      client.print(storage2.Latitud,5);
      client.println("\">");
      client.print("Longitud ");
      client.print("<input type=\"text\"<input maxlength=\"5\" size=\"5\" name=\"Longitud\" value =\"");
      client.print(storage2.Longitud,5);
      client.println("\">");
      client.print("<br><br>");
      //
      // Device Golbal Name
      client.print("<b>Device Name: </b>");
     client.print("<input type=\"text\"<input maxlength=\"30\" size=\"35\" name=\"DevGlobal\" value =\"");
      client.print(storage2.DevName);
      client.println("\"><br><br>");
      //
      client.print("<font size = \"4\"><b><u>Time Setup</u></font></b><br><br>"); 
      //Time Zone
      client.print("<font size = \"3\"><b>NTP Network Setup</font></b><br>"); 
      client.print("Time Zone ");
      client.print("<input type=\"text\"<input maxlength=\"6\" size=\"7\" name=\"TZH\" value =\"");
      client.print(storage[0].TimeZone,2);
      client.println("\">");
//      if (Error4 == true) client.print(ErrMsg); else client.print(" (hours)");
      client.print("<br><br>");
      //IP Addtess if time server
      client.print("Time Server IP : <i>(default 129.6.15.28)</i><br>");
      client.print(" <input type=\"text\"<input maxlength=\"3\" size=\"3\" name=\"IP_1\"value =\"");
      client.print(storage[0].IP_1);    
      client.print("\">");
      client.print(" <input type=\"text\"<input maxlength=\"3\" size=\"3\" name=\"IP_2\"value =\"");
      client.print(storage[0].IP_2);    
      client.print("\">");
      client.print(" <input type=\"text\"<input maxlength=\"3\" size=\"3\" name=\"IP_3\"value =\"");
      client.print(storage[0].IP_3);    
      client.print("\">");
      client.print(" <input type=\"text\"<input maxlength=\"3\" size=\"3\" name=\"IP_4\"value =\"");
      client.print(storage[0].IP_4);    
      client.print("\">");
//      if (Error5 == true) client.print(ErrMsg);
      //Button 1
      client.println("<br><br>");
      client.println("<input type=\"submit\" name =\"SaveBtn4\" value=\"Save\"><br>");
      // Draw line
      client.print("<hr />");        

      // Set Time Inputs
      client.print("<font size = \"3\"><b>Local Time Adjust</font></b><br>"); 
      client.print("<br>Time: <input type=\"text\"<input maxlength=\"2\" size=\"2\" name=\"TimeHour\"value =\"");
      client.print(hour());
      client.print("\">");
      client.print(" : <input type=\"text\"<input maxlength=\"3\" size=\"3\" name=\"TimeMinute\"value =\"");
      if (minute() < 10) client.print("0");
      client.print(minute());
      client.print("\">");
//      if (Error6 == true) client.print(ErrMsg); else client.print(" (hh:mm)");
      // Set Date Inputs
      client.print("<br><br>");
      client.print("Date: <input type=\"text\"<input maxlength=\"2\" size=\"2\" name=\"TimeDate\"value =\"");
      client.print(day());
      client.print("\">");
      client.print(" / <input type=\"text\"<input maxlength=\"3\" size=\"3\" name=\"TimeMonth\"value =\"");
      if (month() < 10) client.print("0");
      client.print(month());
      client.print("\">");
      client.print(" / <input type=\"text\"<input maxlength=\"4\" size=\"4\" name=\"TimeYear\"value =\"");
      client.print(year());
      client.print("\">");
//      if (Error7 == true) client.print(ErrMsg); else client.print(" (dd/mm/yyyy)");
      //Button 2
      client.println("<br><br>");
      client.println("<input type=\"submit\" name =\"SaveBtn5\" value=\"Update Time\">");
      //Button 3
      client.println("&emsp;");
      client.println("<input type=\"submit\" name =\"RefreshBtn\" value=\"Refresh\">");
      
      // Draw line
      client.print("<hr />");        
      // Show last time synch
      client.print("<font size = \"2\">"); 
      client.print("</b>Time last updated ");
      client.print(Lastdd);
      client.print("/");
      if (Lastmm < 10) client.print("0");
      client.print(Lastmm);
      client.print("/");
      client.print(Lastyy);
      client.print(" at ");
      if (LastHH < 10) client.print("0");
      client.print(LastHH);
      client.print(":");
      if (LastMM < 10) client.print("0");
      client.print(LastMM);
      client.print("</font>");
    }

        
   

  // Cayenne PAGE
    //============
    if (Page == 6) {
      client.print("<font size = \"4\"><b>Data to Cayenne https://cayenne.mydevices.com/</font></b><br><br>"); 
      // Device Name
      client.print("<b>username: </b>");
      client.print("<input type=\"text\"<input maxlength=\"55\" size=\"55\" name=\"user\" value =\"");
      client.print(storage[0].username);
      client.println("\"><br><br>");
      client.print("<b>password: </b>");
      client.print("<input type=\"text\"<input maxlength=\"55\" size=\"55\" name=\"pass\" value =\"");
      client.print(storage[0].password);
      client.println("\"><br><br>");
      client.print("<b>clientID: </b>");
      client.print("<input type=\"text\"<input maxlength=\"55\" size=\"55\" name=\"cli\" value =\"");
      client.print(storage[0].clientID);
      client.println("\"><br><br>");

      //Button
      client.println("<br>");
      client.println("<input type=\"submit\" name =\"SaveBtn6\" value=\"SaveG\">");
      client.print("&emsp;");
      client.println("<input type=\"submit\" name =\"ClearBtn6\" value=\"ClearG\">");
    }
    client.println("</body>");
    client.println("</html>");
    // End of Web Page
  
}



//###############################################################################################
// NTP Code - do not change
//
//###############################################################################################
const int NTP_PACKET_SIZE = 48;                 // NTP time is in the first 48 bytes of message
byte      packetBuffer[NTP_PACKET_SIZE];        //buffer to hold incoming & outgoing packets

time_t getNtpTime() {
  while (Udp.parsePacket() > 0) ;               // discard any previously received packets
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 5000) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      if (TimeOk ==false) {
        TimeOk = true;
      }
      TimeCheck   = true;
      return secsSince1900 - 2208988800UL + storage[0].TimeZone * SECS_PER_HOUR;
    }
  }
  return 0; // return 0 if unable to get the time
}


//###############################################################################################
// send an NTP request to the time server at the given address
//
//###############################################################################################
void sendNTPpacket(IPAddress & address) {
  // set all bytes in the buffer to 0
  //------------------------------------------------
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //------------------------------------------------
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  //------------------------------------------------
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  //NTP requests are to port 123
  //------------------------------------------------
  Udp.beginPacket(address, 123); 
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


//###############################################################################################
// callback notifying us of the need to save config
// 
//###############################################################################################
void saveConfigCallback () {
  shouldSaveConfig = true;
}


//###############################################################################################
// Start WiFi
// 
//###############################################################################################
void StartWiFi() {
  //read configuration from FS json
  //----------------------------------------------------------------------
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        //DynamicJsonBuffer jsonBuffer;
        DynamicJsonDocument json(1024);
       // JsonObject& json = jsonBuffer.parseObject(buf.get());
       serializeJson(json, Serial);

       // json.printTo(Serial);
        //if (json.success()) {
          //strcpy(mqtt_server, json["mqtt_server"]);
          //strcpy(mqtt_port, json["mqtt_port"]);
          //strcpy(blynk_token, json["blynk_token"]);
//          if(json["ip"]) {
//            strcpy(static_ip, json["ip"]);
//            strcpy(static_gw, json["gateway"]);
//            strcpy(static_sn, json["subnet"]);
//          }
         
      }
    }
  } 
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  //----------------------------------------------------------------------
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  //----------------------------------------------------------------------
  WiFiManager wifiManager;
  //set config save notify callback
  //----------------------------------------------------------------------
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //set static ip
  //----------------------------------------------------------------------
//  IPAddress _ip,_gw,_sn;
//  _ip.fromString(static_ip);
//  _gw.fromString(static_gw);
//  _sn.fromString(static_sn);
//  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  //reset settings
  //----------------------------------------------------------------------
  if (ResetWiFi == true) {
    wifiManager.resetSettings();
    ResetWiFi == false;
  }
  //set minimum quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //----------------------------------------------------------------------
  wifiManager.setMinimumSignalQuality();
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep in seconds
  //----------------------------------------------------------------------
  wifiManager.setTimeout(120);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "IoT Timer", password = password
  //and goes into a blocking loop awaiting configuration
  //----------------------------------------------------------------------
  if (!wifiManager.autoConnect(DefaultName)) {
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
  //save the custom parameters to FS
  //----------------------------------------------------------------------
  if (shouldSaveConfig) {
    //DynamicJsonBuffer jsonBuffer;
    DynamicJsonDocument doc(1024);
    //JsonObject& json = jsonBuffer.createObject();
    doc["ip"] = WiFi.localIP().toString();
    doc["gateway"] = WiFi.gatewayIP().toString();
    doc["subnet"] = WiFi.subnetMask().toString();
    File configFile = SPIFFS.open("/config.json", "w");
    serializeJson(doc, Serial);
    serializeJson(doc, configFile);
    //json.prettyPrintTo(Serial);
    //json.printTo(configFile);
    configFile.close();
    //end save
  }
  //if you get here you have connected to the WiFi
  // Read settings from EEPROM
  //----------------------------------------------------------------------
Serial.println("local ip");
  Serial.println(WiFi.localIP());
  
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
  EEPROM.begin(512);
  loadConfig();    
  // Setup NTP time requests
  //----------------------------------------------------------------------
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(NTPfastReq);
  // Begin IoT server
  //----------------------------------------------------------------------
  server.begin();
}



//###############################################################################################
// Scan for NTP Time changes
// 
//###############################################################################################
void CheckNTPtime() {

  // This line needed to keep NTP Time Synch active
  //------------------------------------------------
  timeNow = (10000 * hour()) + (minute() * 100) + second();

  // See if NTP time was set
  //------------------------
  if ( (TimeOk == true) and (NTPtimeOk == false) and (TimeCheck == true) ){
      setSyncInterval(NTPslowReq);
      NTPtimeOk = true;
  }
  // See if NTP Time was updated
  //----------------------------
  if (TimeCheck == true) {
    LastHH = hour();
    LastMM = minute();
    Lastdd = day();
    Lastmm = month();
    Lastyy = year();
    TimeCheck = false;
  }
}


//###############################################################################################
// See if time has changed and update output according to programs
// 
//###############################################################################################
void DoTimeCheck(int j) {
   boolean Output = false;
  // Is mode = Off
  if (storage[j].Mode == 0) {
    Output = false;
     //Turn off Output
     control_rele(j,0);
    return;
  }
  
  // Is mode = On
  if (storage[j].Mode == 1) {
    Output = true;
    //Turn on Output
    control_rele(j,1);
    return;
  }
  
  // Is mode invalid
  if (storage[j].Mode != 2) {
    return;
  }
  
   
  // Mode = 2, See if time changed
  timeNow = (100 * hour()) + minute();
  if (timeOld != timeNow) {
    // Time changed - check outputs
    timeOld = timeNow;
    for (byte i = 0; i < 7; i++) {
      // See if Buzzer can be controlled
      if (TimeOk != false) {
        //Time Ok, check if output must be on
        // See if Ontime < OffTime (same day)
        if (storage[j].On_Time[i] < storage[j].Off_Time[i]) {
          if ( (timeNow >= storage[j].On_Time[i]) and (timeNow < storage[j].Off_Time[i]) ) {
            // See if current day is selected
            if (storage[j].On_Days[i][weekday() - 1] == true) {
              Output = true;
            }
          }
        }
        // See if Ontime > OffTime (over two days)
        if (storage[j].On_Time[i] > storage[j].Off_Time[i]) {
          if ( (timeNow < storage[j].Off_Time[i]) or (timeNow>= storage[j].On_Time[i]) ) {
            int PrevDay = weekday() - 2;
            if (PrevDay < 0) PrevDay = 6;
            // Check current day
            if (timeNow >= storage[j].On_Time[i]) {
              if (storage[j].On_Days[i][weekday() - 1] == true) {
                Output = true;
              }
            }
            // Check previous day
            if (timeNow < storage[j].Off_Time[i]) {
              if (storage[j].On_Days[i][PrevDay] == true) {
                 Output = true;
              }
            }
          }
        } 
      }
    }
    
   
    // Set output
    control_rele(j,Output);
   // if (Output == true) digitalWrite(Relay,HIGH);else digitalWrite(Relay,LOW);
  } //cada minuto hace el chequeo del tiempo
  
}

void control_rele(int j, int Output){

  Output=!Output; //en el NodemCu van al revés....
  
   switch( j){
    case 0:
      digitalWrite(Relay_D5,Output);
      break;
    case 1:
     digitalWrite(Relay_D6,Output);
     break;
    case 2:
     digitalWrite(Relay_D7,Output);
     break;
    case 3:
     digitalWrite(Relay_D8,Output);
     break;
   }
}




void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  /*
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t=0; t<sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);

    for (unsigned int t=0; t<sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);

        timeServer[0] = storage[0].IP_1;
        timeServer[1] = storage[0].IP_2;
        timeServer[2] = storage[0].IP_3;
        timeServer[3] = storage[0].IP_4;
        Serial.println( "leyendo los datos de configuración");
        Serial.println(storage[0].DevName);
        */
        //nuvea versión FS
        if(!SPIFFS.begin()){
        Serial.println("Card Mount Failed");
        return;
        }
        File myFile = SPIFFS.open("/conf.txt", "r");
        myFile.read((byte *)&storage, sizeof(storage));
        myFile.read((byte *)&storage2, sizeof(storage2));
        myFile.close();
        Serial.println("leyendo configuracion");
        Serial.println(storage[0].DevName);
        
        //Serial.printf( "read: %s, %.5f\n", myStruct.someString, myStruct.someFloat );
}

void saveConfig() {
  Serial.println("saving config");
  /*Serial.println(storage[0].On_TimeG[0]);
    for (unsigned int t=0; t<sizeof(storage); t++)
      EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
    EEPROM.commit();
    */
    //nueva versión a un fichero de flash
    
    if(!SPIFFS.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    File myFile = SPIFFS.open("/conf.txt", "w");
    myFile.write((byte *)&storage, sizeof(storage));
    myFile.write((byte *)&storage2, sizeof(storage2));
    myFile.close(); 
    Serial.println("guardando configuracion");
    Serial.println(storage[0].DevName); 
}


//###############################################################################################
// Scan for Button presses
// 
//###############################################################################################
void ScanButton() {
  //apagar todos o encenderlos con el boton o el boton web
  boolean Output=true;
  
  if ( (digitalRead(Button) == LOW) or (WebButton == true) ) {
    WebButton= false;
      //si algún rele está activado, todos a OFF
       for(int j=0;j<Num_reles;j++){
       if(storage[j].estado_rele){
        Output=false;
       }
      }
    for(int j=0;j<Num_reles;j++){
     control_rele(j,  Output);
    }
  }
}

////////////////////////////////Cayenne //////////////////////////
CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
  Cayenne.virtualWrite(0, millis());
  Serial.println("escribiendo en out");
  float valor=sun.dAzimuth-180;
   Cayenne.virtualWrite(1, hour());
   Cayenne.virtualWrite(2, minute());
   Cayenne.virtualWrite(3, second());
   
  Cayenne.virtualWrite(4, valor);
  Cayenne.virtualWrite(5, sun.dZenithAngle,TYPE_VOLTAGE,UNIT_VOLTS  );
 /* Cayenne.virtualWrite(6, angulo);
  Cayenne.virtualWrite(7, FC_cerrada);
  Cayenne.virtualWrite(8, FC_abierta);
  Cayenne.virtualWrite(9, storage[0].control_manualD5);
  Cayenne.virtualWrite(10, storage[0].t_apertura);
  Cayenne.virtualWrite(11, storage[0].t_cierre);
  Cayenne.virtualWrite(12, puerta_cerrada);
  Cayenne.virtualWrite(13, puerta_abierta);
  Cayenne.virtualWrite(12, storage[0].control_manualD5);
  Cayenne.virtualWrite(13, storage[0].control_sol);
  //Cayenne.virtualWrite(14, WiFi.localIP());
  Cayenne.virtualWrite(15, storage2.Latitud);
  Cayenne.virtualWrite(16, storage[0].Longitud);
  int valor1= WiFi.localIP()[0];
  Cayenne.virtualWrite(17, valor1);
  valor1= WiFi.localIP()[1];
  Cayenne.virtualWrite(18, valor1);
  valor1= WiFi.localIP()[2];
  Cayenne.virtualWrite(19, valor1);
  valor1= WiFi.localIP()[3];
  Cayenne.virtualWrite(20, valor1);
  //WiFi.localIP()
*/
///////////////////////
/*
  Cayenne.virtualWrite(21, puerta_abierta);
  Cayenne.virtualWrite(21, puerta_cerrada);
  float valor3=storage[0].angulo_puesta_sol[0];
  Cayenne.virtualWrite(23, valor3);
  Cayenne.virtualWrite(24, storage[0].control_manualD5);
  
  Cayenne.digitalSensorWrite(30, FC_abierta);
  Cayenne.digitalSensorWrite(31, FC_cerrada);
   
  // Some examples of other functions you can use to send data.
  //Cayenne.celsiusWrite(1, 22.0);
  //Cayenne.luxWrite(2, 700);
  //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
  */
}

CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
/*
  Serial.print("request.channel recibido: ");
  Serial.println(request.channel);
  if(request.channel==21){
    //abrir puerta
    Serial.println("abriendo puerta remoto");
    control_D5();
    Cayenne.virtualWrite(21, puerta_abierta);
  }
    if(request.channel==22){
    //cerrar puerta
    Serial.println("cerrando puerta remoto");
    control_uv();
    Cayenne.virtualWrite(22, puerta_cerrada);
  }
  if(request.channel==23){
    //cerrar puerta
    Serial.print("ángulo de apertura/cierre: ");
    Serial.println(getValue.asString());
    String valor=getValue.asString();
    storage[0].angulo_puesta_sol[0]= valor.toFloat();
    Cayenne.virtualWrite(23, storage[0].angulo_puesta_sol[0]);
  }
    if(request.channel==24){
    //cerrar puerta
    Serial.print("Control manual: ");
    storage[0].control_manualD5=!storage[0].control_manualD5;
    Cayenne.virtualWrite(24, storage[0].control_manualD5);
  }
 */
}

///////////////FIN Cayenne //////////save configuration ////////////////////
