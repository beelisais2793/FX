/*
* File: adaptiveFreqOsc1.ino
* Purpose: Adaptive Frequency Oscillator Demo
* Version: 1.0.1
* Date: 18-12-2018
* Created by: Martin Stokroos
* URL: https://github.com/MartinStokroos/adaptiveFreqOsc
* License: MIT License
*
* This adaptive Hopf frequency oscillator synchronizes to the periodic input signal applied to the analog
* input pin, defined with 'AI_PIN'. The periodic input signal should be biased on 2.5V DC for the ADC and
* the amplitude of the drive signal should be limited to 2.5V. Without drive signal (but with DC-bias), the
* oscillator starts outputting the intrinsic frequency, preprogrammed with the variable 'ohm'.
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

#include "Arduino.h"

#define LED_PIN 13
#define PWM_OUT 3 // define PWM output pin 3 or pin 11.
#define AI_PIN A0 // analog input pin

#define LPERIOD 1000L // loop period time in us.
#define T LPERIOD/1000000.0 // loop period time T=1ms.
unsigned long nextLoop;

// adaptive Hopf oscillator parameters
float Eps = 15; // coupling strength (K>0).
           	    // Oscillator locks faster at higher values, but the ripple of the frequency is larger.
float gamma = 100.0; // the oscillator follows the amplitude of the stimulus better for larger values for g.
          	  	// May causes instability at large values for gamma.
float mu = 1.0; // Seems to scale the output amplitude. May Cause instability at large values for mu.

float x = 1;  // starting point of the oscillator. x=1, y=0 starts as a cosine.
float x_new = 0;
float x_d = 0;
float y = 0;
float y_new = 0;
float y_d = 0;
float ohm = 2*PI; //intrinsic frequency = 1Hz
float ohm_new = 0;
float ohm_d = 0;
float F=0;

int adcVal;
int dcBias = 512; //dc-bias byte value
char c; // incoming character from console

//The setup function is called once at startup of the sketch
void setup()
{
	pinMode(LED_PIN, OUTPUT); // for checking loop period time and loop execution time (signal high time)
	pinMode(AI_PIN, INPUT); // analog input pin for the (periodic) input signal.
							// The input voltage must be biased on 2.5VDC.
	Serial.begin(115200);

	TCCR2B = (TCCR2B & 0xF8)|0x01; // set Pin3 + Pin11 PWM frequency to 31250Hz
	nextLoop = micros() + LPERIOD; // set the loop timer variable.
}


// The loop function is called in an endless loop
void loop()
{
digitalWrite(LED_PIN, true);

adcVal = analogRead(AI_PIN) - dcBias;
F = (float)adcVal/512;	// make input sample float and normalize.

x_d = gamma*( mu-(sq(x) + sq(y)) )*x - ohm*y + Eps*F;
y_d = gamma*( mu-(sq(x) + sq(y)) )*y + ohm*x;
ohm_d = -Eps * F * y/( sqrt(sq(x) + sq(y)) );
x_new = x + T*x_d;
y_new = y + T*y_d;
ohm_new = ohm + T*ohm_d;

x = x_new;
y = y_new;
ohm = ohm_new;

analogWrite(PWM_OUT, round(100*x) + 127); // add 127 to convert to unsigned.

if(Serial.available()){
	c = Serial.read();
    if(c == 'q') {	// use the 'q' and 'a' keys to set Eps
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
    	Serial.println(ohm/(2*PI),1);
    }
}

digitalWrite(LED_PIN, false); // execution time, about 400us.

while(nextLoop > micros());  // wait until the end of the time interval
	nextLoop += LPERIOD;  // set next loop time at current time + LOOP_PERIOD
}

