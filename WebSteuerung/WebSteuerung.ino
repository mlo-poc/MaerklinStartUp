#include <Railuino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
const char COPYRIGHT_SYMBOL[] = { 0xa9, '\0' };

const char *ssid = "Pfarrhaus";
const char *password = "Bettina28101974Martin17071972";


TrackControllerInfrared tir = TrackControllerInfrared();

String command;
int function;
int loco = 2;

WebServer server(80);

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

String buildHTML(int status) {
  char temp[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 1000,
           "<html>\
        <head>\
        <title>ESP32 Railuino</title>\
        <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        </style>\
        </head>\
        <body>\
        <h1>Hello from ESP32!</h1>\
        <p>Uptime: %02d:%02d:%02d</p>\
        <h3>Last Command result: %d</h3>\
        <form id=\"form_12410\" method=\"post\" action=\"/cmd\">\
        <label for=\"lok\">Lok </label>\
        <select id=\"lok\" name=\"lok\"> \
        <option value=\"1\" >1</option>\
        <option value=\"2\" selected=\"selected\">2</option>\
        <option value=\"3\" >3</option>\
        <option value=\"4\" >4</option>\
        </select></br>\
        <button type=\"submit\" name=\"func\" value=\"acc\">beschleunigen</button>\
        <button type=\"submit\" name=\"func\" value=\"dec\">bremsen</button>\
        <button type=\"submit\" name=\"func\" value=\"turn\">Fahrtrichtung</button>\
        <button type=\"submit\" name=\"func\" value=\"power\">ein/aus</button>\
        </form>\
        </body>\
        </html>",
           hr, min % 60, sec % 60, status
          );
  return temp;
}

void handleRoot() {
  server.send(200, "text/html", buildHTML(0));
}

void handleNotFound() {
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
}

void showIP() {
  char buf[20];
  sprintf(buf, "IP:%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.drawStr(0, 10, buf);
  u8g2.sendBuffer();
}
void showCmd() {
  char buf[20];
  sprintf(buf, "Lok: %d Cmd: %s", loco, command);
  showIP();
  u8g2.drawStr(0, 20, buf);
  u8g2.sendBuffer();
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

  showIP();

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

void setup() {
  Serial.begin(115200);
  while (!Serial);

  u8g2.begin();
  u8g2_prepare();

  startServer();
  tir.start();
  Serial.println("100 Ready");
}

void receiveRequest() {
  Serial.print("Command: ");
  Serial.print(server.method(), BIN);
  Serial.print(" ");
  Serial.print(server.uri());
  Serial.print(" : ");
  for (int i = 0; i < server.args(); i++) {
    Serial.print(server.argName(i));
    Serial.print("=");
    Serial.print(server.arg(i));
    Serial.print(", ");
  }
  Serial.println();
  loco = server.arg("lok").toInt();
  command = server.arg("func");
  
  showCmd();
}

word stringToWord(String s) {
  word result = 0;

  for (int i = 0; i < s.length(); i++) {
    result = 10 * result + (s.charAt(i) - '0');
  }

  return result;
}

boolean dispatch() {
  if (command == "acc") {
    return tir.accelerateLoco(loco);
  } else if (command == "dec") {
    return tir.decelerateLoco(loco);
  } else if (command == "turn") {
    return tir.toggleLocoDirection(loco);
    //  } else if (command == "setLocoDirection") {
    //    return tir.setLocoDirection(a, arguments[1]);
  } else if (command == "func") {
    return tir.toggleLocoFunction(loco, function);
  } else if (command == "set_t") {
    return tir.setTurnout(loco, function);
  } else if (command == "power") {
    return tir.setPower(loco);
    //  } else if (command == "setLocoSpeed") {
    //    return tir.setLocoSpeed(a, arguments[1]);
  } else {
    return false;
  }
}

void loop() {
  //  Serial.println("handleClient");
  server.handleClient();
  //  Serial.println("send");
  server.send(200, "text/html", buildHTML(dispatch()));
}
