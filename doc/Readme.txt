STANDANLONE bridge2txt

0. Version
   -------

   Version 1.0.0


1. Build
   -----

    1.1 Unpack
        ------
        To build the executable first unpack the package file by entering
            tar -czf bridge2txt_standalone_2.0.0.tgz
        followed by the return key.

        Enter the bridge2txt_standalone directory by typing
            cd bridge2txt_standalone
        followed by the return key.

        To list all files type
            ls -l
        followed by the return key. The following files should be present
            Makefile                                                the Makefile
            Readme.txt                                              this file
            ant_processing.[cpp,h]                                  c++ code for ANT+ processing base class
            aero_ant2txt.cpp                                        c++ code for main function for aero sensor ANT+ data processing
            am_split_string.[cpp,h]                                 c++ code for class that deals with splitting strings into substrings (words)
            ant_aero_processing.[cpp,h]                             c++ code for ANT+ aero sensor class
            ant_all_processing.[cpp,h]                              c++ code for class that processes all supported ANT+ sensors 
            ant_audio_processing.[cpp,h]                            c++ code for ANT+ audio control class
            ant_blood_pressure_processing.[cpp,h]                   c++ code for ANT+ blood pressure sensor class
            ant_cadence_processing.[cpp,h]                          c++ code for ANT+ cadence only sensor class
            ant_cadence_speed_processing.[cpp,h]                    c++ code for base class for ANT+ sensors that contain speed and cadence
            ant_constants.h                                         definition of common constants
            ant_environment_processing.[cpp,h]                      c++ code for ANT+ environmental sensor class
            ant_hrm_processing.[cpp,h]                              c++ code for ANT+ heart rate monitor class
            ant_multi_sport_speed_dist_processing.[cpp,h]           c++ code for ANT+ multi sport speed and distance sensor class
            ant_power_processing.[cpp,h]                            c++ code for ANT+ power meter class
            ant_spcad_processing.[cpp,h]                            c++ code for ANT+ speed and cadence sensor class
            ant_speed_only_processing.[cpp,h]                       c++ code for ANT+ speed only sensor class
            ant_speed_processing.[cpp,h]                            c++ code for base class for ANT+ sensors that contain speed
            ant_stride_speed_dist_processing.[cpp,h]                c++ code for ANT+ stride based speed and distance sensor class
            ant_weight_processing.[cpp,h]                           c++ code for ANT+ weight scale class
            audio_ant2txt.cpp                                       c++ code for main function for audio control ANT+ data processing
            b2t_utils.[cpp,h]                                       c++ code for auxiliary functions
            blood_pressure_ant2txt.cpp                              c++ code for main function for blood pressure sensor ANT+ data processing
            bridge2txt.cpp                                          c++ code for main function for processing all supported ANT+ sensors
            cadence_ant2txt.cpp                                     c++ code for main function for cadence only sensor ANT+ data processing
            environment_ant2txt.cpp                                 c++ code for main function for environmental sensor ANT+ data processing
            hrm_ant2txt.cpp                                         c++ code for main function for heart rate monitor ANT+ data processing
            multi_sport_speed_dist_ant2txt.cpp                      c++ code for main function for multi sport speed and distance ANT+ data processing
            power_ant2txt.cpp                                       c++ code for main function for power meter ANT+ data processing
            spcad_ant2txt.cpp                                       c++ code for main function for speed and cadence sensor ANT+ data processing
            speed_only_ant2txt.cpp                                  c++ code for main function for speed only sensor ANT+ data processing
            stride_speed_dist_ant2txt.cpp                           c++ code for main function for stride based speed and distance sensor ANT+ data processing
            weight_ant2txt.cpp                                      c++ code for main function for weight scale ANT+ data processing

    1.2 Compile and Create the Executables
        ----------------------------------
        To create the executable enter
            make
        followed by the return key.

        If successful, the executables
            aero_ant2txt                                            executable that handles ANT+ aero sensors
            audio_ant2txt                                           executable that handles ANT+ audio control sensors
            blood_pressure_ant2txt                                  executable that handles ANT+ blood pressure sensors
            bridge2txt                                              executable that handles all supported ANT+ sensors
            cadence_ant2txt                                         executable that handles ANT+ cadence only sensors
            environment_ant2txt                                     executable that handles ANT+ environmental sensors
            hrm_ant2txt                                             executable that handles ANT+ heart rate monitors
            multi_sport_speed_dist_ant2txt                          executable that handles ANT+ multi sport speed and distance sensors
            power_ant2txt                                           executable that handles ANT+ power meters
            spcad_ant2txt                                           executable that handles ANT+ speed and cadence sensors
            speed_only_ant2txt                                      executable that handles ANT+ speed only sensors
            stride_speed_dist_ant2txt                               executable that handles ANT+ stride based speed and distance sensors
            weight_ant2txt                                          executable that handles ANT+ weight scales
        are created.



2. Tests
   -----

    2.1 Help
        ----
        To get a list and a description of all options for the executable <executable> type
           ./<executable> -h

    2.2 Input from file
        ---------------

        To test reading data from file run the commands (where <executable> is one of the executables listed above)
           ./<executable> -f testfile.ant                   [read ANT+ data from file and output in fully cooked mode]
           ./<executable> -f testfile.ant -J                [read ANT+ data from file and output as JSON objects
           ./<executable> -f testfile.ant -S                [read ANT+ data from file and output in semi-cooked mode]
           ./<executable> -f testfile.ant -S -J             [read ANT+ data from file and output in semi-cooked mode as JSON objects]
           ./<executable> -f testfile.ant -r                [read ANT+ data from file and output as raw bytes]
           ./<executable> -f testfile.ant -D                [read ANT+ data from file and output in diagnostic mode]


    2.3 Input from stdin
        ----------------
        To test reading data from stdin run the commands
           cat testfile.ant | ./<executable> -1             [read ANT+ data from stdin and output in fully cooked mode]
           cat testfile.ant | ./<executable> -1 -J          [read ANT+ data from stdin and output as JSON objects
           cat testfile.ant | ./<executable> -1 -S          [read ANT+ data from stdin and output in semi-cooked mode]
           cat testfile.ant | ./<executable> -1 -S -J       [read ANT+ data from stdin and output in semi-cooked mode as JSON objects]
           cat testfile.ant | ./<executable> -1 -r          [read ANT+ data from stdin and output as raw bytes]
           cat testfile.ant | ./<executable> -1 -D          [read ANT+ data from stdin and in diagnostic mode]
    
    2.4 Input from multicast
        -------------------
        To test reading data from stdin run the commands
           ./<executable>                                   [read ANT+ data from stdin and output in fully cooked mode]
           ./<executable> -M -J                             [read ANT+ data from stdin and output as JSON objects
           ./<executable> -M -S                             [read ANT+ data from stdin and output in semi-cooked mode]
           ./<executable> -M -S -J                          [read ANT+ data from stdin and output in semi-cooked mode as JSON objects]
           ./<executable> -M -r                             [read ANT+ data from stdin and output as raw bytes]
           ./<executable> -M -D                             [read ANT+ data from stdin and in diagnostic mode]
    

