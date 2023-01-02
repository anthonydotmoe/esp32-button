#pragma once

/// @brief struct representation of the SPIFFS config file read by the program
struct ButtonConfig {
    // WiFi SSID and PSK
    char wifi_ssid[33];
    char wifi_passwd[64];
    
    // WiFi security mode
    // 0: PSK, 1: PEAP-MSCHAPv2
    bool wifi_mode;
    
    // admin page username and password
    char admin_user[16];
    char admin_pass[64];
    
    // EAP username and password
    char eap_user[16];
    char eap_pass[64];
    
    // GET endpoint address and mode
    char get_url[256];
    // 0: HTTP, 1: HTTPS
    bool get_tls;
    
    // Button pin number
    int button_pin;
    // 0: Normally open, 1: Normally closed
    bool button_nonc;
};

/// @brief Default configuration for this project
const ButtonConfig DEFAULT_BUTTON_CONFIG = {
    "",	        // wifi_ssid
    "",	        // wifi_passwd
    0,	        // wifi_mode
    "admin",	// admin_user
    "admin",	// admin_pass
    "",	        // eap_user
    "",	        // eap_pass
    "",	        // get_url
    0,	        // get_tls
    37,	        // button_pin
    0	        // button_nonc
};