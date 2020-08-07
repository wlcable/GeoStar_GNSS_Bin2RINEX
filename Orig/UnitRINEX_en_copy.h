/**   @file
     @brief
*/
#pragma once
#ifndef INCLUDED_UNITRINEX_EN_COPY_H
#define INCLUDED_UNITRINEX_EN_COPY_H

// #include "Windows.h"
// #include "Messages.h"
// #include "SysUtils.h"
// #include "DateUtils.h"
// #include "Variants.h"
// #include "Classes.h"
// #include "Graphics.h"
// #include "Controls.h"
// #include "Forms.h"

// #include "Dialogs.h"
// #include "StdCtrls.h"
// #include "ComCtrls.h"

// #include "Registry.h"
// #include "ExtCtrls.h"
// #include "RzRadGrp.h"
// #include "RzPanel.h"
// #include "Mask.h"
// #include "JvExMask.h"
// #include "JvToolEdit.h"
// #include "AdvEdit.h"

// #include "AdvEdBtn.h"
// #include "AdvFileNameEdit.h"

struct /*class*/ TFormRNX : public TForm

{
  // TMemo OknoTP;
  // TProgressBar PBarRNX;
  // TButton Button2;
  // TRzRadioGroup RGSV;
  // TJvFilenameEdit FNEdit;
  // TLabel Label1;
  // TEdit EditElMask;
  // TCheckBox ChBoxClockOFF;
  void FormCreate(TObject Sender);
  void FormClose(TObject Sender, TCloseAction& Action);
  void ConvComplete();
  void ExCm0A();
  void ExCm0F();
  void ExCm10();
  void ExCm13();
  void ExCm14();
  void ExCm19();
  void ExCm1A();
  void ExCm1B();
  void ExCm1C();
  void ExCm1D();
  void ExCm1E();
  void ExCm20();
  void ExCm24();
  void ExCm22();
  void ExCm23();
  void ExCmC1();
  void ExCm88();
  void ExCm3E();
  void Button2Click(TObject Sender);

  void RGSVClick(TObject Sender);

 private:
  /* Private declarations */

 public:
  /* Public declarations */
};

typedef u_int32_t TArLw[/*?*/];  // data structure for packages for PSGG

struct TCoorUser {
  double DtTmUTC;        // UTC date and time
  double Lat;            // latitude
  double Lon;            // longitude
  double Alt;            // height above ellipsoid
  double dEG;            // deviation of an ellipsoid from a geoid
  int Nfix;              // number of spacecraft in solution
  int mdFix;             // type of solution NC
  double GDOP;           // Gdop
  double PDOP;           // PDOP
  double TDOP;           // TDOP
  double HDOP;           // HDOP
  double VDOP;           // Vdop
  int PrValNT;           // evidence of reliability of the decision of the NC
  u_int32_t NValNT;  // the number of continuous reliable decisions NZ
  double speed;          // speed in terms
  double heading;        // course
} __attribute__ ((packed)); //to make sure struct is 1 byte aligned

// GLONASS CA ephemeris structure
struct TEphGLN {
  u_int32_t dtrc;  // date and time of data recording in block 1
  u_int16_t N4t;  // generalized. four-year and day number 1461 * N4 + Nt
  u_int16_t NA;   // the day number to which the almanac is transmitted 1
  double pX;
  double pY;
  double pZ;  // position, speeds, spacecraft accelerations at time t_b 6
  double vX;
  double vY;
  double vZ;  // 6
  double aX;
  double aY;
  double aZ;      // 6
  double gm;      // relates. carrier frequency deviation 2
  double tau;     // BSW shift relative to SSW 2
  double tauc;    // UWS shift relative to UTC (SU) 2
  double taugps;  // discrepancy of NW and NW GPS 2
  u_int16_t
      prNNK;  // frame invalid sign
              // 0 bit - ln from line 2 EI
              // 1 bit - ln from line 3 EI
              // 2 bits - ln from line 5 EI
              // 4 bits - ln from line 7 AC
              // 5 bits - ln from line 9 AC
              // 6 bits - ln from line 11 AC
              // 7 bits - ln from line 13 AC
              // 8 bits - ln from line 15 AC
              // 9 bits - inverse value of CnA for spacecraft from the almanac
  u_int16_t pss;  // other signs of CA 1
                       // 0..1 - sign P1
                       // 2..3 - sign of Bn
                       // 4 - sign P2
                       // 5 - sign P3
                       // 6..7 - sign P
                       // 8 - sign P4
                       // 9..11 - sign of Day &amp; Hm
                       // 12.13 - sign M
  u_int8_t
      t_b;  // the interval over which the ephemeris are calculated (1..95)
  u_int8_t enE;  // the age of EI days (1..31)
  u_int8_t Ft;   // predicted measurement error (1..15)
  u_int8_t n;    // KA system point number (1..24) 1
};

// GPS satellite ephemeris structure
struct TEphGPS {
  u_int32_t dtrc;  // date and time of data recording in block 1
  float dn;            // average dynamic difference with calculated values 1
  double M;            // average anomaly for a given period of time 2
  double e;            // eccentricity 2
  double A;            // semi-root square root 2
  double OMEGA_0;      // longitude of the ascending node in the orbital plane 2
  double I0;           // inclination of the orbit in a given period of time 2
  double w;            // argument (angle) perigee 2
  double OMEGADOT;     // right ascension speed 2
  double af0;          // ShV KA 2 correction factors
  float IDOT;          // orbital inclination coefficient 1
  float C_uc;  // amplitude of the cosine harmonic correction of the term to the
               // argument of latitude 1
  float C_us;  // amplitude of the sine harmonic correction of the term to the
               // argument of latitude 1
  float C_rc;  // amplitude of the cosine harmonic correction of the term to the
               // radius of the orbit 1
  float C_rs;  // amplitude of the sine harmonic correction of the term to the
               // radius of the orbit 1
  float C_ic;  // amplitude of the cosine harmonic correction of the term to the
               // angle of inclination 1
  float C_is;  // amplitude of the sine harmonic correction of the term to the
               // angle of inclination 1
  float Tgd;   // group delay difference 1
  float af2;   // ShV KA 1 correction factors
  float af1;   // 1
  int t_oe;    // starting point in time ephemeris 1
  int t_oc;    // clock data reference time 1
  u_int16_t weekN;   // week number
  u_int16_t IODC;    //
  u_int8_t SVaccur;  // accuracy factor (0..15)
  u_int8_t SVhelth;  // satellite health (6 bits)
  u_int8_t n;        // spacecraft number (1..32) 1
  u_int8_t IODE;     // data age (ephemeris) 1
};

// data structure of visible spacecraft
struct TVisSA {
  short lit;  // Number of frequency letters for GLN. If not GLN, then 0 is
              // transmitted
  u_int8_t
      idnm;  // spacecraft number according to Satellite ID number (NMEA)
  u_int8_t idch;  // Tracking channel number (0..31). If the spacecraft is
                       // not connected, the 0xFF code is transmitted
  u_int32_t ss;  // Channel status word. If the spacecraft is not connected,
                     // then 0 is transmitted.
  float Amp;         // Signal to noise ratio, dBHz
  float el;          // KA elevation angle, glad
  float aZ;          // Azimuth KA, glad
};

// packet structure 0x22
struct TPac22 {
  int NSat;       // The number of satellites through which information is
                  // transmitted
  TVisSA vs[43];  // data structure of visible spacecraft
};

struct TPac23 {
  double DtTmUTC;  // UTC date and time
  double X;        // X: X coordinate component, WGS-84
  double Y;        // Y: Y coordinate component, WGS-84
  double Z;        // Z: coordinate component along the Z axis, WGS-84
  double vX;       // VX: component of the velocity vector along the X axis
  double vY;       // VY: component of the velocity vector along the y axis
  double vZ;       // VZ: component of the velocity vector along the Z axis
  double acPos;    // RMSE 3D coordinate errors
  double acVel;    // SDE 3D speed errors
  int NumEffSV;    // number of spacecraft in solution
  u_int16_t useExtrapolate;  // Sign extrapolation RTK solutions
  u_int16_t TypeSolve;       // Type of Solution
  double ClockOffGPS;             // ShV receiver shift relative to ShV GPS
  double lps;                     // ShV receiver departure speed
  double ClockOffRaB;             // ShV rover shift relative to ShV base
  double ageDiff;                 // Age of differential corrections
  double reserv;
};

// packet structure 0x24
struct TPac24 {
  double DtTmUTC;  // UTC date and time
  double Lat;      // latitude
  double Lon;      // longitude
  double Alt;      // height above ellipsoid
  double dEG;      // deviation of an ellipsoid from a geoid
  double speed;    // speed in terms
  double absVecSpeed;
  double heading;  // course
  double GDOP;     // Gdop
  double PDOP;     // PDOP
  double TDOP;     // TDOP
  double HDOP;     // HDOP
  double VDOP;     // Vdop
  double acPos;    // RMSE 3D coordinate errors
  double acVel;    // SDE 3D speed errors
  int NumEffSV;    // number of spacecraft in solution
  u_int16_t ModeRTK;
  u_int16_t TypeSolve;
  int SurveyTime;
  int reserv;
};

// package structure 0x13
struct TPac13 {
  double dt[16];
};

// package structure 0x14
struct TPac14 {
  double dtUTC;   // Current date and time (UTC), sec (0 - 00:00 January 1, 2008
                  // UTC)
  double dtLOC;   // Current date and time (local), sec
  double ac1PPS;  // 1PPS Signal Accuracy Assessment, nsec
  u_int32_t TmGPS;  // GPS time since the beginning of the week, sec
  u_int32_t TmGLN;  // GLN time from the beginning of the day, sec
  u_int16_t NmWk;  // GPS week number
  u_int16_t RlWk;  // GPS Week Counter Overflows
  u_int16_t Nt;    // The number of days within the four-year GLN
  u_int16_t N4;    // Fourth Anniversary Number of GLN
  u_int8_t revb;   // Reserve
  u_int8_t KP;     // A sign of the planned correction of NW GLN (0 - not
                        // planned, 1 - correction to &quot;+ 1sec&quot;, 2 -
                     // decision not made, 3 - correction to &quot;-1sec&quot;
  u_int8_t flp;    // The planned value of the discrepancy between UTC and
                        // GPS for an integer number of seconds, sec
  u_int8_t lps;    // The current value of the discrepancy between UTC and
                        // GPS for an integer number of seconds, sec
  u_int32_t rev14;  // Reserve
};

// GLONASS KA almanac structure
struct TAlmnGLN {
  double e;    // eccentricity of the orbit (0 ... 0.03)
  double ddT;  // dragon change speed. period (+ -2e-8)
  double Omg;  // orbit argument (+ -1)
  double dt;   // amendment to wednesday meaning fights. period (+ -3,6e + 3)
  double tl;   // nearest passage of the first sunrise. node (0 ... 44100)
  double Lmd;  // longitude of the first sunrise. node (+ -1)
  double di;   // amendment to avg. orbital inclination value (+ -0.067)
  double ta;   // gross shift of BSW to SSW (+ -1.9e-3)
  u_int16_t num;     // spacecraft number by almanac
  u_int16_t day;     // the day on which the almanac is transmitted
  u_int16_t Cn_mdf;  // Sign Cn (15 bit) KA modification (0.1 bit)
  short Alt;              // almanac letter number
};

// GPS almanac structure GPS
struct TAlmnGPS {
  double M;         // mean anomaly
  double e;         // eccentricity
  double A;         // semimajor root
  double OMEGA_0;   // the longitude of the ascending node at 00h.00min.00sec.
                    // base date
  double dI0;       // deviation of the orbit
  double w;         // perigee argument
  double OMEGADOT;  // rate of change of the ascending node of the orbit
  double af0;       // correction to the onboard clock
  double af1;       // rate of change af0
  int t_oa;         // time from base date (sec.)
  u_int16_t num;   // system point number
  u_int16_t stSv;  // satellite status
};

// structure of ionosphere parameters
struct TIonParam {
  float a0;  // alpha 0
  float a1;
  float a2;
  float a3;
  float b0;  // beta 0
  float b1;
  float b2;
  float b3;
};

// GPS time reference settings
struct TUTCParam {
  double a0;
  double a1;
  int tot;              // Start time for UTC data, sec
  u_int16_t dtls;  // The current value of the discrepancy between UTC and
                        // GPS for an integer number of seconds, sec
  u_int16_t wnt;   // Starting week number UTC
  u_int16_t
      wnlsf;  // The week number from which the planned discrepancy between UTC
              // and GPS is valid for an integer number of seconds
  u_int16_t
      dn;  // The day number from the end of which the planned discrepancy
           // between UTC and GPS is valid for an integer number of seconds
  u_int16_t dtlsf;  // The planned value of the discrepancy between UTC and
                         // GPS for an integer number of seconds, sec
  u_int16_t wn;     // Current week number
};

// GLONASS time reference parameters
struct T25GlnParam {
  double tauc;
  double taugps;
  double b1;
  double b2;
  u_int16_t N4;
  u_int16_t Nt;
  u_int16_t KP;
  u_int16_t NA;
};

// structure of measuring information for 0x10 package
struct TMeasOut {
  u_int8_t tdt;  // data reliability integral characteristic
  signed char
      lit;  // frequency letter number for GLONASS in the additional code
  u_int8_t
      idnum;  // spacecraft number according to Satellite ID number (NMEA)
  u_int8_t idChn;  // tracking channel number
  float ampsg;          // signal amplitude
  double prng;          // Pseudorange, m
  double pvel;          // Pseudo speed, m / s
  double padr;          // Relate pseudorange change
  double pphi;          // Carrier phase, glad (????)
  float acpr;           // Evaluation of the standard deviation of pseudorange
                        // measurements, m
  float acvl;  // Evaluation of the standard deviation of pseudo-velocity
               // measurements, m / s
  float erpr;  // Pseudorange error estimate, m
  float ervl;  // Evaluation of the pseudo-velocity error, m / s
};
// package structure 0x10
struct TPac10 {
  double dtUTC;             // Date and time of measurement information, sec (0 - 00:00
                            // January 1, 2008 UTC)
  int NFs;                 // The number of cycles on the measured interval
  u_int16_t LeapSec;  // The current value of the discrepancy between UTC
                           // and GPS for an integer number of seconds, sec
  u_int16_t NMSat;    // The number of satellites that transmit measurement
                           // information (NSat)
  double ClockOffGPS;      // ShV receiver shift relative to ShV GPS in meters
  TMeasOut Ms[43];         // measurement information structure
};

// structure of measuring information for 0x0F package
struct TMeasContOut {
  u_int8_t tdt;  // data reliability integral characteristic
  signed char
      lit;  // frequency letter number for GLONASS in the additional code
  u_int8_t
      idnum;  // spacecraft number according to Satellite ID number (NMEA)
  u_int8_t idChn;  // tracking channel number
  float ampsg;          // signal amplitude
  double prng;          // Pseudorange, m
  double pvel;          // Pseudo speed, m / s
  double pphi;          // Carrier phase, glad (????)
  double X;             // Satellite X coordinate, m
  double Y;             // Satellite Y coordinate, m
  double Z;             // Coordinate of satellite Z, m
  double vX;            // Satellite speed along the X axis, m / s
  double vY;            // Satellite speed along the Y axis, m / s
  double vZ;            // Satellite speed along the Z axis, m / s
};
// packet structure 0x0F
struct TPac0F {
  double dtUTC;  // Date and time of measurement information, sec (0 - 00:00
                 // January 1, 2008 UTC)
  int NFs;       // The number of shreds on the measured interval
  u_int16_t LeapSec;  // The current value of the discrepancy between UTC
                           // and GPS for an integer number of seconds, sec
  u_int16_t NMSat;    // The number of satellites that transmit measurement
                           // information (NSat)
  double ClockOffGPS;      // ShV receiver shift relative to ShV GPS in meters
  TMeasContOut Ms[43];     // measurement information structure
};

static double /*?*/ const L1freq = 1575.42e6;
static double /*?*/ const SL = 299792458;
static double /*?*/ const pi = 3.1415926535897932384626433832795;

TFormRNX FormRNX;
TFormatSettings formatSettings;
TRegistry Reg;
TRegistry FReg;
std::string AppDir;  // launcher directory
std::string Fnam;
std::string Nkml1;

int ArDat[/*?*/];           // data array inform. the package
double RegTime = 39448;     // 01/01/2008
double RegTimeGPS = 29226;  // 01/06/1980
std::string StrFile1;
AnsiString StrFile;
TextFile F_RinMO;
TextFile F_RinMN;  // Rinex MO Mixed Obs. and Rinex MN-Nav file. All GNSS
TextFile LOG;      // program log file
File F1;           // of char    // BIN file
File F2;           // of char
int hnd;
int siz;
int kol;  // file size and processed number
std::string NameFrnx;

int indGPS;
int indGLN;
bool FEnter;  // sign of synchronization
bool Hold;

int LeapSec = 0;  // The current value of the discrepancy between UTC and GPS
                  // for an integer number of seconds, sec
double ClockOffset;
double ClockOffset1;
int WRollOver = 0;  // GPS Week Counter Overflows
std::string StYear10;
std::string StMonth10;
std::string StDay10;
std::string StHour10;
std::string StMinute10;
std::string StSecond10;
std::string StKA10;
time_t TimeEposh1;  // epoch time for writing coordinate file ROVER
double pc10dtUTC_OLD;    // previous time to control the monotony of eras
std::string StPac10;     // package line 0x10
std::string ArSVsOBS[43];

u_int8_t FirstAnalysis = 1;

bool pr0x14 = false;
bool pr0x19 = false;
bool pr0x1C = false;
bool pr0x1D = false;
bool pr0x1E = false;

bool pr0x0F = false;
bool pr0x10 = false;
bool pr0x13 = false;
bool pr0x0A = false;
bool pr0x1A = false;
bool pr0x1B = false;
bool pr0x88 = false;

bool G_OBS = false;
bool R_OBS = false;
bool E_OBS = false;
bool J_OBS = false;

int ArLitNumKA[/*?*/ /*1..24*/ (24) - (1) +
               1];  // array of correspondence of the spacecraft number and the
                    // letter
float el[/*?*/ /*1..199*/ (199) - (1) +
         1];          // UM array for all spacecraft in solution
bool GinFix = false;  // sign of decision
std::string verSt = "";
std::string StSN = "";

TEphGPS ArTEphGPS[/*?*/ /*1..32*/ (32) - (1) + 1];
TEphGLN ArTEphGLN[/*?*/ /*1..24*/ (24) - (1) + 1];
TEphGPS ArTEphGAL[/*?*/ /*1..36*/ (36) - (1) + 1];

TEphGPS ArTEphGPS_od[/*?*/ /*1..32*/ (32) - (1) +
                     1];  // array of received ephemeris without date
TEphGLN ArTEphGLN_od[/*?*/ /*1..24*/ (24) - (1) +
                     1];  // array of received ephemeris without date

std::string ArMesg1C[8];  // 0x1C message data array GPS ionosphere parameters
std::string ArMesg1D[4];  // GPS time reference 0x1D data array
std::string ArMesg1E[3];  // GLO time reference 0x1E data array

time_t FTime;  // time of the first measurement in the form of a system
double FdtUTC = 0;  // time of the first measurement from the first message 0x10
bool ChDT = false;  // flag for highlighting the date and time from the
                    // navigation message
bool RecEph_od =
    false;  // flag to record the entire array of ephemeris without date
bool GGFix = false;  // sign of joint decision

double coorX;
double coorY;
double coorZ;
bool errCS10;
bool errCS13;

int ArSVsOBS_GPS[43];  // array of measurements for all GPS spacecraft in the
                       // entire file; 1- the measurement was; 0-measurements
                       // were not
TAlmnGPS ArTAlmnGPS[/*?*/ /*1..32*/ (32) - (1) + 1];

double TimeUTC;  // current decision time

u_int16_t num3E = 0;  // number of receipts for restart G5 (usually 4pcs)

#endif  // INCLUDED_UNITRINEX_EN_COPY_H
// END
