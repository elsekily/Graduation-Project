import sqlalchemy
import urllib.request as urllib
from sqlalchemy import Table, Column, Integer, String,Float,DateTime,Boolean, MetaData, ForeignKey
import datetime
import paho.mqtt.client as mqtt
import json
import time 

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    #client.subscribe("$SYS/#")

def on_message(client, userdata, msg):   
    if(msg.topic=="elsekily/feeds/project.server"):
        x=msg.payload.decode("utf-8")
        y=json.loads(x)
        ins = landparameters.insert().values(devicename='raspberry pi',
         temperature=y["temp"],humidity=y["humidity"],
         moisture01=y["moisture01"],moisture02=y["moisture02"],
         water_amount=y["water_amount"],motor_status=y["motor_status"])
        r=conn.execute(ins)
        Ti=str(datetime.datetime.now().time())
        M=str((float(y["moisture02"])+float(y["moisture02"]))/2)
        H=float(y["humidity"])
        T=y["temp"]
        WA=str(0.0)
        data =  {
                "Inputs": {
                        "input1":
                        {
                        "ColumnNames": ["ID", "M", "H", "T", "Time", "WA"],
                        "Values": [ [ "15", M, H , T , Ti , WA ], [ "0", "0", "0", "0", "", "0" ], ]
                        },        },
                "GlobalParameters": {

                }
                }
        try:
                body = str.encode(json.dumps(data))
                url = 'https://ussouthcentral.services.azureml.net/workspaces/6737e641dddb41e8a7bac8108a35b310/services/1afac6428351447191c74576490bd138/execute?api-version=2.0&details=true'
                api_key = 'meYM1Zczgu6xFIjhZRnjD60aBYMSJXJv5goibBZTPu5ffLlLzE9jYoAXoWRsbB8EiEFvtfxkfNpRsgL/WwKllw==' # Replace this with the API key for the web service
                headers = {'Content-Type':'application/json', 'Authorization':('Bearer '+ api_key)}
                req = urllib.Request(url, body, headers) 
                response = urllib.urlopen(req)
                result = response.read()
                client.publish("elsekily/feeds/project.device",result,0)
        except:
                print("error")
        

engine = sqlalchemy.create_engine('postgres://wonwkifzlmmlqu:c7cecfb3c54edd61c0b633eaf9cb009a556ae2609e0cdc9cb558b1d0835055f3@ec2-79-125-2-142.eu-west-1.compute.amazonaws.com:5432/d5unpr69espjp')
metadata = MetaData() 
landparameters = Table('landparameters', metadata,
        Column('message_id', Integer,primary_key=True),
        Column('devicename', String(50)),
        Column('temperature', Float(precision=2)),
        Column('humidity',Float(precision=2)),
        Column('moisture01',Integer),
        Column('moisture02',Integer),
        Column('water_amount',Integer),
        Column('motor_status',Boolean),
        Column('created_at',DateTime, default=datetime.datetime.utcnow),
        )


conn = engine.connect()
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("elsekily", password="4109b6d2b091498bb87d98831f4441ba")
client.connect("io.adafruit.com", 1883, 60)
client.subscribe("elsekily/feeds/project.server",0)
client.loop_start()
while(1):
        pass