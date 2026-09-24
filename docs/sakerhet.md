# Security Analysis

## 1. Overview

Security is important because the ESP32, MQTT broker and API are communicating over a network.

I have added some basic security measures to protect the system and to avoid accepting incorrect data.

The main security measures I used are:
- MQTT username and password
- Keeping passwords outside GitHub
- Input validation
- Using a local network during development

---

## 1. MQTT Authentication

The MQTT broker does not allow anonymous users.

The ESP32 and the Python API connect to the MQTT broker using a username and password.

The MQTT username is: esp32client
The password is not written directly in the main source code.

This helps prevent unknown devices from connecting anonymously to the MQTT broker.

---

## 2. Protecting Passwords

The Wi-Fi password and MQTT password are stored in: main/secrets.h
The secrets.h file is added to .gitignore

This means the file should not be uploaded to GitHub.
The passwords are therefore kept locally and are not included in the public source code.
If another person wants to run the project, they need to create their own secrets.h file with their own credentials.

---

## 3. Input Validation

The system also checks the sensor data before accepting it.
The Python API checks that the MQTT message contains valid JSON.
It also checks the temperature and humidity values.
The accepted ranges are: 
- Temperature: -40 °C to 80 °C
- Humidity:     0% to 100%
This helps protect the API from incorrect or unexpected values.

---

## 4. Security Risks

### Risk 1: Unauthorized MQTT access

Someone on the network could try to connect to the MQTT broker.
To reduce this risk, MQTT username and password authentication is enabled and anonymous access is disabled.

### Risk 2: Passwords being exposed

If the Wi-Fi or MQTT passwords were written directly in the source code and uploaded to GitHub, someone could see them.
To reduce this risk, the passwords are stored in secrets.h and the file is excluded using .gitignore.

### Risk 3: Invalid sensor data

Someone or something could send incorrect data to the MQTT topic.
The Python application therefore checks the JSON and the sensor value ranges before storing the data.
Invalid data is rejected and logged.

---

## 5. MQTT and TLS

The current MQTT connection uses:mqtt://1883
There is currently no TLS encryption.
This means that although the MQTT broker requires authentication, the MQTT communication itself is not encrypted.
For a production system, I would improve this by using MQTT over TLS and a secure port.

---

## 6. Network Security

The MQTT broker is currently running on my computer and is used on the local development network.
The IP addresses are local and can change when using another network.

---