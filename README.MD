# MagicPoi PlatformIO Program

PlatformIO based firmware for MagicPoi Lite, an IOT device that displays LED patterns based on timeline data retrieved from a server. The program manages network connections, authentication, and LED pattern updates.

## Table of Contents

- [Introduction](#introduction)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [License](#license)

## Introduction

MagicPoi is an Arduino program designed to control MagicPoi hardware. It connects to Wi-Fi, authenticates with a server, and displays dynamic LED patterns based on timeline data received from the server.

## Requirements

To run this program, you will need the following:

- Arduino-compatible hardware (e.g., ESP8266-based board)

## Getting Started

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/tomjuggler/magicpoi_lite_firmware.git

   ```

2. Setting up Environment Variables (Important):

    Create a file named secrets.h in the include directory. In secrets.h, define the following constants with your actual credentials:

   ```c++
    #define WIFI_SSID "Your-WiFi-SSID"
    #define WIFI_PASS "Your-Wi-Fi-Password"
    #define SERVER_IP "magicpoi.circusscientist.com"; // server address, don't change this
    #define USER "your-magicpoi-username"
    #define PASS "your-magicpoi-password"
    ```
    These credentials should not be stored directly in your code to ensure security.
3. Open VSCode with PlatformIO and load the MagicPoi Lite Firmware

4. Upload the program. Currently only D1 mini (ESP8266) is supported. 

## Usage

- The program will connect to the Wi-Fi network using the variables for SSID and password defined in secrets.h

- It will authenticate with the MagicPoi server using variables for the username and password defined in secrets.h. Requires a free account sign-up at http://magicpoi.circusscientist.com

- It will retrieve timeline and colour sequence data from the server and display LED patterns on the MagicPoi Lite hardware.

- To customize LED patterns or add additional functionality, refer to the program's source code or see the [documentation](https://devsoft-co-za.github.io/magicpoi-lite-firmware/)

## License

This program is open-source software released under the GNU General Public License Version 3 (GPL-3.0). You can find the full license text [here](https://www.gnu.org/licenses/gpl-3.0.en.html).