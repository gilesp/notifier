/* Define which pins to use */
const int redPin = 9;
const int bluePin = 10;
const int greenPin = 11;

/* common variables */
const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;
unsigned long time;
int colour[3] = {0,0,0};

/* Command parsing variables */
char command = 's';
String inputString = "";
boolean parseInput = false;

/* Pulse variables */
const int PULSE_PERIOD = 2500; //pulse cycle in milliseconds
//float rMultiplier = 0, gMultiplier = 0, bMultiplier = 0;

/* Flash variables */
const int FLASH_PERIOD = 500;
bool ledsOn = false;
unsigned long previousMillis = 0;

void setup() {
  //initialise serial
  Serial.begin(9600);
  // reserve 200 bytes for the input string
  inputString.reserve(200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  setColour(colour[RED],colour[GREEN],colour[BLUE]);
}

void loop() {
  if(parseInput) {
    //get command
    command = inputString.charAt(0);
    parseColour(colour, inputString.substring(1, inputString.length()));
    
    if(command == 's') {
      Serial.print("static ");
      setColour(colour[RED], colour[GREEN], colour[BLUE]);
    } else if (command == 'p') {
      Serial.print("pulse ");
    } else if (command == 'f') {
      Serial.print("flash ");
    } else {
      Serial.print("unknown ");
    }

    // print the three numbers in one string as hexadecimal:
    Serial.print(colour[RED], HEX);
    Serial.print(colour[GREEN], HEX);
    Serial.println(colour[BLUE], HEX);

    //clear input
    inputString = "";
    parseInput = false;
  }

  if(command == 'p'){    
    stepPulse();
  } else if (command == 'f') {
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
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      parseInput = true;
    } 
  }
}

/* Expects a string of the form x,x,x  where x is a number from 0 to 255 */
void parseColour(int colour[], String colourString) {
  int commaPosition = -1;

  if(colourString.length() >= 5){
    for(int i = 0; i < 3; i++){
      commaPosition = colourString.indexOf(',');
      if(commaPosition > 0){
        colour[i] = colourString.substring(0, commaPosition).toInt();
        colourString = colourString.substring(commaPosition+1);
      } else {
        colour[i] = colourString.toInt();
      }
    }
  }
}

//common anode leds need the values set to the inverse of common cathode 
//hence the 255-value
void setColour(int r, int g, int b) {
  analogWrite(redPin, checkColour(255-r));
  analogWrite(greenPin, checkColour(255-g));
  analogWrite(bluePin, checkColour(255-b));  
}

int checkColour(int value) {
  return constrain(value, 0, 255);
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