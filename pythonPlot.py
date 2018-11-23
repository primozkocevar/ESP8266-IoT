# -*- coding: utf-8 -*-
"""
Created on Tue May 03 22:37:46 2016

@author: Primoz Kocevar

ZAŽENI BAZO PRED ZAČETKOM!!!
Koda se ne ukvarja z JSONOM ampak samo z mongodb-jevimi osebnimi 
oblikami podatkov collection-i ki so pa podobni jsonu. te najprej spremenimo
v liste in liste v dataframe od pandasa!

- da bi mogoče šel v vsak stolpec posebej dajta uro, minuto, sekundo.

Ugotovitve:
-zapis v obliko datatime64 ni nic odvisen od stevila nul prikazanih.
-freq celotnega zapisa je od tega odvisna vendar je ne znam spreminjati.
-bolj naravno je ce imas cas podan v columnu od indexa!
-ko je samo en element za izrisat je x-os pravilno oznacena in zato so moje 
predpostavke glede frekvence ki se spremeni ko imas vec vzorcov tocne!
RESAMPLE in FREQ:
- freq od dataframe-a mors spremenit ker zdej je najmanjsi mozen in je ful nul zaradi tega!!
(nimamo popolnoma vedno enake frekvnece(med 30 in 31 sekundami)in zato nam ne more 
freq nastavit in moramo imeti tako obliko!)
http://stackoverflow.com/questions/26342713/changing-time-frequency-in-pandas-dataframe
-PO VEC MERITVAH NI VEC PROBLEMA NICEL !!! CIST LEPO SE PRIKAZE CASOVNICA, PROBLEM
SE SAM RESI! ni nekak odvisno od freknence vseeno, zgleda da bolj od stevila podatkov!!!
in potem on sam scalira časovno os, glede na število podatkov.

"""
import pandas as pd
import pymongo
import matplotlib
import matplotlib.pyplot as plt
matplotlib.style.use('ggplot')
# da vidiš več stilov matplotlib.style.available
# zgon database same in povedat ji moras kje se nahaja(ponavadi vedno enako)
uri = "mongodb://127.0.0.1:27017"
client = pymongo.MongoClient(uri)
# definiraš kot osnovno databaso in potem uporabljas to spremenljivko
db = client.test

# poimenovat collection in ga vzet kot spremenljivko
collection=db.pub
meritve=collection.find()

# collection kot dictionary pretvorimo v list ki nam je dost lazji za obdelavo!
# mongodb sicer shranjuje default v collecion ki ga pa direkt ne moremo prikazat
# ampak ga moramo najprej pretvorit v nek drug bolj uporabno obliko!
# Source: Udemy Course Video
listMeritve=[]
for meritev in meritve:
    listMeritve.append(meritev)
    
# naložimo celotno data v DataFrame od pandas ki ga lahko sedaj obdelujemo za prikaz!
# samo kot posrednik smo uporabili LIST!
# http://stackoverflow.com/questions/20638006/convert-list-of-dictionaries-to-dataframe
data = pd.DataFrame(listMeritve)   

# pretvorimo še objekte stringov v primerno obliko vsakega posebej
# http://stackoverflow.com/questions/33126477/pandas-convert-objectsconvert-numeric-true-deprecated
data.Temperature = pd.to_numeric(data.Temperature)
data.Humidity = pd.to_numeric(data.Humidity)
data.Light = pd.to_numeric(data.Light)
data.Pressure = pd.to_numeric(data.Pressure)
data.Date = pd.to_datetime(data.Date,dayfirst=True)
# dayfirst mu pove da podatki prihajajo z dnevom naprej, pol pa on postavi v dataframe na svoj način
# alternativen zastarel način da convertaš vse skup!
# http://stackoverflow.com/questions/16729483/converting-strings-to-floats-in-a-dataframe
#data = data.convert_objects(convert_numeric=True)
# http://pandas.pydata.org/pandas-docs/stable/generated/pandas.to_datetime.html

# Naredimo nov dataframe kjer je date v indeksu in enostavnejse plotamo!
data1 = pd.DataFrame(listMeritve, index=data.Date)
# http://stackoverflow.com/questions/13411544/delete-column-from-pandas-dataframe
data1 = data1.drop('Date',1)
# http://pandas.pydata.org/pandas-docs/stable/generated/pandas.DataFrame.resample.html

typesOfData = data.dtypes
print typesOfData
print data.describe() # izpiše ti kratko analizo podatkov!

# sama vizualizacija: http://synesthesiam.com/posts/an-introduction-to-pandas.html
# http://pandas.pydata.org/pandas-docs/stable/10min.html
plt.close() # zapremo presnji plot! ce je kaksen.
plt.plot(data1.Temperature)
#plt.plot(data1.Humidity)
#plt.plot(data1.Light)
plt.show()
# od freq od dataframe so odvisne decimalke na plotu!!!
