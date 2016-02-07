/*
 * Copyright 2016 Pieter Pareit
 * 
 * EEPROMManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * EEPROMManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EEPROMManager.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <EEPROM.h>

static bool verbose = false;

void setup() {

  // initialize Serial
  Serial.begin(9600);
  while (!Serial)
    ;

  // use builtin led for feedback on arduino
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("EEPROM Manager by Pieter Pareit");
  print_usage();

}

void loop() {
  String command;
  if (Serial.available()) {
    if (!parse_next_string(&command))
      return;
    log("Parsed string: " + command);
    if (command == "read") {
      int begin, end;
      if (!parse_next_range(&begin, &end))
        return;
      log(String("Parsed address range: [") + begin + ", " + end + "]");
      if (begin < 0 || EEPROM.length() <= begin || end < 0 || EEPROM.length() <= end || begin > end) {
        handle_read_error("ADDRESS or RANGE out of valid range");
        return;
      }
      log(String("Retrieving EEPROM at [") + begin + ", " + end + "]");
      digitalWrite(LED_BUILTIN, HIGH);
      while (begin <= end) {
        Serial.print(String() + EEPROM[begin] + " ");
        if (begin%20 == 0) digitalWrite(LED_BUILTIN, LOW);
        if (begin%40 == 0) digitalWrite(LED_BUILTIN, HIGH);
        if (begin==end) digitalWrite(LED_BUILTIN, HIGH);
        begin++;
      }
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println();
    } else if (command == "write") {
      int begin, end;
      if (!parse_next_range(&begin, &end))
        return;
      log(String("Parsed address range: [") + begin + ", " + end + "]");
      if (begin < 0 || EEPROM.length() <= begin || end < 0 || EEPROM.length() <= end || begin > end) {
        handle_read_error("ADDRESS or RANGE out of valid range");
        return;
      }
      int value;
      if (!parse_next_integer(&value))
        return;
      log(String("Parsed value: ") + value);
      if (value < 0 || 255 < value) {
        handle_read_error("Can only write bytes to the EEPROM");
        return;
      }
      log(String("Updating EEPROM at [") + begin + ", " + end + "] to value " + value);
      digitalWrite(LED_BUILTIN, HIGH);
      while (begin <= end) {
        EEPROM[begin] = value;
        if (begin%20 == 0) digitalWrite(LED_BUILTIN, LOW);
        if (begin%40 == 0) digitalWrite(LED_BUILTIN, HIGH);
        if (begin==end) digitalWrite(LED_BUILTIN, HIGH);
        begin++;
      }
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
    } else if (command == "zero") {
      log("Zero total EEPROM");
      digitalWrite(LED_BUILTIN, HIGH);
      for (int i = 0; i < EEPROM.length(); ++i) {
        if (i%20 == 0) digitalWrite(LED_BUILTIN, LOW);
        if (i%40 == 0) digitalWrite(LED_BUILTIN, HIGH);
        EEPROM[i] = 0;
      }
      digitalWrite(LED_BUILTIN, LOW);
    } else if (command == "range") {
      log("Retrieve and print EEPROM range");
      Serial.print("[0, ");
      Serial.print(EEPROM.length() - 1);
      Serial.println("]");
    } else if (command == "verbose") {
      String arg;
      if (!parse_next_string(&arg))
        return;
      log(String("Parsed argument: ") + arg);
      if (arg == "on") {
        verbose = true;
        log("Setting verbosity level on");
      } else if (arg == "off") {
        log("Setting verbosity level off");
        verbose = false;
      } else {
        handle_read_error("Unknown argument for verbose, must be one of {on, off}");
      }
    } else if (command == "help") {
      log("Print usage info");
      print_usage();
    } else if (command == "about") {
      log("Print about info");
      Serial.println("EEPROM Manager version 1 by Pieter Pareit for IOT coursera course");
      Serial.println("Contact at pieter.pareit@gmail.com");
    } else {
      handle_read_error("Unknown command " + command);
      return;
    }
  }
}

/*
 * If the verbosity level allows it, we print MSG to the console
 */
void log(String msg) {
  if (verbose) {
    Serial.println("LOG: " + msg);
  }
}

/*
 * Parses a string into STRING
 */
bool parse_next_string(String *string) {
  while (!Serial.available())
    ;
  while (isWhitespace(Serial.peek()) || isControl(Serial.peek()))
    Serial.read();
  while (!Serial.available())
    ;
  if (!isAlpha(Serial.peek())) {
    handle_read_error("Expected string");
    return false;
  }
  *string = Serial.readStringUntil(' ');
  return true;
}

/*
 * Parses an integer range [BEGIN, END]
 * When only one integer VALUE is parsed then begin=end=value
 */
bool parse_next_range(int *begin, int *end) {
  while (!Serial.available())
    ;
  while (isWhitespace(Serial.peek()))
    Serial.read();
  while (!Serial.available())
    ;
  if (isDigit(Serial.peek())) {
    *begin = *end = Serial.parseInt();
    return true;
  } else if (Serial.peek() == '[') {
    Serial.read();
    while (!Serial.available())
      ;
    while (isWhitespace(Serial.peek()))
      Serial.read();
    while (!Serial.available())
      ;
    if (!isDigit(Serial.peek())) {
      handle_read_error("Expected integer");
      return false;
    }
    *begin = Serial.parseInt();
    while (!Serial.available())
      ;
    while (isWhitespace(Serial.peek()))
      Serial.read();
    while (!Serial.available())
      ;
    if (Serial.peek() != ',' && Serial.peek() != ';') {
      handle_read_error("Expected divider ,");
      return false;
    }
    Serial.read();
    while (!Serial.available())
      ;
    while (isWhitespace(Serial.peek()))
      Serial.read();
    while (!Serial.available())
      ;
    if (!isDigit(Serial.peek())) {
      handle_read_error("Expected integer");
      return false;
    }
    *end = Serial.parseInt();
    while (!Serial.available())
      ;
    while (isWhitespace(Serial.peek()))
      Serial.read();
    while (!Serial.available())
      ;
    if (Serial.peek() != ']') {
      handle_read_error("Expected ]");
      return false;
    }
    Serial.read();
    return true;
  } else {
    handle_read_error("Expected number or range");
    return false;
  }
}

/*
 * Parses a single integer into INTEGER
 */
bool parse_next_integer(int *integer) {
  while (!Serial.available())
    ;
  while (isWhitespace(Serial.peek()))
    Serial.read();
  while (!Serial.available())
    ;
  if (!isDigit(Serial.peek())) {
    handle_read_error("Expected integer");
    return false;
  }
  *integer = Serial.parseInt();
  return true;
}

void print_usage() {
  Serial.println("Usage: ");
  Serial.println(" read RANGE         | displays EEPROM address at RANGE where range is a single address or an interval [begin, end]");
  Serial.println(" write RANGE VALUE  | writes VALUE in EEPROM address at at RANGE where range is a single address or an interval [begin, end]");
  Serial.println(" zero               | fills the full EEPROM with zero's");
  Serial.println(" range              | displays valid EEPROM adress range");
  Serial.println(" verbose on|off     | set the verbosity level");
  Serial.println(" help               | displays usage info");
  Serial.println(" about              | displays about");
}

/*
 * Displays an error message MESSAGE and cleans out the input as good as possible
 */
void handle_read_error(String message) {
  Serial.println("Error: " + message);
  delay(100);
  while (Serial.available())
    Serial.read();
}





