#include <EnableInterrupt.h>

#define SERIAL_PORT_SPEED 9600
#define RC_NUM_CHANNELS  4

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3

#define RC_CH1_PIN  8
#define RC_CH2_PIN  7
#define RC_CH3_PIN  4
#define RC_CH4_PIN  2

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

int motor1pin1 = 5;
int motor1pin2 = 6;
int motor2pin1 = 10;
int motor2pin2 = 9;
int speed1 = 0;
int speed2 = 0;
int speed3 = 0;
int left = 1;
int degree = 0;
void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() {
  calc_input(RC_CH1, RC_CH1_PIN);
}
void calc_ch2() {
  calc_input(RC_CH2, RC_CH2_PIN);
}
void calc_ch3() {
  calc_input(RC_CH3, RC_CH3_PIN);
}
void calc_ch4() {
  calc_input(RC_CH4, RC_CH4_PIN);
}

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RC_CH1_PIN, INPUT);
  pinMode(RC_CH2_PIN, INPUT);
  pinMode(RC_CH3_PIN, INPUT);
  pinMode(RC_CH4_PIN, INPUT);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  enableInterrupt(RC_CH1_PIN, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_PIN, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_PIN, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_PIN, calc_ch4, CHANGE);
}

void loop() {
  rc_read_values();
  
//  Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
//  Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
//  Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
//  Serial.print("CH4:"); Serial.println(rc_values[RC_CH4]);
  if (rc_values[RC_CH2] <= 1504){
  speed1 = map(rc_values[RC_CH2],972, 1504, 100,0);
  }
  else{
    speed1 = map(rc_values[RC_CH2],1506, 2040, 0,100);
  }
  if (rc_values[RC_CH4] > 1000){
  if (rc_values[RC_CH1] > 1516){
    left = 1;
    degree = map(rc_values[RC_CH1], 1512,2048,0,70);
    speed2 = speed1 + degree;
  }
  else if (rc_values[RC_CH1] < 1512){
    left = 2;
    degree = map(rc_values[RC_CH1], 980,1512,70,0);
    speed2 = speed1 - degree;
    speed3 = speed2;
    speed2 = speed1;
    speed1 = speed3;
    }
  else{
    left = 0;
    speed2 = speed1;
  }
  digitalWrite(motor1pin1, speed1);
  analogWrite(motor1pin2, LOW);
   digitalWrite(motor2pin1, speed2);
  analogWrite(motor2pin2, LOW);
  }
  else{
    if (rc_values[RC_CH1] > 1516){
    left = 1;
    degree = map(rc_values[RC_CH1], 1512,2048,0,70);
    speed2 = speed1 + degree;
  }
  else if (rc_values[RC_CH1] < 1512){
    left = 2;
    degree = map(rc_values[RC_CH1], 980,1512,70,0);
    speed2 = speed1 - degree;
    speed3 = speed2;
    speed2 = speed1;
    speed1 = speed3;
    }
  else{
    left = 0;
    speed2 = speed1;
  }
    digitalWrite(motor1pin1, LOW);
  analogWrite(motor1pin2, speed1*2);
   digitalWrite(motor2pin1, LOW);
  analogWrite(motor2pin2, speed2*2);
  }
  Serial.print("speed1:");Serial.print(speed1);
  Serial.println();
  Serial.print("speed2:");Serial.print(speed2);
  delay(200);
}
