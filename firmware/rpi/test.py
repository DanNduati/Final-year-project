import RPi.GPIO as GPIO
import time
import datetime
from firebase import firebase
import Adafruit_DHT
import urllib2, urllib, httplib
import json
import os
from functools import partial


GPIO.setmode(GPIO.BCM)
GPIO.cleanup()
GPIO.setwarnings(False)


#set GPIO Pins
GPIO_TRIGGER = 18
GPIO_ECHO = 24
GPIO_LM1 = 20
GPIO_LM2 = 19
GPIO_DHT11 = 4

GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)
GPIO.setup(GPIO_LM1, GPIO.IN)
GPIO.setup(GPIO_LM2, GPIO.IN)

#dht11 setup
sensor = Adafruit_DHT.DHT11

#firebase setup
firebase =firebase.FirebaseApplication('https://elevator-3a561-default-rtdb.firebaseio.com/', None)

def get_distance():
    GPIO.output(GPIO_TRIGGER, True)
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
    StartTime = time.time()
    StopTime = time.time()
    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()
    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()
    TimeElapsed = StopTime - StartTime
    distance = (TimeElapsed * 34300) / 2
    return distance

def update_firebase():
    humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)
    lm1_state = GPIO.input(GPIO_LM1)
    lm2_state = GPIO.input(GPIO_LM2)
    distance = get_distance()
    firebase.post('/lmsw1/value',lm1_state)
    firebase.post('/lmsw2/value',lm2_state)
    firebase.post('/Distance/value',distance)
    firebase.post('/Humidity/value',humidity)
    firebase.post('/Temperature/value',temperature)

while True:
    update_firebase();
    time.sleep(5)
