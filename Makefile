default: upload

compile:
	arduino-cli compile -b arduino:avr:uno .

debug:
	arduino-cli compile -b arduino:avr:uno . --build-property "build.extra_flags=\"-DDINOSAUR_DEBUG\""

upload-only:
	arduino-cli upload -p /dev/ttyACM0 -b arduino:avr:uno .

upload: compile upload-only
