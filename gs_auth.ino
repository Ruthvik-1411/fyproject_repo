#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include<SPI.h>
#include<MFRC522.h>

#define ob_led 2
#define Rled D2
#define Gled D3
#define buzz D4
#define SS_PIN D8//10
#define RST_PIN D0//9

//-->Wifi ssid,password,mess and id
const char* ssid = "DONTCONNECT";
const char* password = "itoldyounot2";
const String mess="B";
String ID="";
String roll="";
//--> Cards Database
String mydb[25][2]={
  {"30 55 B4 D4","B190001EE"},
  {"10 34 6A D4","B190002EE"},
  {"70 BA 68 D4","B190003EE"},
  {"80 29 A9 D4","B190004EE"},
  {"90 34 AF D4","B190005EE"},
  {"E0 F2 62 D4","B190006EE"},
  {"10 2A B0 D4","B190007EE"},
  {"30 79 AB D4","B190008EE"},
  {"80 75 5A D4","B190009EE"},
  {"F0 D0 AE D4","B190010EE"},
  {"70 94 A9 D4","B190011EE"},
  {"20 59 5A D4","B190012EE"},
  {"20 89 5A D4","B190013EE"},
  {"E0 49 AE D4","B190014EE"},
  {"40 A0 74 D4","B190015EE"},
  {"90 DA 6D D4","B190016EE"},
  {"A0 46 62 D4","B190017EE"},
  {"50 86 B2 D4","B190018EE"},
  {"B0 30 BA D4","B190019EE"},
  {"D0 6D A9 D4","B190020EE"},
  {"10 91 D9 70","B190021EE"},
  {"40 24 C1 70","B190022EE"},
  {"E0 D6 C9 70","B190023EE"},
  {"90 42 C0 70","B190024EE"},
  {"F0 2E BA 70","B190025EE"} 
};

//Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbwAhqTXT0mgnyyMJyXBRlD1AvVtUVN3cactkKZAJqPRMc6bPu2zwVTJAeGqG_fDlNAS";

WiFiClientSecure client;
MFRC522 my_mfrc522(SS_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  
  WiFi.begin(ssid, password);
  Serial.println("");
    
  pinMode(ob_led,OUTPUT);
  digitalWrite(ob_led, HIGH);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ob_led, LOW);
    delay(250);
    digitalWrite(ob_led, HIGH);
    delay(250);
  }
  digitalWrite(ob_led, HIGH);
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();

  pinMode(Rled,OUTPUT);
  pinMode(Gled,OUTPUT);
  pinMode(buzz,OUTPUT);
  digitalWrite(Rled,LOW);
  digitalWrite(Gled,LOW);
  digitalWrite(buzz,LOW);
  
  SPI.begin();
  my_mfrc522.PCD_Init();
  my_mfrc522.PCD_DumpVersionToSerial();
  Serial.println("Put Your card to Access...");
  Serial.println();
}

void loop() {

  if(!my_mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  if(!my_mfrc522.PICC_ReadCardSerial()){
    return;
  }
  byte letters;
  for(byte i=0;i<my_mfrc522.uid.size;i++){
    ID.concat(String(my_mfrc522.uid.uidByte[i]<0x10 ? "0" : " "));
    ID.concat(String(my_mfrc522.uid.uidByte[i],HEX));
  }
  ID=ID.substring(1);
  proc();
  ID.toUpperCase();
  
  String Tag = "Tag ID is : " + ID + " , ";
  String Mess = "Scanned at : " + mess + " mess";
  Serial.print(Tag);
  Serial.println(Mess);
  if(iselementpresent(ID)){ //-->If card is present in database
    access();
    sendData(roll, mess);
    Serial.print("Hello: ");
    Serial.println(roll);
  }
  else{
    deny();
    Serial.println(" Access Denied");
  }
  ID="";
}

//-->Function for sending data to Google Sheets
void sendData(String id, String mess) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
//-->Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
//-->Sending data 
  String url = "/macros/s/" + GAS_ID + "/exec?id=" + id + "&mess=" + mess;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");

//-->Whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } 
  else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
} 
//--> Function to check if card is present database
boolean iselementpresent(String element){
  for (int i=0;i<25;i++){
    if(mydb[i][0]==element){
      roll=mydb[i][1];
      return true;
    }
  }
  return false;
}
//--> Functions for Buzzer, Green Led and Red Led
void proc(){
  analogWrite(buzz, 255);
  delay(200);
  digitalWrite(buzz, LOW);
}
void access(){
  digitalWrite(Gled,HIGH);
  delay(500);
  digitalWrite(Gled,LOW);
}
void deny(){
  for(int i=0;i<10;i++){
    digitalWrite(Rled,HIGH);
    delay(200);
    digitalWrite(Rled,LOW);
    delay(200);
    proc();
  }
}
