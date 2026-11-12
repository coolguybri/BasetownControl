#include "Arduino.h"
#include "SeaRobLight.h"
#include "SeaRobLogger.h"
#include "SeaRobSpringButton.h"
#include "SeaRobSpringButtonLight.h"
#include "SeaRob_SSD1306.h"


// Constants: Specific I/O Pins that must be used.
// Assumes the Arduino Mega 3560 R3 Board.

// Constants: LCD Display subsystem
#define PIN_I2C_SDA                 20 // Dedicated SDA Output Pin (mega only)
#define PIN_I2C_SCL                 21 // Dedicated SCL Output Pin (mega only)
#define PIN_DISPLAY_BUTTON_1        31 // Digital Pin, input
#define PIN_DISPLAY_BUTTON_2        32 // Digital Pin, input

// Constants: Windmill Subsystem
#define PIN_WINDMILL_BUTTON_PWR     22 // Digital Pin, input
#define PIN_WINDMILL_BUTTON_DIR     23 // Digital Pin, input

// Constants: Train subsystem
#define PIN_TRAIN_BUTTON_PWR        24 // Digital Pin, input
#define PIN_TRAIN_BUTTON_DIR        25 // Digital Pin, input

// Constants: Lego PowerFunctions Light Array
#define MAX_PF_LIGHTS               5
#define PIN_PF_LIGHT_BUTTON_1       27 // Digital Pin, input 
#define PIN_PF_LIGHT_CTRL_1         40 // Digital Pin, output [40-44]
#define PIN_PF_LIGHT_MODE_SELECTOR  35 // Digital Pin, output [35]

// Constants: 5v USB "brickstuff" street lights and light effects
#define PIN_BRICKSTUFF_STREETLIGHT_BUTTON       26 // Digital Pin, input
#define PIN_BRICKSTUFF_STREETLIGHT_CTRL         46 // Digital Pin, output
#define PIN_BRICKSTUFF_STORM_REDBEAM_BUTTON     28 // Digital Pin, input
#define PIN_BRICKSTUFF_STORM_REDBEAM_CTRL       5  // Digital PWM Pin, output
#define PIN_BRICKSTUFF_STORM_INTERNAL_BUTTON    29 // Digital Pin, input
#define PIN_BRICKSTUFF_STORM_INTERNAL_CTRL      47 // Digital Pin, output

#define PIN_UNMAPPED_FIFTH_BUTTON                30

// Constants: OLED Display subsystem
#define OLED_I2C_ADDRESS            0x3C
#define OLED_DRAW_INTERVAL          40 // 25 frames per second maximum; slower if system clock runs slower

// Constants: Fade time for light effects
#define STORM_RED_DURATION_ON 1000
#define STORM_RED_DURATION_OFF 5000
#define STORM_RED_DURATION_FADE 5000


// Global Variables: Global Run state of the entire aplication.
unsigned long   startTime = 0; 
unsigned long   lastUpdateTime = 0;
String          buildName = String("BasementControl");
const char      buildTimestamp[] =  __DATE__ " " __TIME__;
const char      buildDatestamp[] = __DATE__;


// Globals: OLED Display subsystem, connected via I2C interface
boolean               useDisplay = true; 
unsigned long         lastDrawTime = 0;
SeaRob_SSD1306 *      seaDisplay = NULL;
SeaRobSpringButton *  displayButton1 = NULL;
SeaRobSpringButton *  displayButton2 = NULL;


// Globals: Windmill subsystem.
boolean       useWindmill = true;
boolean       windmillPower = false;
boolean       windmillDirection = true;
int           windmillVelocity = 255;
SeaRobSpringButton *  buttonWindmillPwr = NULL;
SeaRobSpringButton *  buttonWindmillDir = NULL;


// Globals: Train subsystem.
boolean       useTrain = true;
boolean       trainPower = false;
boolean       trainDirection = true;
int           trainVelocity = 200;
SeaRobSpringButton *  buttonTrainPwr = NULL;
SeaRobSpringButton *  buttonTrainDir = NULL;


// Globals: PowerFunctions (PF) Lights (9 volts, turned on via transistor)
boolean                         usePFLight = false;
//SeaRobSpringButtonLightList *   buttonLightList = NULL;


// Globals: 5-volt lights (USB); examples: LightMyBricks, BrickStuff
boolean                     useUSBLight = true;
SeaRobSpringButtonLight *   frontLights = NULL;
SeaRobSpringButtonLight *   stormRedBeamLight = NULL;
SeaRobSpringButtonLight *   stormInternalLight = NULL;
SeaRobSpringButton *        unmappedButton1 = NULL;
SeaRobSpringButton *        unmappedButton2 = NULL;

/*
 * Windmill button callbacks
 */
 
void onButtonDownWindmillPwr(SeaRobSpringButton *button, long updateTime) {
  windmillPower = !windmillPower;
  bclogger("%s: toggled to %d", button->GetName().c_str(), windmillPower);

  if (windmillPower) {
   // motor_set_state(&motorWindmill, windmillDirection ? MotorState_Forward : MotorState_Reverse);
  } else {
    // motor_set_state(&motorWindmill, MotorState_Off);
  }
}

void onButtonDownWindmillDir(SeaRobSpringButton *button, long updateTime) {
  windmillDirection = !windmillDirection;
  bclogger("%s: toggled to %d", button->GetName().c_str(), windmillDirection);

  if (windmillPower) {
    //motor_set_state(&motorWindmill, windmillDirection ? MotorState_Forward : MotorState_Reverse);
  } else {
    // motor_set_state(&motorWindmill, MotorState_Off);
  }
}


/*
 * Train button callbacks
 */
 
void onButtonDownTrainPwr(SeaRobSpringButton *button, long updateTime) {
  trainPower = !trainPower;
  bclogger("%s: toggled to %d", button->GetName().c_str(), trainPower);

  if (trainPower) {
    //motor_set_state(&motorTrain, trainDirection ? MotorState_Forward : MotorState_Reverse);
  } else {
    //motor_set_state(&motorTrain, MotorState_Off);
  }
}

void onButtonDownTrainDir(SeaRobSpringButton *button, long updateTime) {
  trainDirection = !trainDirection;
  bclogger("%s: toggled to %d", button->GetName().c_str(), trainDirection);

  if (trainPower) {
   // motor_set_state(&motorTrain, trainDirection ? MotorState_Forward : MotorState_Reverse);
  } else {
   // motor_set_state(&motorTrain, MotorState_Off);
  }
}

void onButtonDownDisplay1(SeaRobSpringButton *button, long updateTime) {
  bclogger("onButtonDownDisplay1: %s", button->GetName().c_str());
}

void onButtonDownDisplay2(SeaRobSpringButton *button, long updateTime) {
  bclogger("onButtonDownDisplay2: %s", button->GetName().c_str());
}

/*
 * USB light callbacks
 */
    
void onButtonDownLightBasic(SeaRobSpringButtonLight *buttonLight, long updateTime) {
  bclogger("onButtonDownLightBasic: %s", buttonLight->GetName().c_str());
}


/*
 * Entrypoint: called once when the program first starts, just to initialize all the sub-components.
 */
void setup() {  
  
  // Record what time we started.
  startTime = millis();
  
  // Init the serial line; important for debug messages back to the Arduino Serial Monitor.
  Serial.begin(19200);
  bclogger(""); // Skip line to seperate from last instance of the program.
  bclogger("setup: \"%s\"", buildName.c_str());
  bclogger("build: \"%s\"", buildTimestamp);

   // Init the OLED display
  if (useDisplay) {
    bclogger("setup: OLED start...");
    
    seaDisplay = new SeaRob_SSD1306(buildTimestamp, OLED_I2C_ADDRESS);
    // TODO: convert to SeaRobObject with init
    // seaDisplay->Init(startTime);

    // 2 display buttons next to the screen.
    displayButton1 = new SeaRobSpringButton("display-btn-1", PIN_DISPLAY_BUTTON_1, true, &onButtonDownDisplay1);
    displayButton1->Init(startTime);
    displayButton2 = new SeaRobSpringButton("display-btn-2", PIN_DISPLAY_BUTTON_2, true, &onButtonDownDisplay2);
    displayButton2->Init(startTime);
    
    bclogger("setup: OLED complete");
  }

  if (useWindmill) {
    bclogger("setup: windmill start...");
    
    //motor_setup(&motorWindmill, "windmill", PIN_WINDMILL_MOTOR_IN1, PIN_WINDMILL_MOTOR_IN2, PIN_WINDMILL_MOTOR_ENB);
    //windmillVelocity = motor_set_pulsewidth(&motorWindmill, windmillVelocity); // middle value.
    
    buttonWindmillPwr = new SeaRobSpringButton("windmill-power", PIN_WINDMILL_BUTTON_PWR, true, &onButtonDownWindmillPwr);
    buttonWindmillPwr->Init(startTime);
    buttonWindmillDir = new SeaRobSpringButton("windmill-direction", PIN_WINDMILL_BUTTON_DIR, true, &onButtonDownWindmillDir);
    buttonWindmillDir->Init(startTime);

    bclogger("setup: windmill complete, power=%d, dir=%d, speed=%d/255", windmillPower, windmillDirection, windmillVelocity);
  }

  if (useTrain) {
    bclogger("setup: train start...");
    
    //motor_setup(&motorTrain, "train", PIN_TRAIN_MOTOR_IN1, PIN_TRAIN_MOTOR_IN2, PIN_TRAIN_MOTOR_ENB);
   // trainVelocity = motor_set_pulsewidth(&motorTrain, trainVelocity); // middle value.
    
    buttonTrainPwr = new SeaRobSpringButton("train-power", PIN_TRAIN_BUTTON_PWR, true, &onButtonDownTrainPwr);
    buttonTrainPwr->Init(startTime);
    buttonTrainDir = new SeaRobSpringButton("train-direction", PIN_TRAIN_BUTTON_DIR, true, &onButtonDownTrainDir);
    buttonTrainDir->Init(startTime);

    bclogger("setup: train complete, power=%d, dir=%d, speed=%d/255", trainPower, trainDirection, trainVelocity);
  }

  if (usePFLight) {
    bclogger("setup: pf-light starting with maxLights=%d", MAX_PF_LIGHTS);

    int buttonPins[] = { 30, 31, 32, 33, 34 }; // PIN_PF_LIGHT_BUTTON_1
    int lightPins[] = { 40, 41, 44, 43, 42 }; // PIN_PF_LIGHT_CTRL_1 - dealing with swapped physical wiring
    //buttonLightList = new SeaRobSpringButtonLightList(String("rooftop lights"), MAX_PF_LIGHTS, buttonPins, lightPins, PIN_PF_LIGHT_MODE_SELECTOR);
    
    bclogger("setup: pf-light complete");
  }

  if (useUSBLight) {
    bclogger("setup: usb-light starting");

    // Button 1: Row of Front Street Lights; 5v usb circuit driven right from arduino pin.
    frontLights = new SeaRobSpringButtonLight("front-row-lights", 
        PIN_BRICKSTUFF_STREETLIGHT_BUTTON, PIN_BRICKSTUFF_STREETLIGHT_CTRL, false, true, onButtonDownLightBasic);
    frontLights->Init(startTime);
    frontLights->GetLight()->ToggleOnOff(); // Default to on at startup.

    // Button 2: Stormbase Throbbing Red Light; 5v usb circuit driven right from arduino pin.
    stormRedBeamLight = new SeaRobSpringButtonLight("storm-redbeam", 
        PIN_BRICKSTUFF_STORM_REDBEAM_BUTTON, PIN_BRICKSTUFF_STORM_REDBEAM_CTRL, true, true, onButtonDownLightBasic);
    stormRedBeamLight->Init(startTime);
    stormRedBeamLight->GetLight()->UpdateBlinkConfig(0, 0, STORM_RED_DURATION_ON, STORM_RED_DURATION_OFF, 
        false, STORM_RED_DURATION_FADE, STORM_RED_DURATION_FADE);
    stormRedBeamLight->GetLight()->UpdateState(SeaRobLight::LightState::UniformBlink);

    // Button 3: Stormbase Internal Lighting; 5v usb circuit driven right from arduino pin.
    stormInternalLight = new SeaRobSpringButtonLight("storm-internal", 
          PIN_BRICKSTUFF_STORM_INTERNAL_BUTTON, PIN_BRICKSTUFF_STORM_INTERNAL_CTRL, false, true, onButtonDownLightBasic);
    stormInternalLight->Init(startTime);

    bclogger("setup: usb-light complete");
  }

  // Init the rest of our internal state.
  bclogger("setup: complete for \"%s\"", buildName.c_str());
}


/**
 * Main Loop: called over and over again as the robot runs.
 * Disopatches this event to all the subsystems.
 */
void loop() {

  // Get the current time.
  lastUpdateTime = millis();

  if (useWindmill) {
      // Process inputs first so they have immediate impact.
      if (buttonWindmillPwr) buttonWindmillPwr->ProcessLoop(lastUpdateTime);
      if (buttonWindmillDir) buttonWindmillDir->ProcessLoop(lastUpdateTime);
    
      // Increment the rest of the state machines.
      //motor_loop(&motorWindmill, lastUpdateTime);
  }

  if (useTrain) {
      // Process inputs first so they have immediate impact.
      if (buttonTrainPwr) buttonTrainPwr->ProcessLoop(lastUpdateTime);
      if (buttonTrainDir) buttonTrainDir->ProcessLoop(lastUpdateTime);

      // Increment the rest of the state machines.
      //motor_loop(&motorTrain, lastUpdateTime);
  }

  if (usePFLight) {
    //buttonLightList->ProcessLoop(lastUpdateTime);
  }

  if (useUSBLight) {
    if (frontLights) frontLights->ProcessLoop(lastUpdateTime);
    if (stormRedBeamLight) stormRedBeamLight->ProcessLoop(lastUpdateTime);
    if (stormInternalLight) stormInternalLight->ProcessLoop(lastUpdateTime);
  }

  if (useDisplay) {

    if (displayButton1) displayButton1->ProcessLoop(lastUpdateTime);
    if (displayButton2) displayButton2->ProcessLoop(lastUpdateTime);
    
      // Update the OLED screen with our current state.

      /*#define LINE_BUFFER_SIZE 50
      char headerBuffer[LINE_BUFFER_SIZE];
      snprintf(headerBuffer, LINE_BUFFER_SIZE, "coolguybri cntrl 3.5");
      
      // Format the Uptime.
      unsigned long upSecs = (lastUpdateTime - startTime) / 1000;
      char line1Buffer[LINE_BUFFER_SIZE];
      snprintf(line1Buffer, LINE_BUFFER_SIZE, "%09lu %s", upSecs, buildDatestamp);

       // Train monitoring
      char line2Buffer[LINE_BUFFER_SIZE];
      snprintf(line2Buffer, LINE_BUFFER_SIZE, "t [%c] %s %d%%", 
          trainPower ? '*' : ' ', trainDirection ? "<-" : "->", (trainVelocity * 100) / 255);
          
      // Windmill monitoring.
      char line3Buffer[LINE_BUFFER_SIZE];
      snprintf(line3Buffer, LINE_BUFFER_SIZE, "w [%c] %s %d%%", 
          windmillPower ? '*' : ' ', windmillDirection ? "<-" : "->", (windmillVelocity * 100) / 255);

      // PF-Light monitoring
      char line4Buffer[LINE_BUFFER_SIZE];
      strcpy(line4Buffer, "l ");
      int litstrlen = strlen(line4Buffer);
     // buttonLightList->GetStatusString(line4Buffer + litstrlen, LINE_BUFFER_SIZE - litstrlen);
      litstrlen = strlen(line4Buffer); */
      
      // USB-Light monitoring
     /* snprintf(line4Buffer + litstrlen, LINE_BUFFER_SIZE - litstrlen, " [%c%c%c%c]", 
        frontLights->IsOn() ? '*' : 'o',
        stormRedBeamLight->IsOn() ? '*' : 'o',
        stormInternalLight->IsOn() ? '*' : 'o',
        false ? '*' : 'o'); // TODO: add new button */

      if ((lastDrawTime + OLED_DRAW_INTERVAL) < lastUpdateTime) {  // Periodically draw to screen. 
        lastDrawTime = lastUpdateTime;
        Adafruit_SSD1306 &display = seaDisplay->getDisplayRef();
       
        // Reset the internal pixel-buffer, lets draw the entire screen.
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
   
        // Title with Fun little animation to prove that we are not locked up.
        seaDisplay->drawTitle("coolguybri basetown", 8, 1);
        seaDisplay->drawCircleAnimation(lastUpdateTime, 123, 4, 0, true);
        int linePos = 11;
        display.drawLine(0, linePos, 128, linePos, SSD1306_WHITE); 

      /*#define LINE_BUFFER_SIZE 50
      int colOffset = 0;
      
      // Power Functions Port 1
      if (usePowerSlot1) {
        display.setTextSize(2);
        display.setCursor(colOffset, 18);
        char l1Buffer[10];
        snprintf(l1Buffer, LINE_BUFFER_SIZE, "%4d", (int) sensorSample1.current_mA);
        display.println(l1Buffer);
        display.setTextSize(1);
        display.setCursor(colOffset + 48, 25);
        display.println(F("mA"));
      
        display.setTextSize(2);
        display.setCursor(colOffset, 37);
        char l2Buffer[10];
        snprintf(l2Buffer, LINE_BUFFER_SIZE, "%4d", (int) (sensorSample1.busvoltage * 1000.0f));
        display.println(l2Buffer);
        display.setTextSize(1);
        display.setCursor(colOffset + 48, 45);
        display.println(F("mV"));
      }

      if (usePowerSlot1) {
        display.setTextSize(1);
        display.setCursor(colOffset + 6, 56);
        char lYoBuffer[20];
        snprintf(lYoBuffer, LINE_BUFFER_SIZE, "pf-1: %d", (int) (currentPowerLevel1 * 100.0));
        display.println(lYoBuffer);
      }
    
      display.drawLine(63, 12, 63, 64, SSD1306_WHITE); // middle vertical line
    
      colOffset = 68;
      
      // Power Functions Port 2
      if (usePowerSlot2) {
        display.setTextSize(2);
        display.setCursor(colOffset, 18);
        char l3Buffer[10];
        snprintf(l3Buffer, LINE_BUFFER_SIZE, "%4d", (int) sensorSample2.current_mA);
        display.println(l3Buffer);
        display.setTextSize(1);
        display.setCursor(colOffset + 48, 25);
        display.println(F("mA"));

        display.setTextSize(2);
        display.setCursor(colOffset, 37);
        char l4Buffer[10];
        snprintf(l4Buffer, LINE_BUFFER_SIZE, "%4d", (int) (sensorSample2.busvoltage * 1000.0f));
        display.println(l4Buffer);
        display.setTextSize(1);
        display.setCursor(colOffset + 48, 45);
        display.println(F("mV")); 
      }

      if (usePowerSlot2) {
        display.setTextSize(1);
        display.setCursor(colOffset + 6, 56);
        char lYoBuffer[20];
        snprintf(lYoBuffer, LINE_BUFFER_SIZE, "pf-2: %d", (int) (currentPowerLevel2 * 100.0));
        display.println(lYoBuffer);
      } */
      
        // Push the buffer to the physical OLED
        display.display();
      }
  }
}
