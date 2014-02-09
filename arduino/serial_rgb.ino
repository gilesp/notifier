const int redPin = 9;
const int bluePin = 10;
const int greenPin = 11;

/* common variables */
unsigned long time;

/* Command parsing variables */
int commaPosition;
char command = 's';
String inputString = "";
boolean stringComplete = false;

int redValue = 0, greenValue = 0, blueValue = 0;

/* Pulse variables */
const int PULSE_PERIOD = 2500; //pulse cycle in milliseconds
float rMultiplier = 0, gMultiplier = 0, bMultiplier = 0;

/* Flash variables */
bool ledsOn = false;
unsigned long previousMillis = 0;
const int FLASH_PERIOD = 500;

void setup() {
  //initialise serial
  Serial.begin(9600);
  // reserve 200 bytes for the input string
  inputString.reserve(200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  setColour(0,0,0);
}

void loop() {
  if(stringComplete) {
    //parse values

    command = inputString.charAt(0);
    inputString = inputString.substring(1, inputString.length());

    //extract rgb values
    commaPosition = inputString.indexOf(',');
    if(commaPosition != -1) {
      redValue = inputString.substring(0, commaPosition).toInt();
      inputString = inputString.substring(commaPosition+1, inputString.length());

    } 
    else {
      redValue = 0;
      greenValue = 0;
      blueValue = 0;
    }

    commaPosition = inputString.indexOf(',');
    if(commaPosition != -1) {
      greenValue = inputString.substring(0, commaPosition).toInt();
      inputString = inputString.substring(commaPosition+1, inputString.length());
      blueValue = inputString.toInt();
    } 
    else {
      redValue = 0;
      greenValue = 0;
      blueValue = 0;
    }

    //set colour
    if(command == 's') {
      Serial.print("static ");
      setColour(redValue, greenValue, blueValue);
    } 
    else if (command == 'p') {
      Serial.print("pulse ");
      initPulse(redValue, greenValue, blueValue);
    } 
    else if (command == 'f') {
      Serial.print("flash ");
      //initFlash(redValue, greenValue, blueValue);
    } 
    else {
      Serial.print("unknown ");
    }

    // print the three numbers in one string as hexadecimal:
    Serial.print(redValue, HEX);
    Serial.print(greenValue, HEX);
    Serial.println(blueValue, HEX);

    //clear input
    inputString = "";
    stringComplete = false;
  }

  if(command == 'p'){
    //pulse
    stepPulse();
  } 
  else if (command == 'f') {
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
      stringComplete = true;
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

void stepPulse() {
  setColour(rMultiplier+rMultiplier*cos(2*PI/PULSE_PERIOD*time),
  gMultiplier+gMultiplier*cos(2*PI/PULSE_PERIOD*time),
  bMultiplier+bMultiplier*cos(2*PI/PULSE_PERIOD*time));
}

void stepFlash() {
  if(time - previousMillis > FLASH_PERIOD) {
    previousMillis = time;
    if(!ledsOn) {
      setColour(redValue, greenValue, blueValue);
      ledsOn = true;
    } 
    else {
      setColour(0, 0, 0);
      ledsOn = false;
    }
  }
}

void initPulse(int r, int g, int b) {
  rMultiplier = r/2;
  gMultiplier = g/2;
  bMultiplier = b/2;
}

int checkColour(int value) {
  if(value < 0){
    value = 0;
  } 
  else if (value > 255) {
    value = 255;
  }
  return value;  
}

