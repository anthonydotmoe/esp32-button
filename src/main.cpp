// Library Includes
#include <Arduino.h>
#include <SPIFFS.h>
//#include <WiFiAP.h>
#include <ESPmDNS.h>
//#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
//#include <HTTPClient.h>
#include <WiFiClientSecure.h>
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
bool load_config(ButtonConfig &config, const char *filename);
bool save_config(const char *filename, const ButtonConfig &config);
void print_config(const char *filename);
bool init_wifi();
void init_webserver();

void setup() {
  Serial.begin(115200);
  led_color(0, 0, 0);

  // Init SPIFFS
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    log_e("SPIFFS mount failed!");
    led_color(0, 255, 0);
    while(1);
  }
  log_i("SPIFFS initialized");
  
  // Load config from SPIFFS
  if(load_config(button_config, filename) == false) {
    if(save_config(filename, button_config)) {
      log_e("Failed saving config!");
      led_color(255, 0, 0);
      while(1);
    }
  }
  log_i("Loaded configuration");
  
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
/// @param r Red Brightness, 0 to 255.
/// @param g Green Brightness, 0 to 255.
/// @param b Blue Brightness, 0 to 255.
void led_color(uint8_t r, uint8_t g, uint8_t b) {
  led.setPixelColor(0, led.Color(r, g, b));
  //led.setPixelColor(0, ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
  led.show();
  log_d("Set LED color to %d, %d, %d", r, g, b);
  return;
}

/// @brief Load configuration from SPIFFS file, or from default values
/// @param config (dest) ButtonConfig object
/// @param filename (src) config file name
/// @return True: Config loaded from file, False: Config loaded from defaults
bool load_config(ButtonConfig &config, const char *filename) {
  File file = SPIFFS.open(filename);
  if(!file) {
    log_e("load_config: Failed to open file!");
  }
  
  // Allocate a temporary JsonDocument
  DynamicJsonDocument doc(1024);
  
  // Deserialize the JSON document
  DeserializationError e = deserializeJson(doc, file);
  
  // Close file handle
  file.close();

  if(e) {
    log_e("ArduinoJson: %s", e.c_str());
    log_w("load_config: Failed to read file \"%s\", using default configuration", filename);
    button_config = DEFAULT_BUTTON_CONFIG;
    return false;
  }

  // Copy values from the JsonDocument or default values if document is empty
  strlcpy(config.wifi_ssid, doc["wifi_ssid"], sizeof(config.wifi_ssid));
  strlcpy(config.wifi_passwd, doc["wifi_passwd"], sizeof(config.wifi_passwd));
  config.wifi_mode = doc["wifi_mode"];
  strlcpy(config.admin_user, doc["admin_user"], sizeof(config.admin_user));
  strlcpy(config.admin_pass, doc["admin_pass"], sizeof(config.admin_pass));
  strlcpy(config.eap_user, doc["eap_user"], sizeof(config.eap_user));
  strlcpy(config.eap_pass, doc["eap_pass"], sizeof(config.eap_pass));
  strlcpy(config.get_url, doc["get_url"], sizeof(config.get_url));
  config.get_tls = doc["get_tls"];
  config.button_pin = doc["button_pin"];
  config.button_nonc = doc["button_nonc"];
  
  return true;
}

/// @brief Saves data from a ButtonConfig object to JSON file
/// @param filename (dest) config file name
/// @param config (src) ButtonConfig object
/// @return True: Successful file save, False: File save unsuccessful
bool save_config(const char *filename, const ButtonConfig &config) {
  File file = SPIFFS.open(filename, "w");
  if(!file) {
    log_e("Failed to create file \"%s\"", filename);
    return false;
  }
  
  // Allocate a temporary document
  DynamicJsonDocument doc(1024);

  // Set the values in the document
  doc["wifi_ssid"] = config.wifi_ssid;
  doc["wifi_passwd"] = config.wifi_passwd;
  doc["admin_user"] = config.admin_user;
  doc["admin_pass"] = config.admin_pass;
  doc["eap_user"] = config.eap_user;
  doc["eap_pass"] = config.eap_pass;
  doc["get_url"] = config.get_url;
  doc["get_tls"] = config.get_tls;
  doc["button_pin"] = config.button_pin;
  doc["button_nonc"] = config.button_nonc;
  
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    log_e("Failed to write to file!");
    file.close();
    return false;
  }
  
  file.close();
  return true;
}

void print_config(const char *filename) {
  File file = SPIFFS.open(filename);
  if(!file) {
    log_e("Failed to open file for reading");
    return;
  }
  
  // Extract each character one by one
  while(file.available())
    Serial.print((char)file.read());
  
  file.close();
  return;
}

bool init_wifi() {
  IPAddress local_ip;
  WiFi.mode(WIFI_STA);
  WiFi.begin(button_config.wifi_ssid, button_config.wifi_passwd);
  log_i("Connecting...");
  
  int counter = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    counter++;
    if(counter > 60) {
      log_w("Station failed to associate");
      WiFi.disconnect();
      WiFi.softAP("SETUP", NULL);
      break;
    }
  }

  return true; 
  
  /*
  if(WiFi.status() != WL_CONNECTED) {
    log_w("Did not connect to WiFi in 30 seconds, enabling setup mode");
    log_e("Setup mode needs to be implemented...");
    while(1);
    return false;
  } else {
    log_i("WiFi connected\nSSID: %s\nIP: %s", button_config.wifi_ssid, WiFi.localIP());
    return true;
  }
  */
}

String template_processor(const String& var) {
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
  
  if(var == "CONFIG_BUTTON_PIN")
    return String(button_config.button_pin);
  
  if(var == "NO_CHECKED")
    return button_config.button_nonc ? String() : "checked";

  if(var == "NC_CHECKED")
    return button_config.button_nonc ? "checked" : String();
  
  // If a template value isn't found, return empty string
  return String();
}

void init_webserver() {
  web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *r) {
    if(!r->authenticate(button_config.admin_user, button_config.admin_pass))
      return r->requestAuthentication();
    r->send_P(200, "text/html", index_html, template_processor);
  });
  
  web_server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *r) {
    if(!r->authenticate(button_config.admin_user, button_config.admin_pass))
      return r->requestAuthentication();
    r->send_P(200, "text/html", setup_html, template_processor);
  });
  
  web_server.on("/get", HTTP_GET, [](AsyncWebServerRequest *r) {
    int params = r->params();
    for(int i = 0; i < params; i++) {
      AsyncWebParameter *p = r->getParam(i);
      if(p->name() == "get_url") {
        strlcpy(button_config.get_url, p->value().c_str(), sizeof(button_config.get_url));
        continue;
      }
      if(p->name() == "wifi_mode") {
        button_config.wifi_mode = p->value().toInt();
        continue;
      }
      if(p->name() == "wifi_ssid") {
        strlcpy(button_config.wifi_ssid, p->value().c_str(), sizeof(button_config.wifi_ssid));
        continue;
      }
      if(p->name() == "wifi_passwd") {
        strlcpy(button_config.wifi_passwd, p->value().c_str(), sizeof(button_config.wifi_passwd));
        continue;
      }
      if(p->name() == "eap_user") {
        strlcpy(button_config.eap_user, p->value().c_str(), sizeof(button_config.eap_user));
        continue;
      }
      if(p->name() == "eap_pass") {
        strlcpy(button_config.eap_pass, p->value().c_str(), sizeof(button_config.eap_pass));
        continue;
      }
      if(p->name() == "button_pin") {
        button_config.button_pin = p->value().toInt();
        continue;
      }
      if(p->name() == "button_nonc") {
        button_config.button_nonc = p->value().toInt();
        continue;
      }
    }
    
    // Save new configuration
    if(!save_config(filename, button_config)) {
      r->send(500, "text/html", "Error while saving settings");
      return;
    }
    r->send(200, "text/html", "Settings saved successfully");
  });
  
  web_server.onNotFound([](AsyncWebServerRequest *r) {
    r->send_P(404, "text/html", not_found_html);
  });
  
  web_server.begin();
}
