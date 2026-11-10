# Standalone keyboard
This is my project for third week Half Life, a keyboard powered with a ESP32 connected to a power bank that uses a speaker to play 3 diferent sounds (acoustic, electric and organ). That's what makes it standalone!

I enjoyed very much making this project (specialy the PCB). For me, the most difficult part, was making the code so it can properly function.

# PCB Schematic:
<img width="1169.5" height="827" alt="Schematic_standalone_keyboard" src="https://github.com/user-attachments/assets/49da876b-3dab-4ec5-ac18-89215c3d9e5d" />
<img width="618.5" height="303" alt="Captura de pantalla 2026-10-25 212012" src="https://github.com/user-attachments/assets/97700b54-d6af-4df5-a1c8-d14b4bae77e6" />

# Some PCB Renders:

Front:
<img width="4288" height="2296" alt="Keypad_Front" src="https://github.com/user-attachments/assets/1ca90d4e-e7d5-4a82-b9af-1a98614de189" />

Back:
<img width="4288" height="2296" alt="Keypad_Back" src="https://github.com/user-attachments/assets/0543b50f-66c2-415c-887f-6703e26f1966" />

# Bill Of Materials

| Item | Qty | Supplier | Cost/item | Total Cost | link |
| --- | --- | --- | --- | --- | --- |
| PCB | 1 | JLCPCB | $5.00 | $5.00 | [JLCPCB](https://jlcpcb.com/) |
| Speaker | 1 | ElectroHobby | $1.66 | $1.66 | [ElectroHobby](https://www.electrohobby.es/sonido/305-altavoz-2w-28mm.html) |
| Amplifier | 1 | ElectroHobby | $2.78 | $2.78 | [ElectroHobby](https://www.electrohobby.es/sonido/984-max98357-amplificador-i2s.html) |
| Rotary Encoder | 1 | ElectroHobby | $1.28 | $1.28 | [ElectroHobby](https://www.electrohobby.es/entradassalidas/454-modulo-encoder-ky-040.html) |
| ESP32-S3-Zero | 1 | ElectroHobby | $5.00 | $5.00 | [ElectroHobby](https://www.electrohobby.es/esp32/957-esp32-c3-zero.html) |
| MPR121 | 1 | Amazon ES | $7.80 | $7.80 | [Amazon ES](	https://amzn.eu/d/0ajJMPyk) |
| 40x Blue Pin Header | ElectroHobby | $0.33 | $0.33 | [ElectroHobby](https://www.electrohobby.es/tiras-de-pines/236-tira-40-pin-macho-azul.html) |
| **Estimated total shipping** | **1** | — | — | **$3.49** | — |
| **Total Cost** | **1** | — | — | **$27.34** | — |

# Desing Process

I designed the PCB with KiCad (the only PCB design software i know how to use) following some parts from the starter guide to have a base idea. I wanted to make a case for it but i think it will look cooler to have it raw. A sleek black PCB will look very cool.

# How To Assemble

1. First grab the PCB and verify if it's clean and well-made.
2. Grab the ESP32, the MPR121 and the Amplifier and solder the pin headers. For the ESP32 it's two 9 pin header, for the MPR121, it's a 6 pin header and a 12 pin header and finaly for the amplifier it's two one pin header and a 7 pin header.
3. Weld the ESP32, the MPR121, the Amplifier, the JST connector for the speaker, the three switches and the rotary encoder to the PCB
4. There's two parts on the pack of the JST connector. One it's welded on the PCB. The other you have to solder it to the who cables of the speaker. **MAKE SURE THE RED WIRE GOES TO THE UPPER PART OF THE JST CONNECTOR**
5. Hot glue the speaker to the PCB

# How to upload the firmware

1. Open the Arduino IDE
2. Open **File → Preferences**. In **Additional Boards Manager URLs**, add:

   ~~~text
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ~~~

3. Open **Tools → Board → Boards Manager**, search for esp32, and install **esp32 by Espressif Systems**.
4. Under **Tools → Board → esp32**, select **Waveshare ESP32-S3-Zero**.
5. Open **Sketch → Include Library → Manage Libraries**. Install this library:

   - Adafruit MPR121

   If Arduino asks to install dependencies, choose **Install All**.

6. Connect the ESP32 to the computer.
7. Open keyboard_code.ino and click Arduino's arrow-shaped **Upload** button to upload the firmware to the board.

# Controls

| Control (from left to right) | What it does |
| --- | --- |
| First button | Plays with the Acoustic sound |
| Second button | Plays with the Electric sound |
| Third button | Plays with the Organ sound |
| Rotary encoder | Control the volume |
