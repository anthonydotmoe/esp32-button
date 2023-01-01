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
};
