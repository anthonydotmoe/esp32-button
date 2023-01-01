#pragma once

#include <Arduino.h>

/*
   
   The following template variables are used:
   %TITLE%          Page title and h1 text
   %CONFIG_*%       Value from config file (replace * with value from config file)
   %BUTTON_STATE%   Current state of the button (useless?)
   %*_CHECKED%      If the value should be checked, replace with "checked", else empty string

*/
const String header_html = \
"<!DOCTYPE html>\
<html>\
    <head>\
        <title>%TITLE%</title>\
        <style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; color: #000088; }\
        </style>\
    </head>\
<body>\
    <h1>%TITLE%</h1>";

const String footer_html = \
"</body>\
</html>";

const String index_html = header_html + \
"<p>Current web hook: %CONFIG_GET_URL%</p>\
<p>Is Pressed: %BUTTON_STATE%</p>\
<button onclick=\"window.location.href='/config.html';\">Button setup</button>\
<meta http-equiv=\"refresh\" content=\"30\"/>"\
+ footer_html;

const String setup_html = header_html + \
"<h2>WebHook settings</h2>\
<p>\
    <form action=\"/get\">\
        <label for=\"WebHook\">URL:</label><br>\
        <input type=\"text\" value=\"%CONFIG_GET_URL%\" id=\"WebHook\" name=\"get_url\"><br>\
        <label for=\"HTTP\">HTTP</label>\
        <input type=\"radio\" id=\"HTTP\" name=\"get_tls\" value=\"1\" %HTTP_CHECKED%><br>\
        <label for=\"HTTPS\">HTTPS</label>\
        <input type=\"radio\" id=\"HTTPS\" name=\"get_tls\" value=\"0\" %HTTPS_CHECKED%><br>\
        <input type=\"submit\" value=\"Submit\">\
    </form>\
</p>\
<br><br>\
<h2>WiFi Settings</h2>\
<p>\
    <form action\"/get\">\
        <label for=\"PSK\">PSK</label>\
        <input type=\"radio\" id=\"PSK\" name=\"wifi_mode\" value=\"0\" %PSK_CHECKED%><br>\
        <label for=\"EAP\">PEAP-MSCHAPv2</label>\
        <input type=\"radio\" id=\"EAP\" name=\"wifi_mode\" value=\"1\" %EAP_CHECKED%><br>\
        <label for=\"wifi_ssid\">SSID: </label>\
        <input type=\"text\" value=\"%CONFIG_WIFI_SSID%\" id=\"wifi_ssid\" name=\"wifi_ssid\"><br>\
        <label for=\"wifi_passwd\">PSK Password: ></label>\
        <input type=\"password\" id=\"wifi_passwd\" name=\"wifi_passwd\"><br>\
        <label for=\"eap_user\">EAP Username: ></label>\
        <input type=\"text\" value=\"%CONFIG_EAP_USER%\" id=\"eap_user\" name=\"eap_user\"><br>\
        <label for=\"eap_pass\">EAP Password: ></label>\
        <input type=\"password\" id=\"eap_pass\" name=\"eap_pass\"><br>\
        <input type=\"submit\" value=\"Submit\">\
    </form>\
</p>"\
+ footer_html;