#include<Servo.h>
const int inputPin=2;
const int debounceDelay=500;
const int angle0=55;
Servo myservo;
boolean debounce(int pin)
{
  boolean state=0;
  boolean previousState;
  int counter;
  previousState=digitalRead(pin);
  for(counter=0;counter<debounceDelay;counter++)
  {
    delay(1);
    state=digitalRead(pin);
    if(state!=previousState)
    {
      counter=0;
      previousState=state;
      }
    }
    return state;
  }
void push()
{
  int i;
  for(i=angle0;i<angle0+60;i++)
  {myservo.write(i);delay(40);}
  for(;i>angle0;i--)
  {myservo.write(i);delay(40);}
}
void setup()
{
  pinMode(inputPin,INPUT);
  digitalWrite(inputPin,HIGH);
  myservo.attach(9);
  myservo.write(angle0);
}

void loop()
{
  if(debounce(inputPin))
  {delay(2000);push();}
}
