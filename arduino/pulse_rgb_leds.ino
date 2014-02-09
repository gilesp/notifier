int redPin = 9;
int bluePin = 10;
int greenPin = 11;
int brightness = 0;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
}

void loop() {
  setColour(brightness, brightness, brightness);
  delay(10);
  brightness++;
  if(brightness > 255) {
    brightness = 0;
  }
}

void setColour(int r, int g, int b) {
  analogWrite(redPin, checkColour(r));
  analogWrite(greenPin, checkColour(g));
  analogWrite(bluePin, checkColour(b));  
}

int checkColour(int value) {
  if(value < 0){
    value = 0;
  } else if (value > 255) {
    value = 255;
  }
  return value;  
}
