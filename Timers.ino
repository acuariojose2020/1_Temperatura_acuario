void randomTimers(){
    //crea las variables aleatorias una vez al dia
    int ds=weekday() - 2; //(1=domingo) (1~7)
    int ds_anterior=-1;
    char str[6];
            
            int mm = 60.0 * (spa.sunrise - (int)(spa.sunrise));
            sprintf(str, "%02d:%02d",  (int)(spa.sunrise),(int)mm);
            conf.sunrise=(int)spa.sunrise*100+(int)mm;
            mm = 60.0 * (spa.sunset - (int)(spa.sunset));
            sprintf(str, "%02d:%02d",  (int)(spa.sunset),(int)mm);
            conf.sunset=(int)spa.sunset*100+(int)mm;
   
     if (ds==ds_anterior){
      ds_anterior=ds;
      return;
     }

    // timers[nr].Tiempo_procesado[7];
    
    for (i=0;i<Num_reles;i++){
      for (j=0;j<7;j++){
                    timers[i].Tiempo_procesado[j]=timers[i].ON_Time[j];
                   if(timers[i].sun_set[j]==1 ){ //en este caso es un delay
                        timers[i].Tiempo_procesado[j]=conf.sunrise+timers[i].sign_delay[j]*timers[i].delay_time[j];
                     }
                    if( timers[i].sun_set[j]==2){
                        timers[i].Tiempo_procesado[j]=conf.sunset+timers[i].sign_delay[j]*timers[i].delay_time[j];
                    }
                   if(timers[i].aleatorio_Time[j]!=0){
                          int aleat=random(-timers[i].aleatorio_Time[j],timers[i].aleatorio_Time[j]);
                          timers[i].Tiempo_procesado[j]+=aleat;
                   }
      }
    }
     Serial.println("Random timers :");
     Serial.println(timers[0].Tiempo_procesado[0]);
}


void controlTimers(int j){ 
  timeNow = (100 * hour()) + minute();
  boolean salida=false;
  
  //j es numero de rele 0 a 3
  //i de 0 a 6 --> 7 programas

   if (timers[j].Mode == 0) {
    salida = false;
    control_rele(j,0);
    return;
  }
  
  if (timers[j].Mode == 1) {
    salida = true;
    control_rele(j,1);
    return;
  }
  
  // Is mode invalid or auto=3
  if (timers[j].Mode != 2) {
    return;
  }

  if (time_old[j] != timeNow) {
      time_old[j] = timeNow;
      randomTimers();
         int ds=weekday() - 2; //(1=domingo) (1~7)
         if (ds<0){ds=6;} //si ds=-1  es domingo dia 6
          int pactivo=0;
          byte p=0;
          byte d=0;
          for ( d = 0; d <= ds; d++) { //hasta ds dia semana de hoy
             for ( p = 0; p < 7; p++) {
              if (timers[j].ON_Days[p][d] == true) { //el programa está activo ya que hay un dia activado
                  pactivo++;
                  if(d<ds){
                    if (timers[j].onoff[p] >0){salida=true;}    
                    if (timers[j].onoff[p] <=0){salida=false;} 
                  }
                 if(ds==0){// es lunes, hay que mirar los valores del domingo
                    if (timers[j].onoff[6] >0){salida=true;}    
                    if (timers[j].onoff[6] <=0){salida=false;} 
                  }
                  if (d==ds && timeNow >= timers[j].Tiempo_procesado[p]) {
                    if (timers[j].onoff[p] >0){salida=true; Serial.println("salida true");}    
                    if (timers[j].onoff[p] <=0){salida=false;Serial.println("salida false");} 
                  }
              }
            }//d
      } //p

//si pactivo==0 entonces no hay ningún programa activo hasta el dia de hoy
// hay que mirar los dias siguientes comenzando por el domingo

  long t_anterior=0;
  p=0;
  d=0;
  
    if(pactivo==0){
      Serial.println("buscando desde el domingo");
              for ( d = 6; d > 0; d--) { //comineza en domingo y baja
                  for ( p = 0; p < 7; p++) {
                  if (timers[j].ON_Days[p][d] == true) {
                     pactivo++;
                      if(timers[j].ON_Time[p]>t_anterior ){ 
                        t_anterior = timers[j].ON_Time[p];
                        if (timers[j].onoff[p] >0){salida=true;}    
                        if (timers[j].onoff[p] <=0){salida=false;} 
                      }
                     }
                }//p
                break; //ya se encontro un timer anterior activado y salimos.
        } //d
      }

        if (pactivo>0){ //si no hay ningún dia seleccionado dejar en manual
          control_rele(j,salida);
        }
       
      Serial.print("control timers rele :");
      Serial.print(j);
      //Serial.print(" dia semana :");
      //Serial.println(ds);
      Serial.print(" Salida: ");
      Serial.println(salida);
      Serial.print(" ds: ");Serial.print(ds);Serial.print(" timeNow: ");Serial.print(timeNow);
      Serial.print(" p: ");Serial.print(p);
      Serial.print(" timers[0].ON_Time[0]: ");Serial.println(timers[0].ON_Time[0]);
      Serial.print(" timers[j].Tiempo_procesado[p]: ");Serial.println(timers[j].Tiempo_procesado[p]);
      Serial.print(" timers[0].onoff[0]: ");Serial.println(timers[0].onoff[0]);
      Serial.print(" timers[0].onoff[p]: ");Serial.println(timers[0].onoff[p]);
      
      
      }//timeold
   
  
  
}
  
void control_rele(int j, int Output){
  timers[j].Estado_rele=Output;
  estados[j]=Output;
  Output=!Output; //en el NodemCu van al revés...
  
  //digitalWrite(GPIOPIN[j],Output);
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
