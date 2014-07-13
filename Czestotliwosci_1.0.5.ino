/*PULSE GENERATOR 1,05

Tomasz Pasierbki
VALEO 2014

*/


#include <TimerOne.h>
#include <LiquidCrystal.h>

  int freq;
  int freqtemp;
  
  int DC;
  int DCtemp;
  
const int Button1 = A0;                     //PWMup
const int Button2 = A5;                     //PWMdown
const int Button3 = A1;                     //DCup
const int Button4 = A4;                     //DCdown
const int Button5 = A2;                     //ON
const int Button6 = A3;                     //Dioda
const int Przekaznik = 0;                   //Sterowanie automatyczne z przekaznika

//Wyświetlacz
const int numRows = 2;                      //Definicja liczby wierszy
const int numCols = 16;                     //Definicja Liczby kolumn
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);      //Podłaczone piny

const int OnOff = 8;                        //Pin wyjscia OnOff
const int DIODE = 7;                        //Pin sterowania dioda


#define pwmRegister OCR1A                   // Ustawienie rejestu timer1 dla OCR1A
const int   outPin =  9;                    // Wyjscie PWM dla OCR1A znajduje sie P9



long period = 1000;                         // Ustawienie czasu początkowiego w mikrosekund
int duty = 512;                             // ustawienie DC początkowego z przedziało 0-1023 wartosc 512 daje 50%
int prescale[] = {0,1,8,64,256,1024};       // definicja wartosci preskalerów 

//Czułości przycisków
long time = 0;
long ButtonDebounceShort = 100;
long ButtonDebounceLong = 500;


//Ustawinie stanów na tranzystorach
int stateOnOff = HIGH;                       //Brak wyjscia PWM

int stateDIODE = HIGH;                       //Dioda włączona

int automat;


void setup()
{
  pinMode(outPin, OUTPUT);
  Timer1.initialize(period);                                            // inicjalizacja timer1, 1000 mikrosekund
  //Timer1.pwm(outPin, duty);                                             // ustawienie pwm na P9 oraz DC 50%
  
  automat = HIGH;
  
//Inicjalizacja przycisków sterująctych  
  pinMode(Button1, INPUT);                                              //ustawienie typu wejscia
  digitalWrite(Button1, HIGH);                                          //ustawienie rezystowow pull up
    
  pinMode(Button2, INPUT);
  digitalWrite(Button2, HIGH);  
  
    pinMode(Button3, INPUT);
  digitalWrite(Button3, HIGH);  
  
    pinMode(Button4, INPUT);
  digitalWrite(Button4, HIGH);  
  
    pinMode(Button5, INPUT);
  digitalWrite(Button5, HIGH);  
  
    pinMode(Button6, INPUT);
  digitalWrite(Button6, HIGH);  
    
    pinMode(Przekaznik, INPUT_PULLUP);
  digitalWrite(Przekaznik, HIGH);
  
  
//Inicjalizacja wyświetlacza
  lcd.begin(numCols, numRows); 
  LCDstart();
  //LCDwork();
  
  
//Inicjalizacja diody oraz właczenia wylaczenia
    pinMode(OnOff, OUTPUT);                         //OnOff
    digitalWrite(outPin, LOW);
        
    
    pinMode(DIODE, OUTPUT);                         //Dioda
    //digitalWrite(DIODE, HIGH);

  //Ustawiena poczatkowe
      freq = 200;
      DC = 50;

}


void loop()
{
  int buttonPWMup       =       digitalRead(Button1);                               // read input value
  int buttonPWMdown     =       digitalRead(Button2);
  int buttonDCup        =       digitalRead(Button3);
  int buttonDCdown      =       digitalRead(Button4);
  int buttonOnOff       =       digitalRead(Button5);
  int buttonDIODE       =       digitalRead(Button6);
  int PrzekaznikState   =       digitalRead(Przekaznik);
  

  if (buttonPWMup == LOW && millis() - time > ButtonDebounceShort)      //Inkrementacja PWM
  {
     {
        freq++;
     }
     time = millis();
  }
  
  if (buttonPWMdown == LOW && millis() - time > ButtonDebounceShort)    //Dekrementacja PWM
  {
    {
        freq--;
    }
    time = millis();
  }

  if (buttonDCup == LOW && millis() - time > ButtonDebounceShort)       //Inkrementacja DC
  {
    {
        DC++;   
    }
    time = millis();
  }

  if (buttonDCdown == LOW && millis() - time > ButtonDebounceShort)     //Dekrementacja DC
  {
    {
        DC--;  
    }
    time = millis();
  }
  
    if (buttonOnOff == LOW && millis() - time > ButtonDebounceLong)                            //Włączenie wyłączenie wyjscia PWM
  {
          if (stateOnOff == HIGH)
         {     
            // turn on:    
            //digitalWrite(OnOff, LOW);
            Timer1.initialize(period);
            Timer1.pwm(outPin, duty);
            freqtemp++;
            freq=freq+1-1;
            stateOnOff = LOW;  
          } 
          else {
            // turn off:
            //digitalWrite(OnOff, HIGH); 
            digitalWrite(outPin, LOW);
            stateOnOff = HIGH;
            }
    time = millis();
   
  }
  
  
   else if (buttonDIODE == LOW && millis() - time > ButtonDebounceLong)    //Włączenie wyłączenie DIODY
  {
           if (stateDIODE == HIGH)
         {     
            // turn on:    
            digitalWrite(DIODE, HIGH);
            stateDIODE = LOW;  
          } 
          else {
            // turn off:
            digitalWrite(DIODE, LOW); 
            stateDIODE = HIGH;
          }
    time = millis();
  }

  
  if (stateOnOff == LOW && PrzekaznikState == LOW )
  {
    digitalWrite(outPin, LOW);
    automat = LOW;
  }

  if (automat == LOW && PrzekaznikState == HIGH )
  {
            Timer1.initialize(period);
            Timer1.pwm(outPin, duty);
            freqtemp++;
            freq=freq+1-1;
            automat = HIGH;
  }

   processSerial();
}


void processSerial()
{

      if(freq != freqtemp && freq>0 && freq<501)
      {      
            
            lcd.setCursor(2, 0);
            if (freq>=10 && freq<100)
                {
                  lcd.print(" ");
                  lcd.print(freq);
                }
            else if (freq>0 && freq<10)
                {
                  lcd.print("  ");
                  lcd.print(freq);
                }
            else
            {
              lcd.print(freq);
            }
        
        period = 1000000 / freq;
        Timer1.setPeriod(period);
        duty = map(DCtemp,1,99, 1, ICR1);
        Timer1.setPwmDuty(outPin, duty);  // don't change the duty cycle
        pwmRegister = duty;
        
       }
     freqtemp = freq;

     if(DC != DCtemp && DC >0 && DC < 100)
     {
         lcd.setCursor(13, 0);   
         if (DC<10)
          {
            lcd.print(" ");
            lcd.print(DC);
          }
          else
          {
            lcd.print(DC);
          }
       
        duty = map(DC,1,99, 1, ICR1);
        pwmRegister = duty;
     }
     DCtemp = DC;

LCDwork();
}

void LCDstart()
{
 lcd.clear(); //czyszczenie LCD
 lcd.setCursor(0,0);
 lcd.print("PULSE  GENERATOR");  
 lcd.setCursor(0,1);
 lcd.print("VALEO  Ver. 1,05");  
 delay(3000);
 lcd.clear();
}


void LCDwork()
{
lcd.setCursor(0, 0);
lcd.print("f=");
lcd.setCursor(5, 0);
lcd.print("Hz   ");
lcd.setCursor(10, 0);
lcd.print("DC=");
lcd.setCursor(15, 0);
lcd.print("%");
          
          
    if (stateOnOff == LOW)
         {     
            lcd.setCursor(0, 1);
            lcd.print("WYJSCIE");
         } 
    else {
            lcd.setCursor(0, 1);
            lcd.print("       ");
          }


    if (stateDIODE == HIGH)
         {     
            lcd.setCursor(10, 1);
            lcd.print("DIODA");
          } 
    else {
            lcd.setCursor(10, 1);
            lcd.print("     ");
          }

}
