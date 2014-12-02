#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"

// Define pins for the lamps
int GREEN = 13;
int RED   = 12;
int BL    = 11;
int BR    = 10;
int TL    =  9;
int TR    =  8;

// Define pins for the turns
int TURN_R = 2;
int TURN_L = 3;
int TURN_TIME = 2000;
int turn_start = 0;
int turning_r = false;
int turning_l = false;

// Blink states
bool g_blink               = false;
bool g_on                  = false;
unsigned int g_blink_start = 0;
bool r_blink               = false;
bool r_on                  = false;
unsigned int r_blink_start = 0;
unsigned int blink_length  = 1000;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  for(int pin = 0; pin < 14; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  Serial.begin(9600);
}

void greenSolid() {
  setNone();
  digitalWrite(GREEN, LOW);
}

void greenBlink() {
  if(!g_blink) {
    setNone();
    g_blink = true;
    g_blink_start = millis();
    g_on = true;
    digitalWrite(GREEN,LOW);
  }
}

void redBlink() {
  if(!r_blink) {
    setNone();
    r_blink = true;
    r_blink_start = millis();
    r_on = true;
    digitalWrite(RED,LOW);
  }
}

void redSolid() {
  setNone();
  digitalWrite(RED, LOW);
}

void denied() {
  setNone();
  digitalWrite(BL, LOW);
  digitalWrite(BR, LOW);
}

void allowed() {
  setNone();
  digitalWrite(BL, LOW);
  digitalWrite(TL, LOW);
}

void carefull() {
  setNone();
  digitalWrite(BL, LOW);
  digitalWrite(TR, LOW);
}

void cancel() {
  setNone();
  digitalWrite(BR, LOW);
  digitalWrite(TL, LOW);
}

void setNone() {
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(TL, HIGH);
  digitalWrite(TR, HIGH);
  digitalWrite(BL, HIGH);
  digitalWrite(BR, HIGH);
  g_blink = false;
  g_on = false;
  r_blink = false;
  r_on = false;
}

void turnRight() {
  if(!turning_r && !turning_l) {
    turning_r = true;
    turn_start = millis();
    digitalWrite(TURN_R, LOW);
  }
}

void turnLeft() {
  if(!turning_l && !turning_r) {
    turning_l = true;
    turn_start = millis();
    digitalWrite(TURN_L, LOW);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  work();
  communicate();
}

void work() {
  // Control the right blink
  if(g_blink) {
    if(g_blink_start + blink_length < millis()) {
      digitalWrite(GREEN, (g_on ? LOW : HIGH));
      g_on = (g_on ? false : true);
      g_blink_start = millis();
    }
  }

  // Control the left blink
  if(r_blink) {
    if(r_blink_start + blink_length < millis()) {
      digitalWrite(RED, (r_on ? LOW : HIGH));
      r_on = (r_on ? false : true);
      r_blink_start = millis();
    }
  }

  // Control turning right
  if(turning_r) {
    if(turn_start + TURN_TIME < millis()) {
      digitalWrite(TURN_R, HIGH);
      turning_r = false;
    }
  }

  // Control turning left
  if(turning_l) {
    if(turn_start + TURN_TIME < millis()) {
      digitalWrite(TURN_L, HIGH);
      turning_l = false;
    }
  }

}

void communicate() {
  // Find out if we need to read the serial port
  if(Serial.available() > 1) {
    char serial_data[2];
    Serial.readBytes(serial_data,2);
    int address = (serial_data[0] << 1) | (( serial_data[1] >> 7 ) & 1);
    int logic_address = (serial_data[1] >> 4) & 15;
    int instruction = serial_data[1] & 15;

    switch(instruction) {
      case 1:
        // Signal denied
        denied();
        break;
      case 2:
        // Signal allowed
        allowed();
        break;
      case 3:
        // Signal cancel
        cancel();
        break;
      case 4:
        // Signal carefull
        carefull();
        break;
      case 5:
        // Signal greenSolid (go)
        greenSolid();
        break;
      case 6:
        // Signal redSolid (stop)
        redSolid();
        break;
      case 7:
        // Signal greenBlink (go go go)
        greenBlink();
        break;
      case 8:
        // Signal redBlink (stop and go)
        redBlink();
        break;
      case 0:
        byte wb[2] = {192,0};
        Serial.write(wb,2);
        break;
    }
  }
}
