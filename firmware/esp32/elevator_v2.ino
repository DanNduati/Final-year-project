#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino_JSON.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "DHT.h"

//dht11
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//wifi creds
const char* ssid  = "Warazi";
const char* password = "Warazi@123";
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
const int num_sensors = 5;
String payload_names[num_sensors] = {"Distance", "Temperature", "Humidity","lmsw1","lmsw2"};

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
int interval = 100;

void setup() {
  Serial.begin(115200);
  dht.begin();
  wifiInit();
  firebaseInit();
  pinInit();
}

void loop() {
  if (millis() - lastSendTime > interval)
  {
    /*
    float distance = get_distance();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int sw1 = digitalRead(floor_switches[0]);
    int sw2 = digitalRead(floor_switches[1]);
    */
    //pseudo data
    float distance = 69.0;
    float h = 52;
    float t = 23;
    int sw1 = 0;
    int sw2 = 0;
    //package the sensor data
    float sensor_data[num_sensors] = {distance, t, h, float(sw1),float(sw2)};
    sendToFirebase(sensor_data);
    Serial.print("Switch 1: ");
    Serial.print(sw1);
    Serial.print(" Switch 2: ");
    Serial.print(sw2);
    Serial.print(F("  Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(" Distance (cm): ");
    Serial.println(distance);
    lastSendTime = millis();
  }
}

void pinInit() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  for (int i = 0; i < switch_num; i++) {
    pinMode(floor_switches[i],INPUT);
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

void sendToFirebase(float payload[3]) {
  Serial.println("------------------------------------");
  Serial.println("Sending Sensor data to firebase");
  String root_node = database_path;
  for (int i = 0; i < num_sensors; i++) {
    String sensor_node = root_node + "/" + payload_names[i];
    String node = sensor_node + "/value";
    float s_data = payload[i];
    if (Firebase.set(fbdo, node.c_str(), s_data))
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
}
