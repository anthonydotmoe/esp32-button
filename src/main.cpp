// Library Includes
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
//#include <WiFiAP.h>
//#include <ESPmDNS.h>
//#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
//#include <HTTPClient.h>
//#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// Local Includes
#include "buttonconfig.h"
#include "html.h"

// Local Defines
#define FORMAT_SPIFFS_IF_FAILED true

// Global Objects
Adafruit_NeoPixel led(1, 27, NEO_GRB + NEO_KHZ400);
AsyncWebServer web_server(80);
//HTTPClient http_client;
//DNSServer dns_server;
ButtonConfig button_config;

const char *filename = "/config.json";

void led_color(uint8_t r, uint8_t g, uint8_t b);
void load_config(ButtonConfig &config, const char *filename);
int save_config(const char *filename, const ButtonConfig &config);
void print_config(const char *filename);
int init_wifi();
void init_webserver();

void setup() {
  led_color(0, 0, 0);

  // Init SPIFFS
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    log_e("SPIFFS mount failed!");
    led_color(0, 255, 0);
    while(1);
  }
  log_i("SPIFFS initialized");
  
  // Load config from SPIFFS
  load_config(button_config, filename);
  log_i("Loaded configuration");

  // Save configuration (TODO: just because we *may* not have had one before)
  if(!save_config(filename, button_config)) {
    log_e("Failed saving config.");
    led_color(255, 0, 0);
    while(1);
  }
  log_i("Configuration saved");
  
  // Init WiFi
  if(!init_wifi()) {
    log_e("WiFi failed.");
    led_color(100, 0, 0);
    while(1);
  }
  log_i("WiFi initialized");
  
  init_webserver();
}

void loop() {
  // put your main code here, to run repeatedly:
}

/// @brief Set the color of the LED
/// @param n Adafruit_NeoPixel Class
/// @param r Red Brightness, 0 to 255.
/// @param g Green Brightness, 0 to 255.
/// @param b Blue Brightness, 0 to 255.
void led_color(uint8_t r, uint8_t g, uint8_t b) {
  led.setPixelColor(0, ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
  led.show();
  log_v("Set LED color to %d, %d, %d", r, g, b);
  return;
}

void load_config(ButtonConfig &config, const char *filename) {
  File file = SPIFFS.open(filename);
  if(!file) {
    log_e("load_config: Failed to open file!");
  }
  
  // Allocate a temporary JsonDocument
  StaticJsonDocument<768> doc;
  
  // Deserialize the JSON document
  DeserializationError e = deserializeJson(doc, file);
  if(e) {
    log_e("ArduinoJson: %s", e.c_str());
    log_w("load_config: Failed to read file \"%s\", using default configuration", filename);
  }

  // Copy values from the JsonDocument or default values if document is empty
  strlcpy(config.wifi_ssid, doc["wifi_ssid"] | "Setup", sizeof(config.wifi_ssid));
  strlcpy(config.wifi_passwd, doc["wifi_passwd"] | "Password", sizeof(config.wifi_passwd));
  config.wifi_mode = doc["wifi_mode"] | false;
  strlcpy(config.admin_user, doc["admin_user"] | "admin", sizeof(config.admin_user));
  strlcpy(config.admin_pass, doc["admin_pass"] | "admin", sizeof(config.admin_pass));
  strlcpy(config.eap_user, doc["eap_user"] | "", sizeof(config.eap_user));
  strlcpy(config.eap_pass, doc["eap_pass"] | "", sizeof(config.eap_pass));
  strlcpy(config.get_url, doc["get_url"] | "http://example.com", sizeof(config.get_url));
  config.get_tls = doc["get_tls"] | false;

  file.close();
}

int save_config(const char *filename, const ButtonConfig &config) {
  File file = SPIFFS.open(filename, "w");
  if(!file) {
    log_e("save_config: Failed to create file \"%s\"", filename);
    return -1;
  }
  
  // Allocate a temporary document
  StaticJsonDocument<768> doc;

  // Set the values in the document
  doc["wifi_ssid"] = config.wifi_ssid;
  doc["wifi_passwd"] = config.wifi_passwd;
  doc["admin_user"] = config.admin_user;
  doc["admin_pass"] = config.admin_pass;
  doc["eap_user"] = config.eap_user;
  doc["eap_pass"] = config.eap_pass;
  doc["get_url"] = config.get_url;
  
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    log_e("save_config: Failed to write to file!");
    file.close();
    return -1;
  }
  
  file.close();
  return 0;
}

void print_config(const char *filename) {
  File file = SPIFFS.open(filename);
  if(!file) {
    log_e("Filed to open file");
    return;
  }
  
  // Extract each character one by one
  while(file.available())
    Serial.print((char)file.read());
  
  file.close();
  return;
}

int init_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(button_config.wifi_ssid, button_config.wifi_passwd);
  
  for(int i = 0; (i < 60) || (WiFi.status() != WL_CONNECTED); i++) {
    delay(500);
  }
  
  if(WiFi.status() != WL_CONNECTED) {
    log_w("Did not connect to WiFi in 30 seconds, enabling setup mode");
    log_e("Setup mode needs to be implemented...");
    while(1);
    return -1;
  } else {
    log_i("WiFi connected\nSSID: %s\nIP: %s", button_config.wifi_ssid, WiFi.localIP().toString());
    return 0;
  }
}

String template_processor(const String& var) {
  if(var == "TITLE")
    return "Title text here";

  if(var == "CONFIG_GET_URL")
    return button_config.get_url;

  if(var == "BUTTON_STATE")
    return "Button state here";
    
  if(var == "HTTP_CHECKED")
    return button_config.get_tls ? String() : "checked";
  
  if(var == "HTTPS_CHECKED")
    return button_config.get_tls ? "checked" : String();
  
  if(var == "PSK_CHECKED")
    return button_config.wifi_mode ? String() : "checked";
  
  if(var == "EAP_CHECKED")
    return button_config.wifi_mode ? "checked" : String();
  
  if(var == "CONFIG_WIFI_SSID")
    return button_config.wifi_ssid;
  
  if(var == "CONFIG_EAP_USER")
    return button_config.eap_user;
  
  // If a template value isn't found, return empty string
  return String();
}

void init_webserver() {
  web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *r) {
    r->send_P(200, "text/html", index_html, template_processor);
  });
  
  web_server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *r) {
    r->send_P(200, "text/html", setup_html, template_processor);
  });
  
  web_server.begin();
}