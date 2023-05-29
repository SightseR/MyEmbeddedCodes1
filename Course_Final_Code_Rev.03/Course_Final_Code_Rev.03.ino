


//Include necessary libraries
#include <Ethernet.h>                                 
#include <PubSubClient.h>                                  
#include <TimerOne.h> 
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>

// Set up the LCD display
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

// Define keypad matrix size
const byte ROWS = 4;
const byte COLS = 4;

// Variables to hold button states and wind direction
volatile boolean buttonPressedA = false;
volatile boolean buttonPressedB = false;
volatile boolean buttonPressedC = false;
String windDirection;
int buttonState = 0;
int previousButtonState = 0;
bool displayDirection;
bool displaySpeed;
bool displayBoth;
int degree_Value;

// Variables for wind speed calculation
volatile unsigned long pulseCount;
volatile unsigned long windFrequency;
float windSpeed;
unsigned long i_time;

const int signalInputPin = 2;

// Define the keypad matrix
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Define the pins for the keypad
byte rowPins[ROWS] = { 42, 43, 44, 45 };
byte colPins[COLS] = { 46, 47, 48, 49 };

// Create the keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Ethernet client object
EthernetClient ethClient;                               

// Ethernet MAC address
static uint8_t mymac[6] = { 0x44,0x76,0x58,0x10,0x00,0x62 };

// MQTT server and device information
unsigned int Port = 1883;                          
byte server[] = { 10,6,0,21 };                    
char* deviceId     = "esdevice23";                   
char* clientId     = "escg223";                      
char* deviceSecret = "tamk";                        

// MQTT callback function prototype
void callback(char* topic, byte* payload, unsigned int length);   

// MQTT client object
PubSubClient client(server, Port, callback, ethClient);

// MQTT topics
#define inTopic    "ICT4_in_2020"     
#define outTopic   "ICT4_out_2020"    

void setup() {
    // Initialize serial communication
    Serial.begin(9600);                               
    Serial.println("Start 19.3.2021");      
    delay(500);
    fetch_IP(); // Obtain IP address
    Connect_MQTT_server(); // Connect to MQTT server

    // Initialize the LCD display
    lcd.begin(16, 2);

    // Set up the wind speed sensor
    pinMode(A2, INPUT);
    pulseCount = 0;
    windFrequency = 0;
    windSpeed = 0.0;
    i_time = 0;

    // Attach interrupt for wind speed sensor
    attachInterrupt(digitalPinToInterrupt(signalInputPin), pulseCountFunction, RISING);

    // Set up the timer interrupt
    Timer1.initialize(500000);
    Timer1.attachInterrupt(timerInterruptFunction);
}

void loop(){
    // Read the keypad input
    char key = keypad.getKey();
    if(key == '1'){
        buttonPressedA = true;
        buttonPressedB = false;
        buttonPressedC = false;
    } else if(key =='2'){
        buttonPressedA = false;
        buttonPressedB = true;
        buttonPressedC = false;
    } else if(key == '3'){
        buttonPressedA = false;
        buttonPressedB = false;
        buttonPressedC = true;
    } else {
        buttonPressedA = false;
        buttonPressedB = false;
        buttonPressedC = false;
    }

    // Set display flags based on button states
    if(buttonPressedA == true){
        displayDirection = true;
        displaySpeed = false;
        displayBoth = false;
    } else if(buttonPressedB == true){
        displayDirection = false;
        displaySpeed = true;
        displayBoth = false;
    } else if(buttonPressedC == true){
        displayDirection = false;
        displaySpeed = false;
        displayBoth = true;
    }

    // Print options on the LCD display
    lcd.setCursor(20, 1);
    lcd.print("1-Dir 2-Spd 3-Both");

    // Read wind direction from the sensor and calculate the degree value
    float analog_Value = analogRead(A2);
    float voltage_Value = 5 * analog_Value / 1023;

    if (voltage_Value < 1.2) {
        degree_Value = 0;
    } else{
        degree_Value = ((voltage_Value - 1.2) / 3.8) * 360;
    }

    // Display wind direction, speed, or both depending on the button pressed
    if (displayDirection == true) {
      lcd.setCursor(0, 0);
        if (voltage_Value <= 1.19) {
            windDirection = "Wait....";
        } else if (degree_Value >= 337.5 || (degree_Value > -1 && degree_Value <= 22.4)) {
            windDirection = "North";
        } else if (degree_Value > 22.4 && degree_Value <= 67.5) {
            windDirection = "North East";
        } else if (degree_Value > 67.5 && degree_Value <= 112.5) {
            windDirection = "East";
        } else if (degree_Value > 112.5 && degree_Value <= 157.5) {
            windDirection = "South East";
        } else if (degree_Value > 157.5 && degree_Value <= 202.5) {
            windDirection = "South";
        } else if (degree_Value > 202.5 && degree_Value <= 247.5) {
            windDirection = "South West";
        } else if (degree_Value > 247.5 && degree_Value <= 292.5) {
            windDirection = "West";
        } else {
            windDirection = "North West";
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Dir : ");
        lcd.print(windDirection);
        lcd.setCursor(0, 1);
        lcd.print(degree_Value);
        lcd.print(" degrees");
        lcd.setCursor(20, 1);
        lcd.print("1-Dir 2-Spd 3-Both");
    } else if (displaySpeed == true){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wind Speed: ");
        lcd.setCursor(0, 1);
        lcd.print("Freq: ");
        lcd.setCursor(7, 1);
        lcd.print(windFrequency);
        lcd.setCursor(13, 1);
        lcd.print("Hz");
        lcd.setCursor(13, 0);
        lcd.print(windSpeed);
        lcd.setCursor(20, 1);
        lcd.print("1-Dir 2-Spd 3-Both");
    } else if (displayBoth == true){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Dir : ");
        lcd.print(degree_Value);
        lcd.setCursor(0, 1);
        lcd.print("Wind Speed: ");
        lcd.setCursor(13, 1);
        lcd.print(windSpeed);
        lcd.setCursor(20, 1);
        lcd.print("1-Dir 2-Spd 3-Both");
    }

    // Send MQTT message if the client is connected
    if (client.connected()){
        const char* windDirections = windDirection.c_str();
      send_MQTT_message(degree_Value, windSpeed);
    } else {
        Serial.println("No, re-connecting" );
        client.connect(clientId, deviceId, deviceSecret);
    }

    // Add a delay before the next loop iteration
    delay(1000);
  }

  // Fetch IP function - gets the IP from DHCP server and prints the W5100 revision
  void fetch_IP(void){
  byte rev = 1;
  rev = Ethernet.begin(mymac);
  Serial.print(F("\nW5100 Revision "));

  if (rev == 0){
      Serial.println(F("Failed to access Ethernet controller"));
  }

  Serial.println(F("Setting up DHCP"));
  Serial.print("Connected with IP: ");
  Serial.println(Ethernet.localIP());
  }

  // Function to send MQTT message with wind direction and wind speed
  void send_MQTT_message(int dir, float spd){
      int timeDelay = 1000;

      // Send wind direction data
      if (displayDirection == true){
          const size_t bufferSize = JSON_OBJECT_SIZE(2) + 30;
          StaticJsonDocument<bufferSize> jsonDoc;
          jsonDoc["S_name1"] = "ES Wind Direction";
          jsonDoc["S_value1"] = dir;

          String jsonStr;
          serializeJson(jsonDoc, jsonStr);

          char message[jsonStr.length() + 10];
          sprintf(message, "IOTJS=%s", jsonStr.c_str());
          Serial.println(message); 
          client.publish(outTopic, message); 
          delay(timeDelay);
      }

      // Send wind speed data
      else if (displaySpeed == true){
          const size_t bufferSize = JSON_OBJECT_SIZE(2) + 30;
          StaticJsonDocument<bufferSize> jsonDoc;


          jsonDoc["S_name1"] = "ES Wind Speed";
          jsonDoc["S_value1"] = spd;

          String jsonStr;
          serializeJson(jsonDoc, jsonStr);

          char message[jsonStr.length() + 10];
          sprintf(message, "IOTJS=%s", jsonStr.c_str());
          Serial.println(message); 
          client.publish(outTopic, message);
          delay(timeDelay);
      }

      // Send both wind direction and wind speed data
      else if (displayBoth == true) {
          const size_t bufferSize = JSON_OBJECT_SIZE(2) + 30;
          StaticJsonDocument<bufferSize> jsonDoc;


          jsonDoc["S_name1"] = "ES Wind Direction";
          jsonDoc["S_value1"] = dir;

          String jsonStr;
          serializeJson(jsonDoc, jsonStr);

          char message[jsonStr.length() + 10];
          sprintf(message, "IOTJS=%s", jsonStr.c_str());

          Serial.println(message);
          client.publish(outTopic, message);

          jsonDoc.clear();
          delay(500);

          StaticJsonDocument<bufferSize> jsonDoc2;

          jsonDoc2["S_name1"] = "ES Wind Speed";
          jsonDoc2["S_value1"] = spd;

          String jsonStr2;
          serializeJson(jsonDoc2, jsonStr2);

          char message2[jsonStr2.length() + 10];
          sprintf(message2, "IOTJS=%s", jsonStr2.c_str());

          Serial.println(message2); 
          client.publish(outTopic, message2);
          delay(timeDelay);
      }
  }

  // Function to connect to MQTT server
  void Connect_MQTT_server(){
      Serial.println(" Connecting to MQTT" );
      Serial.print(server[0]); Serial.print(".");
      Serial.print(server[1]); Serial.print(".");
      Serial.print(server[2]); Serial.print(".");
      Serial.println(server[3]);

      // If not connected, try to connect to MQTT server
      if (!client.connected()){
          if (client.connect(clientId, deviceId, deviceSecret)){
          Serial.println(" Connected OK " );
          client.subscribe(inTopic);
          }
          else{
          Serial.println(client.state());
          }
      }
    }

  // MQTT callback function to handle incoming messages
  void callback(char* topic, byte* payload, unsigned int length){
      char* receiv_string;
      receiv_string = (char*) malloc(length + 1);
      memcpy(receiv_string, payload, length);
      receiv_string[length] = '\0';
      Serial.println(receiv_string);
      free(receiv_string);
  }

  // Function to handle pulse count for wind speed calculation
  void pulseCountFunction() {
      pulseCount++;
  }

  // Timer interrupt function to update wind speed and frequency
  void timerInterruptFunction() {
      i_time++;
      if (i_time > 19) {
          windFrequency = pulseCount / 10;
          pulseCount = 0;
          windSpeed = -0.24 + windFrequency * 0.699;
          i_time = 0;
      }
  }