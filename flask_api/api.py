from flask import Flask,jsonify,request
from flask_sqlalchemy import SQLAlchemy
from flask_restful import Api,Resource
import datetime
import json

import sys

app = Flask(__name__)

#configuring database
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgres://wonwkifzlmmlqu:c7cecfb3c54edd61c0b633eaf9cb009a556ae2609e0cdc9cb558b1d0835055f3@ec2-79-125-2-142.eu-west-1.compute.amazonaws.com:5432/d5unpr69espjp'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

#configuring mqtt 

api = Api(app)
db = SQLAlchemy(app)


class LandModel(db.Model):
    ##telling python about tables and its columns
    __tablename__ = 'landparameters'
    message_id  = db.Column(db.Integer,primary_key=True)
    devicename = db.Column(db.String(50))
    temperature = db.Column(db.Float(precision=2))
    humidity = db.Column(db.Float(precision=2))
    moisture01 = db.Column(db.Integer)
    moisture02 = db.Column(db.Integer)
    water_amount = db.Column(db.Integer)
    motor_status = db.Column(db.Boolean)
    created_at  = db.Column(db.DateTime, default=datetime.datetime.utcnow)
    
    #inserting new row 
    def __init__(self,temperature,humidity, moisture01,moisture02,water_amount,motor_status):
        self.devicename = "raspberry pi 3"#we are using one device so no problem to set it manually
        self.temperature = temperature
        self.humidity = humidity
        self.moisture01 = moisture01
        self.moisture02 = moisture02
        self.water_amount = water_amount
        self.motor_status = motor_status
        self.created_at = datetime.datetime.now()
    
    def save_to_db(self):#saving to db for this table 
        db.session.add(self)
        db.session.commit()
    
    def json1(self):#retun dict for one row in this table 
        return {'message_id': self.message_id, 'devicename': self.devicename,'temperature':self.temperature,'humidity': self.humidity,'moisture': (self.moisture01+self.moisture02)/2,
        'water_amount': self.water_amount,'motor_status':self.motor_status,'created_at':self.created_at}


def writing_to_db(a,b,c,d,e,g):#make some changes after subscribing
    item = LandModel(a,b,int(c),int(d),int(e),bool(g))#init class LandModel to insert new row
    try:#sometimes it fails to insert so try at least one and max 2
        item.save_to_db()
    except:
        item.save_to_db()

    
#main page of the api greating to our supervisor
@app.route('/')
def home():
    return "Hello, Dr.Abdelkariam"

#get last values of data
class AllData(Resource):
    def get(self):
        obj = LandModel.query.order_by(LandModel.message_id.desc()).first()#get last messege_id row
        return jsonify(obj.json1()) # convert dict to json

api.add_resource(AllData, '/lastvalues')
## this creates local wsgi and doesn't run with uWSGI
if __name__ == '__main__':
    app.run(port=5000,use_reloader=True, debug=True)
