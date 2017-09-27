
#define DEBUG (true)

void setup() {

// Setup Serial


Serial.begin(57600);
Serial.println(F("INFO: Starting - Hello and welcome!"));


// Setup Outputs

digitalWrite(PumpPin, HIGH);  //OFF - Logic is inverted on relay board.
pinMode(PumpPin, OUTPUT);

digitalWrite(BoilerPin, HIGH);
pinMode(BoilerPin, OUTPUT);

//Set up the system defaults
SystemState = SYSTEM_OFF;


}

