/*下位机代码，位置修正功#8006006000000+000-000$能被注释掉了，接收的x,y是以棋盘左下角为原点，
 * 单位毫米，z是以步进电机处于最上端时为原点，向下为正，且只能为正*/
#include<Servo.h>
#include<Stepper.h>
Servo camera;
Servo basel;
Servo arm1;
Servo arm2;
Servo arm3;
Servo hand;
Stepper leftstepper(200, 54,55,56,57);
Stepper rightstepper(200, 50, 51, 52, 53);
Stepper frontstepper(60, 42, 43, 44, 45);
const int x0 = -300;
const int y0 = 0;
const int z0 = 50;
const int xEC = 5; //error correcting
const int yEC = 5;
const int rEC = 7; //rEC=sqrt(xEC*xEC+yEC*yEC);
const int angEC = 50; //angEC=asin(xEC/rEC);
const int armlenth = 600; //lenth of robot arm
const int angle10 = 21;                                                                                            //origin angle
const int angle20 = -18;
const int angle30 = 12;
const int baselangle0 = 0;
const int handon = 144;
const int handoff = 140;
const int cameraangle0 = 0;
//const int steplenth1 = 2;
const int steplenth2 = 1;
int steps=300;
int angle1 = 0, angle2, angle3, angle4, frontstep = 0, handangle = 0, cameraangle = 0, leftstep = 0, rightstep = 0; //variables to execute
int angle1now, angle2now, angle3now, baselanglenow, handanglenow, cameraanglenow; //variables to save current para
int x = 0, y = 0, z = 0, x1 = 0, y1 = 0, cam, left, right; //getted variables
int i=0,piece0[2]={142,54},piece[13][13][2]={
{{80,110},{75,145},{75,180},{75,220},{75,260},{75,290},{70,325},{70,365},{65,400},{70,440},{70,480},{70,520},{70,540}},
{{110,120},{113,150},{110,190},{115,220},{108,255},{108,295},{108,335},{108,370},{108,400},{108,445},{108,475},{110,520},{110,550}},
{{140,130},{140,160},{145,196},{145,230},{142,275},{142,300},{140,340},{140,370},{140,410},{140,445},{140,480},{140,525},{140,555}},
{{180,140},{175,170},{175,200},{180,230},{180,260},{180,300},{175,340},{175,370},{175,410},{180,450},{180,480},{180,520},{185,560}},
{{210,150},{210,175},{210,210},{215,240},{210,275},{210,305},{215,340},{215,380},{215,415},{220,450},{220,490},{230,530},{230,560}},
{{250,160},{250,180},{255,215},{255,245},{255,275},{255,315},{255,345},{255,385},{255,415},{255,450},{255,490},{265,530},{265,560}},
{{290,170},{290,185},{290,215},{290,245},{290,285},{290,315},{290,345},{290,385},{290,415},{290,450},{290,490},{290,530},{300,560}},
{{330,160},{330,190},{330,220},{330,250},{330,280},{330,320},{325,350},{325,385},{330,415},{330,460},{330,495},{330,530},{335,560}},
{{370,160},{370,190},{365,220},{365,250},{365,280},{365,315},{365,345},{365,385},{365,420},{370,455},{370,485},{370,530},{370,560}},
{{410,160},{405,190},{405,220},{400,250},{405,290},{400,315},{405,350},{405,385},{405,415},{405,450},{405,490},{410,530},{410,560}},
{{450,150},{445,190},{445,220},{445,250},{440,285},{440,315},{440,355},{440,385},{440,425},{440,455},{440,495},{450,525},{450,555}},
{{480,150},{480,180},{480,220},{480,250},{475,280},{475,310},{475,340},{480,380},{480,420},{480,450},{480,495},{485,530},{485,555}},
{{515,150},{515,180},{515,210},{515,250},{510,275},{515,315},{515,345},{515,390},{520,415},{520,445},{520,490},{520,525},{520,550}}
};
byte head = 0, state = 1;
char blank[22];//save data from serial
double r = 0, ang = 0; //temp variables
int servomove(Servo servo, int angle, int anglenow) //buffer to ease shaking
{
  int t;
  t=80-angle1/2;
  if (angle > anglenow)
    for (; anglenow != angle;)
    {
      servo.write(anglenow+=1);
      delay(t);
    }
  else if (angle < anglenow)
    for (; anglenow != angle;)
    {
      servo.write(anglenow-=1);
      delay(t);
      }
     return angle;
}

void armangle(int ang)
{
  angle1 = (angle10 + ang);
  angle2 = (angle20 + 180 - 2 * ang);
  angle3 = (angle30 + 90 - ang)*175/180;
  if (angle1now < angle1)
  { for (; angle1now < angle1;)
    {
      arm1.write(angle1now += 1);
      arm2.write(angle2now -= 2);
      arm3.write(angle3now -= 1);
      delay(60);
    }
    arm2.write(angle2now = angle2);
  }
  else
  { for (; angle1now > angle1;)
    {
      arm1.write(angle1now -= 1);
      arm2.write(angle2now += 2);
      arm3.write(angle3now += 1);
      delay(60);
    }
  }
}
void rest()
{
  baselanglenow = servomove(basel, 90, baselanglenow);
  armangle(80);
  Serial.println("rest complete");
}
void drop()
{
  frontstepper.step(steps);
  delay(1500);
  for (; handangle < handon;)
  {
    hand.write(handangle += 1);
    delay(60);
  }
  frontstepper.step(-steps);
  for (; handangle > handoff;)
  {
    hand.write(handangle -= 1);
    delay(30);
  }
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  Serial.println("drop complete");
}
void take()
{
  for (; handangle < handon;)
  {
    hand.write(handangle += 1);
    delay(30);
  }
  frontstepper.step(700);
  for (; handangle > handoff;)
  {
    hand.write(handangle -= 1);
    delay(30);
  }
  frontstepper.step(-700);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  Serial.println("take complete");
}
void run1()
{
  
}
void grasp()
{
  armangle(piece0[1]);
  baselanglenow = servomove(basel,piece0[0], baselanglenow);
  delay(2000);
  for (; handangle < handon;)
  {
    hand.write(handangle += 1);
    delay(30);
  }
  frontstepper.step(220);
  delay(1000);
  for (; handangle > handoff;)
  {
    hand.write(handangle -= 1);
    delay(30);
  }
  delay(1000);
  frontstepper.step(-220);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  Serial.println("grasp complete");
}
void setup()
{
  /*camera.attach(7);
  basel.attach(6);
  arm1.attach(4);
  arm2.attach(5);
  arm3.attach(2);
  hand.attach(3);*/
  camera.attach(34);
  basel.attach(2);
  arm1.attach(3);
  arm2.attach(4);
  arm3.attach(5);
  hand.attach(8);
  leftstepper.setSpeed(30);
  rightstepper.setSpeed(30);
  frontstepper.setSpeed(300);
  Serial.begin(9600);
  arm1.write(angle1now = angle10 + 80);
  arm2.write(angle2now = angle20 + 20);
  arm3.write(angle3now = angle30 + 10);
  hand.write(handangle = handoff);
  camera.write(cameraanglenow = cameraangle0 + 90);
  basel.write(baselanglenow = baselangle0 + 90);
}
void loop()
{
  if (Serial.available() >= 23 && Serial.read() == '#')
  {
    head = 0; state = 0;
    x1 = 0; y1 = 0; z = 0, cam = 0; left = 0; right = 0;
    memset(blank, 0, 22 * sizeof(char));
    Serial.readBytes(blank, 22);
    if (blank[21] == '$')
    {
      head = blank[0];
      if ((head >> 4) % 8 == 4)
      {
        grasp();
        state = 1;
      }
      else if ((head >> 4) % 8 == 5)
      {
        take();
        state = 1;
      }
      else if ((head >> 4) % 8 == 6)
      {
        drop();
        state = 1;
      }
      else if ((head >> 4) % 8 == 7)
      {
        rest();
        state = 1;
      }
      else if ((head >> 4) % 8 == 2)
      {
        run1();
        Serial.println("running!");
        state = 1;
      }
      else
      {
        if ((head >> 3) % 2
            && isDigit(blank[1]) && isDigit(blank[2]) && isDigit(blank[3])
            && isDigit(blank[4]) && isDigit(blank[5]) && isDigit(blank[6]))
        {
          x1 += (blank[1] - '0') * 100 + (blank[2] - '0') * 10 + (blank[3] - '0');
          y1 += (blank[4] - '0') * 100 + (blank[5] - '0') * 10 + (blank[6] - '0');
          Serial.print("x=");
          Serial.println(x1);
          Serial.print("y=");
          Serial.println(y1);
          x = x0 + piece[x1][y1][0];
          y = y0 + piece[x1][y1][1];
          r = sqrt(pow(x, 2) + pow(y, 2));
          ang = acos((double)x  / r) * 180 / 3.14159 ;
          if(r>400&&r<470)
          steps=350-r/3;
          else if(r>400)steps=300-r/3;
          else
          steps=300;
          //r = sqrt((double)(r * r + rEC * rEC - 2 * r * rEC * cos(angEC))); //error correcting;
          //ang += asin((double)(rEC * sin(angEC) / r));
          if (r >= armlenth || r < 100 )
            Serial.println("XYNumberFalse");
          else {
            angle1 = floor(acos(r / armlenth) * 180 / 3.14159 + 0.5);
            Serial.print("angle1=");
            Serial.println(angle1);
            baselanglenow = servomove(basel, baselangle0 + floor(180 - ang + 0.5), baselanglenow);
            armangle(angle1);
            Serial.print("baselangle=");
            Serial.println(baselanglenow);
            delay(2000);
            state = 1;
          }
        }
        if ((head >> 2) % 2 && isDigit(blank[7]) && isDigit(blank[8]) && isDigit(blank[9]))
        {
          z += (blank[7] - '0') * 100 + (blank[8] - '0') * 10 + (blank[9] - '0');
          if (z > 100 || z < 0)
            Serial.println("ZFalse");
          else {
            Serial.print("Z:");
            Serial.println(z);
            frontstepper.step(z * 10 - frontstep);
            frontstep = z * 10;
            digitalWrite(42, LOW);
            digitalWrite(43, LOW);
            digitalWrite(44, LOW);
            digitalWrite(45, LOW);
            Serial.print("frontstep:");
            Serial.println(frontstep);
            state = 1;
          }
        }
        if ((head >> 1) % 2 && isDigit(blank[10]) && isDigit(blank[11]) && isDigit(blank[12]))
        {
          cam += (blank[10] - '0') * 100 + (blank[11] - '0') * 10 + (blank[12] - '0');
          if (cam < 10 || cam > 170)
            Serial.println("False");
          else {
            cameraangle = cam;
            cameraanglenow = servomove(camera, cameraangle, cameraanglenow);
            Serial.print("camera:");
            Serial.println(cameraangle);
            state = 1;
          }
        }
        if (head % 2
            && isDigit(blank[14]) && isDigit(blank[15]) && isDigit(blank[16])
            && isDigit(blank[18]) && isDigit(blank[19]) && isDigit(blank[20])
            && (blank[13] == '+' || blank[13] == '-') && (blank[17] == '+' || blank[17] == '-'))
        {
          left += (blank[14] - '0') * 100 + ( blank[15] - '0') * 10 + (blank[16] - '0');
          if (blank[13] == '-')
          {
            left = 0 - left;
          }
          right += (blank[18] - '0') * 100 + (blank[19] - '0') * 10 + (blank[20] - '0');
          if (blank[17] == '-')
          {
            right = 0 - right;
          }
          leftstepper.step(-left);
          rightstepper.step(-right);
          Serial.print("leftstep:");
          Serial.println(left);
          Serial.print("rightstep:");
          Serial.println(right);
          digitalWrite(46, LOW);
          digitalWrite(47, LOW);
          digitalWrite(48, LOW);
          digitalWrite(49, LOW);
          digitalWrite(50, LOW);
          digitalWrite(51, LOW);
          digitalWrite(52, LOW);
          digitalWrite(53, LOW);
          Serial.println("StepComplete");
          state = 1;
        }
      }
      if (state = 1)
        Serial.println("Success");
      else
        Serial.println("False");
    }
  }
}

