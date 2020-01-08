# Eine Bienenstockwaage im Eigenbau
## Bee Weight Cell based on Rasperry Pi/Zero + POE + EPaper Display
##### 17.02.2019 by Randolph Esser
(english version coming soon)

Eine detaillierte Anleitung zum Eigenbau und Anwendung einer Bienenstockwaage bestehend aus:
- Der Basis: Raspberry Pi 
  * kompatibel zu 3B oder Zero
- Der Sensorik
  * Einer Bosche Wägezelle 100kg
  * 3 Temperatursensoren DS18B20
- ePaper Display 2.7"
- RJ45 LAN Anschluss(100MBit) zur Kommunication via FTP, HTTP und SSH (PUTTY)
- Stromversorgungoptionen
  * POE support über RJ45 Anschluss
  * USB Port für ein Photovoltaik Modul (inkl. 5V Regler)

findet sich in dem File **[BeeLogDoc_v1.3.md](https://github.com/mchresse/BeeLog/blob/master/BeeLogDoc_v1.3.md)**.

### Features:
- Alle Sensordaten werden alle 10 Minuten via FTP versendet
- Die Daten werden intern zur statistischen Auswertung zusammengefasst ud plausiblisiert.  
- Dadurch ist Alarmmanagement aufgrund von starken Abweichungen möglich
- Eine Temperaturkompensation ist bereits in der Wägezelle vorgesehen.
- lokale Anzeige wichtigster Betriebsdaten per ePaper Display im Gehäuse eingebaut.
- Performante Kommunikation und SW update remote via LAN Kabel oder lokalem WiFi
- POE Stromversorgung über das Outdoor LAN Kabel
- Sprache der SW Module: C, WebPage: Html + Javascript + Dygraph-lib
- Support eines lokalen Web Service via "Apache Web service" installation

Die Stockwaage live im Einsatz findet sich unter http://randolphesser.de/imkerei/index.html
=> Menü: BienenStockwaage

*Zur Anwendung des geladenen Dygraph Diagramms:*
- Mauszeiger über das Diagramm fahren liefert die genaue Messdaten am akt. Punkt
- Mauszeiger auf die Eventpunkte auf den Kurven lieferte weitere Eventinformationen
- Die Zoom Buttons unterhalb liefern Ausschnitte verschiedener Zeitfenster

Die Raspberry Variante ist allerdings aufgrund der benötigten Netzwerkverbindung (LAN oder WiFi) und höherem Stromverbrauch mehr für den lokalen Einsaz im Garten geeignet.
Eine mobile Variante mit geringerem Stromverbrauch auf Arduino-ESP32 Basis + Batterie + LoRaWan Kommunikation ist im Folgeprojekt bereits in der Entwicklung:
**[BeeIoT](https://github.com/mchresse/BeeIoT)** (public status coming soon).


### Installation:
- Download repository to local IDE (e.g. Netbeans)
- required Libs: WiringPi, Curl
- Compile with 
```
-L/usr/lib/arm-linux-gnueabihf  -lcurl -lwiringPi
```

### License:
See the **[LICENSE](https://github.com/mchresse/BeeLog/blob/master/LICENSE)** file for license rights and limitations (New BSD-clause).


Viel Spass damit

Randolph Esser




  
