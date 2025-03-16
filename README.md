## Myrlejon temp sensor

This project was made for fun but also as learning experience for embedded systems, MCU's and building graphics. 

### Parts used -

* ESP32-C3
* DHT22 temperature/humidity sensor
* 3x AA-battery holder
* MCP1700-33 TO-92 voltage regulator (LDO 3.3V)
* 2.13 inch waveshare e-paper display 

### Context

E-paper display is set up from scratch from documentation. The project includes a image rendering library that is made from scratch. Temperature/humidity is read from sensor and converted into numbers on the display every 15 minutes. When the MCU wakes up it reads values, updates the e-paper display and goes back to sleep for optimal battery use. Record values are stored in NVS that is resetted every 24 hours. All of it is contained in a ventilated shell that has been modelled and 3D printed.

![myrlebee sensor](img/myrlejon%20temp%20sensor.jpg)