#include "includes.h"
/// @brief calcule les bpm grace a la periode entre chaque pic sur la courbe
void heartSensor::heartBeat(bool draw)
{   
    unsigned long startTime,tempsEnHaut,period = 0;
    int moyenne;
    int buffer[20] = {};

    //on rempli le tableau
    for(int i = 0; i < 20;i++)
    {
        buffer[i] = analogRead(sensorHeartCapt);
    }
    //on fait la moyenne des 20 valeurs
    for(int j = 0; j < 20;j++)
    {
        moyenne += buffer[j];
        if(j == 19){
            moyenne = moyenne /20;
        }
    }
    
    //drawing sinusoide on oled screen
    
    screen->cursorPos.x = screen->cursorPos.x + 1;
    if(screen->cursorPos.x > 128)
    {
      screen->cursorPos.x = screen->cursorPos.x - 128;
      screen->pDisp->clearDisplay();
    }
    
    
    for(int k = 0; k < 20;k++)
    {
        if(buffer[k] > moyenne){
            //vers le haut
            screen->cursorPos.y = screen->cursorPos.y - (buffer[k] - moyenne);
        }
        else{
            //vers le bas
            screen->cursorPos.y = screen->cursorPos.y + (moyenne - buffer[k]);
        }
        //ligne pour debug la sinusoide
        screen->pDisp->drawLine(screen->cursorPos.x,32,screen->cursorPos.x,screen->cursorPos.y,WHITE);
    }
    /*
        avoir les changement des fronts 
        il faut que a chaque changement de front on compte le temps jusqu'au prochain changement de front
        et a partir de la, calculer la frequence
    */
    if(screen->cursorPos.y > 32){
        //etat bas
        if(start){
            tempsEnHaut = (currentMillis - startTime); //car le calcul est sur le front descendant
            period = (tempsEnHaut - oldTime)/4;

            bpm = periodToBPM(period);
            if(bpm>30 && bpm < 200){
              heart->screen->pDisp->setCursor(5,5);
              heart->screen->pDisp->setTextColor(WHITE,BLACK);
              heart->screen->pDisp->setTextSize(2);
              heart->screen->pDisp->println(heart->bpm);
              
            }
            oldTime = tempsEnHaut; 
            start = !start;
        }
    }
    else{
        //etat haut
        if(!start){
            start = !start;
            startTime = currentMillis;
            
        }
    }
} 
void heartSensor::addTobuffer(){
  if((currentMillis - bufferTiming) > 1000){
    if(bufferCount < 10){
      buffer10[bufferCount] = bpm;
    }
    else{
      //FIFO
      buffer10[0] = buffer10[1];
      buffer10[1] = buffer10[2]; 
      buffer10[2] = buffer10[3]; 
      buffer10[3] = buffer10[4]; 
      buffer10[4] = buffer10[5]; 
      buffer10[5] = buffer10[6]; 
      buffer10[6] = buffer10[7];
      buffer10[7] = buffer10[8]; 
      buffer10[8] = buffer10[9]; 
      buffer10[9] = bpm;  
      //Serial.print("bpm ecrit : ");
      //Serial.println(bpm);
    }
  fillBuffer1min();
  bufferCount++;
  bufferTiming = currentMillis;
  }
}
void heartSensor::fillBuffer1min()
{
 
  if(bufferCount%5 == 0 && bufferCount != 0){ //on rentre toute les 5 secondes
    
    if(k1 <12){
      buffer60[k1] = moyenneBuffer10();
     
      if((buffer60[k1] > 100 || buffer60[k1] == 0) && (k1 != 0)){
        buffer60[k1] = buffer60[k1-1];
      }
      //Serial.println(buffer60[k1]);
    }
    else{
      //FIFO
     
      buffer60[0] = buffer60[1];
      buffer60[1] = buffer60[2]; 
      buffer60[2] = buffer60[3]; 
      buffer60[3] = buffer60[4]; 
      buffer60[4] = buffer60[5]; 
      buffer60[5] = buffer60[6]; 
      buffer60[6] = buffer60[7];
      buffer60[7] = buffer60[8]; 
      buffer60[8] = buffer60[9]; 
      buffer60[9] = buffer60[10];  
      buffer60[10] = buffer60[11]; 
      buffer60[11] = moyenneBuffer10(); 
      
  
    }
    k1++;
  
  }
 
}
int heartSensor::moyenneBuffer10()
{
  int moyenne,f;
  if((bufferCount%5 == 0) && (bufferCount != 0)){
    for(int i = 0; i < 5;i++){
      moyenne += buffer10[i];
    }
    f = (moyenne / 5);
    return f;
  }
}
// period * x = 60 <=> x = 60/ period
int heartSensor::periodToBPM(unsigned long t){
    return (60000/t);
}
/// @brief https://pdf1.alldatasheet.com/datasheet-pdf/download/254790/MAXIM/DS1302.html page 8/13
/// @param addr l'addresse du registre a ecrire
/// @param data la valeur a mettre dans le registre precedent
void clock::writeRegister(unsigned char addr,unsigned char data)
{ 
//https://pdf1.alldatasheet.com/datasheet-pdf/download/254790/MAXIM/DS1302.html 
//voir bas page 8 <=> on reproduit en code
  unsigned char i;
  // on met ce a 1 jusqu'a la fin
  digitalWrite(resetPin, HIGH);
  delay(1);
  // Read = high / Write = low, ici low car on ecrit
  digitalWrite(dataPin, LOW);   
  delay(1);
  //on met clock a 1 puis 0 pour ecrire la value dans le registre
  digitalWrite(clockPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, LOW);   
  delay(1);
  // Ecriture de l'adresse
  for (i=0 ; i<=4 ; i++)
  {
    digitalWrite(dataPin, (addr & (1<<i))>>i);   
    delay(1);
    digitalWrite(clockPin, HIGH);   
    delay(1);
    digitalWrite(clockPin, LOW);   
    delay(1);
  }
  // Ram  = HIGH et clock = LOW (ici low)
  digitalWrite(dataPin, LOW);   
  delay(1);
  digitalWrite(clockPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, LOW);   
  delay(1);
  // le dernier bit est toujours un 1
  digitalWrite(dataPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, LOW);   
  delay(1);   
  // Ecriture de la donnee de 8 bits
  for (i=0 ; i<=7 ; i++)
  {
    digitalWrite(dataPin, (data & (1<<i))>>i);   
    delay(1);
    digitalWrite(clockPin, HIGH);   
    delay(1);
    digitalWrite(clockPin, LOW);   
    delay(1);
  }
  // reset a 0, fin
  digitalWrite(resetPin, LOW);
  delay(1); 
}
/// @brief https://pdf1.alldatasheet.com/datasheet-pdf/download/254790/MAXIM/DS1302.html page 8/13
/// @param addr l'addresse du registre a lire
unsigned char clock::readRegister(unsigned char addr)
{ 
  //voir comment dans writeRegister()
  unsigned char i;
  unsigned char data = 0;
  digitalWrite(resetPin, HIGH);
  delay(1);
 
  digitalWrite(dataPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, LOW);   
  delay(1);
  // Ecriture de l'adresse
  for (i=0 ; i<=4 ; i++)
  {
    digitalWrite(dataPin, (addr & (1<<i))>>i);   
    delay(1);
    digitalWrite(clockPin, HIGH);   
    delay(1);
    digitalWrite(clockPin, LOW);   
    delay(1);
  }
  digitalWrite(dataPin, LOW);   
  delay(1);
  digitalWrite(clockPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, LOW);   
  delay(1);
  digitalWrite(dataPin, HIGH);   
  delay(1);
  digitalWrite(clockPin, HIGH);   
  delay(1);
  // Lecture de la donnee de 8 bits donc input 
  pinMode(dataPin, INPUT);    
  delay(1);
  for (i=0 ; i<=7 ; i++)
  {
    digitalWrite(clockPin, LOW);   
    delay(1);
    data = data + (digitalRead(dataPin)<<i);   
    digitalWrite(clockPin, HIGH);   
    delay(1);
  }
  digitalWrite(clockPin, LOW);
  delay(1); 
  digitalWrite(clockPin, LOW);   
  delay(1);
  pinMode(dataPin, OUTPUT);   
  delay(1);
  return data;
}
/// @brief ecrit dans les registres les valeurs souhaités 
/// @return true si sucessfull
bool clock::initClock()
{
  writeRegister(7,0b00000000);    // access aux registres
  writeRegister(0,0b00000000);    // secondes
  writeRegister(1,0b01010001);    // minutes
  writeRegister(2,0b00010000);    // 24 heures
  writeRegister(3,0b00010010);    // jour (numero)
  writeRegister(4,0b00010001);    // mois
  writeRegister(5,0b00000110);    // date (de 1-7 ex mardi)
  writeRegister(6,0b00100010);    // annee 
  return true;
}
/// @brief cette fonction recupere la valeur des registre et les affiche sur l'ecran oled
void clock::afficheHeure()
{
  heart->screen->pDisp->clearDisplay();


  heart->screen->pDisp->setCursor(75,1);
  heart->screen->pDisp->setTextSize(1);
  heart->screen->pDisp->setTextColor(WHITE);
  //heures
  buffer = readRegister(0x02);
  heart->screen->pDisp->print((buffer & 0b00110000)>>4,DEC);
  heart->screen->pDisp->print((buffer & 0b00001111),DEC);
  heart->screen->pDisp->print(":");
  //minutes
  buffer = readRegister(0x01);
  heart->screen->pDisp->print((buffer & 0b11110000)>>4,DEC);
  heart->screen->pDisp->print((buffer & 0b00001111),DEC);
  heart->screen->pDisp->print(":");
  //secondes
  buffer = readRegister(0x00); 
  heart->screen->pDisp->print((buffer & 0b01110000)>>4,DEC);
  heart->screen->pDisp->print((buffer & 0b00001111),DEC);
}
/// @brief cette fonction sert a switch entre l'heure et la courbe bpm. montage "Pull down" pour l'heure (deprecated ! )
/// @return -1 si aucune condition remplie, 1 si le beep a été toggle, 2 si il a eu double push
int oled::isButtonPressed()
{
  bButton = (bool)digitalRead(buttonPin);
  doublePressed = false;
  if(bButton){    //si on le bouton est appuyé
    
    if((heart->currentMillis - doublePush) > 1000){  //si ça fait plus de 1s que je ne suis pas rentré
      press = true;                              
      doublePush = heart->currentMillis;            
    }
    if(press && (((heart->currentMillis - doublePush) > 400) && ((heart->currentMillis - doublePush) < 1000))){ //si ça fait plus de 250ms que je ne suis pas rentré ET que ça fais moins de 500ms que je ne suis pas rentre
        doublePressed = true;
        press = false;
        pressed = false;
        return 2;
    }
    //ne pas revenir dans cette condition pendant 2 secondes
    if((heart->currentMillis - delayMax) > 2000){
      pressed = !pressed;
      delayMax = heart->currentMillis;
      return 1;
    }
  }
  return -1;
}
/// @brief ecran noir
void oled::drawBlackScreen()
{
  heart->screen->pDisp->fillRect(0,0,128,64,BLACK);
}
/// @brief allume les led en fonction des bpm
void heartSensor::healthLeds(bool active)
{
  if(bpm >= 50 && bpm < 90){
    digitalWrite(greenLed,HIGH);
    digitalWrite(redLed,LOW);
    digitalWrite(yellowLed,LOW);
  }
  else if(bpm >= 90 && bpm < 170){
    digitalWrite(yellowLed,HIGH);
    digitalWrite(redLed,LOW);
    digitalWrite(greenLed,LOW);
  }else if((bpm >= 170 && bpm < 220) || (bpm < 50 && bpm >= 30)){
    digitalWrite(redLed,HIGH);
    digitalWrite(greenLed,LOW);
    digitalWrite(yellowLed,LOW);
  }
  else{
    digitalWrite(redLed,LOW);
    digitalWrite(greenLed,LOW);
    digitalWrite(yellowLed,LOW);
  }
  
}
/// @brief beep en fonction 
void heartSensor::beebBpm()
{
  interval = (float)((float)60 / (float)bpm);
  
  if(bpm < 50){
    toneValue = 255;
  }
  else if((bpm >= 50) && (bpm <=90)){
    toneValue = 90;
  }else if((bpm>= 90) && (bpm <= 170)){
    toneValue = 2;
  }else if((bpm > 170) && (bpm < 220)){
    toneValue = 1;
  }
  if(((currentMillis - lastBeep) > (interval*1000)) && (bpm > 30)){
    tone(buzzerPin,toneValue,50);
    lastBeep = currentMillis;
  }
  
}
void oled::drawGraph()
{
  pDisp->setTextSize(1);
  pDisp->setTextColor(WHITE);

  //repere orthonorme
  pDisp->drawLine(startingPoint.x,startingPoint.y,startingPoint.x + len.x,startingPoint.y,WHITE);
  pDisp->drawLine(startingPoint.x,startingPoint.y,startingPoint.x,startingPoint.y - len.y,WHITE);

  //echelle
  POTvalue = analogRead(POTpin);
  echelleABS = map(POTvalue,0,1023,0,100);
  for(int j = 1; j <= 5;j++){ //ordonées
    pDisp->drawLine(startingPoint.x-2,startingPoint.y-(j*10),startingPoint.x+2,startingPoint.y-(j*10),WHITE);
    pDisp->setCursor(0,startingPoint.y-(j*10)),
    pDisp->println(j+5); //bpm a metre a *10
  }
  if(echelleABS <= 50){ //10secondes <=> on decoup en 10
    secondes = 10;
    tailleDesDecoupes = (float)((float)len.x / (float)secondes);
    for(int i = 0; i < secondes;i++){
      pDisp->drawLine(startingPoint.x + (i*tailleDesDecoupes),startingPoint.y+2,startingPoint.x + (i*tailleDesDecoupes),startingPoint.y-2,WHITE);
      pDisp->setCursor(startingPoint.x + (i*tailleDesDecoupes),startingPoint.y+3);
      pDisp->println(i);
      
    }
    pDisp->setCursor(startingPoint.x + (9*tailleDesDecoupes)+6,startingPoint.y+3);
    pDisp->println("s");
    
  }else if((echelleABS > 50) ){ //1min
    counter = 0;
    secondes = 60;
    tailleDesDecoupes = (float)((float)len.x / (float)(secondes/5));
    for(int i = 0; i < secondes;i+=5){
      pDisp->drawLine(startingPoint.x + (counter*tailleDesDecoupes),startingPoint.y+2,startingPoint.x + (counter*tailleDesDecoupes),startingPoint.y-2,WHITE);
      pDisp->setCursor((startingPoint.x + (counter*tailleDesDecoupes))-5,startingPoint.y+3);
      if(i == 5 || i == 25 || i == 55){
        pDisp->println(i);
      }
      counter++;
      

    }
    pDisp->setCursor(startingPoint.x + (11*tailleDesDecoupes)+6,startingPoint.y+3);
    pDisp->println("s");
  }
  //tracage des rectangles
  if(echelleABS <= 50){
    int increment = 0;
    for(int l = 10; l > 0; l--){
      if(heart->buffer10[l] > 51 && heart->buffer10[l] < 100){
        pDisp->fillRect(startingPoint.x+(increment*12),startingPoint.y-((heart->buffer10[l])-50),12,(heart->buffer10[l])-50,WHITE);
      }
      else{ 
        pDisp->setCursor(startingPoint.x+(l*12),startingPoint.y-5);
        pDisp->println("F");
      }
      increment++;
    }
  }
  else if((echelleABS > 50) ){
    int tailleDecoupe = 10;
    int increment = 0;
    for(int h = 12; h >0;h--){
      if(heart->buffer60[h] > 51 && heart->buffer60[h] < 100 ){
        //Serial.println(heart->buffer60[h]);
        //Serial.println(h);
        pDisp->fillRect(startingPoint.x+(increment*10),startingPoint.y-((heart->buffer60[h])-50),10,(heart->buffer60[h])-50,WHITE);
      }else{
        pDisp->setCursor(startingPoint.x+(h*tailleDecoupe),startingPoint.y-5);
        pDisp->println("F");
      }
      increment++;
   }
  }
 
}
void heartSensor::sendDataToEEPROM()
{
  buttonEEPROM = (bool)digitalRead(pinEEPROM);
  
  if(buttonEEPROM)
  {
    if((currentMillis - EEPROMtiming) > 1000) //toutes les secondes on peut rerentré
    {
      EEPROM.write(addr,bpm);
      lastAddr = addr;
      addr+= 1;
      EEPROMtiming = currentMillis;
    }
  }
}