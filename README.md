# TKN Programmierpraktikum: Hardware

Unsere Aufgabe ist es, mithilfe von mehreren Mikrokontroller-Einheiten ToF (Time of Flight) Messungen zu einer anderen Mikrokontroller-Einheit zu sammeln. Aus diesen Messungen kann vom Datenteam später die Position dieser Einheit berechnet werden. Eine Einheit besteht aus einem [Adafruit Feather m0](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module) und einem [Decawave DW3000](https://www.decawave.com/product/decawave-dw3000-ic/). Der DW3000 macht die Messungen mit +/- 10cm Genauigkeit möglich, der Feather m0 steuert den DW3000 und ist für den Austausch von Befehlen und Daten mit dem Data Team zuständig.

## User Guide

Im Folgenden findet ihr die Informationen und Schritte, um das System auf der Hardware-Seite einsatzbereit zu machen.

---

### Toolchain Setup

1. Installiert [VS Code](https://code.visualstudio.com/) und die [PlatformIO Extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

2. Öffnet unsere Repo in VS Code und geht über die PlatformIO Sidebar auf `PIO Home > Projects & Configuration`. Falls unser Projekt dort nicht auftaucht, klickt auf `Add Existing` und wählt es darüber aus. Wenn alles funktioniert hat, sollte es jetzt in der Sidebar eine Sektion `PROJECT TASKS` geben.

---

### Hardware Setup

Um den DW3000 mit einem separaten Mikrokontroller steuern zu können, befindet er sich auf einem weiterem [DWM3000 Breakout Board](https://www.decawave.com/product/dwm3000-module/), welches Pin-Anschlüsse zur Verfügung stellt.
In der unteren Tabelle steht, welcher Pin auf dem DWM3000 mit welchem Pin auf dem Feather verbunden werden muss.

| DWM3000   | Feather m0 |
|-----------|------------|
| 1         | 11         |
| 2         | 3V         |
| 3         | MOS        |
| 4         | GND        |
| 5         | MIS        |
| 7         | SCK        |
| 9         | 12         |
| 11        | 10         |

Der Feather benötigt außerdem einen Stromanschluss. Dafür könnt ihr entweder ein Micro USB Kabel oder eine für den links über dem Micro USB Port liegenden Anschluss kompatible Batterie benutzen. Wenn der Feather Strom bekommt, sollte die rote LED anfangen zu leuchten.

---

### Firmware Upload

Wir gehen im Folgenden davon aus, dass ihr auf einem Mac oder Linux-System seid. Auf Windows sollte es auch funktionieren, haben wir aber nicht getestet, deswegen können wir dafür keine Garantien machen.
Falls ihr ein Linux-System habt, befolgt zuerst [diese Schritte](https://docs.platformio.org/en/latest/faq.html#platformio-udev-rules) und macht erst dann weiter.

Um unseren Code auf den Feather hochzuladen, verbindet den Feather über ein USB-Kabel mit eurem Computer. Stellt sicher, dass das USB-Kabel ein Datenkabel ist! Geht dafür über die PlatformIO Sidebar auf `PIO Home > Devices`. Drückt ein paar Sekunden nach dem Einstecken auf `Refresh`, dann sollte der Feather angezeigt werden.

Klickt zum Hochladen auf `PROJECT TASKS > tracker1_coordinator/tracker2/tracker3/trackee1 > Upload`. Es sollte immer nur ein Feather gleichzeitig verbunden sein. Wenn ihr trotzdem mehrere Feathers gleichzeitig angeschlossen haben wollt, müsst ihr in `platformio.ini` den [`upload_port`](https://docs.platformio.org/en/latest/projectconf/section_env_upload.html#id1) selbst angeben.

Wenn ihr mehr als drei Tracker haben wollt, müsst ihr dafür in `platformio.ini` neue Environments erstellen. Achtet darauf, dass jedes Environment eine unterschiedliche `UNIT_ID` hat. Es sollte außerdem nur einen `COORDINATOR` und einen `TRACKEE` geben.

---

### Inbetriebnahme

Verbindet den Feather, auf dem ihr das `tracker1_coordinator` Environment hochgeladen habt, mit einem Micro USB Kabel mit dem Computer, auf dem der Code des Data Teams läuft. Hier ist wieder wichtig, dass das Kabel ein Datenkabel ist. Für alle anderen Feathers ist es nur wichtig, dass sie Strom bekommen.

---

### Troubleshooting

Wenn keine Messungen ankommen, liegt es in unserer Erfahrung eigentlich immer daran, dass ein Kabel falsch oder nicht richtig eingesteckt ist. Ein eindeutiges Zeichen dafür ist, dass der Feather anfängt zu blinken.
Ihr solltet daraus aber nicht schließen, dass alles in Ordnung ist, wenn der Feather nicht blinkt. Wir können leider nicht jede falsche Konfiguration in der Software erkennen.

Wenn das Problem ein loses oder falsch eingestecktes Kabel war, müsst ihr den Feather neustarten. Drückt dafür ein Mal auf den Reset-Knopf rechts über dem Micro USB Anschluss.

Ansonsten kann es sein, dass die Distanz zwischen zwei DWs einfach zu groß ist. Wir haben mit maximal 15m getestet, theoretisch sollten aber bis zu 40m möglich sein.

Achtet außerdem immer darauf, dass die DWs richtig ausgerichtet sind. Dafür sollte vor der grünen Vorderseite des DWM3000 Breakout Boards kein Hindernis den Sichtweg blockieren.

---

### Weiteres

Falls ihr das System alleinstehend ohne den Code der anderen Teilgruppen testen wollt, erstellt zuerst ein [Python Virtual Environment]((https://docs.python.org/3/library/venv.html)) und installiert darin [`rich`](https://rich.readthedocs.io/en/stable/introduction.html) und [`pyserial`](https://pyserial.readthedocs.io/en/latest/pyserial.html).

Wenn ihr danach `src/data_team_interface.py` ausführt, werden 100 Messungen gemacht und einige Statistiken dazu ausgegeben. Stellt vorher sicher, dass in `SERIAL_PORT` der für euch richtige Port steht.