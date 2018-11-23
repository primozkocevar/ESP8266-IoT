# -*- coding: utf-8 -*-
"""
Created on Tue Apr 12 15:00:56 2016
 
Help by:
http://bottlepy.org/docs/dev/tutorial.html#dynamic-routes
https://api.mongodb.org/python/current/tutorial.html

PROGRESS:
Deluje shranjevanje v meritve collection!! 
samo še obogatit moramo podatke in jih prikazat!

Dodali smo še podatek o pressure ki se prvo ni dobro shranjeval
v bazo, ampak z resetom celotnega sistema deluje lepo in shrani vse podatke 
in jih inserta kot en post(objekt) v nas collection!

TO DO:

Pogruntaj kako sprejemaš name=value način podatkov!
Pošiljanje je verjetno pravilno, poglej se kar se tice tistega vprasaja 
v browserjih!!! ali moras tudi tisto dodat !

@author: PrimozK
"""

from bottle import run, route
import pymongo
import datetime
# zgon database same in povedat ji moras kje se nahaja(ponavadi vedno enako)
uri = "mongodb://127.0.0.1:27017"
client = pymongo.MongoClient(uri)
# definiraš kot osnovno databaso in potem uporabljas to spremenljivko
db = client.test



@route('/data/<temp>&<hum>&<light>&<press>') 
# GET v arduinotu more biti take oblike!
def saveData(temp,hum,light,press):
    # https://api.mongodb.org/python/current/tutorial.html
    meritev = {"Temperature": temp,
            "Humidity": hum,
            "Light": light,
            "Pressure": press,
            "Date": datetime.datetime.utcnow()}
    meritve = db.collection # collection se kar sam ustvari,ti mu das ime!
    meritve.insert_one(mera) # vstavi podatke not v meritve
    listMeritve = [meritev for meritev in meritve.find({})]
    return "Sprejeti podatki!" + "\r"  
    
    

run(host='192.168.1.4',port=8080, debug=True)
# ta na koncu zelo pomembna saj drzi celoten server gor !


"""
Naredit je potrebno še en rout ki bo pokazal vso shranjeno dato in dogajanje na 
serverju ! (mongodb)

"""