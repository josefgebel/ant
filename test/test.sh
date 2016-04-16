#!/bin/bash

SEMICOOKED=0
JSON=0
DEVICE=ALL
RUN=1
INPUT_FILE=testfile.ant

until [ -z "$1" ]
do   
	case $1 in  
		-D | D)
			shift
			DEVICE=$1
			;;
		-h | h)
			echo "Usage"
			echo "    $0 [-D <device_type>] [-h] [-S]"
			echo "where"
			echo "    -D <device_type>: Run test for device <device_type>."
			echo "                      Supported device types:"
			echo "                           ALL       : All Sensors."
			echo "                           AERO      : Aero Sensor."
			echo "                           AUDIO     : Audio Sensor (1)."
			echo "                           BLOOD     : Blood Pressure Sensor (1)."
			echo "                           CAD       : Cadence Sensor."
			echo "                           ENV       : Environment Sensor (1)."
			echo "                           HRM       : Heart Rate Sensor."
			echo "                           MULTI     : Multi-Sport Speed and Distance Sensor (1)."
			echo "                           POWER     : Power Sensor."
			echo "                           SPB7      : Speed Only Sensor."
			echo "                           SPCAD     : Speed and Cadence Sensor."
			echo "                           STRIDE    : Stride Based Speed and Distance Sensor (1)."
			echo "                           WEIGHT    : Weight Scale Sensor (1)."
			echo "                      Default: <device_type> = ${DEVICE}"
			echo "    -h              : Display this message."
			echo "    -S              : Run test for semi-cooked output."
			echo "                      Default: fully-cooked output."
			echo "(1) Tests are not yet available due to lack of ANT+ input data."
        		RUN=0
			;;
		-J | J)
			JSON=1
			;;
		-S | S)
			SEMICOOKED=1
			;;
	esac
	shift
done

if [ "${RUN}" = "1" ]; then
	if [ "${DEVICE}" = "ALL" ]; then
		EXECUTABLE=bridge2txt
		DATA_FILE_ROOT=allTest
	elif [ "${DEVICE}" = "AERO" ]; then
		EXECUTABLE=aero_ant2txt
		DATA_FILE_ROOT=aeroTest
	elif [ "${DEVICE}" = "AUDIO" ]; then
		EXECUTABLE=audio_ant2txt
		DATA_FILE_ROOT=audioTest
	elif [ "${DEVICE}" = "BLOOD" ]; then
		EXECUTABLE=blood_pressure_ant2txt
		DATA_FILE_ROOT=bloodTest
	elif [ "${DEVICE}" = "CAD" ]; then
		EXECUTABLE=cadence_only_ant2txt
		DATA_FILE_ROOT=cadTest
	elif [ "${DEVICE}" = "ENV" ]; then
		EXECUTABLE=environment_ant2txt
		DATA_FILE_ROOT=envTest
	elif [ "${DEVICE}" = "HRM" ]; then
		EXECUTABLE=hrm_ant2txt
		DATA_FILE_ROOT=hrmTest
	elif [ "${DEVICE}" = "MULTI" ]; then
		EXECUTABLE=multi_sport_speed_dist_ant2txt
		DATA_FILE_ROOT=multiTest
	elif [ "${DEVICE}" = "POWER" ]; then
		EXECUTABLE=power_ant2txt
		DATA_FILE_ROOT=powerTest
	elif [ "${DEVICE}" = "SPB7" ]; then
		EXECUTABLE=speed_only_ant2txt
		DATA_FILE_ROOT=speedOnlyTest
	elif [ "${DEVICE}" = "SPCAD" ]; then
		EXECUTABLE=spcad_ant2txt
		DATA_FILE_ROOT=spcadTest
	elif [ "${DEVICE}" = "STRIDE" ]; then
		EXECUTABLE=stride_speed_dist_ant2txt
		DATA_FILE_ROOT=strideTest
	elif [ "${DEVICE}" = "WEIGHT" ]; then
		EXECUTABLE=weight_ant2txt
		DATA_FILE_ROOT=weightTest
	else
        	RUN=0
	fi
fi

if [ "${RUN}" = "1" ]; then
	if [ "${SEMICOOKED}" = "1" ]; then
		SEMI_FULLY=Semi
		SEMI_ARG=" -S"
	else
		SEMI_FULLY=Fully
		SEMI_ARG=""
	fi
	if [ "${JSON}" = "1" ]; then
		JSON=JSON
		JSON_ARG=" -J"
	else
		JSON=
		JSON_ARG=""
	fi
	DATA_DIRECTORY=test

	DATA_FILE=${DATA_DIRECTORY}/${DATA_FILE_ROOT}${SEMI_FULLY}${JSON}.txt

	bin/${EXECUTABLE}${SEMI_ARG}${JSON_ARG} -x -f "${DATA_DIRECTORY}/${INPUT_FILE}" > "${DATA_FILE}.tmp"

	diff "${DATA_FILE}" "${DATA_FILE}.tmp"

	if [ "$?" = "0" ]; then
		if [ "${JSON}" = "JSON" ]; then
			echo "Test ${DEVICE} ${SEMI_FULLY}-Cooked JSON Result: OK"
		else
			echo "Test ${DEVICE} ${SEMI_FULLY}-Cooked Result: OK"
		fi
	else
		if [ "${JSON}" = "1" ]; then
			echo "Test ${DEVICE} ${SEMI_FULLY}-Cooked JSON Result: Fail"
		else
			echo "Test ${DEVICE} ${SEMI_FULLY}-Cooked Result: Fail"
		fi
	fi
	rm "${DATA_FILE}.tmp"

#	echo "bin/${EXECUTABLE} ${SEMI_ARG} -x -f \"${DATA_DIRECTORY}/${INPUT_FILE}\" > \"${DATA_FILE}.tmp\""
fi

