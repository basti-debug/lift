*** 

![[liftfoto.png]]

Projektmitglieder: 
* Sebastian Mayrhofer / *zuständig für Hardware und Dokumentation*
* Joel Rupp / *zuständig für Software*

 **HTL Rankweil**  4chel                                                                 Schuljahr 2022-23 

***

[TOC]

***

## Aufgabenstellung 

Im Projekt "Liftsteuerung" wird wie der Name schon verrät, das Konzept einer Liftsteuerung in Miniatur umgesetzt. Der Lift soll mit Hilfe von Lego Componenten gesteuert werden und mit Hilfe einer Lichtschranke soll die Position der Kabine herausgefunden werden. Ebenfalls werden zwei Hall Sensoren als Endstops genutzt. 


## Umsetzung

Uns wurde ein sehr minimalistischer und filigraner Liftschacht ohne Technik oder der Gleichen gegeben, wir haben den Liftschacht verstärkt und der Kabine Rollen für ein stabileres auf und ab fahren zu ermöglichen. 

### Benutztes Material

* Lego-Steine  (Lift Schacht und Kabine) 
* MEGACard  (Steuerung des Motors, auswertung der Sensoren)
* Hall Sensoren (End-Stops)
* Lichtschranke (Stockwerkbestimmung)


### Stockwerk-bestimmung (Lichtschranke)

Wir nutzen eine Lichtschranke, die von einem groben Zahnrad auf der Antriebswelle durchbrochen wurde, um zu Messen um wieviel Umdrehungen die Antriebsachse zurück gelegt hat. 

**REFERENZ CODE!!**

**FOTOS MODEL!!**

![[endstopfoto.jpg]]

**Lichtschranke Bild!!**

### Endstops 

Mit Hilfe von Hall Sensoren im obersten und im untersten Geschoss wurde ein Endstop realisiert. Dieser war ein nützliches Tool um die Position, nach einem Reset bzw Neustart der MegaCard, zu bestimmen. 

**Foto Endstops!!**

Wiring

![[wiring.png]]

