# HWEP [DE]
 [HWEP] Hardware School Project with my bestest frens

## BSB - Blockschaltbild

![BSB](/docu/v2/BSB_Sequencer_I2S.drawio.svg)

## KUB - Kurzbeschreibung

Der Sequenzer ist ein musisches Instrument, welches vom Benutzer einstellbare Töne nacheinander abspielt und dadurch eine Melodie erzeugt. Die Tonlage wird durch Potentiometer analog eingestellt und anschließend an die chromatische Skala angepasst. Die acht Töne werden mit jeder steigenden Flanke des Takts durchgeschaltet.
 
Mit einem Pause-Taster kann der Zyklus pausiert werden. Mit dem Reset-Taster kann zum Anfang des Zyklus gesprungen werden.
Der CLRR-Taster setzt das Tonleiter Register zurück in den normalen Zustand.
Der Sequencer ist mit zwei Prescalern für Gate-Länge und Taktrate ausgestattet. Dieser teilt den Takt das Gate des Tones durch 1, 2, 4, 8 oder 16.
 
Zum Bedienen des Sequencers gibt es verschiedene Parameter, die mithilfe eines einzigen Dreh-Encoder gesteuert werden können. Der Dreh-Encoder hat einen integrierten Taster, mit welchem man die Parameter, die man verändern will, einstellen kann. Die Veränderung der Parameter wird je nach Modus an verschiedenen Status LEDs und Anzeigen abgezeichnet. Der Event-Taster hat je nach Modus eine andere Funktion.
 
Der Sequencer wird mit dem ESP32 32-bit Microcontroller betrieben. Der Digitale Audioausgang erfolgt über das I²S (Inter-IC-Sound) Protokoll und kann 16-, 24- und 32-bit Sounds bei einer Abtastrate von 44.1kHz übertragen und durch einen I²S-DAC (PCM5102a) direkt als Audio-Signal ausgegeben werden.

## FUB - Funktionsbeschreibung
1. 8 Potentiometer als analoger Eingang:
 - dient als analoger Eingang für die Tonlage der einzelnen Töne
 - jedes Potentiometer umfasst 12 Halbtöne bzw. eine Oktave
 - die analogen Werte einem 8 Channel ADC (ADC088S052) über die SPI Schnittstelle eingelesen
 
2. 16 Status LED’s:
 - 8 EN-LED’s:
   - zeigen an, welcher Ton spielen wird oder stummgeschalten ist (LED aus)
 - 8 Index/Cursor-LED’s:
   - **Index:** zeigen an, welcher Ton gerade spielt
   - **Cursor:** bzw. dient zum Anzeigen eines Cursors
3. 3x 7-Segent Display:
 - nützen zur Anzeige von Parametern
 - werden mit einem MCP23S08 via die SPI Schnittstelle seriell angesteuert
 
4. Dreh-Encoder:
 - zum Einstellen der Parameter der verschiedenen Modi
 - um den Drehencoder befinden sich 12 RBG-LED’s, welche den Rotationstatus anzeigen
 
5. Taster: 
> Jeder Taster wird mittels eines MCP23S08 via SPI eingelesen.
 - **Pause:** pausiert den Zyklus
 - **Reset:** setzt den Zyklus asynchron zurück
 - **Reset Key Register:** setzt die Tonleitereinstellungen auf Default Settings (C4 Moll)
 - **Event:** zum Einstellen der Parameter der verschiedenen Modi
 - **Prescaler:** einstellen von zwei verschiedenen Prescalern für Gate und Takt
   - 5 Taster für jeden Prescaler
 
6. 4 Modi
> Jeder Modus verändert die Anzeigen an den Status LED’s und Displays bzw. die Funktionalität verschiedener Eingaben. Nach wechseln eines Modus wird der aktuelle Modus am 7-Segment Display angezeigt, bis die Anzeige benötigt wird, um einen Parameter anzuzeigen.
 
 - BPM:
   - **7-Segment Display:** zeigt die eingestellten BPM an und die Wellenform
   - **Index/Cursor:** zeigt den Index an
   - **Drehencoder:** zum Verändern der BPM
   - **Event Taster:** Drehencoder verändert die Wellenform
 - Tonleiter Einstellungen:
   - **7-Segment Display:** zeigt den Ton an, der gerade spielt
   - **Index/Cursor:** zeigt den Index an
   - **Drehencoder:** zum Verändern den Tonleiter
   - **Event Taster:** verändert den Modus des Tonleiters (Dur,Moll…)
 - Enable/Reset:
   - **7-Segment Display:**
   - **Index/Cursor:** zeigt den Cursor an
   - **Drehencoder:** bewegt den Cursor. Dort wo der Cursor stehen bleibt, setzt sich der Zyklus zurück.
   - **Event Taster:** stummt/entstummt einen Ton an der Cursorstellung
 
 - Transpose:
   - **7-Segment Display:** zeigt an um wieviel Halbtöne die Melodie transponiert wird
   - **Index/Cursor:** zeigt den Index an
   - **Drehencoder:** zum Verändern der Tonlage
   - **Event Taster:** Drehencoder verändert die Tonlage um eine Oktave
 
7. ESP32 32-Bit Microcontroller:
 - Zentrale Recheneinheit für den Sequenzer
 - Berechnung der Wavetables
 - SPI als Übertragungsprotokoll zur Ansteuerung der internen Peripherien
 - Audio-Output:
   - Digitale Audioübertragung via I²S Protokoll
   - Auswahl zwischen 16-, 24- und 32-bit Audio-Signale mit Abtastrate von 44,1kHz
   - Ein I²S-DAC (PCM5102a) decodiert die Audio-Daten und gibt sie Analog an eine 3,5mm Klinkenbuchse aus.
