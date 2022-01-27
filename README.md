# pulsed-dc-converter
12-20V DC to Pulsed DC Converter

Transforms 12-20V DC to a kind of pulsed DC, compatible with some Power Tools using phase cutting
control (for example Proxxon Micromot), in the range up to around 40W.

Like that, with the respective Adapter, those small Power Tools can be operated from a USB C PD
Power Bank. Adapter Example: https://aliexpress.com/wholesale?SearchText=power+adapter+usb+c+dc+20v

**Progress:**

 [x] Simple loop works on breadboard with similar schematic as in kicad
 
 [x] PCB designed and ordered
 
 [x] PCB tested and working with 12V/1.5A from USB-C Powerbank and 20V/3A from USB-C Power Supply (with respective USB-C Adapters). Populated parts are mainly as in the schematic, except the Zener(D3) in the flyback path is just bridged with a wire, see [21861574f8b53dd02fe86965473d7c7fb414512f](https://github.com/bluesceada/pulsed-dc-converter/commit/21861574f8b53dd02fe86965473d7c7fb414512f)
 
 [x] Use ADC feedback to increase the ON-time of the transistor to maximize power for powerbanks

**Note:** I still have PCBs ([Rev-0.1](https://github.com/bluesceada/pulsed-dc-converter/releases/tag/Rev-0.1)) available, message me if you are interested to get it for net cost, ~1€ + shipping from DE/Germany 🇩🇪. I also still have 2x attiny85 that I could pre-program if needed. The only real PCB-errata is a missing connection of Attiny Pin 5 (PB0) to R1, which is easily bridged.

<img align="left" src="https://raw.githubusercontent.com/bluesceada/pulsed-dc-converter/main/pcb_kicad/kicad_schematic.png" alt="kicad schematic"/>

<img align="left" src="https://raw.githubusercontent.com/bluesceada/pulsed-dc-converter/main/pcb_kicad/kicad_screenshot.png" alt="kicad pcb"/>

<img align="left" src="https://raw.githubusercontent.com/bluesceada/pulsed-dc-converter/main/pcb_measurements/working_pcb_setup.jpg" alt="real hardware pcb setup picture"/>
