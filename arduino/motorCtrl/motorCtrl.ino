int encoderCLK = D5;
int encoderDT = D6;
int encoderSW = D7;

int motorPWM = D1;
int motorDIR = D3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

bool running = false;

//long readEncoderValue(void) {
//  return encoderValue / 4;
//}

boolean isButtonPushDown(void) {
  if (!digitalRead(encoderSW)) {
    delay(5);
    if (!digitalRead(encoderSW))
      return true;
  }
  return false;
}

void setup() {
  Serial.begin (9600);

  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  pinMode(encoderSW, INPUT);

  pinMode(motorPWM, OUTPUT);
  pinMode(motorDIR, OUTPUT);
  digitalWrite(motorDIR, HIGH);

  digitalWrite(encoderCLK, HIGH); //turn pullup resistor on
  digitalWrite(encoderDT, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(encoderCLK, updateEncoder, CHANGE);
  attachInterrupt(encoderDT, updateEncoder, CHANGE);

}

void loop() {

  if (lastencoderValue != encoderValue) {
    Serial.print("Speed ");
    Serial.print(encoderValue);
    digitalWrite(motorPWM, encoderValue);
    lastencoderValue = encoderValue;
  }



  if (isButtonPushDown()) {
    if (running) {
      Serial.println("Stop");
      running = false;
      digitalWrite(motorPWM, 0);
    } else {
      Serial.println("Start");
      running = true;
      digitalWrite(motorPWM, lastencoderValue);
    }
  }

  delay(10);
}


ICACHE_RAM_ATTR void updateEncoder() {
  int MSB = digitalRead(encoderCLK); //MSB = most significant bit
  int LSB = digitalRead(encoderDT); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  encoderValue = constrain(encoderValue, 0, 255);
  lastEncoded = encoded; //store this value for next time
}
