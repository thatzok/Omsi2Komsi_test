# Omsi2Komsi
Based on a DLL framework from BBI2 (Bus Board Interface 2)<br>

This is Work in Progress.

Omsi2Komsi is a Plugin (Win32 DLL) for the OMSI 2 Bus Simulator.<br>

Omsi2Komsi reads several internal variables (for speed, lamps, etc.) from OMSI 2 and sends them to the serial port (USB) using the KOMSI protocol.

An Arduino/ESP32 or similar connected to the USB port can then read these messages and display the data on a bus dashboard (e.g. speed on a speedometer, lamp lighting, etc.).

