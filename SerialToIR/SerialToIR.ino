/*********************************************************************
 * Railuino - Hacking your Märklin
 *
 * Copyright (C) 2012 Joerg Pleumann
 * 
 * This example is free software; you can redistribute it and/or
 * modify it under the terms of the Creative Commons Zero License,
 * version 1.0, as published by the Creative Commons Organisation.
 * This effectively puts the file into the public domain.
 *
 * This example is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * LICENSE file for more details.
 */

#include <Railuino.h>

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
  
  tir.start();
  Serial.println("100 Ready");
}

void receiveRequest() {
  char buffer[256];
  int length = 0;
  boolean newline = false;

  while (!newline && length < sizeof(buffer)) {
    if (Serial.available() > 0) {
      int c = Serial.read();
      
      if (c == 10) {
        buffer[length++] = 0;
        newline = true;
      } else if (c == 13) {
        // Ignore
      } else {
        buffer[length++] = c;
      }
    }
  }
  
  request = String(buffer);
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
  if (b != 0 && arguments[0] != loco) {
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
  receiveRequest();
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
