ARDUINO_DIR = /usr/share/arduino
AVRDUDE_CONF = /usr/share/arduino/hardware/tools/avrdude.conf

BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyACM0

ARDUINO_LIBS = LiquidCrystal arduino-sht EEPROM eFLL Wire

include $(ARDUINO_DIR)/Arduino.mk
