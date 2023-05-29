//    Alyk  jatko course 2021       30.3.2020 KN
//                                  2021 SV, EK
//    

#include <Ethernet.h>                                 // incluide Ethernet library W5100
#include <PubSubClient.h>                             // include MQTT library      
#include <TimerOne.h>                                 // include timer library

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
    Connect_MQTT_server();                              // connect to MQTT server    
}

void loop(){
  int inx=10;

  while(true){  
    Serial.println("Are we connected?");
    send_MQTT_message(inx);
    inx++;
    delay(1500);
  }
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
                                                         
void send_MQTT_message(int num){                     // Send MQTT message
  char bufa[50];                             //  Print message to serial monitor
  if (client.connected()){ 
    sprintf(bufa,"ES Course Group 2: value =%d", num);               // create message with header and data
    Serial.println( bufa ); 
    client.publish(outTopic,bufa);                        // send message to MQTT server        
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
