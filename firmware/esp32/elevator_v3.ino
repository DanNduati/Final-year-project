#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino_JSON.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

//wifi creds
const char* ssid  = "TP-Link_9BC2";
const char* password = "33646619";

//firebase
const char* api_key = "AIzaSyBpX0r75at6hZEfyjTC6iydB63elHBSCWc";
const char* database_url = "https://elevator-3a561-default-rtdb.firebaseio.com";
String data_header = "elevatorData";
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String database_path = "";
String fuid = "";
bool isAuthenticated = false;
String payload = "floor";

//Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//buttons
#define groundBtn 12
#define firstBtn 14
#define secondBtn 27

//button states
int gBtnState, fBtnState, sBtnState = 0;

//firebase send interval
long lastSendTime = 0;
int interval = 100;

// floors
int levels[3] = {0, 1, 2};
int currentFloor;
int previousFloor;

void wifiInit() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Successfully connected to: ");
  Serial.println(ssid);
}

void firebaseInit() {
  // configure firebase API Key
  config.api_key = api_key;
  // configure firebase realtime database url
  config.database_url = database_url;
  // Enable WiFi reconnection
  Firebase.reconnectWiFi(true);
  Serial.println("------------------------------------");
  Serial.println("Sign up new user...");
  // Sign in to firebase Anonymously
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Success");
    Serial.println("------------------------------------");
    isAuthenticated = true;
    database_path = "/" + data_header;
    fuid = auth.token.uid.c_str();
  }
  else
  {
    Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
    isAuthenticated = false;
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
}

void sendToFirebase(int currentFloor) {
  Serial.println("------------------------------------");
  Serial.println("Sending Sensor data to firebase");
  String floor_node = database_path + "/" + payload;
  String node = floor_node + "/value";
  if (Firebase.set(fbdo, node.c_str(), currentFloor))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.println("ETag: " + fbdo.ETag());
    Serial.print("VALUE: ");
    printResult(fbdo); //see addons/RTDBHelper.h
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200);
  wifiInit();
  firebaseInit();
  pinMode(groundBtn, INPUT);
  pinMode(firstBtn, INPUT);
  pinMode(secondBtn, INPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  //move the elevator ground on start
  currentFloor, previousFloor = levels[0];
  sendToFirebase(currentFloor);
}

void loop() {
  display.clearDisplay();
  gBtnState = digitalRead(groundBtn);
  fBtnState = digitalRead(firstBtn);
  sBtnState = digitalRead(secondBtn);
  /*
    Serial.print("Ground: ");
    Serial.print(gBtnState);
    Serial.print(" ,First: ");
    Serial.print(fBtnState);
    Serial.print(" ,Second: ");
    Serial.println(sBtnState);
  */
  displayData(currentFloor, true);
  //check btn floor changes
  if (gBtnState == HIGH and previousFloor != 0) {
    //move to ground floor
    currentFloor = levels[0];
    sendToFirebase(currentFloor);
    displayData(currentFloor, true);
    previousFloor = levels[0];
  }
  if (fBtnState == HIGH and previousFloor != 1) {
    //move to ground floor
    currentFloor = levels[1];
    sendToFirebase(currentFloor);
    displayData(currentFloor, true);
    previousFloor = levels[1];
  }
  if (sBtnState == HIGH and previousFloor != 2) {
    //move to ground floor
    currentFloor = levels[2];
    sendToFirebase(currentFloor);
    displayData(currentFloor, true);
    previousFloor = levels[2];
  }

}



void displayData(int currentFloor, bool elevatorStatus) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(40, 0);
  display.print("Elevator");
  display.setCursor(20, 15);
  display.print("Status : ");
  display.setTextSize(1);
  display.setCursor(70, 15);
  if (elevatorStatus) {
    display.print("Occupied");
  } else {
    display.print("Free");
  }
  display.setCursor(62, 30);
  display.setTextSize(2);
  if (currentFloor == 0) {
    display.print("G");
  } else {
    display.print(String(currentFloor));
  }
  display.display();
  //delay(1000);
}
