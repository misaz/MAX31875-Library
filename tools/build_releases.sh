#!/bin/bash
ORIGINAL_PWD=$(pwd)
SENSOR_NAME="MAX31875"

mkdir -p releases

for port in Generic PSoC6 EFR32BG22; do
	if [ "$port" == "Generic" ]; then
		PLATFORM_SPECIFIC_SOURCES_PREFIX="${SENSOR_NAME}_PlatformSpecific"
	else
		PLATFORM_SPECIFIC_SOURCES_PREFIX="${SENSOR_NAME}_PlatformSpecific_$port"
	fi

	OUTPUT_NAME="${SENSOR_NAME}_${port}.zip"

	TEMP=$(mktemp)
	rm $TEMP
	mkdir $TEMP
	cp src/${SENSOR_NAME}.h $TEMP/${SENSOR_NAME}.h
	cp src/${SENSOR_NAME}.c $TEMP/${SENSOR_NAME}.c
	cp src/${PLATFORM_SPECIFIC_SOURCES_PREFIX}.h $TEMP/${SENSOR_NAME}_PlatformSpecific.h
	cp src/${PLATFORM_SPECIFIC_SOURCES_PREFIX}.c $TEMP/${SENSOR_NAME}_PlatformSpecific.c
	cd $TEMP
	zip $OUTPUT_NAME * > /dev/null
	cd $ORIGINAL_PWD
	cp $TEMP/$OUTPUT_NAME releases/
	rm -r $TEMP

	echo releases/$OUTPUT_NAME
done
