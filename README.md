#Eine Bienenstockwaage im Eigenbau
##Bee Weight Cell based on Rasperry Pi/Zero + POE + EPaper Display
###17.02.2019 by Randolph Esser

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

findet sich in dem File ** [BeeLogDoc_v1.3.md](https://github.com/mchresse/BeeLog/BeeLogDoc_v1.3.md) **.

Diese Variante ist allerdings aufgrund der benötigten Netzwerkverbindung (LAN oder WiFi) mehr für den Lokalen Einsaz im Garten geeignet.
Eine Mobile Variante mit geringerem Stromverbrauch auf Arduino-ESP32 Basis + LoRaWan Kommunikation folgt im Folgeprojekt: BeeIoT (coming soon).

Viel Spass damit
Randolph Esser



  
