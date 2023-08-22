/*
	BPM clock source
	It is meant to generate a clock signal with 50% duty cycle at settable 
	dual multiple of the frequency (x1, x2, x4) and generate a sync signal

	LCD
	 0 1 2 3 4 5 6 7 
	 * * * * * * * * 
	|  x x x . x    |	0
	| d i v : y     | 	1
*/

#define IN_PP_PIN 16
#define IN_STOP_PIN 17
bool State = false; // false => stop/pause, true => play
bool flag1= false, flag2= false;

#define R_LED_PLAY_PIN 11
#define G_LED_PLAY_PIN 12
#define R_LED_RST_PIN 10

#define OUT_CK_PIN 2
#define SYNC_PIN  1
#define OUT_PLAY_PIN 3
#define OUT_RST_PIN 0

#include <LiquidCrystal.h>
#define rs 9
#define en 4
#define d4 8
#define d5 7
#define d6 6
#define d7 5
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include <rEncoder.h>
#define pinA 13
#define pinB 14
#define pinP 15
rEncoder ENC(pinA, pinB, pinP,false);

#include <Timer.h>

Timer T(MILLIS);

float BPM = 120.0, Step = 0.5;
#define maxBPM 	300
#define minBPM	60 

int multiplier = 1; // 1,2,4

bool updateVal = false;
const bool hiState = false;
void setup(){
	pinMode(IN_PP_PIN, INPUT_PULLUP); 	//play pause button GPI
	pinMode(IN_STOP_PIN, INPUT_PULLUP); //Stop/ reset button GPI

	pinMode(OUT_PLAY_PIN, OUTPUT);
	pinMode(OUT_RST_PIN, OUTPUT);

	pinMode(OUT_CK_PIN,	OUTPUT);		//tip of 3.5mm jack
	pinMode(SYNC_PIN,	OUTPUT);		//cener of 3.5mm jack
}	

void loop(){
	BPM = ENC.updateEncoderMult(BPM,Step);
	updateBPM();

	if(ENC.getPBState() == hiState && updateVal == false){
		updateLCD();
		updateVal = true;
	}
   if(ENC.getPBState() == !hiState && updateVal == true){
        updateVal = false;
   }

	if(BPM >= 200) Step = 1;
	else{
Step = 0.5;
	}

	//Play - pause control

	if(!digitalRead(IN_PP_PIN) &&!flag1){
		flag1 = true;
	}
	if (digitalRead(IN_PP_PIN) &&flag1){
		State = !State;
		digitalWrite(OUT_PLAY_PIN, State);
		flag1 = false;
	}

	//Stop state control

	if(!digitalRead(IN_STOP_PIN) &&!flag2){
		flag2 = true;
	}
	if (digitalRead(IN_PP_PIN) &&flag2){
		
		digitalWrite(OUT_RST_PIN, 1);	//RESET HI
		digitalWrite(OUT_PLAY_PIN, 0);	//stop signal (LOW)
		delay(10);
		digitalWrite(OUT_RST_PIN, 0);	//RESET LOW +10ms
		flag2 = false;
	}
}

float oldBPM;
void updateBPM(){
	if (BPM !=oldBPM){
		T.setClock((BPM/60.0)*1000*multiplier, 50);
		oldBPM = BPM;
	}
}

void updateLCD(){
	lcd.setCursor(0,0);
	lcd.print("     ");
	if(BPM >= 100)		lcd.setCursor(0,0);
	else if(BPM >= 10)	lcd.setCursor(1,0);
	else
		lcd.setCursor(2,0);
	lcd.print(BPM,1);
}
void setColors(bool R1, bool G1, bool R2){
    digitalWrite(R_LED_PLAY_PIN, R1);
    digitalWrite(G_LED_PLAY_PIN, G1);
    digitalWrite(R_LED_RST_PIN, R2);
}
