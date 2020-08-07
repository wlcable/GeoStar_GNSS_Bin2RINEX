# GeoStar GNSS Bin2RINEX Utility
 This utility converts binary data from a GeoStar GeoS-5M GNSS into RINEX format.
 
## Background
 **Disclaimer:** I am by no means a C++ wiz and this is my first project so I am sure there are things that could be done better in this code. I look forward to comments and suggestions that might make the code better.

 This code is based on and was converted from code written in Pascal (with comments in Russian) that was supplied to me by the manufactures of the GNSS, [GeoStar Navigation](http://geostar-navi.com). The original code can be found in the Orig folder and the compiled Windows version can be downloaded from their [website](http://geostar-navi.com/en/software/geos5tornx/).

## Code structure
 All of the functions that deal with reading and converting the binary messages can be found in GeoStar.cpp. The GeoS2RINEX.cpp simply creates a command-line interface for the utility.
 The Makefile is currently not properly written, maybe someone would like to fix this. I have been simply running *make -B GeoS2RINEX* in the src folder.
 
 In addition to creating the RINEX observation and navigation I have added a feature to output data in a format to be used for GPS-IR. 
 
## Command-line Options
	GeoS2RINEX path/input_file [options]
	Options:
			-StationID XXXX, default=XXXX, limit 4 characters
			-SVlog, enable generation of a csv file with Satellite Vehicle Info (CNR, El, Az)
			-Outpath /path/path/, default is same as input
			-CreationAgency XXXX, Name of Agency that creates this file (20)
			-MarkerName XXXX, Name of the Marker (60)
			-MarkerNumber XXXX, Number of the Marker (20)
			-MarkerType XXXX, default=NON_GEODETIC, Name of the Marker (20)
			-NameObserver XXXX, Name of the Observer (21)
			-NameAgency XXXX, Name of the Agency (41)
				
## Known Problems
* Time in the messages is given as seconds since 01/01/2008 00:00:00 (GeoStar Base Time) or 06/01/1980 (GPS Base Time), to do this conversion I simply make a Base Time, *BTGeo* and *BTGPS* that can be added to the time in the messages. When I compile this on my Raspberry Pi this works fine; however, when I compile it on my PC the times are all 1 hour behind. To fix this I have just changed *BTGeo.tm_hour* and *BTGPS.tm_hour* to 1 but there is surely a more elegant way to handle this. I am not sure what is causing this problem but possibly it has to do with differences between the compilers, 32 vs 64-bit?
* The code is currently not dealing with fractions of a second as time_t doesn't give this possibly. This isn't a problem if your data is 1Hz but if you are collecting at faster rates then the seconds will be truncated.
* Only GPS and GLONASS related messages are fully implemented. Messages from other constellations with likely not be correctly converted or ignored completely.

 Let me know if you find something else!

## To Do
* I have compared the output of this code with that of the Windows utility supplied by GeoStar and they match but more checks on the output and code should be done.
* Eventually, I would like to make a utility that could take the raw binary stream from the GNSS connected via Serial and make RINEX files in real-time.


	William Cable
	Bill.Cable@awi.de
	Permafrost Research
	Alfred Wegener Institute Helmholtz Centre for Polar and Marine Research
	Telegrafenberg A45
	14473 Potsdam