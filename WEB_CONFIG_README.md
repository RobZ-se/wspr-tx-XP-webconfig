# ESP32 WSPR Web Configuration System

## Introduction
The ESP32 WSPR Web Configuration System is designed for managing WSPR (Weak Signal Propagation Reporter) transmissions using an ESP32 microcontroller. This system provides a user-friendly web interface that enables easy setup and management of WSPR parameters.

## Features
- **Web-based configuration:** Access the configuration interface through a web browser.
- **WSPR mode settings:** Easily modify transmission settings specific to WSPR.
- **Real-time monitoring:** View current status and parameters of the WSPR transmissions in real-time.
- **User-friendly interface:** Simplified navigation and configuration options.

## Requirements
- **Hardware:**
  - ESP32 microcontroller
  - Power source (USB or battery)
- **Software:**
  - Arduino IDE or PlatformIO for compiling the firmware
  - Libraries: necessary libraries for Wi-Fi and WSPR functionality

## Installation
1. **Clone the repository:**
   ```bash
   git clone https://github.com/RobZ-se/wspr-tx-XP-webconfig.git
   cd wspr-tx-XP-webconfig
   ```
2. **Open the project in Arduino IDE or PlatformIO.**
3. **Install the required libraries.**
4. **Configure the Wi-Fi settings in the source code.**
5. **Upload the firmware to your ESP32.**

## Configuration
Once the firmware is uploaded:
1. Connect your computer or smartphone to the ESP32's Wi-Fi network.
2. Open a web browser and navigate to `http://192.168.4.1`.
3. Use the web interface to configure the WSPR settings.

## Usage
After configuration:
1. Ensure the WSPR configuration settings are correct.
2. Start the WSPR transmissions from the web interface.
3. Monitor real-time data and logs through the interface.

## Troubleshooting
- **Cannot connect to ESP32 Wi-Fi:**
  - Ensure the ESP32 is powered on and properly programmed.
- **Settings not saving:**
  - Double-check that you've correctly configured the web interface.

## Contribution
Contributions are welcome! Please fork the repository and submit a pull request for any improvements or features.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

## Contact
For any inquiries, please contact the repository owner, RobZ-se.