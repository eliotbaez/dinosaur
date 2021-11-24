default: upload

compile:
	arduino-cli compile -b arduino:avr:uno .

upload-only:
	arduino-cli upload -p /dev/ttyACM0 -b arduino:avr:uno .

upload: compile upload-only
