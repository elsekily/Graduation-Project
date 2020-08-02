## raspberry pi code
##this device gathering data from devices in the land and send it to server
##and subscribing motor topic and send its data to device that controls this motor

import paho.mqtt.client as mqtt
import time
import json

moisture01=0
moisture02=0
temp=0
hum=0
motor=False
WA=0
#when connected successfully to server
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    

##when message received calls this method
def on_message(client, userdata, msg):  
    global  moisture01,moisture02,temp,hum,WA,motor
    ##checking topics and publish every to cloud topic
    if(msg.topic=="project/moisture/1"):
        moisture01=msg.payload.decode("utf-8")
    
    elif(msg.topic=="project/moisture/2"):
        moisture02=msg.payload.decode("utf-8")

    elif(msg.topic=="project/temp"):
        temp=msg.payload.decode("utf-8")

    elif(msg.topic=="project/hum"):
        hum=msg.payload.decode("utf-8")
        
    elif(msg.topic=="elsekily/feeds/project.device"):
        Y=msg.payload.decode("utf-8")
        X=json.loads(Y)
        WA=float(X["Results"]["output1"]["value"]["Values"][0][6])*0.009
        if(WA>=5):
            client.publish("project/motor",1,0)
            motor=True
            time.sleep(10)
            client.publish("project/motor",0,0)
        else:
            
            motor=Fasle


    else:
        print("unknown topic")

           
if __name__ == '__main__':
    client = mqtt.Client() #start client to local server
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("10.42.0.1", 1883, 60) #connecting to local server

    client2 = mqtt.Client()#start client to adafruit
    client2.on_connect = on_connect
    client2.on_message = on_message
    client2.username_pw_set("elsekily", password="4109b6d2b091498bb87d98831f4441ba")#set password and username
    ##################################### see io.adafruit.com to user name and password
    client2.connect("io.adafruit.com", 1883, 60)##connect to adafruit

    client.loop_start()
    client2.loop_start()
    client.subscribe([
        ("project/moisture/1", 0),
        ("project/moisture/2", 0),
        ("project/temp", 0),
        ("project/hum", 0)])
    client2.subscribe("elsekily/feeds/project.device",0)
    #time.sleep(30)
    while(1):
        time.sleep(30)
        mes=json.dumps({"moisture01": moisture01, "moisture02": moisture02, "motor_status": motor, "humidity": hum, "temp": temp, "water_amount": WA})
        client2.publish("elsekily/feeds/project.server",mes,0)