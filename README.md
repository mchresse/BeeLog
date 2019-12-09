# Eine Bienenstockwaage im Eigenbau

## Bee Weight Cell based on Rasperry Pi/Zero + POE + EPaper Display

### 17.02.2019 by Randolph Esser

Eine detaillierte Anleitung zum Eigenbau und Anwendung einer Bienenstockwaage bestehend aus:
- Die Basis: Raspberry Pi 
  * Kompatibel zu 3B oder Zero
- Der Sensorik
  * Einer Bosche Wägezelle 100kg
  * 3 Temperatursensoren DS18B20
- ePaper Display 2.7"
- RJ45 LAN Anschluss(100MBit) zur Kommunication via FTP, HTTP und SSH (PUTTY)
- Stromversorgungoptionen
  * POE support über RJ45 Anschluss
  * USB Port für ein Photovoltaik Modul (inkl. 5V Regler)

findet sich in dem File **[BeeLogDoc_v1.3.md](https://github.com/mchresse/BeeLog/blob/BeeLogDoc_v1.3.md)**.

## Features:
- Alle Sensordaten werden alle 10 Minuten via FTP versendet
- Die Daten werden intern zur statistischen Auswertung zusammengefasst ud plausiblisiert.  
- Dadurch ist Alarmmanagement aufgrund von starken Abweichungen möglich
- Eine Temperaturkompensation ist bereits in der Wägezelle vorgesehen.
- lokale Anzeige wichtigster Betriebsdaten per ePaper Display im Gehäuse eingebaut.
- Performante Kommunikation und SW update remote via LAN Kabel oder lokalem WiFi
- POE Stromversorgung über das Outdoor LAN Kabel
- Sprache der SW Module: C, WebPage: Html + Javascript + Dygraph-lib
- Support eines lokalen Web Service via "Apache Web service" installation

## Installation:
- Download repository to local IDE (e.g. Netbeans)
- required Libs: WiringPi, Curl
- Compile with 
```
-L/usr/lib/arm-linux-gnueabihf  -lcurl -lwiringPi
```

Diese Variante ist allerdings aufgrund der benötigten Netzwerkverbindung (LAN oder WiFi) mehr für den Lokalen Einsaz im Garten geeignet.
Eine Mobile Variante mit geringerem Stromverbrauch auf Arduino-ESP32 Basis + LoRaWan Kommunikation folgt im Folgeprojekt: BeeIoT (coming soon).

Viel Spass damit

Randolph Esser




  
