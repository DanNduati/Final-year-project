#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino_JSON.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

//motor driver
#define IN1 2
#define IN2 15
#define EN 4

//wifi creds
const char *ssid = "Warazi";
const char *password = "Warazi@123";

//firebase
const char *api_key = "AIzaSyBpX0r75at6hZEfyjTC6iydB63elHBSCWc";
const char *database_url = "https://elevator-3a561-default-rtdb.firebaseio.com";
String data_header = "elevatorData";
FirebaseData fbdo;   //Define Firebase Data object
FirebaseData stream; //Firebase Stream object
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

//firebase callback
int floor_data_callback = 0;

void wifiInit()
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.print("Successfully connected to: ");
    Serial.println(ssid);
}
void streamCallback(StreamData data)
{
    floor_data_callback = data.intData();
    currentFloor = levels[floor_data_callback];
    displayData(currentFloor, true);
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
        Serial.println("stream timed out, resuming...\n");

    if (!stream.httpConnected())
        Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}
void firebaseInit()
{
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
    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);

    if (!Firebase.beginStream(stream, "/elevatorData/floor/value"))
        Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

    Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);
}

void sendToFirebase(int currentFloor)
{
    Serial.println("------------------------------------");
    Serial.println("Sending Sensor data to firebase");
    String floor_node = database_path + "/" + payload;
    String floor_value_node = floor_node + "/value";
    if (Firebase.set(fbdo, floor_value_node.c_str(), currentFloor))
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

void displayData(int currentFloor, bool elevatorStatus)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.print("Elevator");
    display.setCursor(20, 15);
    display.print("Status : ");
    display.setTextSize(1);
    display.setCursor(70, 15);
    if (elevatorStatus)
    {
        display.print("Occupied");
    }
    else
    {
        display.print("Free");
    }
    display.setCursor(62, 30);
    display.setTextSize(2);
    if (currentFloor == 0)
    {
        display.print("G");
    }
    else
    {
        display.print(String(currentFloor));
    }
    display.display();
    //delay(1000);
}

//shitty delay implementation logic for physical elevator movement
void panda(int factor)
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(EN, HIGH);
    //delay(factor*500);
    digitalWrite(EN, LOW);
}
void shuka(int factor)
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(EN, HIGH);
    //delay(factor*850);
    digitalWrite(EN, LOW);
}
void setup()
{
    Serial.begin(115200);
    wifiInit();
    firebaseInit();
    pinMode(groundBtn, INPUT);
    pinMode(firstBtn, INPUT);
    pinMode(secondBtn, INPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(EN, OUTPUT);
    //oled display setup
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    delay(2000);
    display.clearDisplay();
    display.setTextColor(WHITE);
    //move the elevator ground on start
    currentFloor, previousFloor = levels[0];
    //sendToFirebase(currentFloor); no need to send this on setup anymore
}

void loop()
{
    display.clearDisplay();
    gBtnState = digitalRead(groundBtn);
    fBtnState = digitalRead(firstBtn);
    sBtnState = digitalRead(secondBtn);
    displayData(currentFloor, true);
    previousFloor = currentFloor;
    //check btn floor changes
    if (gBtnState == HIGH and previousFloor != 0)
    {
        if (levels[0] < previousFloor)
        {
            Serial.print("Shuka: ");
            Serial.println(String(abs(levels[0] - previousFloor)));
            shuka(abs(levels[0] - previousFloor));
        }
        else
        {
            Serial.print("Panda: ");
            Serial.println(String(abs(levels[0] - previousFloor)));
            panda(abs(levels[0] - previousFloor));
        }
        sendToFirebase(levels[0]);
    }
    if (fBtnState == HIGH and previousFloor != 1)
    {
        if (levels[1] < previousFloor)
        {
            Serial.print("Shuka: ");
            Serial.println(String(abs(levels[1] - previousFloor)));
            shuka(abs(levels[1] - previousFloor));
        }
        else
        {
            Serial.print("Panda: ");
            Serial.println(String(abs(levels[1] - previousFloor)));
            panda(abs(levels[1] - previousFloor));
        }
        sendToFirebase(levels[1]);
    }
    if (sBtnState == HIGH and previousFloor != 2)
    {
        if (levels[2] < previousFloor)
        {
            Serial.print("Shuka: ");
            Serial.println(String(abs(levels[2] - previousFloor)));
            shuka(abs(levels[2] - previousFloor));
        }
        else
        {
            Serial.print("Panda: ");
            Serial.println(String(abs(levels[2] - previousFloor)));
            panda(abs(levels[2] - previousFloor));
        }
        sendToFirebase(levels[2]);
    }
}