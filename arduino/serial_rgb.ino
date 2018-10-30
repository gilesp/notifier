/* Define which pins to use */
const int redPin = 9;
const int bluePin = 10;
const int greenPin = 11;

/* common variables */
const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;
unsigned long time;
char command = 's';
int colour[3] = {0,0,0};

/* Command parsing variables */
int parseState = 0;
int inputCommand = '\0';
int inputColour[3] = {0,0,0};

/* Pulse variables */
const int PULSE_PERIOD = 2500; //pulse cycle in milliseconds
//float rMultiplier = 0, gMultiplier = 0, bMultiplier = 0;

/* Flash variables */
const int FLASH_PERIOD = 500;
bool ledsOn = false;
unsigned long previousMillis = 0;

//common anode leds need the values set to the inverse of common cathode 
//hence the 255-value
void setColour(int r, int g, int b) {
  analogWrite(redPin, 255-r);
  analogWrite(greenPin, 255-g);
  analogWrite(bluePin, 255-b);
}

void stepPulse() {
  float halfRed = colour[RED]/2;
  float halfGreen = colour[GREEN]/2;
  float halfBlue = colour[BLUE]/2;
  float multiplier = cos(2*PI/PULSE_PERIOD*time);

  setColour(halfRed + (halfRed*multiplier),
            halfGreen + (halfGreen*multiplier),
            halfBlue + (halfBlue*multiplier));
}

void stepFlash() {
  if(time - previousMillis > FLASH_PERIOD) {
    previousMillis = time;
    if(!ledsOn) {
      setColour(colour[RED], colour[GREEN], colour[BLUE]);
      ledsOn = true;
    } 
    else {
      setColour(0, 0, 0);
      ledsOn = false;
    }
  }
}

void setup() {
  //initialise serial
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
}

void loop() {
  if(command == 's') {
      setColour(colour[RED], colour[GREEN], colour[BLUE]);
      command = '\0';
  } else if(command == 'p') {
    stepPulse();
  } else if(command == 'f') {
    stepFlash(); 
  }

  time = millis();
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    parseChar(inChar);
  }
}

/* Input parsing states:
 *  -1: invalid input seen (discard rest of line)
 *   0: looking for command
 *   1: looking for start of RED
 *   2: parsing RED, looking for comma
 *   3: looking for start of GREEN
 *   4: parsing GREEN, looking for comma
 *   5: looking for start of BLUE
 *   6: parsing BLUE, looking for newline
 *   7: valid input seen
 * Valid input matches (with numeric values less than 256):
 *   ^[spf][0-9]+,[0-9]+,[0-9]+$
 */

void parseChar(char inChar) {
    parseState = nextState(parseState, inChar);
    int colidx = parseState == 2 ? RED : parseState == 4 ? GREEN : parseState == 6 ? BLUE : -1;
    switch (parseState) {
      case 0:
        Serial.println("invalid");
      case 1:
        inputCommand = inChar;
        inputColour[RED] = 0;
        inputColour[GREEN] = 0;
        inputColour[BLUE] = 0;
        break;
      case 2: case 4: case 6:
        inputColour[colidx] = inputColour[colidx] * 10 + (inChar - '0');
        if(inputColour[colidx] > 255) {
            parseState = -1;
        }
        break;
      case 7:
        command = inputCommand;
        colour[RED] = inputColour[RED];
        colour[GREEN] = inputColour[GREEN];
        colour[BLUE] = inputColour[BLUE];
        parseState = 0;
        showCommand();
        break;
    }
}

int nextState(int state, char input) {
  switch (state) {
    case -1:
      if(input == '\r' || input == '\n') {
        return 0;
      }
      break;
    case 0:
      if(input == '\r' || input == '\n') {
        return 0;
      }
      if(input == 's' || input == 'p' || input == 'f') {
        return 1;
      }
      return -1;
    case 1: case 3: case 5:
      if(isDigit(input)) {
        return state + 1;
      }
      return -1;
    case 2: case 4: case 6:
      if(isDigit(input)) {
        return state;
      }
      if(state != 6) {
        if(input == ',') {
          return state + 1;
        }
      } else {
        if(input == '\r' || input == '\n') {
          return 7;
        }
      }
      return -1;
    default:
      return -1;
  }
}

void showCommand() {
  Serial.print(command == 's' ? "static(" : command == 'p' ? "pulse(" : "flash(");
  Serial.print(colour[RED]);
  Serial.print(",");
  Serial.print(colour[GREEN]);
  Serial.print(",");
  Serial.print(colour[BLUE]);
  Serial.println(")");
}
