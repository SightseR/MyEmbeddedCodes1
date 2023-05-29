//    Alyk  jatko course 2021       30.3.2020 KN
//                                  2021 SV, EK
//    

#include <Ethernet.h>                                 // incluide Ethernet library W5100
#include <PubSubClient.h>                             // include MQTT library      
#include <TimerOne.h> 

//libraries from week10
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>

///////////////////////////////////////////////////////
//variables from week10
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

const byte ROWS = 4;
const byte COLS = 4;

//wind direction variables

const int buttonInputPin = 3;
volatile boolean buttonPressedA;
volatile boolean buttonPressedB;
String windDirection;
int buttonState = 0;
int previousButtonState = 0;
bool displayDirection = true;
int degree_Value;


//wind speed variables

// Define variables
volatile unsigned long pulseCount;
volatile unsigned long windFrequency;
//volatile boolean buttonPressed;
float windSpeed;
unsigned long i_time;

// Define pin numbers
const int signalInputPin = 2;
//const int buttonInputPin = 3;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 42, 43, 44, 45 };
byte colPins[COLS] = { 46, 47, 48, 49 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
////////////////////////////////////////////////////////////////////////////

                                // include timer library

EthernetClient ethClient;                               // Ethernet object var  

static uint8_t mymac[6] = { 0x44,0x76,0x58,0x10,0x00,0x62 };

//  MQTT settings
 
unsigned int Port = 1883;                          //  MQTT port number
byte server[] = { 10,6,0,21 };                    // TAMK IP

char* deviceId     = "ekrkjhgfejh";                   // * set your device id (will be the MQTT client username) *yksilöllinen*
char* clientId     = "a731fsd4";                       // * set a random string (max 23 chars, will be the MQTT client id) *yksilöllinen*
char* deviceSecret = "tamk";                        // * set your device secret (will be the MQTT client password) *kaikille yhteinen*

//  MQTT Server settings  

void callback(char* topic, byte* payload, unsigned int length); // subscription callback for received MQTTT messages   

PubSubClient client(server, Port, callback, ethClient);   // mqtt client 

//  MQTT topic names 

 #define inTopic    "ICT1B_in_2020"                    // * MQTT channel where data are received 
 #define outTopic   "ICT1B_out_2020"                   // * MQTT channel where data is send 
                          
//  SETUP section

void setup() {
    Serial.begin(9600);                                 // Serial monitor baudrate  = 9600
    Serial.println("Start 19.3.2021");                   // print to serial monitor
    delay(500);
    fetch_IP();                                         // initialize Ethernet connection
    Connect_MQTT_server(); 
    
                                 // connect to MQTT server    

////////////////////////////////////////////////////////
//Setup from week10
 lcd.begin(16, 2);
 

  //wind direction void setup

  pinMode(A2, INPUT);
  pinMode(buttonInputPin, INPUT_PULLUP);

  //wind speed void setup

   pulseCount = 0;
  windFrequency = 0;
  buttonPressedA = false;
  buttonPressedB = false;

  windSpeed = 0.0;
  i_time = 0;

  // Attach interrupt function to signal input pin
  attachInterrupt(digitalPinToInterrupt(signalInputPin), pulseCountFunction, RISING);

  // Set up timer for 0.5 second intervals
  Timer1.initialize(500000);
  Timer1.attachInterrupt(timerInterruptFunction);

////////////////////////////////////////////////////////  

}

void loop(){

//////////////////////////////////////////////
// Loop from week10

  // Read wind direction and speed from sensors and display on LCD
  lcd.setCursor(20, 1);
  lcd.print("A-WindDir B-WindSpd");
  float analog_Value = analogRead(A2);
  float voltage_Value = 5 * analog_Value / 1023;
  if (voltage_Value < 1.2) {
    degree_Value = 0;
  } else{
    degree_Value = ((voltage_Value - 1.2) / 3.8) * 360;
  }
  if (displayDirection) {
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
    lcd.clear(); // clear the display before printing new data
    lcd.setCursor(0, 0);
    lcd.print("Dir : ");
    lcd.print(windDirection);
    lcd.setCursor(0, 1);
    lcd.print(degree_Value);
    lcd.print(" degrees");
  } else {
    lcd.clear(); // clear the display before printing new data
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
  }

  // Check if A or B button is pressed to switch between wind direction and speed display
  char key = keypad.getKey();
  if(key == 'A'){
    buttonPressedA = true;
    buttonPressedB = false;
  } else if(key == 'B'){
    buttonPressedB = true;
    buttonPressedA = false;
  }
  
  if(buttonPressedA == true && buttonPressedB == false){
    displayDirection = true;
  } else if(buttonPressedB == true && buttonPressedA == false){
    displayDirection = false;
  }

  // Send MQTT message to broker
  if (client.connected()){ 
    const char* windDirections = windDirection.c_str();
    send_MQTT_message(windDirections,windSpeed);
  } else {                                                           // Reconnect if connection is lost
    delay(500);
    Serial.println("No, re-connecting" );
    client.connect(clientId, deviceId, deviceSecret);
    delay(1000);                                            // Wait for reconnecting
  }

  // Delay before checking again
  delay(1000);
}

//  GET IP number from DHCP server

void fetch_IP(void){
  byte rev=1;
  rev=Ethernet.begin(mymac);                  // get IP number
  Serial.print( F("\nW5100 Revision ") );
  
  if (rev == 0){
    Serial.println( F( "Failed to access Ethernet controller" ) );
  }    
                 
  Serial.println( F( "Setting up DHCP" ));
  Serial.print("Connected with IP: "); 
  Serial.println(Ethernet.localIP()); 
  delay(1500);
}

//  MQTT Routines
                                                         
void send_MQTT_message(const char* dir, int spd){                     // Send MQTT message
  char bufa[50];
  char fufa[50];                             //  Print message to serial monitor
  if (client.connected()){ 
    sprintf(bufa,"Wind Direction: %s", dir);              // create message with header and data
    Serial.println( bufa ); 
    client.publish(outTopic,bufa);  
    sprintf(fufa,"Wind Speed (m/s): %d", spd);              // create message with header and data
    Serial.println( fufa ); 
    client.publish(outTopic,fufa); 
    delay(10000);    
                      // send message to MQTT server        
  }
  else{                                                           //   Re connect if connection is lost
    delay(500);
    Serial.println("No, re-connecting" );
    client.connect(clientId, deviceId, deviceSecret);
    delay(1000);                                            // wait for reconnecting
  }
}


//  MQTT server connection  
                                           
void Connect_MQTT_server(){ 
  Serial.println(" Connecting to MQTT" );
  Serial.print(server[0]); Serial.print(".");     // Print MQTT server IP number to Serial monitor
  Serial.print(server[1]); Serial.print(".");
  Serial.print(server[2]); Serial.print(".");
  Serial.println(server[3]); 
  delay(500);
  
  if (!client.connected()){                                   // check if allready connected  
    if (client.connect(clientId, deviceId, deviceSecret)){ // connection to MQTT server 
      Serial.println(" Connected OK " );
      client.subscribe(inTopic);                        // subscript to in topic        
    } 
    else{
       Serial.println(client.state());
    }    
  } 
}

//  Receive incoming MQTT message   
 
void callback(char* topic, byte* payload, unsigned int length){ 
  char* receiv_string;                               // copy the payload content into a char* 
  receiv_string = (char*) malloc(length + 1); 
  memcpy(receiv_string, payload, length);           // copy received message to receiv_string 
  receiv_string[length] = '\0';           
  Serial.println( receiv_string );
  free(receiv_string); 
} 

///////////////////////////////////////////////////
//Functions from week10

// Interrupt function to count pulses
void pulseCountFunction() {
  pulseCount++;
}

// Interrupt function to calculate wind frequency every 0.5 seconds
void timerInterruptFunction() {
  i_time++;
  if (i_time > 19) { // Perform wind speed calculation every 5 seconds
    windFrequency = pulseCount / 10; // Multiply pulse count by 2 to get frequency in Hz
    pulseCount = 0; // Reset pulse count
    windSpeed = -0.24 + windFrequency * 0.699; // Calculate wind speed using equation
    i_time = 0; // Reset time counter
  }
}




/*
 Sample code from internet!
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Ethernet shield MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// MQTT broker credentials
const char* mqttServer = "your_MQTT_broker_IP_address";
const int mqttPort = 1883;
const char* mqttUser = "your_MQTT_username";
const char* mqttPassword = "your_MQTT_password";
const char* outTopic = "your_MQTT_topic";

EthernetClient ethernetClient;
PubSubClient client(ethernetClient);

void setup() {
  Serial.begin(115200);

  // Start Ethernet connection
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (true) {
      // Do nothing, just loop forever
    }
  }
  Serial.println("Ethernet connected");

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("arduino", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // create a JSON document
  DynamicJsonDocument doc(1024);
  doc["S_name"] = "Temp1";
  doc["S_value"] = 3.13;

  // serialize the JSON document into a String
  String jsonMessage;
  serializeJson(doc, jsonMessage);

  // publish the JSON message
  if (client.connected()) {
    client.publish(outTopic, jsonMessage.c_str());
    Serial.println("Published message:");
    Serial.println(jsonMessage);
  } else {
    Serial.println("Failed to publish message: not connected to MQTT broker");
  }

  delay(10000);
}


*/