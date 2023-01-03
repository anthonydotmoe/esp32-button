# ESP32 Button

This is a refactoring of my friend's project to create a configurable WiFi
button using an ESP32 microcontroller.

### Desired features:

  - [x] Store all configuration on internal flash
  - [x] Allow configuration to be updated via web interface
  - [x] If connecting to the configured AP fails, start a "SETUP" AP
    - [ ] Setup AP uses a captive portal to direct the user to the setup page
  - [ ] User configurable WebHook to fire when the button is pressed
  - [x] User configurable HTTP GET request when the button is pressed 
  - [ ] Should be platform agnostic. i.e. allow different ESP32 boards to be
        used with the button attached to any pin, or be active high or low.

### Warning:

This project is incomplete and **insecure!** There's no CA certificate bundles,
passwords aren't hashed, the web interface doesn't support TLS, etc.
