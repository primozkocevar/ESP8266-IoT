# -*- coding: utf-8 -*-
"""
Created on Tue Apr 12 15:00:56 2016
 
Help by:
http://bottlepy.org/docs/dev/tutorial.html#dynamic-routes
https://api.mongodb.org/python/current/tutorial.html
https://docs.python.org/2/library/datetime.html
Notes:
RUN THE MONGODB DATABASE BEFORE YOU RUN THIS CODE

PROGRESS:
Deluje shranjevanje v meritve collection!! 

Dodali smo še podatek o pressure ki se prvo ni dobro shranjeval
v bazo, ampak z resetom celotnega sistema deluje lepo in shrani vse podatke 
in jih inserta kot en post(objekt) v nas collection!

Pogruntaj kako sprejemaš name=value način podatkov!
Pošiljanje je verjetno pravilno, poglej se kar se tice tistega vprasaja 
v browserjih!!! Te podatke pobiras s pomocjo request.query.name ker tako parsas
kr avtomatsko get request poslan in dobivas vn zeljene parametre, vec o tem :
 http://bottlepy.org/docs/dev/tutorial.html#query-variables
 
 iz collectiona naredit list in iz lista dataframe !!! nc nebomo meli z jsonom 
posla ampak kar direkt z objekti databaze!!!

Ni tuk enostavno da narediš za vsak dan nov collection, do nadaljnega bomo imeli
vse v istem collectionu in potem to ločevali!, videt treba kako zgledajo podatki
od typing tutoriala!

dodaj deviceID

TO DO:
-dodati zunanjo uro iz arduina in ne iz serverja ali pa zahtevek na server naredit!
-pogruntaj kako bo najlazje shranit cas da ga bos pol bral.

@author: PrimozK
"""

from bottle import run, route, request
import pymongo
import datetime
# zgon database same in povedat ji moras kje se nahaja(ponavadi vedno enako)
uri = "mongodb://127.0.0.1:27017"
client = pymongo.MongoClient(uri)
# definiraš kot osnovno databaso in potem uporabljas to spremenljivko
db = client.test # določaš bazo!

@route('/data') 
def saveData():
# https://api.mongodb.org/python/current/tutorial.html
# pobiranje parametrov s pomočjo query in ne direkt iz zahteve:
# http://bottlepy.org/docs/dev/tutorial.html#query-variables
    meritev = {"DeviceID": request.query.deviceid,
               "Temperature": request.query.temperature,
               "Humidity": request.query.humidity,
               "Light": request.query.light,
               "Pressure": request.query.pressure,
               "Date": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
               # ameriški način zapisa datuma, ker ga tako razume
               # https://docs.python.org/2/library/datetime.html#datetime-objects
               # now() bolj natančen od today() odstranimo microsekunde https://docs.python.org/2/library/datetime.html#strftime-and-strptime-behavior          
    meritve = db.ceta # določaš v kater collection se ti shranjujejo podatki
    meritve.insert_one(meritev) # vstavi podatke not v meritve
    return "Sprejeti podatki!" + "\r"  
    
    

run(host='192.168.1.13',port=8080, debug=True)
# ta na koncu zelo pomembna saj drzi celoten server gor !
