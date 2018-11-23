# -*- coding: utf-8 -*-
"""
Created on Tue Apr 12 15:00:56 2016
 
Help by:
http://bottlepy.org/docs/dev/tutorial.html#dynamic-routes
https://api.mongodb.org/python/current/tutorial.html
https://docs.python.org/2/library/datetime.html

PROGRESS:
Deluje shranjevanje v meritve collection!! 
samo še obogatit moramo podatke in jih prikazat!

Dodali smo še podatek o pressure ki se prvo ni dobro shranjeval
v bazo, ampak z resetom celotnega sistema deluje lepo in shrani vse podatke 
in jih inserta kot en post(objekt) v nas collection!

TO DO:
dodat se uro iz arduinota 

@author: PrimozK
"""

from bottle import run, route, request
import pymongo
import datetime
# zagon database same in povedat ji moras kje se nahaja(ponavadi vedno enako)
uri = "mongodb://127.0.0.1:27017"
client = pymongo.MongoClient(uri)
# definiraš kot osnovno databaso in potem uporabljas to spremenljivko
db = client.test



@route('/data') 
# GET v arduinotu more biti take oblike!
def saveData():
# https://api.mongodb.org/python/current/tutorial.html
# http://bottlepy.org/docs/dev/tutorial.html#query-variables
    meritev = {"Temperature": request.query.temperature,
            "Humidity": request.query.humidity,
            "Light": request.query.light,
            "Pressure": request.query.pressure,
            "Date": datetime.datetime.now().isoformat()}
    meritve = db.testnav2 # collection se kar sam ustvari,ti mu das ime!
    meritve.insert_one(meritev) # vstavi podatke not v meritve
    listMeritve = [meritev for meritev in meritve.find({})]
    return "Sprejeti podatki!" + "\r"  
    
    

run(host='192.168.1.209',port=8080, debug=True)
# ta na koncu zelo pomembna saj drzi celoten server gor !


"""
Naredit je potrebno še en rout ki bo pokazal vso shranjeno dato in dogajanje na 
serverju ! (mongodb)

"""