#pragma once

/*
   
   The following template variables are used:
   %CONFIG_*%       Value from config file (replace * with value from config file)
   %*_CHECKED%      If the value should be checked, replace with "checked", else empty string

*/

const char *index_html = R"literal(
<!DOCTYPE html>
<html>
    <head>
        <title>Button Statistics</title>
        <style>
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; color: #000088; }
        </style>
        <script>
            socket = new WebSocket("ws://" + location.host + "/ws");
            socket.onmessage = function(e) {
                document.getElementById("button_state").innerHTML = e.data;
            };
        </script>
    </head>
    <body>
        <h1>Button Statistics</h1>
        <p>Current web hook: %CONFIG_GET_URL%</p>
        <p>Is Pressed: <div id="button_state">No</div></p>
        <button onclick="window.location.href='/config.html';">Button setup</button>
    </body>
</html>
)literal";

const char *setup_html = R"literal(
<!DOCTYPE html>
<html>
    <head>
        <title>Button Setup</title>
        <style>
body { 
    background-color: #cccccc;
    font-family: Arial, Helvetica, Sans-Serif;
    color: #000088;
}
        </style>
    </head>
    <body>
        <h1>Button Setup</h1>
        <h2>WebHook settings</h2>
        <p>
            <form action="/get">
                <label for="get_url">URL:</label><br>
                <input type="text" value="%CONFIG_GET_URL%" id="get_url" name="get_url"><br>
                <label for="HTTP">HTTP</label>
                <input type="radio" id="HTTP" name="get_tls" value="0" %HTTP_CHECKED%><br>
                <label for="HTTPS">HTTPS</label>
                <input type="radio" id="HTTPS" name="get_tls" value="1" %HTTPS_CHECKED%><br>
                <input type="submit" value="Submit">
            </form>
        </p>
        <br>
        <h2>WiFi Settings</h2>
        <p>
            <form action="/get">
                <label for="PSK">PSK</label>
                <input type="radio" id="PSK" name="wifi_mode" value="0" %PSK_CHECKED%><br>
                <label for="EAP">PEAP-MSCHAPv2</label>
                <input type="radio" id="EAP" name="wifi_mode" value="1" %EAP_CHECKED%><br>
                <label for="wifi_ssid">SSID: </label>
                <input type="text" value="%CONFIG_WIFI_SSID%" id="wifi_ssid" name="wifi_ssid"><br>
                <div class="psk_settings">
                    <label for="wifi_passwd">PSK Password: </label>
                    <input type="password" id="wifi_passwd" name="wifi_passwd"><br>
                </div>
                <div class="eap_settings">
                    <label for="eap_user">EAP Username: </label>
                    <input type="text" value="%CONFIG_EAP_USER%" id="eap_user" name="eap_user"><br>
                    <label for="eap_pass">EAP Password: </label>
                    <input type="password" id="eap_pass" name="eap_pass"><br>
                </div>
                <input type="submit" value="Submit">
            </form>
        </p>
        <br>
        <h2>Button Settings</h2>
        <p>
            <form action="/get">
                <label for="button_pin">Button Pin: </label>
                <input type="number" value="%CONFIG_BUTTON_PIN%" id="button_pin" name="button_pin"><br>
                <label for="NO">Normally Open</label>
                <input type="radio" id="NO" name="button_nonc" value="0" %NO_CHECKED%><br>
                <label for="NC">Normally Closed</label>
                <input type="radio" id="NC" name="button_nonc" value="1" %NC_CHECKED%><br>
                <input type="submit" value="Submit">
            </form>
        </p>
    </body>
</html>)literal";

const char *not_found_html = R"literal(
<!DOCTYPE html>
<html>
    <head>
        <title>Not found</title>
    </head>
    <body>
        <h1>Not found</h1>
        <p>The requested URL was not found on this server</p>
    </body>
</html>
)literal";
