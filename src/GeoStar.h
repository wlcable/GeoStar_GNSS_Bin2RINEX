/**  
 * Message Format
    Input and output messages have the same data format.
    Endianness: little endian for both input and output messages.
    Message structure
    * <preamble: 64>: preamble “GEOSr3PS” (length: 8 bytes 534F4547 53503372
    * <ndat>: number of 32 bit words in data fiel ds (length: 2 bytes)
    * <ncmd>: message ID (length: 2 bytes)
    * <dat1, dat2, …, data (length: 4 bytes) fields; N=<ndat>
    * <сs>: check sum (length: 4 bytes); calculated as bit by bit “exclusive OR” of all data fields
*/
#pragma once
#ifndef INCLUDED_GEOSTAR_H
#define INCLUDED_GEOSTAR_H

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
// #include <experimental/filesystem>
// #include <string>

using namespace std;

// namespace fs = std::experimental::filesystem;

namespace GeoStar
{

#define GPSprn 32
#define GLNprn 24
#define GALprn 36
#define maxChan 44 //maximum number of parallel tracking channels (0..43)

//  public:
/* Public declarations */
//RSW: Receiver Status Word
struct TRSW
{
  bool SRAMtest : 1;          //0 Backup SRAM test results:
  bool RTCtest : 1;           //1 RTC test results:
  bool rfPLLstatus : 1;       //2 RF PLL status
  bool reserved1 : 1;         //3
  bool GPS_AGC : 1;           //4 GPS AGC:
  bool GLN_AGC : 1;           //5 GLONASS AGC
  unsigned int AntStatus : 2; //6,7 Antenna status: 0: not measured, 1: overload, 2: open, 3: OK
  bool GPSalm : 1;            //8 GPS almanac availability indicator:
  bool GLNalm : 1;            //9 GLONASS almanac availability indicator:
  bool GALalm : 1;            //10 GALILEO almanac availability indicator:
  bool QZSSalm : 1;           //11 QZSS almanac availability indicator:
  bool TmNav : 1;             //12 Time decoded from navigation message availability indicator:
  bool DtNav : 1;             //13 Date decoded from navigation message availability indicator:
  bool ionGPS : 1;            //14 Ionospheric model/UTC data decoded from GPS message availability indicator:
  bool reserved2 : 1;         //15
  bool fixType : 1;           //16 2D /3D position fix indicator: 0: 3D  1: 2D
  bool reserved3 : 1;         //17
  bool fix1 : 1;              //18 Indicator that at least one position fix has been available after receiver start:
  bool FIX : 1;               //19 Position fix indicator:
  bool staticNav : 1;         //20 Static navigation indicator 0: off (the receiver in motion) 1: on (the receiver static)
  bool DRmode : 1;            //21 DR mode indicator: 0: position fix calculated 1: position fix extrapolated
  bool Diffmode : 1;          //22 Differential mode indicator: 0: autonomous mode 1: differential mode
  bool active : 1;            //23 ACTIVE/SLEEP indicator: 0: SLEEP 1: ACTIVE
  bool RTCMa : 1;             //24 Using RTCM corrections in solution indicator: 0: not used 1: used
  bool RTCMb : 1;             //25 Using RTCM corrections in solution indicator: 0: not used 1: used
  bool Survey : 1;            //26 Survey in status indicator 0: disabled or completed 1: in progress
  bool CalcRef_pos : 1;       //27 Calculated and reference position match indicator 0: delta between calculated and reference coordinates exceeds the threshold and the receiver cannot transit to coordinates hold mode 1:delta between calculated and reference coordinates doesn’t exceed the threshold Effective for coordinates hold mode only
  bool reserved4 : 1;         //28
  bool reserved5 : 1;         //29
  bool Flash : 1;             //30 Saving data to Flash indicator 0: saving not active 1: saving in progress
  bool reserved6 : 1;         //31
} __attribute__((packed));    //to make sure struct is 1 byte aligned

//0x20: Geographic Coordinates: Basic Dataset
struct TCoorUser
{
  double DtTmUTC;          // UTC date and time (s)
  double Lat;              // latitude, rad
  double Lon;              // longitude, rad
  double Alt;              // height above ellipsoid, m
  double dEG;              // Geoial seperation, m, deviation of an ellipsoid from a geoid
  u_int32_t Nfix;          // number of SV in solution
  struct TRSW RSW;         // RSW, type of solution NC
  double GDOP;             // Gdop
  double PDOP;             // PDOP
  double TDOP;             // TDOP
  double HDOP;             // HDOP
  double VDOP;             // Vdop
  u_int32_t PosFixVal;     // Position fix validity indicator: 0: fix valid else: fix not valid
  u_int32_t NumFix;        // the number of continuous reliable fixes
  double speed;            // Horiontal Vel., m/s
  double heading;          // course, rad
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x1B, 0x8B: GLONASS Ephemeris, 24 SV
struct TEphGLN
{
  u_int32_t dtrc; // date and time of data recording in block 1
  u_int16_t N4t;  // generalized. four-year and day number 1461 * N4 + Nt
  u_int16_t NA;   // the day number to which the almanac is transmitted 1
  double pX;
  double pY;
  double pZ; // position, speeds, spacecraft accelerations at time t_b 6
  double vX;
  double vY;
  double vZ; // 6
  double aX;
  double aY;
  double aZ;               // 6
  double gm;               // relates. carrier frequency deviation 2
  double tau;              // BSW shift relative to SSW 2
  double tauc;             // UWS shift relative to UTC (SU) 2
  double taugps;           // discrepancy of NW and NW GPS 2
  u_int16_t prNNK;         // frame invalid sign
                           // 0 bit - ln from line 2 EI
                           // 1 bit - ln from line 3 EI
                           // 2 bits - ln from line 5 EI
                           // 4 bits - ln from line 7 AC
                           // 5 bits - ln from line 9 AC
                           // 6 bits - ln from line 11 AC
                           // 7 bits - ln from line 13 AC
                           // 8 bits - ln from line 15 AC
                           // 9 bits - inverse value of CnA for spacecraft from the almanac
  u_int16_t pss;           // other signs of CA 1
                           // 0..1 - sign P1
                           // 2..3 - sign of Bn
                           // 4 - sign P2
                           // 5 - sign P3
                           // 6..7 - sign P
                           // 8 - sign P4
                           // 9..11 - sign of Day &amp; Hm
                           // 12.13 - sign M
  u_int8_t t_b;            // the interval over which the ephemeris are calculated (1..95)
  u_int8_t enE;            // the age of EI days (1..31)
  u_int8_t Ft;             // predicted measurement error (1..15)
  u_int8_t n;              // KA system point number (1..24) 1
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x1A, 0x8A: GPS/QZSS Ephemeris
struct TEphGPS
{
  u_int32_t dtrc;          // date and time of data recording in block 1
  float dn;                // average dynamic difference with calculated values 1
  double M;                // average anomaly for a given period of time 2
  double e;                // eccentricity 2
  double A;                // semi-root square root 2
  double OMEGA_0;          // longitude of the ascending node in the orbital plane 2
  double I0;               // inclination of the orbit in a given period of time 2
  double w;                // argument (angle) perigee 2
  double OMEGADOT;         // right ascension speed 2
  double af0;              // ShV KA 2 correction factors
  float IDOT;              // orbital inclination coefficient 1
  float C_uc;              // amplitude of the cosine harmonic correction of the term to the
                           // argument of latitude 1
  float C_us;              // amplitude of the sine harmonic correction of the term to the
                           // argument of latitude 1
  float C_rc;              // amplitude of the cosine harmonic correction of the term to the
                           // radius of the orbit 1
  float C_rs;              // amplitude of the sine harmonic correction of the term to the
                           // radius of the orbit 1
  float C_ic;              // amplitude of the cosine harmonic correction of the term to the
                           // angle of inclination 1
  float C_is;              // amplitude of the sine harmonic correction of the term to the
                           // angle of inclination 1
  float Tgd;               // group delay difference 1
  float af2;               // ShV KA 1 correction factors
  float af1;               // 1
  int t_oe;                // starting point in time ephemeris 1
  int t_oc;                // clock data reference time 1
  u_int16_t weekN;         // week number
  u_int16_t IODC;          //
  u_int8_t SVaccur;        // accuracy factor (0..15)
  u_int8_t SVhelth;        // satellite health (6 bits)
  u_int8_t n;              // spacecraft number (1..32) 1
  u_int8_t IODE;           // data age (ephemeris) 1
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x21: Receiver Telemetry
struct TPac21
{
  struct TRSW RSW; //Receiver Status Word
  //Receiver Config. Word1
  bool GLNen : 1;           //0 GLONASS system enabled?
  bool GPSen : 1;           //1 GPS system enabled?
  bool GALen : 1;           //2 GALILEO system enabled?
  bool QZSSen : 1;          //3 QZSS system enabled?
  bool SBASen : 1;          //4 SBAS system enabled?
  bool reserved1 : 1;       //5
  bool SigSearch : 1;       //6 Signal search if GNSS is disabled 0 disabled 1 enabled
  bool PPSen : 1;           //7 1PPS output enabled?
  bool PPSpol : 1;          //8 1PPS polarity: 0: positive 1: negative
  unsigned int PPSsync : 3; //9:11 1PPS synchronization time scale: 0: GPS 1: UTC 2: GLONASS 3: UTC(SU)
  bool Psmoothed : 1;       //12 Pseudorange measurements used in solution 0 unsmoothed 1 smoothed
  bool mode2D : 1;          //13 2D mode: 0: disabled 1: enabled
  bool ffix2D : 1;          //14 2D for the first fix: 0: enabled 1: disabled
  bool KalmanFilter : 1;    //15 Kalman filter 0 off 1 on
  bool reserved2 : 1;       //16
  bool reserved2b : 1;      //17
  unsigned int rate : 2;    //18:19 Output data rate , Hz 0:10 1:5 2:2 3:1
  bool reserved3 : 1;       //20
  unsigned int dynProf : 3; //21:23 User dynamics profile: 0: auto select 1: pedestrian car 2: marine 3: airborne 4 high dynamic
  bool powerSave : 1;       //24 Power-save mode enabled?
  bool psMode : 1;          //25 Power save mode 0: RELAXED FIX 1: FIX BY REQUEST
  bool Psy0x10 : 1;         //26 Pseudorange measurements in message 0х10: 0: unsmoothed 1: smoothed
  bool RAIM : 1;            //27 RAIM (T-RAIM)
  bool OpMode : 1;          //28 Operation mode 0: autonomous or differential 1: coordinates hold
  bool reserved4 : 1;       //29
  bool AntPower : 1;        //30 Antenna power on?
  bool reserved5 : 1;       //31
  //Receiver Config. Word2
  unsigned int reserved6 : 2; //0:1
  bool PRN_SBAS : 1;          //2 PRN SBAS: 0 user defined 1 auto select
  bool SBAScorr : 1;          //3 Using SVs in position fix if SBAS corrections are available: 0: only those SVs for which corrections are available 1: all available SVs
  bool SBAScorrTest : 1;      //4 Using corrections if SBAS SV operates in test mode 0: disabled 1: enabled
  unsigned int reserved7 : 3; //5:7
  bool DiffMode : 1;          //8 Differential mode enabled?
  unsigned int reserved8 : 5; //9:13
  unsigned int CoorSys : 2;   //14:15 Coordinate system 0:WGS 84 1:PZ 90.11 2:user defined
  uint16_t ReceiverType;      //16:31 Receiver type 0xF7FF: GeoS-5M 0xF7FE: GeoS-5MR 0xF7FD: GeoS-5MH
  //end Word2
  uint32_t dtRestart; //Time elapsed since last restart (s)
  uint32_t DtTmUTC;   //Receiver UTC date and time
  float reserved9;
  uint16_t tSurvey; //Time left to completing survey in (s)
  uint16_t reserved10;
  uint8_t SVtracked;       //Number of SVs being tracked
  uint8_t SVfix;           //Number of SVs used in position fix
  uint8_t ChBusy;          //Number of the receiver busy channels
  uint8_t SVview;          //Number of SVs in view
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x22: In View/Active SVs
struct TVisSA
{
  int16_t lit;             // Number of frequency letters for GLN. If not GLN, then 0 is
                           // transmitted
  u_int8_t idnm;           // spacecraft number according to Satellite ID number (NMEA)
  u_int8_t idch;           // Tracking channel number (0..31). If the spacecraft is
                           // not connected, the 0xFF code is transmitted
  u_int32_t ss;            // Channel status word. If the spacecraft is not connected,
                           // then 0 is transmitted.
  float CNR;               // Signal to noise ratio, dBHz
  float el;                // KA elevation angle, rad
  float aZ;                // Azimuth KA, rad
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// packet structure 0x22
struct TPac22
{
  u_int32_t NSat;            // The number of satellites through which information is transmitted
  struct TVisSA vs[maxChan]; // data structure of visible spacecraft
} __attribute__((packed));   //to make sure struct is 1 byte aligned

// 0x23: RTK Output Dataset: ECEF Coordinates
struct TPac23
{
  double DtTmUTC;           // UTC date and time
  double X;                 // X: X coordinate component, WGS-84
  double Y;                 // Y: Y coordinate component, WGS-84
  double Z;                 // Z: coordinate component along the Z axis, WGS-84
  double vX;                // VX: component of the velocity vector along the X axis
  double vY;                // VY: component of the velocity vector along the y axis
  double vZ;                // VZ: component of the velocity vector along the Z axis
  double acPos;             // RMSE 3D coordinate errors
  double acVel;             // SDE 3D speed errors
  int NumEffSV;             // number of spacecraft in solution
  u_int16_t useExtrapolate; // Sign extrapolation RTK solutions
  u_int16_t TypeSolve;      // Type of Solution
  double ClockOffGPS;       // ShV receiver shift relative to ShV GPS
  double lps;               // ShV receiver departure speed
  double ClockOffRaB;       // ShV rover shift relative to ShV base
  double ageDiff;           // Age of differential corrections
  double reserv;
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x24: RTK Output Dataset: Geographic Coordinates
struct TPac24
{
  double DtTmUTC;      // UTC date and time
  double Lat;          // latitude, rad
  double Lon;          // longitude, rad
  double Alt;          // height above ellipsoid, m
  double dEG;          // deviation of an ellipsoid from a geoid, m
  double speed;        // speed, m/s
  double absVecSpeed;  // Vert. speed, m/s
  double heading;      // course, rad
  double GDOP;         // Gdop
  double PDOP;         // PDOP
  double TDOP;         // TDOP
  double HDOP;         // HDOP
  double VDOP;         // Vdop
  double acPos;        // RMSE 3D coordinate errors
  double acVel;        // SDE 3D speed errors
  u_int32_t NumEffSV;  // number of spacecraft in solution
  u_int16_t ModeRTK;   // RTK extrapolation status: 0: in progress; 1: inactive
  u_int16_t TypeSolve; //Solution type: 0: not valid or unavailable; 1: autonomous standard accuracy; 2: code differential; 3: coordinates hold; 4: fixed RTK; 5: float RTK
  int32_t SurveyTime;
  int32_t reserv;
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x13: Navigation Solution State Vector
struct TPac13
{
  double X;                //X axis coordinate of the position, WGS 84 (m)
  double Y;                //Y axis coordinate of the position , WGS 84 (m)
  double Z;                //Z axis coordinate of the position , WGS 84 (m)
  double cs;               //Clock shift relative to GPS time scale (m)
  double vX;               //X axis user velocity (m/s)
  double vY;               //Y axis user velocity (m/s)
  double vz;               //Z axis user velocity (m/s)
  double cd;               //Clock drift (m/s)
  double PDOPn;            //PDOP northing
  double PDOPe;            //PDOP easting
  double PDOPv;            //PDOP vertical
  double reserved[2];      //reserved
  double acPos;            //3D position accuracy estimate (RMS) (m)
  double acVel;            //3D velocity accuracy estimate (RMS) (m)
  double acPPS;            //1PPS accuracy estimate (RMS) (ns)
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x14: Timing Parameters
struct TPac14
{
  double dtUTC;            // Current date and time (UTC), sec (0 - 00:00 January 1, 2008
                           // UTC)
  double dtLOC;            // Current date and time (local), sec
  double ac1PPS;           // 1PPS Signal Accuracy Assessment, nsec
  u_int32_t TmGPS;         // GPS time since the beginning of the week, sec
  u_int32_t TmGLN;         // GLN time from the beginning of the day, sec
  u_int16_t NmWk;          // GPS week number
  u_int16_t RlWk;          // GPS Week Counter Overflows
  u_int16_t Nt;            // The number of days within the four-year GLN
  u_int16_t N4;            // Fourth Anniversary Number of GLN
  u_int8_t revb;           // Reserve
  u_int8_t KP;             // A sign of the planned correction of NW GLN (0 - not
                           // planned, 1 - correction to &quot;+ 1sec&quot;, 2 -
                           // decision not made, 3 - correction to &quot;-1sec&quot;
  u_int8_t flp;            // The planned value of the discrepancy between UTC and
                           // GPS for an integer number of seconds, sec
  u_int8_t lps;            // The current value of the discrepancy between UTC and
                           // GPS for an integer number of seconds, sec
  u_int32_t rev14;         // Reserve
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x19, 0x89: GLONASS Almanac
struct TAlmnGLN
{
  double e;                // eccentricity of the orbit (0 ... 0.03)
  double ddT;              // dragon change speed. period (+ -2e-8)
  double Omg;              // orbit argument (+ -1)
  double dt;               // amendment to wednesday meaning fights. period (+ -3,6e + 3)
  double tl;               // nearest passage of the first sunrise. node (0 ... 44100)
  double Lmd;              // longitude of the first sunrise. node (+ -1)
  double di;               // amendment to avg. orbital inclination value (+ -0.067)
  double ta;               // gross shift of BSW to SSW (+ -1.9e-3)
  u_int16_t num;           // spacecraft number by almanac, PRN 1..24
  u_int16_t day;           // the day on which the almanac is transmitted
  u_int16_t Cn_mdf;        // Sign Cn (15 bit) KA modification (0.1 bit)
  int16_t Alt;             // almanac letter number, Carrier frequency number Hn
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x18: GPS/QZSS Almanac
struct TAlmnGPS
{
  double M;                // mean anomaly
  double e;                // eccentricity
  double A;                // semimajor root
  double OMEGA_0;          // the longitude of the ascending node at 00h.00min.00sec.
                           // base date
  double dI0;              // deviation of the orbit
  double w;                // perigee argument
  double OMEGADOT;         // rate of change of the ascending node of the orbit
  double af0;              // correction to the onboard clock
  double af1;              // rate of change af0
  int t_oa;                // time from base date (sec.)
  u_int16_t num;           // system point number, PRN
  u_int16_t stSv;          // satellite status
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x1C, 0x9C: GPS Ionospheric Parameters
struct TIonParam
{
  float a0; // alpha 0
  float a1;
  float a2;
  float a3;
  float b0; // beta 0
  float b1;
  float b2;
  float b3;
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x1D, 0x9D: GPS Time to UTC Conversion Parameters
struct TUTCParam
{
  double a0;               //Coefficient A0
  double a1;               //Coefficient A1
  u_int32_t tot;           // Start time for UTC data, sec
  u_int16_t dtls;          // Current leap second count, sec
  u_int16_t wnt;           // Starting week number UTC
  u_int16_t wnlsf;         // Future leap second reference Week Number WN LSF
  u_int16_t dn;            // The day number from the end of which the planned discrepancy
                           // between UTC and GPS is valid for an integer number of seconds
  u_int16_t dtlsf;         // The planned value of the discrepancy between UTC and
                           // GPS for an integer number of seconds, sec
  u_int16_t wn;            // Current week number
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// 0x1E, 0x9E: GLONASS Time to UTC Conversion Parameters
struct T25GlnParam
{
  double tauc;             //GLONASS time scale correction to UTC(SU) time tc
  double taugps;           //Correction of GPS time relative to GLONASS time tGPS
  double b1;               //Time shift between UT1 and UTC(SU): polinomial coefficient B1 (bias)
  double b2;               //Time shift between UT1 and UTC(SU): polinomial coefficient B2 (drift)
  u_int16_t N4;            //GLONASS number of four year period
  u_int16_t Nt;            //GLONASS calendar day number within the four year period
  u_int16_t KP;            //Leap second correction notification KP: 0: no correction is scheduled; 1: ‘+1 s’ leap second correction is scheduled; 2: decision has not been made yet; 3: ‘ 1 s’ leap second correction is scheduled
  u_int16_t NA;            //GLONASS calendar day number within the four year period related to almanac data
} __attribute__((packed)); //to make sure struct is 1 byte aligned

//Raw Measurements 0x10, with sub-structure
// structure of measuring information for 0x10 package, each SV
struct TMeasOut
{
  unsigned int reserved : 6; //bit 0:5
  unsigned int LLI : 2;      //bit 6:7 Carrier phase measurements validity (LLI) flag: 0: valid 1: likely cycle slip happened in current measurement interval 2: likely cycle slip happened in previous measurement interval 3: likely cycle slip happened in both current and previous measurement interval
  int8_t Hn;                 // Carrier frequency number H n (two’s complemented Value range: from -7 to +6. Transmits 0 for GPS, SBAS, GALILEO, QZSS
  u_int8_t idnum;            // spacecraft number according to Satellite ID number (NMEA)
  u_int8_t ChNum;            // tracking channel number (0-42)
  float CNR;                 // CNR, signal amplitude (dBHz)
  double prng;               // Pseudorange, m
  double pvel;               // Pseudorange rate, m/s, can be converted to Doppler
  double padr;               // Integrated Doppler, m
  double pphi;               // Carrier phase, cycles
  float acpr;                // Pseudorange error estimate, RMS, m
  float acvl;                // Pseudorange rate error estimate, RMS, m/s
  float erpr;                // Pseudorange residuals, m
  float ervl;                // Pseudorange rate residuals, m / s
} __attribute__((packed));   //to make sure struct is 1 byte aligned

// 0x10: Raw Measurements
struct TPac10
{
  double dtUTC;                // Date and time of measurement information, sec (0 - 00:00 January 1, 2008 UTC)
  int NFs;                     // The number of cycles of 16.369MHz on the measured interval
  u_int16_t LeapSec;           // The current value of the discrepancy between UTC and GPS for an integer number of seconds, sec
  u_int16_t NMSat;             // The number of satellites that transmit measurement information (NSat)
  double ClockOffGPS;          // Clock shift relative to GPS time scale (m)
  struct TMeasOut Ms[maxChan]; // measurement information structure
} __attribute__((packed));     //to make sure struct is 1 byte aligned

// structure of measuring information for 0x0F package
// seems similar to Raw Data 0x10 but not documented in the Binary Protocol
struct TMeasContOut
{
  unsigned int reserved : 6; //bit 0:5
  unsigned int LLI : 2;      //bit 6:7 Carrier phase measurements validity (LLI) flag: 0: valid 1: likely cycle slip happened in current measurement interval 2: likely cycle slip happened in previous measurement interval 3: likely cycle slip happened in both current and previous measurement interval
  int8_t Hn;                 // Carrier frequency number H n (two’s complemented Value range: from -7 to +6. Transmits 0 for GPS, SBAS, GALILEO, QZSS
  u_int8_t idnum;            // spacecraft number according to Satellite ID number (NMEA)
  u_int8_t ChNum;            // tracking channel number (0-42)
  float CNR;                 // signal amplitude
  double prng;               // Pseudorange, m
  double pvel;               // Pseudo rate, m/s
  double pphi;               // Carrier phase, cycles
  double X;                  // Satellite X coordinate, m
  double Y;                  // Satellite Y coordinate, m
  double Z;                  // Coordinate of satellite Z, m
  double vX;                 // Satellite speed along the X axis, m / s
  double vY;                 // Satellite speed along the Y axis, m / s
  double vZ;                 // Satellite speed along the Z axis, m / s
} __attribute__((packed));   //to make sure struct is 1 byte aligned

// packet structure 0x0F
// almost the same as Raw Data 0x10 but not documented in Binary Protocol
struct TPac0F
{
  double dtUTC;                    // Date and time of measurement information, sec (0 - 00:00
                                   // January 1, 2008 UTC)
  int32_t NFs;                     // The number of shreds on the measured interval
  u_int16_t LeapSec;               // The current value of the discrepancy between UTC
                                   // and GPS for an integer number of seconds, sec
  u_int16_t NMSat;                 // The number of satellites that transmit measurement
                                   // information (NSat)
  double ClockOffGPS;              // ShV receiver shift relative to ShV GPS in meters
  struct TMeasContOut Ms[maxChan]; // measurement information structure
} __attribute__((packed));         //to make sure struct is 1 byte aligned

// packet structure 0xC1
// 0xC1: Receiver Type, Firmware Version
struct TPacC1
{
  uint16_t FWver_low;      //FW version, low half word
  uint16_t FWver_high;     //FW version, high half word
  unsigned int FWday : 5;  //FW day, bits 4:0
  unsigned int FWmon : 4;  //FW month, bits 5:8
  unsigned int FWyr : 15;  //FW year, bits 9:23
  unsigned int FWrest : 8; //FW rest of word, bits 24:31, probably 0 or junk
  uint32_t ReceiverType;   //Receiver type, 0xF7FF: GeoS-5M, 0xF7FE: GeoS-5MR, 0xF7FD: GeoS-5MH, 0xE7FE: GeoS-5 RTK
  uint32_t FWcs;           //FW check sum
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// packet structure 0x3E
// 0x3E: Receiver Power-up Message
struct TPac3E
{
  uint32_t SRAMtest;       //Results of backup SRAM integrity test, 0:OK, 1: failed
  uint32_t UTCtest;        //UTC data taken from backup SRAM, 0: data failed, 1: data valid
  uint32_t UTCdata;        //UTC data taken from RTC
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// packet structure 0x3F
// 0x3F: Message Reception Acknowledgement
struct TPac3F
{
  uint32_t messageID;       //Message ID
  uint32_t Ack_code;        //Acknowledgement code
                            // 0: reception is OK
                            // 1: incorrect check sum
                            // 2: incorrect number of data received
                            // 3: incorrect message ID
                            // 4: incorrect value
                            // 5: the message can’t be processed at the time of reception
} __attribute__((packed)); //to make sure struct is 1 byte aligned

// packet structure 0xC3
// 0xC3: Storing Data to Flash Report
struct TPacC3
{
  uint32_t flashReport;    //Saving data to Flash report:
                           // 0: saving data cannot be completed
                           // 1: saving almanacs by command 0xC3 successfully done
                           // 2: saving almanacs automatically successfully done
                           // 3: saving configuration/FW settings successfully done
} __attribute__((packed)); //to make sure struct is 1 byte aligned

const double Fn_GPS = 1575.42e6;                     //GPS carrier frequency (Hz)
const double Fn_GLN = 1602e6;                        //GLONASS carrier frequency (Hz) @ 0th freq. number
const double dFn_GLN = 562.5e3;                      //GLONASS frequency step
const double SL = 299792458;                         //speed of light, m/s
const double pi = 3.1415926535897932384626433832795; //Pi

struct tm BTGeo;    // 01/01/2008 GeoStar Base Time
time_t BaseTimeGeo; // 01/01/2008 GeoStar Base Time
struct tm BTGPS;    // 01/06/1980 GPS Base Time
time_t BaseTimeGPS; // 01/06/1980 GPS Base Time
char StCreateFileTime[22]; //string, file creation date/time, first measurement
//double RegTimeGPS = 29226;  // 01/06/1980, days since 1900
u_int32_t preamble[] = {0x534F4547, 0x53503372}; //"GEOS", "r3PS"
char StationID[5] = "XXXX";                      //Station ID, max 4 char
float verRINEX = 3.02;                           //RINEX version
char obsType = 'M';                              //Observation Type, G:GPS, R:GLONASS, E:Galileo, M:Mixed, etc.
char CreationAgency[21] = "Creation Agency";     //Name of agency creating current file
char CreationProgram[21] = "GeoS2RINEX";         //Name of the program creating current file
char MarkerName[61] = "Marker Name";             //Name of antenna marker
char MarkerNumber[21];                           //Number of antenna marker, optional
char MarkerType[21] = "NON_GEODETIC";            //Type of marker
char NameObserver[21] = "Observer";              //Name of observer
char NameAgency[41] = "Agency";                  //Name of agency
struct antennaType
{
  char num[21];     //Antenna Number
  char type[21];    //Antenna type
  float dH, dE, dN; //antenna offset (m)
} Antenna;

//Structures for holding the latest messages
struct TCoorUser cr20;
struct TPac21 pc21;
struct TPac22 pc22;
struct TPac23 pc23;
struct TPac24 pc24;
struct TPac0F pc0F; //0x0F, Raw Data
struct TPac10 pc10; //0x10, Raw Data
struct TPac13 pc13;
struct TPac14 pc14;
struct TAlmnGLN aGLN;     //GLONASS Almanac
struct TEphGPS eGAL;      //GALILEO Ephemeris
struct TEphGPS eGPS;      //GPS Ephemeris
struct TEphGLN eGLN;      //GLONASS Ephemeris
struct TIonParam ion;     //GPS Ionospheric Parameters
struct TUTCParam utc;     //GPS Time to UTC Conversion Parameters
struct T25GlnParam gln25; //GLONASS Time to UTC Conversion Parameters
struct TPacC1 pcC1;       //Receiver Type, Firmware Version
struct TAlmnGPS aGPS;     //GPS Almanac
struct TPac3E pc3E;       //Receiver Power-up Message
struct TPac3F pc3F;       //Message Reception Acknowledgement
struct TPacC3 pcC3;       //Storing Data to Flash Report

//define functions
void BINfile2RINEX(char *fName);
void Init();
void InitLogging();
void InitOutFiles();
void CloseOutFiles();
void RSWcheck();
u_int32_t CScalc(u_int32_t *dat, u_int16_t ncmd, u_int16_t ndat);
void SVcalc(uint8_t *idnum, char *StSatID, double *kof);
bool svGPS(uint8_t *idnum);
bool svGLN(uint8_t *idnum);
bool svGAL(uint8_t *idnum);
bool svQZSS(uint8_t *idnum);
bool ExCm0A();
bool ExCm0F();
bool ExCm10();
bool ExCm13();
bool ExCm14();
bool ExCm18();
bool ExCm19();
bool ExCm1A();
bool ExCm1B();
bool ExCm1C();
bool ExCm1D();
bool ExCm1E();
bool ExCm20();
bool ExCm24();
bool ExCm22();
bool ExCm21();
bool ExCm23();
bool ExCmC1();
//bool ExCm88();
bool ExCm3E();
bool ExCm3F();
bool ExCmC3();

bool CSpass;                //did the CS pass?
bool mLength;               //is the message the correct length, ndat?
bool LOGGING = true;        //should errors be saved to a log file
bool SVinfoLOGGING = false; //create a file with SV info, CNR, AZ, EL
fstream FGeosBin;           // binary input file
fstream FLog;               // ascii log file
fstream F_RinMO;            //Rinex MO Mixed Obs.
fstream F_RinMN;            //Rinex MN-Nav file. All GNSS
fstream F_SVinfo;           //File with SV info, CNR, AZ, EL

u_int16_t ncmd; //message type
u_int16_t ndat; //number of words in the message, excluding CS
u_int32_t word; //one word read from the stream
char Ch; //one char read from the stream

int LeapSec = 0; // The current value of the discrepancy between UTC and GPS
                 // for an integer number of seconds, sec
double ClockOffset;
u_int8_t WRollOver;   // GPS Week Counter Overflows
struct tm TimeEposh1; // epoch time for writing coordinate file ROVER
time_t pc10dtUTC_OLD; // previous time to control the monotony of eras
time_t systime; //system time

int FirstAnalysis;

char alphabet[27] = "abcdefghijklmnopqrstuvwxyz"; //used for coding the hour as a letter in filename
char buff[1000]; //temp buffer for formatting strings before writing to file

bool pr0x14 = false; //0x14: Timing Parameters found?
bool pr0x18 = false; //0x18: GPS/QZSS Almanac found?
bool pr0x19 = false; //0x19, 0x89 : GLONASS Almanac found?
bool pr0x1C = false; //0x1C, 0x9C: GPS Ionospheric Parameters found?
bool pr0x1D = false; //0x1D, 0x9D: GPS Time to UTC Conversion Parameters found?
bool pr0x1E = false; //0x1E, 0x9E: GLONASS Time to UTC Conversion Parameters found?
bool pr0x0F = false; //0x0F, Raw Measurements found?
bool pr0x10 = false; //0x10: Raw Measurements found?
bool pr0x13 = false; //0x13: Navigation Solution State Vector found?
bool pr0x0A = false; //0x0A, 0x9A : GALILEO Ephemeris found?
bool pr0x1A = false; //0x1A, 0x8A: GPS/QZSS Ephemeris found?
bool pr0x1B = false; //0x1B, 0x8B : GLONASS Ephemeris found?
bool pr0x88 = false; //0x88: Query GPS/QZSS Almanac found?
bool pr0x21 = false; //0x21
bool pr0x20 = false; //0x20
bool pr0x22 = false; //0x22
bool pr0x23 = false; //0x23
bool pr0x24 = false; //0x24

bool G_OBS = false; //GPS OBS?
bool R_OBS = false; //Gln OBS?
bool E_OBS = false; //Gal OBS?
bool J_OBS = false; //QZSS OBS?

int ArLitNumKA[GLNprn + 1];     // array of correspondence of spacecraft number and letter /*1..24*/, index 0 is nothing
float el[198];                  // UM array for all spacecraft in solution /*?*/ /*1..199*/
bool GinFix = false;            // have a fix?
char verSt[10] = "";            //firmware version
char StSN[10] = "";             //Serial number
char StRecType[20] = "unknown"; //Receiver Type
char outpath[200];              //output file path

struct TEphGPS ArTEphGPS[GPSprn + 1]; //*?*/ /*1..32*/
struct TEphGLN ArTEphGLN[GLNprn + 1]; //*?*/ /*1..24*/
struct TEphGPS ArTEphGAL[GALprn + 1]; //*?*/ /*1..36*/

struct TEphGPS ArTEphGPS_od[GPSprn + 1]; // array of received ephemeris without date /*?*/ /*1..32*/
struct TEphGLN ArTEphGLN_od[GLNprn + 1]; // array of received ephemeris without date /*?*/ /*1..24*/

struct tm FTime;   // time of the first measurement in the form of a system
time_t FdtUTC = 0; // time of the first measurement from the first message 0x10

bool ChDT = false;      // flag for highlighting the date and time from the navigation message
bool RecEph_od = false; // flag to record the entire array of ephemeris without date

struct TRSW RSW;          //Latest Receiver Status Word
struct TRSW RSWout = {0}; //a flag for if the error in RSW has been reported in the log file

double coorX;
double coorY;
double coorZ;
bool errCS10;
bool errCS13;

int ArSVsOBS_GPS[maxChan];              // array of measurements for all GPS spacecraft in the
                                        // entire file; 1- the measurement was; 0-measurements
                                        // were not
struct TAlmnGPS ArTAlmnGPS[GPSprn + 1]; //*?*/ /*1..32*/ (32) - (1) + 1

time_t TimeUTC; //current decision time, UTC

u_int16_t num3E = 0; //number of receipts for restart G5 (usually 4pcs)

} // namespace GeoStar

#endif // INCLUDED_UNITRINEX_EN_COPY_H
// END
