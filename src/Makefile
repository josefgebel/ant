CC=g++
CFLAGS=-c -Wall
LDFLAGS=
INCFLAGS=-I .

REMOVE=/bin/rm -f
MOVE=/bin/mv
COPY=/bin/cp
MAKE=/usr/bin/make
BIN_DIR=../bin

COMMON_HEADERS=ant_constants.h

SOURCES=am_string.cpp am_multicast_read.cpp am_multicast_write.cpp am_split_string.cpp ant_processing.cpp
S_OBJECTS=$(SOURCES:.cpp=.o)
CS_SOURCES=ant_cadence_speed_processing.cpp ant_cadence_processing.cpp ant_speed_processing.cpp $(SOURCES)
CS_OBJECTS=$(CS_SOURCES:.cpp=.o)

EXECUTABLE_01=aero_ant2txt
MAIN_01=$(EXECUTABLE_01).cpp
CLASS_01=ant_aero_processing.cpp
SOURCES_01=$(CLASS_01) $(SOURCES)
OBJECTS_01=$(CLASS_01:.cpp=.o) $(MAIN_01:.cpp=.o)
HEADERS_01=$(SOURCES_01:.cpp=.h)
ALL_SOURCES_01=$(MAIN_01) $(SOURCES_01)
ALL_OBJECTS_01=$(ALL_SOURCES_01:.cpp=.o)

EXECUTABLE_02=cadence_only_ant2txt
MAIN_02=$(EXECUTABLE_02).cpp
CLASS_02=ant_cadence_only_processing.cpp
OBJECTS_02=$(CLASS_01:.cpp=.o) $(MAIN_02:.cpp=.o)
SOURCES_02=$(CLASS_02) $(CS_SOURCES)
ALL_SOURCES_02=$(MAIN_02) $(SOURCES_02)
HEADERS_02=$(SOURCES_02:.cpp=.h)
ALL_OBJECTS_02=$(ALL_SOURCES_02:.cpp=.o)

EXECUTABLE_03=hrm_ant2txt
MAIN_03=$(EXECUTABLE_03).cpp
CLASS_03=ant_hrm_processing.cpp
OBJECTS_03=$(CLASS_01:.cpp=.o) $(MAIN_03:.cpp=.o)
SOURCES_03=$(CLASS_03) $(SOURCES)
HEADERS_03=$(SOURCES_03:.cpp=.h)
ALL_SOURCES_03=$(MAIN_03) $(SOURCES_03)
ALL_OBJECTS_03=$(ALL_SOURCES_03:.cpp=.o)

EXECUTABLE_04=speed_only_ant2txt
MAIN_04=$(EXECUTABLE_04).cpp
CLASS_04=ant_speed_only_processing.cpp
OBJECTS_04=$(CLASS_04:.cpp=.o) $(MAIN_04:.cpp=.o)
SOURCES_04=$(CLASS_04) ant_speed_processing.cpp $(SOURCES)
ALL_SOURCES_04=$(MAIN_04) $(SOURCES_04)
HEADERS_04=$(SOURCES_04:.cpp=.h)
ALL_OBJECTS_04=$(ALL_SOURCES_04:.cpp=.o)

EXECUTABLE_05=spcad_ant2txt
MAIN_05=$(EXECUTABLE_05).cpp
CLASS_05=ant_spcad_processing.cpp
OBJECTS_05=$(CLASS_05:.cpp=.o) $(MAIN_05:.cpp=.o)
SOURCES_05=$(CLASS_05) $(CS_SOURCES)
ALL_SOURCES_05=$(MAIN_05) $(SOURCES_05)
HEADERS_05=$(SOURCES_05:.cpp=.h)
ALL_OBJECTS_05=$(ALL_SOURCES_05:.cpp=.o)

EXECUTABLE_06=power_ant2txt
MAIN_06=$(EXECUTABLE_06).cpp
CLASS_06=ant_power_processing.cpp
OBJECTS_06=$(CLASS_06:.cpp=.o) $(MAIN_06:.cpp=.o)
SOURCES_06=$(CLASS_06) $(CS_SOURCES)
ALL_SOURCES_06=$(MAIN_06) $(SOURCES_06)
HEADERS_06=$(SOURCES_06:.cpp=.h)
ALL_OBJECTS_06=$(ALL_SOURCES_06:.cpp=.o)

EXECUTABLE_07=audio_ant2txt
MAIN_07=$(EXECUTABLE_07).cpp
CLASS_07=ant_audio_processing.cpp
OBJECTS_07=$(CLASS_07:.cpp=.o) $(MAIN_07:.cpp=.o)
SOURCES_07=$(CLASS_07) $(SOURCES)
ALL_SOURCES_07=$(MAIN_07) $(SOURCES_07)
ALL_OBJECTS_07=$(ALL_SOURCES_07:.cpp=.o)

EXECUTABLE_08=stride_speed_dist_ant2txt
MAIN_08=$(EXECUTABLE_08).cpp
CLASS_08=ant_stride_speed_dist_processing.cpp
OBJECTS_09=$(CLASS_09:.cpp=.o) $(MAIN_09:.cpp=.o)
SOURCES_08=$(CLASS_08) $(SOURCES)
HEADERS_08=$(SOURCES_08:.cpp=.h)
ALL_SOURCES_08=$(MAIN_08) $(SOURCES_08)
ALL_OBJECTS_08=$(ALL_SOURCES_08:.cpp=.o)

EXECUTABLE_09=environment_ant2txt
MAIN_09=$(EXECUTABLE_09).cpp
CLASS_09=ant_environment_processing.cpp
OBJECTS_09=$(CLASS_09:.cpp=.o) $(MAIN_09:.cpp=.o)
SOURCES_09=$(CLASS_09) $(SOURCES)
HEADERS_09=$(SOURCES_09:.cpp=.h)
ALL_SOURCES_09=$(MAIN_09) $(SOURCES_09)
ALL_OBJECTS_09=$(ALL_SOURCES_09:.cpp=.o)

EXECUTABLE_10=blood_pressure_ant2txt
MAIN_10=$(EXECUTABLE_10).cpp
CLASS_10=ant_blood_pressure_processing.cpp
OBJECTS_10=$(CLASS_10:.cpp=.o) $(MAIN_10:.cpp=.o)
SOURCES_10=$(CLASS_10) $(SOURCES)
HEADERS_10=$(SOURCES_10:.cpp=.h)
ALL_SOURCES_10=$(MAIN_10) $(SOURCES_10)
ALL_OBJECTS_10=$(ALL_SOURCES_10:.cpp=.o)

EXECUTABLE_11=weight_ant2txt
MAIN_11=$(EXECUTABLE_11).cpp
CLASS_11=ant_weight_processing.cpp
OBJECTS_11=$(CLASS_11:.cpp=.o) $(MAIN_11:.cpp=.o)
SOURCES_11=$(CLASS_11) $(SOURCES)
HEADERS_11=$(SOURCES_11:.cpp=.h)
ALL_SOURCES_11=$(MAIN_11) $(SOURCES_11)
ALL_OBJECTS_11=$(ALL_SOURCES_11:.cpp=.o)

EXECUTABLE_12=multi_sport_speed_dist_ant2txt
MAIN_12=$(EXECUTABLE_12).cpp
CLASS_12=ant_multi_sport_speed_dist_processing.cpp
OBJECTS_12=$(CLASS_12:.cpp=.o) $(MAIN_12:.cpp=.o)
SOURCES_12=$(CLASS_12) $(SOURCES)
ALL_SOURCES_12=$(MAIN_12) $(SOURCES_12)
HEADERS_12=$(SOURCES_12:.cpp=.h)
ALL_OBJECTS_12=$(ALL_SOURCES_12:.cpp=.o)

MAIN_99=bridge2txt.cpp
CLASS_99=ant_all_processing.cpp
OBJECTS_99=$(CLASS_99:.cpp=.o) $(MAIN_99:.cpp=.o)
SOURCES_99=$(CLASS_01) \
           $(CLASS_02) \
           $(CLASS_03) \
           $(CLASS_04) \
           $(CLASS_05) \
           $(CLASS_06) \
           $(CLASS_07) \
           $(CLASS_08) \
           $(CLASS_09) \
           $(CLASS_10) \
           $(CLASS_11) \
           $(CLASS_12) \
           $(CLASS_99) \
           $(CS_SOURCES)

ALL_SOURCES_99=$(MAIN_99) $(SOURCES_99)
HEADERS_99=$(SOURCES_99:.cpp=.h)
ALL_OBJECTS_99=$(ALL_SOURCES_99:.cpp=.o)
EXECUTABLE_99=bridge2txt

ALL_SOURCES=$(ALL_SOURCES_01) \
            $(ALL_SOURCES_02) \
            $(ALL_SOURCES_03) \
            $(ALL_SOURCES_04) \
            $(ALL_SOURCES_05) \
            $(ALL_SOURCES_06) \
            $(ALL_SOURCES_07) \
            $(ALL_SOURCES_08) \
            $(ALL_SOURCES_09) \
            $(ALL_SOURCES_10) \
            $(ALL_SOURCES_11) \
            $(ALL_SOURCES_12) \
            $(ALL_SOURCES_99) \
            $(HEADERS_01) \
            $(HEADERS_02) \
            $(HEADERS_03) \
            $(HEADERS_04) \
            $(HEADERS_05) \
            $(HEADERS_06) \
            $(HEADERS_07) \
            $(HEADERS_08) \
            $(HEADERS_09) \
            $(HEADERS_10) \
            $(HEADERS_11) \
            $(HEADERS_12) \
            $(HEADERS_99) \
            $(COMMON_HEADERS) \


OBJECTS=$(CS_OBJECTS) \
        $(ALL_OBJECTS_01) \
        $(ALL_OBJECTS_02) \
        $(ALL_OBJECTS_03) \
        $(ALL_OBJECTS_04) \
        $(ALL_OBJECTS_05) \
        $(ALL_OBJECTS_06) \
        $(ALL_OBJECTS_07) \
        $(ALL_OBJECTS_08) \
        $(ALL_OBJECTS_09) \
        $(ALL_OBJECTS_10) \
        $(ALL_OBJECTS_11) \
        $(ALL_OBJECTS_12) \
        $(ALL_OBJECTS_99)


%.o: %.c %.h ant_constants.h Makefile
	$(CC) $(CFLAGS) -MM -MF $(patsubst %.o,%.d,$@) -o $@ $<


all: $(ALL_SOURCES_01) $(EXECUTABLE_01)\
     $(ALL_SOURCES_02) $(EXECUTABLE_02)\
     $(ALL_SOURCES_03) $(EXECUTABLE_03)\
     $(ALL_SOURCES_04) $(EXECUTABLE_04)\
     $(ALL_SOURCES_05) $(EXECUTABLE_05)\
     $(ALL_SOURCES_06) $(EXECUTABLE_06)\
     $(ALL_SOURCES_07) $(EXECUTABLE_07)\
     $(ALL_SOURCES_08) $(EXECUTABLE_08)\
     $(ALL_SOURCES_09) $(EXECUTABLE_09)\
     $(ALL_SOURCES_10) $(EXECUTABLE_10)\
     $(ALL_SOURCES_11) $(EXECUTABLE_11)\
     $(ALL_SOURCES_12) $(EXECUTABLE_12)\
     $(ALL_SOURCES_99) $(EXECUTABLE_99)\



$(EXECUTABLE_01): $(ALL_OBJECTS_01) $(HEADERS_01) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_01) -o $@

$(EXECUTABLE_02): $(ALL_OBJECTS_02) $(HEADERS_02) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_02) -o $@

$(EXECUTABLE_03): $(ALL_OBJECTS_03) $(HEADERS_03) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_03) -o $@

$(EXECUTABLE_04): $(ALL_OBJECTS_04) $(HEADERS_04) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_04) -o $@

$(EXECUTABLE_05): $(ALL_OBJECTS_05) $(HEADERS_05) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_05) -o $@

$(EXECUTABLE_06): $(ALL_OBJECTS_06) $(HEADERS_06) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_06) -o $@

$(EXECUTABLE_07): $(ALL_OBJECTS_07) $(HEADERS_07) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_07) -o $@

$(EXECUTABLE_08): $(ALL_OBJECTS_08) $(HEADERS_08) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_08) -o $@

$(EXECUTABLE_09): $(ALL_OBJECTS_09) $(HEADERS_09) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_09) -o $@

$(EXECUTABLE_10): $(ALL_OBJECTS_10) $(HEADERS_10) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_10) -o $@

$(EXECUTABLE_11): $(ALL_OBJECTS_11) $(HEADERS_11) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_11) -o $@

$(EXECUTABLE_12): $(ALL_OBJECTS_12) $(HEADERS_12) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_12) -o $@

$(EXECUTABLE_99): $(ALL_OBJECTS_99) $(HEADERS_99) $(COMMON_HEADERS) Makefile
	$(CC) $(LDFLAGS) $(ALL_OBJECTS_99) -o $@


.cpp.o: $(COMMON_HEADERS) Makefile
	$(CC) $(INCFLAGS) $(CFLAGS) $< -o $@

clean:
	$(REMOVE) $(OBJECTS)

clean_all:
	make clean
	$(REMOVE) $(EXECUTABLE_01) $(OBJECTS_01)
	$(REMOVE) $(EXECUTABLE_02) $(OBJECTS_02)
	$(REMOVE) $(EXECUTABLE_03) $(OBJECTS_03)
	$(REMOVE) $(EXECUTABLE_04) $(OBJECTS_04)
	$(REMOVE) $(EXECUTABLE_05) $(OBJECTS_05)
	$(REMOVE) $(EXECUTABLE_06) $(OBJECTS_06)
	$(REMOVE) $(EXECUTABLE_07) $(OBJECTS_07)
	$(REMOVE) $(EXECUTABLE_08) $(OBJECTS_08)
	$(REMOVE) $(EXECUTABLE_09) $(OBJECTS_09)
	$(REMOVE) $(EXECUTABLE_10) $(OBJECTS_10)
	$(REMOVE) $(EXECUTABLE_11) $(OBJECTS_11)
	$(REMOVE) $(EXECUTABLE_12) $(OBJECTS_12)
	$(REMOVE) $(EXECUTABLE_99) $(OBJECTS_99)

rebuild:
	make clean_all; make all

install:
	make all
	$(COPY) $(EXECUTABLE_01) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_02) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_03) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_04) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_05) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_06) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_07) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_08) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_09) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_10) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_11) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_12) $(BIN_DIR)
	$(COPY) $(EXECUTABLE_99) $(BIN_DIR)

install_clean:
	make all
	$(MOVE) $(EXECUTABLE_01) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_02) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_03) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_04) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_05) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_06) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_07) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_08) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_09) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_10) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_11) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_12) $(BIN_DIR)
	$(MOVE) $(EXECUTABLE_99) $(BIN_DIR)
	make clean

# DO NOT DELETE
