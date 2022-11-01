/*
* File: adaptiveFreqOsc2.ino
* Purpose: Adaptive Frequency Oscillator Demo
* Version: 1.0.0
* Date: 07-01-2020
* Created by: Martin Stokroos
* URL: https://github.com/MartinStokroos/adaptiveFreqOsc
*
* License: MIT License
*
* This adaptive Hopf frequency oscillator synchronizes to the periodic input signal applied to the 
* analog input pin A0. The periodic (AC) drive signal should be biased on 2.5V DC.
* The amplitude of the drive signal should be limited to 2.5V. Without drive signal (but with DC-bias), the
* oscillator starts outputting the intrinsic frequency, defined by the constant 'F_INIT'.
* 
* This is a fast implementation on interrupt basis. Timer1 generates the loop frequency and triggers the ADC.
* 3kHz is almost the maximum loop rate to perform all calculations.
*
* The oscillator output pin, defined by PWM_OUT, has a PWM frequency of about 31kHz. Use a low-pass filter
* to observe the oscillator output wave in the frequency range of a few Hz to a few tens of Hz.
*
* - use the 'q' and 'a' keys to modify and set the value for Epsilon
* - use the 'w' and 's' keys to modify and set the value for gamma
* - use the 'e' and 'd' keys to modify and set the value for mu
* - press 'f' to print the current frequency.
*
*/


// Library to be included for fast digital I/O when observing the timing with an oscilloscope.
//#include <digitalWriteFast.h>  // library for high performance digital reads and writes by jrraines
                // see http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1267553811/0
                // and http://code.google.com/p/digitalwritefast/


#define LPERIOD 500000    // the main loop period time in us. In this case 500ms.
#define PIN_LED 13    // PLL locking status indicator LED
#define PIN_PWMA 9  // Timer1 OCR1A 10-bit PWM
#define PIN_PWMB 10 // Timer1 OCR1B 10-bit PWM
#define PIN_PWMC 3 // Timer 2 8-bit PWM
#define PIN_PWMD 11 // Timer 8-bit PWM
#define PIN_DEBUG 4 // test output pin

#define F_INIT 10.0

// global vars
volatile int adcVal;
int dcBias = 512; //dc-bias byte value
unsigned long nextLoop;

char c; // incoming character from console

// adaptive Hopf oscillator parameters
#define T 1/3000.0 // loop frequency = 3kHz.
float Eps = 20.0; // coupling strength (K>0).
                 // Oscillator locks faster at higher values, but the ripple of the frequency is larger.
float gamma = 10.0; // the oscillator follows the amplitude of the stimulus better for larger values for g.
                // May causes instability at large values for gamma.
float mu = 5.0; // Seems to scale the output amplitude. May Cause instability at large values for mu.

float x = 1.0;  // starting point of the oscillator. x=1, y=0 starts as a cosine.
float x_new = 0.0;
float xdot = 0.0;
float y = 0.0;
float y_new = 0.0;
float ydot = 0.0;
float ohm = 2*PI*F_INIT;
float ohm_new = 0.0;
float ohmdot = 0.0;
float F = 0.0;




// ******************************************************************
// Setup
// ******************************************************************
void setup(){
  cli(); // disable interrupts
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_PWMC, OUTPUT);
  pinMode(PIN_PWMD, OUTPUT);
  pinMode(PIN_DEBUG, OUTPUT);

  Serial.begin(115200);

  // initialize ADC for continuous sampling mode
  DIDR0 = 0x3F; // digital inputs disabled for ADC0D to ADC5D
  bitSet(ADMUX, REFS0); // Select Vcc=5V as the ADC reference voltage
  bitClear(ADMUX, REFS1);
  bitClear(ADMUX, MUX0); // select ADC CH# 0
  bitClear(ADMUX, MUX1);
  bitClear(ADMUX, MUX2);
  bitClear(ADMUX, MUX3);
  bitSet(ADCSRA, ADEN); // AD-converter enabled
  bitSet(ADCSRA, ADATE); // auto-trigger enabled
  bitSet(ADCSRA, ADIE); // ADC interrupt enabled

  bitSet(ADCSRA, ADPS0);  // ADC clock prescaler set to 128
  bitSet(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);

  bitClear(ADCSRB, ACME); // Analog Comparator (ADC)Multiplexer enable OFF
  bitClear(ADCSRB, ADTS0); // triggered by Timer/Counter1 Overflow
  bitSet(ADCSRB, ADTS1);
  bitSet(ADCSRB, ADTS2);
  bitSet(ADCSRA, ADSC);    // start conversion

  /* TIMER1 configured for phase and frequency correct PWM-mode 8, top=ICR1 */
  // prescaler = 1:
  bitSet(TCCR1B, CS10);
  bitClear(TCCR1B, CS11);
  bitClear(TCCR1B, CS12);
  // mode 8:
  bitClear(TCCR1A, WGM10);
  bitClear(TCCR1A, WGM11);
  bitClear(TCCR1B, WGM12);
  bitSet(TCCR1B, WGM13);
  // top value. f_TIM1 = fclk/(2*N*TOP)
  ICR1 = 2667; // f=3kHz.
  
  // set output compare for pin 9 and 10  (for now we don't use PWM from Tim1 on pin 9 and 10)
  //bitClear(TCCR1A, COM1A0);  // Compare Match PWM 9
  //bitSet(TCCR1A, COM1A1);
  //bitSet(TCCR1A, COM1B0);  // Compare Match PWM 10 - inverted channel
  //bitSet(TCCR1A, COM1B1);

  // PWM output on pin 3&11 (8-bit Tim2 OCR)
  TCCR2B = (TCCR2B & 0xF8)|0x01; // set Pin3 + Pin11 PWM frequency to 31250Hz
  
  // enable TIMER1 compare interrupt
  bitSet(TIMSK1, TOIE1); // enable Timer1 Interrupt
  sei(); // global enable interrupts

  // start PWM out on pin 3&11
  analogWrite(PIN_PWMC, 127);
  analogWrite(PIN_PWMD, 127);

  nextLoop = micros() + LPERIOD; // Set the loop timer variable for the next loop interval.
}




// ******************************************************************
// Main loop
// ******************************************************************
void loop(){
  digitalWrite(PIN_LED, !digitalRead(PIN_LED)); // toggle the LED

  if(Serial.available()){
  c = Serial.read();
    if(c == 'q') {  // use the 'q' and 'a' keys to set Eps
        Eps += 1;
        Eps = constrain(Eps, 0, 100);
      Serial.print("Epsilon: ");
      Serial.println(Eps,1);
    }
    if(c == 'a') {
        Eps -= 1;
        Eps = constrain(Eps, 0, 100);
      Serial.print("Epsilon: ");
      Serial.println(Eps,1);
    }
    if(c == 'w') { // use the 'w' and 's' keys to set gamma.
        gamma += 1.0;
        gamma = constrain(gamma, 0, 500);
      Serial.print("gamma: ");
      Serial.println(gamma,1);
    }
    if(c == 's') {
        gamma -= 1.0;
        gamma = constrain(gamma, 0, 500);
      Serial.print("gamma: ");
      Serial.println(gamma,1);
    }
    if(c == 'e') { // use the 'e' and 'd' keys to set mu
        mu += 0.1;
        mu = constrain(mu, -10, 10);
      Serial.print("mu: ");
      Serial.println(mu,1);
    }
    if(c == 'd') {
        mu -= 0.1;
        mu = constrain(mu, -10, 10);
      Serial.print("mu: ");
      Serial.println(mu,1);
    }
    if(c == 'f') { // print frequency
        Serial.print("freq: ");
      Serial.println(ohm/(2*PI),2);
    }
  }

  while(nextLoop > micros());  // wait until the end of the time interval
  nextLoop += LPERIOD;  // set next loop time at current time + LOOP_PERIOD
}




/* ******************************************************************
* ADC ISR. The ADC is triggered by Timer1.
* Here we do the fast things at a constant rate. f=3kHz (=60 sampl./grid-period @50Hz)
* *******************************************************************/
ISR(ADC_vect){
  //digitalWriteFast(PIN_DEBUG, HIGH); //for checking the interrupt frequency
 
  // read the current ADC input channel
  adcVal=ADCL; // store low byte
  adcVal+=ADCH<<8; // store high byte

  // write out PWM pin 11, direct through
  //OCR2A = adcVal>>2;

  F = (float)(adcVal-dcBias)/512.0;  // make input sample float and normalize.

  xdot = gamma*( mu-(sq(x) + sq(y)) )*x - ohm*y + Eps*F;
  ydot = gamma*( mu-(sq(x) + sq(y)) )*y + ohm*x;
  ohmdot = -Eps*F*y/( sqrt(sq(x) + sq(y)) );
  x_new = x + T*xdot;
  y_new = y + T*ydot;
  ohm_new = ohm + T*ohmdot;

  x = x_new;
  y = y_new;
  ohm = ohm_new;

  // write out 8-bit PWM pin 3&11, Hopf-outputs
  OCR2B = round(40*x) + 127; // in-phase output. 40 is a rough scaling.
  OCR2A = round(40*y) + 127; // quadrature output

  // write out 10bit PWM output registers A&B (pin9 and pin10).
  //OCR1AH = adcVal>>8; //MSB
  //OCR1AL = adcVal; //LSB
  //(inverted phase) output:
  //OCR1BH = OCR1AH; //MSB
  //OCR1BL = OCR1AL; //LSB
 
  //digitalWriteFast(PIN_DEBUG, LOW);
}




/* ******************************************************************
*  Timer1 ISR
*********************************************************************/
ISR(TIMER1_OVF_vect) {
//empty ISR. Only used for triggering the ADC.
}
