#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino_JSON.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//wifi creds
const char* ssid  = "warazi";
const char* password = "warazi@123";
//firebase
const char* api_key = "AIzaSyBpX0r75at6hZEfyjTC6iydB63elHBSCWc";
const char* database_url = "https://elevator-3a561-default-rtdb.firebaseio.com";
String data_header = "sensorData";
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String database_path = "";
String fuid = "";
bool isAuthenticated = false;

//distance sensor
const int trigPin = 5;
const int echoPin = 18;

#define SOUND_SPEED 0.034
long duration;
float distanceCm;
//limit switches
const int switch_num = 2;
const int floor_switches[switch_num] = {32, 33};

//firebase send interval
long lastSendTime = 0;
int interval = 500;

void setup() {
  Serial.begin(115200);
  wifiInit();
  firebaseInit();
  pinInit();
}

void loop() {
  if (millis() - lastSendTime > interval)
  {
    float distance = get_distance();
    sendToFirebase(distance);
    Serial.print("Distance (cm): ");
    Serial.println(distance);
    lastSendTime = millis();
  }
}

void pinInit() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  for (int i = 0; i < switch_num; i++) {
    pinMode(INPUT, floor_switches[i]);
  }
}

void wifiInit() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println(" ");
  Serial.println("Connected! ");
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

float get_distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  return distanceCm;
}

void sendToFirebase(float distance) {
  Serial.println("------------------------------------");
  Serial.println("Sending Sensor data to firebase");
  String root_node = database_path;
  String sensor_node = root_node + "/" + "Distance";
  String node = sensor_node + "/value";
  float dist = distance;
  if (Firebase.set(fbdo, node.c_str(), dist))
  {
    // Print firebase server response
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

