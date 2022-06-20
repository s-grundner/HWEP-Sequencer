# HWEP - Sequencer [DE]
[![Built with: platformio](https://img.shields.io/badge/built%20with-platformio-orange)](https://platformio.org/)

![Frontpanel](/docu/assets/Frontpanel.svg)

Der **Sequencer** ist ein Schulprojekt der [HTBLuVA Salzburg](http://www.htl-salzburg.ac.at/startseite.html) aus dem Fach Hardware Entwicklung und wurde von den Schülern aus der Abteilung der [Höheren Elektronik](http://www.htl-salzburg.ac.at/elektronik-technische-informatik.html), [Simon Grundner](https://github.com/s-grundner) und [Hanna Ihninger](https://github.com/h-ihninger) Entwickelt. 

|           Neueste Renderung der Platine         |            Assemblierter Prototyp               |
|:-----------------------------------------------:|:-----------------------------------------------:|
|<img src="/docu/assets/render.png" alt="render" width="700"/>|<img src="/docu/assets/prot.jpg" alt="prototype" width="700"/>|


## Projektstatus
Das Projekt ist derzeit unvollständig, dennoch Funktionstüchtig. Alle Angeführten Features sind verfügbar. CAD dateien e.g. Gehäuse sind noch nicht im Projekt inkludiert.
Um die Diagramme in der Dokumentation gut erkennen zu können, wird empfohlen, das Dark-Mode-Theme von GitHub zu Verwenden.

## Repository Struktur

 - [**Hardware DOCS**](manufacturing/hwep_sequencer_v2)

 - [**Software DOCS**](src/SRC_HWEP_Sequencer_V2)

## KUB - Kurzbeschreibung

Der Sequenzer ist ein musisches Instrument, welches vom Benutzer einstellbare Töne nacheinander abspielt und dadurch eine Melodie erzeugt. Die Tonlage wird durch Potentiometer analog eingestellt und anschließend an die chromatische Skala angepasst. Die acht Töne werden mit jeder steigenden Flanke des Takts durchgeschaltet.

Mit einem Pause-Taster kann der Zyklus pausiert werden. Mit dem Reset-Taster kann zum Anfang des Zyklus gesprungen werden. Der "Show Key" Taster zeigt bei Betätigung die Gespielten Noten am Display an. Der Sequencer ist mit zwei Prescalern für Gate-Länge und Taktrate ausgestattet. Dieser teilt den Takt oder das Gate des Tones durch 1, 2, 4, 8 oder 16. Außerdem verfügt der Sequencer über eine Umschalttaste (Shift), die das Funktionsspektrum erweitern. Die Dokumentation für die Taster befindet sich in der Funktionsbeschreibung.

Zum Bedienen des Sequencers gibt es verschiedene Parameter, die mithilfe eines Dreh-Encoders gesteuert werden können. Der Dreh-Encoder hat einen integrierten Taster, mit welchem man die Parameter, die man verändern will, einstellen kann. Die Veränderung der Parameter wird je nach Modus an verschiedenen Status LEDs und Anzeigen abgezeichnet.

Der Sequencer wird mit dem einem RTOS am ESP32 32-bit Microcontroller betrieben. Der Digitale Audioausgang erfolgt über das I²S (Inter-IC-Sound) Protokoll und kann 16-Bit Sounds bei einer Abtastrate von 44.1kHz übertragen und durch einen I²S-DAC (PCM5102a) direkt als Audio-Signal ausgegeben werden.

## FUB - Funktionsbeschreibung

![Button_desc](/docu/assets/Frontpanel_nums_dark.drawio.svg)

#### 1. 8 Potentiometer als analoger Eingang:
 - dient als analoger Eingang für die Tonlage der einzelnen Töne
 - jedes Potentiometer umfasst 24 Halbtöne bzw. zwei Oktaven
 - die analogen Werte einem 8 Channel ADC (ADC088S052) über die SPI Schnittstelle eingelesen

#### 2. 16 Status LED’s:
 - 8 EN-LED’s:
   - zeigen an, welcher Ton spielen wird oder stummgeschalten ist (LED aus)
 - 8 Index/Cursor-LED’s:
   - **Index:** zeigen an, welcher Ton gerade spielt
   - **Cursor:** dient zum Anzeigen eines Cursors

#### 3. 3x 7-Segent Display:
 - nützen zur Anzeige von Parametern
 - werden mit einem MCP23S08 via die SPI Schnittstelle seriell angesteuert
 
#### 4. Dreh-Encoder:
 - zum Einstellen der Parameter der verschiedenen Modi
 - um den Drehencoder befinden sich 12 RGB-LED’s, welche den Rotationstatus anzeigen
 
#### 5. Taster: 
> Jeder Taster wird mittels eines MCP23S08 via SPI eingelesen.
 - **Pause:** pausiert den Zyklus
 - **Reset:** setzt den Zyklus asynchron zurück
 - **Show Keys:** zeigt die gespielten Noten am Display
 - **Shift:** zum Einstellen der Parameter der verschiedenen Modi
 - **Prescaler:** einstellen von zwei verschiedenen Prescalern für Gate und Takt
   - 5 Taster für jeden Prescaler
   - Tasten werden auf 4-Bit Priorität-Encodiert
 
#### 6. 4 Modi
> Jeder Modus verändert die Anzeigen an den Status LED’s und Displays bzw. die Funktionalität verschiedener Eingaben. Nach wechseln eines Modus wird der aktuelle Modus am 7-Segment Display angezeigt, bis die Anzeige benötigt wird, um einen Parameter anzuzeigen.
 
 - BPM:
   - **7-Segment Display:** zeigt die eingestellten BPM bzw. die Wellenform an
   - **Index/Cursor:** zeigt den Index an
   - **Drehencoder:** zum Verändern der BPM
   - **Shift Taster:** Drehencoder verändert die Wellenform
 - Tonleiter Einstellungen:
   - **7-Segment Display:** zeigt den Tonleiter bzw. den Modalen Tonleiter an.
   - **Index/Cursor:** zeigt den Index an
   - **Drehencoder:** zum Verändern den Tonleiter
   - **Shift Taster:** verändert den Modus des Tonleiters (Dur,Moll…)
 - Enable/Reset:
   - **7-Segment Display:**
   - **Index/Cursor:** zeigt den Cursor an
   - **Drehencoder:** bewegt den Cursor. Dort wo der Cursor stehen bleibt, setzt sich der Zyklus zurück.
   - **Shift Taster:** stummt/entstummt einen Ton an der Cursorstellung
 
 - Transpose:
   - **7-Segment Display:** zeigt an um wieviel Halbtöne die Melodie transponiert wird
   - **Index/Cursor:** zeigt den Index an
   - **Drehencoder:** Transponiert die Tonlage um einen Halbton
   - **Shift Taster:** Drehencoder verändert die Tonlage um eine Oktave
 
#### 7. ESP32 32-Bit Microcontroller:
 - Zentrale Recheneinheit für den Sequenzer
 - Berechnung der Wavetables
 - SPI als Übertragungsprotokoll zur Ansteuerung der internen Peripherien
 - Audio-Output:
   - Digitale Audioübertragung via I²S Protokoll
   - 16-bit Audio-Signale mit Abtastrate von 44,1kHz
   - Ein I²S-DAC (PCM5102a) decodiert die Audio-Daten und gibt sie Analog an eine 3,5mm Klinkenbuchse aus.
   - Kompatibel mit Trinity Endstufe 
