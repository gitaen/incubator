ARDUINO_DIR = /usr/share/arduino

BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyACM3

ARDUINO_LIBS = LiquidCrystal Sensirion EEPROM eFLL

include $(ARDUINO_DIR)/Arduino.mk
