![EEPROM Manager](https://raw.githubusercontent.com/ppareit/EEPROMMananger/master/logo.png)
# EEPROMMananger

EEPROM Mananger for the arduino platform

## Why
The program is based on a [Coursera course](https://www.coursera.org/learn/interface-with-arduino/) Peer Assignment:

Write a sketch that allows a user to access data in EEPROM using the serial monitor. In the serial monitor the user should be able to type one of two commands: “read” and “write. "Read" takes one argument, an EEPROM address. "Write" takes two arguments, an EEPROM address and a value.

For example, if the user types “read 3” then the contents of EEPROM address 3 should be printed to the serial monitor. If the user types “write 3 10” then the value 10 should be written into address 3 of the EEPROM.

## What
The program implements the basic functionallity as instructed:

    read ADDRESS
    write ADDRESS VALUE

Here the `ADDRESS` is an integer and `VALUE` is the byte to save at the address.

Next to that, the same commands take also a range of addresses:

	read RANGE
    write RANGE VALUE

In these cases the single integer that represents the address is replaced by a range of adresses `[BEGIN, END]`.

There are more commands, like:

    zero
    range
    verbose
    etc..
 
Those should be self explanatory.

## License
GPL v3

