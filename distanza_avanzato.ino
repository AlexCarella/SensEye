#include <Servo.h>

unsigned long time = 0;
unsigned long dist = 0;
unsigned long refDist[5] = {0,0,0,0,0}; //(5) angles
long distVar = 0; //distance variation from reference value
unsigned long absDistVar = 0; //absolute value of distVar
unsigned long distLimit = 10000000; //nm
int trig = 7;
int echo = 4;
int cont[5] = {0,0,0,0,0}; //how long the measured distance differs from the reference one along the (5) directions
Servo myServo; 
int pinServo = 2;
int rotAng = 30;//desired angular rotation
//since the maximum angle of measurement is 30 degrees, with such angular rotation the sensor should be able to map the whole (150) degrees 


unsigned long getDistance(); //prototype of the function that measures dist
  int getMax(int cont[]); //prototype of the function that returns the maximum element of a vector

void setup(){
  Serial.begin(9600);
  myServo.attach(pinServo);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  /*digitalWrite(echo, HIGH); //pullup resistor*/
  int j = 0;
  for(int pos=0; pos<121; pos+=rotAng){ // 120 degree rotation by steps of rotAng
    myServo.write(pos);
    delay(1000);
    refDist[j] = getDistance();//gets the reference distances ([0], [1], [2]...) : we start to build the familiar environment
    //Serial.println("refDist");
    //Serial.println(j);
    //Serial.println(refDist[j]);
    ++j;
  }
}

void loop(){
  int i = 0; //index of elements of refDist vector (each element is the distance of reference for different angles)
  for(int pos=0; pos<121; pos+=rotAng){ 
    myServo.write(pos);
    delay(1000); //wait until the servo motor finishes its movement before measuring
    dist = getDistance();
    distVar = refDist[i] - dist;
    absDistVar = abs(distVar);
    //Serial.println("absDistVar");
    //Serial.println(i);
    //Serial.println(dist);
    if(absDistVar < distLimit){ //range of accepted values: 10 cm (no danger)
      cont[i] = 0;
      if(getMax(cont)<1){
       Serial.println("familiar environment: no obstacles present in the area"); 
      }else if(/*getMax(cont)>0 && */getMax(cont)<=3){
        Serial.println("possible hazard: double checking in progress");
        }
    }else{
      ++cont[i];
      if(cont[i] > 3){ 
    //if for 3 cycles the object hasn't moved yet then it's an obstacle
        Serial.println("hazard found -> there is an obstacle in the area");
        Serial.println("the obstacle is located at");
        Serial.println(rotAng*i);
        Serial.println("degrees");        
      }else{
        Serial.println("possible hazard: double checking in progress");
        }
    }
    ++i;
  }
}

unsigned long getDistance(){
  unsigned long velSound = 34385; //vel suono a 20 gradi cent [cm/s]
  int i = 1; //number of measurement
  unsigned long dist_i = 0; //distanza misurata l'i-esima volta
  unsigned long dist_sum = 0; //somma dei 10 valori di distanza misurati
  while(i < 16){
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW); /*telling the sensor to send 
    the train of 8 impulses (40Hz) */
    time = pulseIn(echo, HIGH); /* returns the time 
    echo takes to go from HIGH to LOW */ 
    dist_i = (velSound*time)/2;
    
    dist_sum = dist_sum + dist_i;
    ++i;
  }
  dist = dist_sum / i; //media aritmetica della distanza
  return(dist);
}

  int getMax(int cont[]){
     int max_cont = cont[0];
    for(int i = 1; i < 5; ++i){
      max_cont = max(max_cont,cont[i]);  
      }
      return(max_cont);
    }
