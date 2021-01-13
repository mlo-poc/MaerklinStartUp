#include <Railuino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "Pfarrhaus";
const char *password = "Bettina28101974Martin17071972";

WebServer server(80);

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void startServer() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/cmd", receiveRequest);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

TrackControllerInfrared tir = TrackControllerInfrared();

String request;
String function;
word arguments[8];
word numOfArguments;
int loco = 2;
boolean result;



void setup() {
  Serial.begin(115200);
  while (!Serial);

  startServer();
  tir.start();
  Serial.println("100 Ready");
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void receiveRequest() {
  
  request = 
}

word stringToWord(String s) {
  word result = 0;

  for (int i = 0; i < s.length(); i++) {
    result = 10 * result + (s.charAt(i) - '0');
  }

  return result;
}

boolean parse() {
  int lpar = request.indexOf('(');
  if (lpar == -1) {
    function = String(request.substring(0, lpar));
    return true;
  }

  function = String(request.substring(0, lpar));
  function.trim();

  int offset = lpar + 1;
  int comma = request.indexOf(',', offset);
  numOfArguments = 0;
  while (comma != -1) {
    String tmp = request.substring(offset, comma);
    tmp.trim();
    arguments[numOfArguments++] = stringToWord(tmp);
    offset = comma + 1;
    comma = request.indexOf(',', offset);
  }

  int rpar = request.indexOf(')', offset);
  while (rpar == -1) {
    return false;
  }

  if (rpar > offset) {
    String tmp = request.substring(offset, rpar);
    tmp.trim();
    arguments[numOfArguments++] = stringToWord(tmp);
  }

  return true;
}

boolean dispatch() {
  int a = loco;
  if (numOfArguments != 0 && arguments[0] != loco) {
    a = arguments[0];
  }

  if (function == "b") {
    Serial.println(a);
    return tir.accelerateLoco(a);
  } else if (function == "B") {
    return tir.decelerateLoco(a);
  } else if (function == "u") {
    return tir.toggleLocoDirection(a);
    //  } else if (function == "setLocoDirection") {
    //    return tir.setLocoDirection(a, arguments[1]);
  } else if (function == "f") {
    return tir.toggleLocoFunction(a, arguments[1]);
  } else if (function == "setTurnout") {
    return tir.setTurnout(a, arguments[1]);
  } else if (function == "p") {
    return tir.setPower(a);
    //  } else if (function == "setLocoSpeed") {
    //    return tir.setLocoSpeed(a, arguments[1]);
  } else if (function == "l") {
    loco = arguments[0];
  } else {
    return false;
  }

}

void loop() {
  //  Serial.print("201 Waiting ... ");
  server.handleClient();
  Serial.print("recieved: ");
  Serial.println(request);
  if (parse()) {
    if (dispatch()) {
      Serial.println("200 Ok");
    } else {
      Serial.println("300 Command error");
    }
  } else {
    Serial.println("301 Syntax error");
  }
}
