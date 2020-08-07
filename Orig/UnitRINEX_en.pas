unit UnitRINEX;

interface

uses
  Windows, Messages, SysUtils,DateUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls,
  Registry, ExtCtrls, RzRadGrp, RzPanel, Mask, JvExMask, JvToolEdit, AdvEdit,
  AdvEdBtn, AdvFileNameEdit;
type
  TFormRNX = class(TForm)
    OknoTP: TMemo;
    PBarRNX: TProgressBar;
    Button2: TButton;
    RGSV: TRzRadioGroup;
    FNEdit: TJvFilenameEdit;
    Label1: TLabel;
    EditElMask: TEdit;
    ChBoxClockOFF: TCheckBox;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure ConvComplete;
    Procedure ExCm0A;
    Procedure ExCm0F;
    Procedure ExCm10;
    Procedure ExCm13;
    Procedure ExCm14;
    Procedure ExCm19;
    Procedure ExCm1A;
    Procedure ExCm1B;
    Procedure ExCm1C;
    Procedure ExCm1D;
    Procedure ExCm1E;
    Procedure ExCm20;
    Procedure ExCm24;
    Procedure ExCm22;
    Procedure ExCm23;
    Procedure ExCmC1;
    Procedure ExCm88;
    Procedure ExCm3E;
    procedure Button2Click(Sender: TObject);

    procedure RGSVClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

type

  TArLw = array of longword;  // data structure for packages for PSGG

  TCoorUser = record
    DtTmUTC: double;		            // UTC date and time
	  Lat: double;			              // latitude
	  Lon: double;			              // longitude
	  Alt: double;			              // height above ellipsoid
	  dEG: double;			              // deviation of an ellipsoid from a geoid
	  Nfix: integer;			            // number of spacecraft in solution
	  mdFix: integer;			            // type of solution NC
	  GDOP: double; 	   	            // Gdop
	  PDOP: double;		                // PDOP
	  TDOP: double;		                // TDOP
	  HDOP: double;		                // HDOP
	  VDOP: double;		                // Vdop
    PrValNT: integer;		            // evidence of reliability of the decision of the NC
	  NValNT: longword;			          // the number of continuous reliable decisions NZ
    speed: double;		              // speed in terms
	  heading: double;		            // course
  end;

  // GLONASS CA ephemeris structure
  TEphGLN = record
	  dtrc: longword; 	  // date and time of data recording in block 1
	  N4t: word;          // generalized. four-year and day number 1461 * N4 + Nt
    NA: word; 	        // the day number to which the almanac is transmitted 1
    pX, pY, pZ: double; // position, speeds, spacecraft accelerations at time t_b 6
    vX, vY, vZ: double;	// 6
	  aX, aY, aZ: double;	// 6
	  gm: double;       	// relates. carrier frequency deviation 2
    tau: double;      	// BSW shift relative to SSW 2
    tauc: double;     	// UWS shift relative to UTC (SU) 2
    taugps: double;   	// discrepancy of NW and NW GPS 2
    prNNK: word;        // frame invalid sign
                      	// 0 bit - ln from line 2 EI
                      	// 1 bit - ln from line 3 EI
                      	// 2 bits - ln from line 5 EI
                      	// 4 bits - ln from line 7 AC
                      	// 5 bits - ln from line 9 AC
                      	// 6 bits - ln from line 11 AC
                      	// 7 bits - ln from line 13 AC
                      	// 8 bits - ln from line 15 AC
                      	// 9 bits - inverse value of CnA for spacecraft from the almanac
    pss: word;        	// other signs of CA 1
                      	// 0..1 - sign P1
                      	// 2..3 - sign of Bn
                      	// 4 - sign P2
                      	// 5 - sign P3
                      	// 6..7 - sign P
                      	// 8 - sign P4
                      	// 9..11 - sign of Day &amp; Hm
                      	// 12.13 - sign M
	  t_b: byte;          // the interval over which the ephemeris are calculated (1..95)
    enE: byte; 	        // the age of EI days (1..31)
    Ft: byte;           // predicted measurement error (1..15)
   	n: byte;            // KA system point number (1..24) 1
  end;

// GPS satellite ephemeris structure
  TEphGPS = record
	  dtrc: longword; 	  // date and time of data recording in block 1
	  dn: single;       	// average dynamic difference with calculated values 1
 	  M: double;        	// average anomaly for a given period of time 2
	  e: double;       	  // eccentricity 2
	  A: double;        	// semi-root square root 2
	  OMEGA_0: double;  	// longitude of the ascending node in the orbital plane 2
	  I0: double;       	// inclination of the orbit in a given period of time 2
	  w: double;        	// argument (angle) perigee 2
	  OMEGADOT: double; 	// right ascension speed 2
	  af0: double;		    // ShV KA 2 correction factors
	  IDOT: single;     	// orbital inclination coefficient 1
	  C_uc: single;     	// amplitude of the cosine harmonic correction of the term to the argument of latitude 1
	  C_us: single;     	// amplitude of the sine harmonic correction of the term to the argument of latitude 1
	  C_rc: single;     	// amplitude of the cosine harmonic correction of the term to the radius of the orbit 1
	  C_rs: single;     	// amplitude of the sine harmonic correction of the term to the radius of the orbit 1
	  C_ic: single;     	// amplitude of the cosine harmonic correction of the term to the angle of inclination 1
	  C_is: single;     	// amplitude of the sine harmonic correction of the term to the angle of inclination 1
	  Tgd: single;			  // group delay difference 1
	  af2: single;      	// ShV KA 1 correction factors
	  af1: single;			  // 1
	  t_oe: integer;     	// starting point in time ephemeris 1
	  t_oc: integer;     	// clock data reference time 1
   	weekN: word;	      // week number
    IODC: word; 	      // 
	  SVaccur: byte;	    // accuracy factor (0..15)
    SVhelth: byte; 	    // satellite health (6 bits)
	  n: byte;		        // spacecraft number (1..32) 1
    IODE: byte; 	      // data age (ephemeris) 1
  end;

// data structure of visible spacecraft
  TVisSA = record
  	lit: smallint; 	// Number of frequency letters for GLN. If not GLN, then 0 is transmitted
	  idnm: byte;	    // spacecraft number according to Satellite ID number (NMEA)
  	idch: byte;	    // Tracking channel number (0..31). If the spacecraft is not connected, the 0xFF code is transmitted
	  ss: longword;	  // Channel status word. If the spacecraft is not connected, then 0 is transmitted.
  	Amp: single;		// Signal to noise ratio, dBHz
	  el: single;			// KA elevation angle, glad
  	az: single;			// Azimuth KA, glad
  end;

// packet structure 0x22
  TPac22 = record
	  NSat: integer;			// The number of satellites through which information is transmitted
    vs: array [0..42] of TVisSA;		// data structure of visible spacecraft
  end;

  TPac23 = record
   DtTmUTC: double;		        // UTC date and time
	 X: double;			            // X: X coordinate component, WGS-84
	 Y: double;			            // Y: Y coordinate component, WGS-84
	 Z: double;			            // Z: coordinate component along the Z axis, WGS-84
	 Vx: double;			          // VX: component of the velocity vector along the X axis
   Vy: double;		            // VY: component of the velocity vector along the y axis
   Vz: double;                // VZ: component of the velocity vector along the Z axis
   acPos: double;             // RMSE 3D coordinate errors
   acVel: double;             // SDE 3D speed errors
   NumEffSV: integer;			    // number of spacecraft in solution
   useExtrapolate: word;      // Sign extrapolation RTK solutions
   TypeSolve: word;           // Type of Solution
   ClockOffGPS: double;       // ShV receiver shift relative to ShV GPS
   lps: double;               // ShV receiver departure speed
   ClockOffRaB: double;       // ShV rover shift relative to ShV base
   ageDiff: double;           // Age of differential corrections
   reserv: double;
  end;

  // packet structure 0x24
  TPac24 = record
    DtTmUTC: double;		            // UTC date and time
	  Lat: double;			              // latitude
	  Lon: double;			              // longitude
	  Alt: double;			              // height above ellipsoid
	  dEG: double;			              // deviation of an ellipsoid from a geoid
    speed: double;		              // speed in terms
    absVecSpeed: double;
    heading: double;		            // course
    GDOP: double; 	   	            // Gdop
	  PDOP: double;		                // PDOP
	  TDOP: double;		                // TDOP
	  HDOP: double;		                // HDOP
	  VDOP: double;		                // Vdop
    acPos: double;                // RMSE 3D coordinate errors
    acVel: double;                // SDE 3D speed errors
    NumEffSV: integer;			         // number of spacecraft in solution
    ModeRTK: word;
    TypeSolve: word;
    SurveyTime: integer;
    reserv: integer;
  end;

  // package structure 0x13
  TPac13 = record
    dt: array[0..15] of double;
  end;

  // package structure 0x14
  TPac14 = record
  	dtUTC: double;		// Current date and time (UTC), sec (0 - 00:00 January 1, 2008 UTC)
	  dtLOC: double;		// Current date and time (local), sec
	  ac1PPS: double;		// 1PPS Signal Accuracy Assessment, nsec
  	TmGPS: longword;	// GPS time since the beginning of the week, sec
	  TmGLN: longword;	// GLN time from the beginning of the day, sec
	  NmWk: word;       // GPS week number
  	RlWk: word;       // GPS Week Counter Overflows
	  Nt: word;	        // The number of days within the four-year GLN
  	N4: word;	        // Fourth Anniversary Number of GLN
	  revb: byte;	      // Reserve
  	KP: byte;	        // A sign of the planned correction of NW GLN (0 - not planned, 1 - correction to &quot;+ 1sec&quot;, 2 - decision not made, 3 - correction to &quot;-1sec&quot;
	  flp: byte;	      // The planned value of the discrepancy between UTC and GPS for an integer number of seconds, sec
  	lps: byte;	      // The current value of the discrepancy between UTC and GPS for an integer number of seconds, sec
  	rev14: longword;	// Reserve
  end;

  // GLONASS KA almanac structure
  TAlmnGLN = record
   	e: double;           	// eccentricity of the orbit (0 ... 0.03)
    ddT: double;         	// dragon change speed. period (+ -2e-8)
    Omg: double;         	// orbit argument (+ -1)
    dT: double;          	// amendment to wednesday meaning fights. period (+ -3,6e + 3)
    tl: double;          	// nearest passage of the first sunrise. node (0 ... 44100)
    Lmd: double;         	// longitude of the first sunrise. node (+ -1)
    di: double;          	// amendment to avg. orbital inclination value (+ -0.067)
    ta: double;          	// gross shift of BSW to SSW (+ -1.9e-3)
    num: word;            // spacecraft number by almanac
    day: word;		        // the day on which the almanac is transmitted
    Cn_mdf: word;         // Sign Cn (15 bit) KA modification (0.1 bit)
    aLt: SmallInt;        // almanac letter number
  end;

  // GPS almanac structure GPS
  TAlmnGPS = record
  	M: double;		   	    // mean anomaly
    e: double;			      // eccentricity
    A: double;		        // semimajor root
    OMEGA_0: double;		  // the longitude of the ascending node at 00h.00min.00sec. base date
    dI0: double;			    // deviation of the orbit
    w: double;		        // perigee argument
    OMEGADOT: double;	    // rate of change of the ascending node of the orbit
    af0: double;		      // correction to the onboard clock
    af1: double;			    // rate of change af0
	  t_oa: integer;			  // time from base date (sec.)
	  num: word;	          // system point number
    stSv: word;			      // satellite status
  end;

  // structure of ionosphere parameters
  TIonParam = record
    a0: single;	// alpha 0
    a1 : single;
    a2: single;
    a3: single;
    b0: single;	// beta 0
    b1: single;
    b2: single;
    b3: single;
  end;

  // GPS time reference settings
  TUTCParam = record
    a0: double;
    a1: double;
    tot: integer;   // Start time for UTC data, sec
    dtls: word;     // The current value of the discrepancy between UTC and GPS for an integer number of seconds, sec
    wnt: word;      // Starting week number UTC
    wnlsf: word;    // The week number from which the planned discrepancy between UTC and GPS is valid for an integer number of seconds
    dn: word;       // The day number from the end of which the planned discrepancy between UTC and GPS is valid for an integer number of seconds
    dtlsf: word;    // The planned value of the discrepancy between UTC and GPS for an integer number of seconds, sec
    wn: word;       // Current week number
  end;

  // GLONASS time reference parameters
  T25GlnParam = record
    tauc: double;
    taugps: double;
	  B1, B2: double;
    N4: word;
    Nt: word;
    KP: word;
    NA: word;
  end;

  // structure of measuring information for 0x10 package
  TMeasOut = record
    tdt: byte;        // data reliability integral characteristic
    lit: Shortint;    // frequency letter number for GLONASS in the additional code
 	  idnum: byte;		  // spacecraft number according to Satellite ID number (NMEA)
	  idChn: byte;		  // tracking channel number
    ampsg: single;    // signal amplitude
	  prng: double;		  // Pseudorange, m
	  pvel: double;	    // Pseudo speed, m / s
    padr: double;   	// Relate pseudorange change
    pphi: double;		  // Carrier phase, glad (????)
    acpr: single;	    // Evaluation of the standard deviation of pseudorange measurements, m
    acvl: single;	    // Evaluation of the standard deviation of pseudo-velocity measurements, m / s
    erpr: single;	    // Pseudorange error estimate, m
    ervl: single;	    // Evaluation of the pseudo-velocity error, m / s
  end;  
  // package structure 0x10
  TPac10 = record
   dtUTC: double;		    // Date and time of measurement information, sec (0 - 00:00 January 1, 2008 UTC)
	 NFs: integer;			  // The number of shreds on the measured interval
   LeapSec: word;		    // The current value of the discrepancy between UTC and GPS for an integer number of seconds, sec
   NMSat: word;         // The number of satellites that transmit measurement information (NSat)
   ClockOffGPS: double; // ShV receiver shift relative to ShV GPS in meters
   Ms: array [0..42] of TMeasOut;  // measurement information structure
  end;

  // structure of measuring information for 0x0F package
  TMeasContOut = record
   tdt: byte;         // data reliability integral characteristic
   lit: Shortint;     // frequency letter number for GLONASS in the additional code
   idnum: byte;		    // spacecraft number according to Satellite ID number (NMEA)
   idChn: byte;		    // tracking channel number
   ampsg: single;     // signal amplitude
   prng: double;		  // Pseudorange, m
   pvel: double;	    // Pseudo speed, m / s
   pphi: double;		  // Carrier phase, glad (????)
   X: double;	        // Satellite X coordinate, m
   Y: double;	        // Satellite Y coordinate, m
   Z: double;	        // Coordinate of satellite Z, m
   vX: double;	      // Satellite speed along the X axis, m / s
   vY: double;	      // Satellite speed along the Y axis, m / s
   vZ: double;	      // Satellite speed along the Z axis, m / s
  end;
  // packet structure 0x0F
  TPac0F = record
   dtUTC: double;		    // Date and time of measurement information, sec (0 - 00:00 January 1, 2008 UTC)
	 NFs: integer;			  // The number of shreds on the measured interval
   LeapSec: word;		    // The current value of the discrepancy between UTC and GPS for an integer number of seconds, sec
   NMSat: word;         // The number of satellites that transmit measurement information (NSat)
   ClockOffGPS: double; // ShV receiver shift relative to ShV GPS in meters
   Ms: array [0..42] of TMeasContOut;  // measurement information structure
  end;

const
  L1freq = 1575.42e6;
  SL = 299792458;
  pi = 3.1415926535897932384626433832795;

var
  FormRNX: TFormRNX;
  formatSettings : TFormatSettings;
  Reg,FReg: TRegistry;
  AppDir: string;                 // launcher directory
  Fnam,Nkml1:string;

  ArDat: array of integer;            // data array inform. the package
  RegTime:double = 39448;             // 01/01/2008
  RegTimeGPS:double = 29226;          // 01/06/1980
  StrFile1: string;
  StrFile: AnsiString;
  F_RinMO,F_RinMN: TextFile;    // Rinex MO Mixed Obs. and Rinex MN-Nav file. All GNSS
  LOG: TextFile;                // program log file
  F1: File of char;             // BIN file
  F2: File of char;
  hnd,siz,kol: integer;         // file size and processed number
  NameFrnx:string;

  indGPS,indGLN: integer;
  FEnter:boolean;  // sign of synchronization
  Hold:boolean;

  LeapSec: integer=0;     // The current value of the discrepancy between UTC and GPS for an integer number of seconds, sec
  ClockOffset,ClockOffset1: double;
  WRollOver: integer=0; // GPS Week Counter Overflows
  StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10,StKA10: string;
  TimeEposh1: time_t;  // epoch time for writing coordinate file ROVER
  pc10dtUTC_OLD: double; // previous time to control the monotony of eras
  StPac10: string;          // package line 0x10
  ArSVsOBS: array [0..42] of string;

  FirstAnalysis: byte=1;

  pr0x14: boolean=false;
  pr0x19: boolean=false;
  pr0x1C: boolean=false;
  pr0x1D: boolean=false;
  pr0x1E: boolean=false;

  pr0x0F: boolean=false;
  pr0x10: boolean=false;
  pr0x13: boolean=false;
  pr0x0A: boolean=false;
  pr0x1A: boolean=false;
  pr0x1B: boolean=false;
  pr0x88: boolean=false;



  G_OBS: boolean = false;
  R_OBS: boolean = false;
  E_OBS: boolean = false;
  J_OBS: boolean = false;

  ArLitNumKA: array [1..24] of integer; // array of correspondence of the spacecraft number and the letter
  EL: array [1..199] of single;         // UM array for all spacecraft in solution
  GinFix: boolean=false; // sign of decision
  verSt:string = '';
  StSN:string = '';

  ArTEphGPS: array [1..32] of TEphGPS;
  ArTEphGLN: array [1..24] of TEphGLN;
  ArTEphGAL: array [1..36] of TEphGPS;

  ArTEphGPS_od: array [1..32] of TEphGPS; // array of received ephemeris without date
  ArTEphGLN_od: array [1..24] of TEphGLN; // array of received ephemeris without date

  ArMesg1C: array[0..7] of string; // 0x1C message data array GPS ionosphere parameters
  ArMesg1D: array[0..3] of string; // GPS time reference 0x1D data array
  ArMesg1E: array[0..2] of string; // GLO time reference 0x1E data array

  FTime: time_t;       // time of the first measurement in the form of a system
  FdtUTC: double=0;         // time of the first measurement from the first message 0x10
  ChDT: boolean=false;      // flag for highlighting the date and time from the navigation message
  RecEph_od: boolean=false; // flag to record the entire array of ephemeris without date
  GGFix: boolean=false;     // sign of joint decision

  coorX,coorY,coorZ: double;
  errCS10,errCS13: boolean;

  ArSVsOBS_GPS: array[0..42] of integer;  // array of measurements for all GPS spacecraft in the entire file; 1- the measurement was; 0-measurements were not
  ArTAlmnGPS: array [1..32] of TAlmnGPS;

  TimeUTC: double; // current decision time

  num3E: word = 0;                    // number of receipts for restart G5 (usually 4pcs)

implementation

{$R *.dfm}

(*----------------------------------------------------------------------------*)
// gdt file creation function
Function FileGdtCreate(FileName: string; Ng: byte; StHead: Ansistring; PrOrd: integer): boolean;
Var Fgdt: file of Ansichar;
    ik: integer;
    VerFDt, Nh, MgWr: word;
    NGr, Resb: byte;
    Ch1, Ch2: Ansichar;
Begin
 AssignFile(Fgdt,FileName);
 Rewrite(Fgdt);
 VerFDt:=$100*5+0;      // SG version 5.0
 Resb:=PrOrd;
 MgWr:=$18B7;
 NGr:=Ng;
 Nh:=8+Length(StHead);
 Ch1:=Ansichar(VerFDt mod 256); Ch2:=Ansichar(VerFDt div 256); Write(Fgdt,Ch1,Ch2);
 Ch1:=Ansichar(Resb); Ch2:=Ansichar(Ngr); Write(Fgdt,Ch1,Ch2);
 Ch1:=Ansichar(Nh mod 256); Ch2:=Ansichar(Nh div 256); Write(Fgdt,Ch1,Ch2);
 for ik:=1 to Nh-8 do Write(Fgdt,StHead[ik]);
 Ch1:=Ansichar(MgWr mod 256); Ch2:=Ansichar(MgWr div 256); Write(Fgdt,Ch1,Ch2);
 Result:=FileSize(Fgdt)=Nh;
 CloseFile(Fgdt);
End;
(*----------------------------------------------------------------------------*)
procedure TFormRNX.ConvComplete;
begin
// Writeln (F_RinMO, Format (&#39;% s% 56s&#39;, [&#39;END OF FILE&#39;, &#39;COMMENT&#39;]));
// OknoTP.Lines.Add (&#39;bytes wr =&#39; + inttostr (kol));
 OknoTP.Lines.Add('Conversion completed!');
 PBarRNX.Visible:=false;
 Button2.Enabled:=true;
 CloseFile(LOG);
 CloseFile(F_RinMO);
 CloseFile(F_RinMN);
 Screen.Cursor:=crDefault;
end;
(*----------------------------------------------------------------------------*)
// verification of spacecraft number as accepted. spacecraft number, letter and visibility
Function CheckNum(pNum: byte): integer;
Var ik: byte;
Begin
 try
 Result := 0;
 for ik := 1 to 24 do if (pNum = ArLitNumKA[ik]) then Result := ik; // get the spacecraft number
 except
  // FormMain.OknoTP.Lines.Add (&#39;ERR letter request =&#39; + inttostr (plit));
 end;
End;
(*---------------------------------------------------------------------------*)

(*----------------------------------------------------------------------------*)
procedure TFormRNX.Button2Click(Sender: TObject);
Var
 ik,dl,DY: integer;
 St: string;
 StGreateFileTime: string;
 StDY: string;
 StYear,StMonth,StDay,StHour,StMinute,StSecond,StSecF: string;
 FGeosBin: File of Char;
 StF: char;
 DbSecMSec: double;
 count:integer;
 Ch: char;
 iSt, lnPc, lnPw: integer;
 iXt: integer;
 ncmd, ndat: word;
 CSpac, dtlw: integer;

 ASD,PathLOGGeoS3,NameFGeos3: string;

 sizRead, NumRead, LengthStrHalfFile: integer;
 StrHalfFile: string;
 loppp: integer;
  I: Integer;
begin
 formatSettings.DecimalSeparator:='.';
 // initialization of variables
 FillChar(ArSVsOBS_GPS,SizeOf(ArSVsOBS_GPS),0);
 FillChar(ArTAlmnGPS,SizeOf(ArTAlmnGPS),0);

 FillChar(ArTEphGPS,SizeOf(ArTEphGPS),0);
 FillChar(ArTEphGLN,SizeOf(ArTEphGLN),0);
 FillChar(ArTEphGAL,SizeOf(ArTEphGAL),0);

 FillChar(ArTEphGPS_od,SizeOf(ArTEphGPS_od),0);
 FillChar(ArTEphGLN_od,SizeOf(ArTEphGLN_od),0);

 for ik := 1 to 24 do ArLitNumKA[ik]:=100;

 for ik := 0 to 7 do ArMesg1C[ik]:='0';
 for ik := 0 to 3 do ArMesg1D[ik]:='0';
 for ik := 0 to 2 do ArMesg1D[ik]:='0';

 pr0x14:=false;
 pr0x19:=false;
 pr0x1C:=false;
 pr0x1D:=false;
 pr0x1E:=false;

 pr0x10:=false;
 pr0x0F:=false;
 pr0x13:=false;
 pr0x0A:=false;
 pr0x1A:=false;
 pr0x1B:=false;
 pr0x88:=false;

 errCS10:=false;
 errCS13:=false;

 pc10dtUTC_OLD:=0;

 LeapSec:=0;
 ClockOffset:=0;
 ClockOffset1:=0;
 WRollOver:=0;

 FEnter:=false;
 GinFix:=false; // sign of decision
 verSt:= '';
 StSN:= '';

 ChDT:=false;
 RecEph_od:= false;
 GGFix:= false;

 OknoTP.Clear;
 FdtUTC:=0;

 if FNEdit.FileName='' then
    begin
    OknoTP.Lines.Add('Input file error!');
    exit;
    end;

 // first scroll of the file to record headers
 FirstAnalysis:=1;
 Button2.Enabled:=false;
 Screen.Cursor:=crHourGlass;

 // File Designation (Activation)
 AssignFile(F1,FNEdit.FileName);
 Reset(F1) ;
 siz:= filesize(F1);
 sizRead:=0;
 StrFile:='';

 PBarRNX.Position:=0; kol:=0;
 PBarRNX.Max:=trunc(siz);
 PBarRNX.Visible:=true;

 // reading data from a file
 WHILE (sizRead<siz) DO
   BEGIN
   // if the required packages are accepted, then go to the second scroll of the file
   if (pr0x10 or pr0x0F) then
       begin
       if (pr0x14 and pr0x1C and pr0x1D and pr0x1E and
           pr0x13 and pr0x1A and pr0x1B{ and pr0x19}) then break;
       end;

   // determine the size of how much to read from the file
   StrHalfFile:='';
   if ((siz-sizRead)>500000000) then
        SetLength(StrHalfFile, 500000000)
   else SetLength(StrHalfFile, (siz-sizRead));

   // we read a block of a certain size from a file
   BlockRead(F1,StrHalfFile[1],Length(StrHalfFile),NumRead);
   // StrFile: = StrFile + StrHalfFile;
   StrFile:=StrHalfFile;
   sizRead:= sizRead+NumRead;

   // line processing
   WHILE Length(StrFile)>0 DO
     BEGIN
     // sync word search
     if FEnter=false then
        begin
        if Length(StrFile) < 20 then break;// not enough data to analyze
        iSt:=0; iSt:= Pos('GEOSr3PS',StrFile);
        if (iSt=0) then break; // sync word not found
        if Length(StrFile)-iSt < 19 then break;// not enough data to analyze
        // sync word found
        FEnter:=true;
        end
     else
        begin
        if Length(StrFile)-iSt < 19 then // not enough data to analyze
           begin
           Delete(StrFile,1,iSt-1);   // delete processed row
           FEnter:=false;
           break; end;
        ncmd := ord(StrFile[iSt+9]) shl 8 + ord(StrFile[iSt+8]);
        ndat := ord(StrFile[iSt+11]) shl 8 + ord(StrFile[iSt+10]);
        lnPc := 4*ndat+16;                        // length of the entire packet in bytes
        lnPw := ndat+4;                           // longword packet length
        if Length(StrFile) < iSt+lnPc-1 then // not enough data to analyze
           begin // ConvComplete;
           Delete(StrFile,1,iSt-1);   // delete processed row
           FEnter:=false;
           break;
           end;
        CSpac := 0;
        SetLength(ArDat,ndat);
        Application.ProcessMessages;
        // CS calculation
        for ik := 0 to lnPw-1 do
            begin
            dtlw := ord(StrFile[iSt+4*ik+3]) shl 24 + ord(StrFile[iSt+4*ik+2]) shl 16 +
                    ord(StrFile[iSt+4*ik+1]) shl 8 + ord(StrFile[iSt+4*ik]);
            CSpac := CSpac xor dtlw;
            if (ik > 2)and(ik < lnPw-1) then ArDat[ik-3] := dtlw;
            end;
        if CSpac <> 0 then          // counter. package amount did not match
           begin
           // Delete (StrFile, 1, iSt + lnPc-1); // delete the processed string
           Delete(StrFile,1,iSt+3);   // deletion of processed data to the start position of the bad line + GEOS
           iXt:=0; iXt:= Pos('GEOSr3PS',StrFile); // looking for geos
           Delete(StrFile,1,iXt-1);   // delete leftover bad string

           FEnter:=false;
           // break;
           end;
        // copying data and working out the team
        case ncmd of
        $0F: if ndat = 20*(ArDat[3] shr 16)+6 then ExCm0F else CSpac := 1;
        $10: if ndat = 14*(ArDat[3] shr 16)+6 then ExCm10 else CSpac := 1;
        $13: if ndat = 32   then ExCm13 else CSpac := 1;
        $14: if ndat = 12   then ExCm14 else CSpac := 1;
        $0A,$9A: if ndat = 32   then ExCm0A else CSpac := 1;
        $18,$88: if ndat = 20   then ExCm88 else CSpac := 1;
        $19,$89: if ndat = 18   then ExCm19 else CSpac := 1;
        $1A,$8A: if ndat = 32   then ExCm1A else CSpac := 1;
        $1B,$8B: if ndat = 30   then ExCm1B else CSpac := 1;
        $1C,$9C: if ndat = 8    then ExCm1C else CSpac := 1;
        $1D,$9D: if ndat = 8    then ExCm1D else CSpac := 1;
        $1E,$9E: if ndat = 10   then ExCm1E else CSpac := 1;
        $20: if ndat = 28   then ExCm20 else CSpac := 1;
        $23: if ndat = 29   then ExCm23 else CSpac := 1;
        $24: if ndat = 34   then ExCm24 else CSpac:= 1;
        $C1: if ndat = 4    then ExCmC1 else CSpac := 1;

        end;
        // if CSpac = 1 then OknoTP.Lines.Add (&#39;Wrong number of package parameters&#39; + IntToHex (ncmd, 2));
        iSt:=iSt+lnPc;
        // OknoTP.Lines.Add (&#39;iSt =&#39; + inttostr (iSt));
        kol:=kol+lnPc; PBarRNX.Position:=kol;
        end;
     Application.ProcessMessages;
     END; // WHILE Length (StrFile)&gt; 0 DO
   Application.ProcessMessages;
   END; // WHILE (sizRead <siz) DO

 // checking the content of important messages in the log file
 if pr0x14=false then OknoTP.Lines.Add('No messages 0x14 (Timing parameters)!');  // Leap seconds
 if pr0x19=false then OknoTP.Lines.Add('No messages 0х19, 0x89 (GLONASS almanac)!');  // for letters
 if pr0x1C=false then OknoTP.Lines.Add('No messages 0x1C, 0x9C (GPS ionospheric parameters)!');
 if pr0x1D=false then OknoTP.Lines.Add('No messages 0x1D, 0x9D (GPS time to UTC conversion parameters)!');
 if pr0x1E=false then OknoTP.Lines.Add('No messages 0x1E, 0x9E (GLONASS time to UTC conversion parameters)!');
                                      // 0x1F, 0x9F: GST-UTC Conversion Parameters

 if ((pr0x10<>true) and (pr0x0F<>true)) then OknoTP.Lines.Add('No messages 0x10 (Raw measurements)!');
 if pr0x13=false then OknoTP.Lines.Add('No messages 0x13 (Navigation solution state vector)!');
 if pr0x0A=false then OknoTP.Lines.Add('No messages 0x0A, 0x9A (GALILEO ephemeris)!');
 if pr0x1A=false then OknoTP.Lines.Add('No messages 0x1A, 0x8A (GPS/QZSS ephemeris)!');
 if pr0x1B=false then OknoTP.Lines.Add('No messages 0x1B, 0x8B (GLONASS ephemeris)!');

 if pr0x88=false then OknoTP.Lines.Add('No messages 0x18, 0x88 (GPS/QZSS almanac)!');  // for filter 2
 // OknoTP.Lines.Add (&#39;GPS almanac is not available. Filter # 2 will be disabled!&#39;);


 // generating a file creation time line
 // day of the year 1-366
 try
  DY:=DayOfTheYear(SystemTimeToDateTime(FTime));
  if (DY<10) then StDY:='00'+inttostr(DY)
  else if (DY<100)  then StDY:='0'+inttostr(DY)
  else if (DY<1000) then StDY:=inttostr(DY);

  StYear:=inttostr(FTime.wYear);
  if FTime.wMonth<10 then StMonth:='0'+inttostr(FTime.wMonth) else StMonth:=inttostr(FTime.wMonth);
  if FTime.wDay<10   then StDay:='0'+inttostr(FTime.wDay) else StDay:=inttostr(FTime.wDay);
  if (FTime.wHour)<10 then StHour:='0'+inttostr(FTime.wHour) else StHour:=inttostr(FTime.wHour);
  if (FTime.wMinute)<10 then StMinute:='0'+inttostr(FTime.wMinute) else StMinute:=inttostr(FTime.wMinute);
  if (FTime.wSecond)<10 then StSecond:='0'+inttostr(FTime.wSecond) else StSecond:=inttostr(FTime.wSecond);
  StGreateFileTime:=StYear+StMonth+StDay+' '+StHour+StMinute+StSecond+' GPS';

  DbSecMSec:= FTime.wSecond+(FTime.wMilliseconds)*0.001;
  if (DbSecMSec)<10 then StSecF:='0'+FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings)
                    else StSecF:= FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings);

  // creating rinex files F_CoorROVER
  AssignFile(F_RinMO,ExtractFilePath(FNEdit.FileName)+'xxxx00RUS_R_'+StYear+StDY+StHour+StMinute+'_00U_xxU_MO'+'.rnx'); Rewrite(F_RinMO);
  AssignFile(F_RinMN,ExtractFilePath(FNEdit.FileName)+'xxxx00RUS_R_'+StYear+StDY+StHour+StMinute+'_00U_xxU_MN'+'.rnx'); Rewrite(F_RinMN);
  // creating a LOG_ERRORS file
  AssignFile(LOG,ExtractFilePath(FNEdit.FileName)+'LOG_ERRORS.txt'); Rewrite(LOG);

 except
  OknoTP.Lines.Add('Cannot create Rinex file!');
  Screen.Cursor:=crDefault;
  PBarRNX.Visible:=false;
  Button2.Enabled:=true;
  exit;
 end;
   // HEADER - GNSS Navigation Message File -
   Writeln(F_RinMN,'     3.02           N: GNSS NAV DATA    M: MIXED            RINEX VERSION / TYPE');
   Writeln(F_RinMN,'GEOS5toRNX          Geostar             '+StGreateFileTime+' PGM / RUN BY / DATE ');
   Writeln(F_RinMN,Format('%4s%13s%12s%12s%12s%23s',['GPSA',ArMesg1C[0],ArMesg1C[1],ArMesg1C[2],ArMesg1C[3],'IONOSPHERIC CORR'],formatSettings));
   Writeln(F_RinMN,Format('%4s%13s%12s%12s%12s%23s',['GPSB',ArMesg1C[4],ArMesg1C[5],ArMesg1C[6],ArMesg1C[7],'IONOSPHERIC CORR'],formatSettings));
   Writeln(F_RinMN,Format('%4s%18s%16s%7s%5s%26s',['GPUT',ArMesg1D[0],ArMesg1D[1],ArMesg1D[2],ArMesg1D[3],'TIME SYSTEM CORR'],formatSettings));
   // Writeln (F_RinMN, Format (&#39;% 4s% 18s% 16s% 7d% 5d% 26s&#39;, [&#39;GLUT&#39;, ArMesg1E [0], ArMesg1E [1], 0,0, &#39;TIME SYSTEM CORR&#39;], formatSettings)) ;
   // Writeln (F_RinMN, Format (&#39;% 4s% 18s% 16s% 7d% 5d% 26s&#39;, [&#39;GLGP&#39;, ArMesg1E [2], ArMesg1E [1], 0,0, &#39;TIME SYSTEM CORR&#39;], formatSettings)) ;
   Writeln(F_RinMN,Format('%6d%66s',[LeapSec,'LEAP SECONDS']));
   Writeln(F_RinMN,Format('%73s',['END OF HEADER']));
   // 


   // HEADER - GNSS Observation Data File
   Writeln(F_RinMO,'     3.02           OBSERVATION DATA    M                   RINEX VERSION / TYPE');
   Writeln(F_RinMO,'GEOS5toRNX          Geostar             '+StGreateFileTime+' PGM / RUN BY / DATE ');
   Writeln(F_RinMO,'Name of antenna marker                                      MARKER NAME');
   Writeln(F_RinMO,'Number of antenna marker                                    MARKER NUMBER');
   Writeln(F_RinMO,'Type of the marker                                          MARKER TYPE');
   Writeln(F_RinMO,Format('%s%20s%50s',['GEOSTAR','Geostar','OBSERVER / AGENCY']));
   if verSt='' then
        Writeln(F_RinMO,Format('%s%22s%20s%32s',['GeoS5','precise','','REC # / TYPE / VERS']))
   else Writeln(F_RinMO,Format('%s%8s%18s%34s',['GeoS5'+StSN,'precise',verSt,'REC # / TYPE / VERS']));
   Writeln(F_RinMO,Format('%14.4f%14.4f%14.4f%37s',[coorX,coorY,coorZ,'APPROX POSITION XYZ'],formatSettings));
   Writeln(F_RinMO,Format('%14.4f%14.4f%14.4f%38s',[0.0,0.0,0.0,'ANTENNA: DELTA H/E/N'],formatSettings));
   if G_OBS then Writeln(F_RinMO,'G    4 C1C D1C L1C S1C                                      SYS / # / OBS TYPES');
   if R_OBS then Writeln(F_RinMO,'R    4 C1C D1C L1C S1C                                      SYS / # / OBS TYPES');
   if E_OBS then Writeln(F_RinMO,'E    4 C1X D1X L1X S1X                                      SYS / # / OBS TYPES');
   if J_OBS then Writeln(F_RinMO,'J    4 C1C D1C L1C S1C                                      SYS / # / OBS TYPES');
   // Writeln (F_RinO, &#39;18.000 INTERVAL&#39;);
   // Writeln (F_RinO, &#39;G APPL_DCB xyz.uvw.abc//pub/dcb_gps.dat SYS / DCBS APPLIED&#39;);
   Writeln(F_RinMO,'DBHZ                                                        SIGNAL STRENGTH UNIT');
   Writeln(F_RinMO,Format('%6s%6s%6s%6s%6s%14s%7s%26s',
   [StYear,StMonth,StDay,StHour,StMinute,StSecF,'GPS','TIME OF FIRST OBS'],formatSettings));
   // Writeln (F_RinO, &#39;2014 03 24 13 10 36.0000000 GPS TIME OF FIRST OBS&#39;);
   // Writeln (F_RinO, &#39;18 R01 1 R02 2 R03 3 R04 4 R05 5 R06 -6 R07 -5 R08 -4 GLONASS SLOT / FRQ #&#39;);
   // Writeln (F_RinO, &#39;R09 -3 R10 -2 R11 -1 R12 0 R13 1 R14 2 R15 3 R16 4 GLONASS SLOT / FRQ #&#39;);
   // Writeln (F_RinO, &#39;R17 5 R18 -5 GLONASS SLOT / FRQ #&#39;);
   // Writeln (F_RinO, &#39;C1C -10.000 C1P -10.123 C2C -10.432 C2P -10.634 GLONASS COD / PHS / BIS&#39;);
   Writeln(F_RinMO,Format('%6d%66s',[LeapSec,'LEAP SECONDS']));
   Writeln(F_RinMO,Format('%73s',['END OF HEADER']));
   // 


 // second file scroll to write data
 FirstAnalysis:=2;
 PBarRNX.Position:=0; kol:=0; FEnter:=false;

 Reset(F1);
 siz:= filesize(F1);
 sizRead:=0;
 StrFile:='';
 TimeUTC:=SystemTimeToDateTime(FTime);

 // reading data from a file
 WHILE (sizRead<siz) DO
   BEGIN
   // determine the size of how much to read from the file
   StrHalfFile:='';
   if ((siz-sizRead)>200000000) then
        SetLength(StrHalfFile, 200000000)
   else SetLength(StrHalfFile, (siz-sizRead));

   // we read a block of a certain size from a file
   BlockRead(F1,StrHalfFile[1],Length(StrHalfFile),NumRead);
   // StrFile: = StrFile + StrHalfFile;
   StrFile:=StrHalfFile;
   sizRead:=sizRead+NumRead;

   WHILE Length(StrFile)>0 DO
     BEGIN
     if FEnter=false then
        begin
        if Length(StrFile) < 20 then break;// not enough data to analyze
        iSt:=0; iSt:= Pos('GEOSr3PS',StrFile);
        if (iSt=0) then break; // sync word not found
        if Length(StrFile)-iSt < 19 then break;// not enough data to analyze
        // sync word found
        FEnter:=true;
        end
     else
        begin
        if Length(StrFile)-iSt < 19 then // not enough data to analyze
           begin
           // Memo1.Lines.Add (&#39;err msg!&#39; + StrFile);
           // Delete (StrFile, 1, iSt-1); // delete the processed string
           FEnter:=false;
           break; end;
        // if iSt&gt; 1 then Memo1.Lines.Add (&#39;Sync Word Position&#39; + IntToStr (iSt));
        ncmd := ord(StrFile[iSt+9]) shl 8 + ord(StrFile[iSt+8]);
        ndat := ord(StrFile[iSt+11]) shl 8 + ord(StrFile[iSt+10]);
        lnPc := 4*ndat+16;                        // length of the entire packet in bytes
        lnPw := ndat+4;                           // longword packet length
        if Length(StrFile) < iSt+lnPc-1 then // not enough data to analyze
           begin
           // Memo1.Lines.Add (&#39;err msg&#39; + inttohex (ncmd, 2) + &#39;&#39; + StrFile);
           // Delete (StrFile, 1, iSt-1); // delete the processed string
           FEnter:=false;
           break;
           end;
        CSpac := 0;
        SetLength(ArDat,ndat);
        Application.ProcessMessages;
        // CS calculation
        for ik := 0 to lnPw-1 do
            begin
            dtlw := ord(StrFile[iSt+4*ik+3]) shl 24 + ord(StrFile[iSt+4*ik+2]) shl 16 +
                    ord(StrFile[iSt+4*ik+1]) shl 8 + ord(StrFile[iSt+4*ik]);
            CSpac := CSpac xor dtlw;
            if (ik > 2)and(ik < lnPw-1) then ArDat[ik-3] := dtlw;
            end;
        if CSpac <> 0 then          // counter. package amount did not match
           begin
           // skip era
           // DateTimeToSystemTime (now, TimeNow);
           StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
           Writeln(LOG,StPac10+': ERROR! CS msg = '+inttohex(ncmd,2)+';');

           // if (ncmd = $ 10) then errCS10: = true;
           // if (ncmd = $ 13) then errCS13: = true;
           // OknoTP.Lines.Add (&#39;err CS msg&#39; + inttohex (ncmd, 2));
           // Delete (StrFile, 1, iSt + lnPc-1); // delete the processed string

           Delete(StrFile,1,iSt+3);   // deletion of processed data to the start position of the bad line + GEOS
           // Delete (StrFile, 1, iSt-1); // delete processed data to the start position of the bad line + GEOS
           iXt:=0; iXt:= Pos('GEOSr3PS',StrFile); // looking for geos
           Delete(StrFile,1,iXt-1);   // delete leftover bad string

           FEnter:=false;
           // break;
           end
        else
           begin
           // copying data and working out the team
           case ncmd of
           $0F: if ndat = 20*(ArDat[3] shr 16)+6 then ExCm0F else CSpac := 1;
           $10: if ndat = 14*(ArDat[3] shr 16)+6 then ExCm10 else CSpac := 1;
           $13: if ndat = 32   then ExCm13 else CSpac := 1;
         // $ 14: if ndat = 12 then ExCm14 else CSpac: = 1;
           $0A,$9A: if ndat = 32   then ExCm0A else CSpac := 1;
           $1A,$8A: if ndat = 32   then ExCm1A else CSpac := 1;
           $1B,$8B: if ndat = 30   then ExCm1B else CSpac := 1;
         // $ 1C: if ndat = 8 then ExCm1C else CSpac: = 1;
         // $ 1D: if ndat = 8 then ExCm1D else CSpac: = 1;
         // $ 1E: if ndat = 10 then ExCm1E else CSpac: = 1;
           $20: if ndat = 28   then ExCm20 else CSpac := 1;
           $22: if ndat = 5*ArDat[0]+1 then ExCm22 else CSpac := 1;
           $24: if ndat = 34   then ExCm24 else CSpac := 1;

           $3E: if ndat = 3    then ExCm3E else CSpac:= 1;
           end;
           // if CSpac = 1 then OknoTP.Lines.Add (&#39;Wrong number of package parameters&#39; + IntToHex (ncmd, 2));
           iSt:=iSt+lnPc;
           // OknoTP.Lines.Add (&#39;iSt =&#39; + inttostr (iSt));
           kol:=kol+lnPc; PBarRNX.Position:=kol;
           end; // if CSpac &lt;&gt; 0
        end;
     Application.ProcessMessages;
     END; // WHILE Length (StrFile)&gt; 0 DO
   Application.ProcessMessages;
   END; // WHILE (sizRead <siz) DO

ConvComplete;
end;
(*----------------------------------------------------------------------------*)
(*----------------------------------------------------------------------------*)
// receiving the package &quot;consumer coordinates&quot;
Procedure TFormRNX.ExCm20;
Var cr20: ^TCoorUser;

TimeEpochUTC : time_t;
Begin
 try
 cr20 := @ArDat[0];
 // a sign of the decision of the NC and a sign of the reliability of the decision and a sign of allocating time and date
 if ( (((cr20^.mdFix shr 19) and 1)=1)and(cr20^.PrValNT=0) ) then // Fix
     begin
     GinFix:= true;
     TimeUTC:=((cr20^.DtTmUTC+LeapSec)/86400 + 39448);
     ChDT:=true;
     end
 else
     begin
     GinFix:= false;
     ChDT:=false;
     end;
 except
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// receiving the consumer coordinates RTK packet
Procedure TFormRNX.ExCm24;
Var pc24:^TPac24;
Begin
 try
 pc24 := @ArDat[0];
 // sign of decision
 if ( pc24.TypeSolve > 0 ) then // Fix
     begin
     GinFix:= true;
     TimeUTC:=((pc24^.DtTmUTC+LeapSec)/86400 + 39448);
     ChDT:=true;
     end
 else
     begin
     GinFix:= false;
     ChDT:=false;
     end;
 except
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// 
Procedure TFormRNX.ExCm23;
Var
 pc23: ^TPac23;
 St:string;
 a1: double;
Begin
 try
 pc23:=@ArDat[0];
// FileSeek (hgups, 0.2);
// a1: = ((pc23 ^ .DtTmUTC + LeapSec) / 86400 + 39448) ;; FileWrite (hgups, a1.8); // number of restarts
// a1: = pc23.TypeSolve; FileWrite (hgups, a1.8);

 except // wrong package
  // OknoTP.Lines.Add (&#39;Msg (23): error&#39;);
  exit;
 end;
End;
(*----------------------------------------------------------------------------*)
// reception of visible spacecraft
Procedure TFormRNX.ExCm22;
Var
    pc22: ^TPac22;
    ik: integer;
    tel, taz: double;
    num:integer;
    StNum:string;
    Syst:byte;
    infix: integer;
Begin
 try
 pc22:=@ArDat[0];
 for ik := 0 to (pc22^.NSat)-1 do
     Begin
     // Syst: = (pc22 ^ .vs [ik] .ss and $ 700) shr 8; // GPS = 1, GLN = 0
     num:= pc22^.vs[ik].idnm;           // receiving spacecraft number
     tel := 180.0*(pc22^.vs[ik].el)/pi; // select mind
     taz := 180.0*(pc22^.vs[ik].az)/pi; // we select AZ

     // infix: = (pc22.vs [ik] .ss and $ 20,000,000); // sign of using the spacecraft in the solution
     If (num>0)and(num<200)and(tel>0)and(taz>0) then
         begin
         EL[num]:= tel;
         end;
     End; // end of cycle for all spacecraft

 except // wrong package
  // OknoTP.Lines.Add (&#39;Msg (22): error&#39;);
  exit;
 end;
End;
(*----------------------------------------------------------------------------*)
// receiving a packet with measuring information 1st file recording
Procedure TFormRNX.ExCm0F;
Var pc0F: ^TPac0F;
    ik, ichn: integer;
    dttm, DbSecMSec: double;
    St: string;
    TimeEposh: time_t;
    StNKA: string;
    kof: double;
    StLLI: string;
    SVsON: integer;
    iz: integer;
    FlagElMask: boolean;

    numm: integer;
    tim: TDateTime;
    StAll,StNumm,StdtUTC,StPR,StAF0,StTn,StX,StY,StZ,StvX,StvY,StvZ: string;
Begin
 errCS10:=false;
 if (pr0x10=true) then exit;

 try
 kof:=1;
 pc0F:= @ArDat[0];
 if (ChBoxClockOFF.Checked = false) then
     begin
     ClockOffset:=pc0F.ClockOffGPS;   // ShV receiver shift relative to ShV GPS in meters
     end
 else ClockOffset:= 0;
 LeapSec:= pc0F.LeapSec;

 if (FirstAnalysis=1) then
    begin
    if pr0x0F=false then
       begin
       if ((ChDT=true)and(pc0F.NMSat>0)) then
          begin
          if ((LeapSec<>0)and(ClockOffset > -400)and(ClockOffset < 400)) then // filter like in package 0x13 during recording era in OBS file
              begin
              // save the time of the first measurement
              // (pc0F.dtUTC-1) since decision flag comes after 0x10 packet
              // getting the first measurement time for the file names MO and MN - GPS time
              dttm:= ((pc0F.dtUTC-1)+LeapSec)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
              DateTimeToSystemTime(dttm,FTime);
              pr0x0F:=true;
              end;
          end;
       end;
    // to form an array of GPS measurements over the entire file in the first pass
    if pr0x0F then
       begin
       for ik := 0 to pc0F.NMSat-1 do
           begin
           if ((pc0F.Ms[ik].idChn in [0..42]) and (pc0F.Ms[ik].idnum in[1..197])) then
              begin
              // we fix from which systems the measurements were
              if (pc0F.Ms[ik].idnum in[1..32])  then G_OBS:= true; // GPS OBS
              if (pc0F.Ms[ik].idnum in[65..88]) then R_OBS:= true; // GLN
              if (pc0F.Ms[ik].idnum in[101..136]) then E_OBS:= true; // Gal
              if (pc0F.Ms[ik].idnum in[193..197]) then J_OBS:= true; // QZSS

              // update the array of correspondence of the spacecraft number and the letter
              if ((pc0F.Ms[ik].idnum>64) and (pc0F.Ms[ik].idnum<89)) then
                   ArLitNumKA[pc0F.Ms[ik].idnum-64]:= pc0F.Ms[ik].lit;
              // 
		          if ((pc0F.Ms[ik].prng > 1)and(pc0F.Ms[ik].prng < 90000000)and
                  (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng > 2)and
                  (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng < 10)and
                  (pc0F.Ms[ik].ampsg > 27)) then
                   Begin
                   // if checked &quot;only SVs used in position fix&quot;
                   if (((RGSV.ItemIndex=1)and((pc0F.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                       begin
                       // there are measurements from this GPS satellite in this file
		                   if (pc0F.Ms[ik].idnum in[1..32]) then ArSVsOBS_GPS[pc0F.Ms[ik].idnum-1]:= 1;
                       end
			             End;
              end;
           end;
       end;
    exit;
    end;

 if (ClockOffset < -400) and (ClockOffset > 400) then  // take with a margin, and should be from -300m to 300m
     begin
     // skip era
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ClockOffGPS = '+floattostrF(ClockOffset,ffFixed,5,1)+';');
     exit;
     end;

 // sign of highlighting date and time and spacecraft&gt; 0 in communication
 if ((ChDT=false)or(pc0F.NMSat=0)) then
     begin
     // skip era
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ChDT=false or pc0F.NMSat = '+inttostr(pc0F.NMSat)+';');
     exit;
     end;

 // GPS time
 dttm := (pc0F.dtUTC+LeapSec)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
 DateTimeToSystemTime(dttm,TimeEposh);

 // to write the coordinate file ROVER
 TimeEposh1:= TimeEposh;

 StYear10:=inttostr(TimeEposh.wYear);
 if TimeEposh.wMonth<10 then StMonth10:='0'+inttostr(TimeEposh.wMonth) else StMonth10:=inttostr(TimeEposh.wMonth);
 if TimeEposh.wDay<10   then StDay10:='0'+inttostr(TimeEposh.wDay) else StDay10:=inttostr(TimeEposh.wDay);
 if (TimeEposh.wHour)<10 then StHour10:='0'+inttostr(TimeEposh.wHour) else StHour10:=inttostr(TimeEposh.wHour);
 if (TimeEposh.wMinute)<10 then StMinute10:='0'+inttostr(TimeEposh.wMinute) else StMinute10:=inttostr(TimeEposh.wMinute);
 DbSecMSec:= TimeEposh.wSecond+(TimeEposh.wMilliseconds)*0.001;
 if (DbSecMSec)<10 then StSecond10:='0'+FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings)
                   else StSecond10:= FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings);

 // verification of the monotony of eras
 if (pc0F.dtUTC <= pc10dtUTC_OLD) then
     begin
     // skip era
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! Повтор эпохи!;');
     exit;
     end;
 pc10dtUTC_OLD:= pc0F.dtUTC;

 // era-specific trap
 if (TimeEposh.wHour = 19) and (TimeEposh.wMinute=51) and (TimeEposh.wSecond =58)  then
     begin
     TimeEposh1:= TimeEposh;
     end;

 SVsON := 0;
 FillChar(ArSVsOBS,sizeof(ArSVsOBS),0);
 // SetLength (ArSVsOBS, 0);
 // SetLength (ArSVsOBS, pc0F.NMSat);
 // Further, the formation of measurements in this era for each spacecraft
 for ik := 0 to pc0F.NMSat-1 do
 // for ik: = 0 to 42 do
     begin
 // ArSVsOBS [ik]: = &#39;&#39;;
     if ((pc0F.Ms[ik].idChn in [0..42]) and (pc0F.Ms[ik].idnum in[1..197])) then
        begin
        // add spacecraft number to point
        // GPS
        if (pc0F.Ms[ik].idnum in[1..32])  then
            begin
            StNKA := 'G'+Format('%.2d',[pc0F.Ms[ik].idnum]);
            kof:= L1freq/SL;
            end;
        // GLN
        if (pc0F.Ms[ik].idnum in[65..88]) then
            begin
            StNKA := 'R'+Format('%.2d',[pc0F.Ms[ik].idnum-64]);
            kof:=(1602e6+(pc0F.Ms[ik].lit*562.5e3))/SL;
            end;
        // Gal
        if (pc0F.Ms[ik].idnum in[101..136]) then
            begin
            StNKA := 'E'+Format('%.2d',[pc0F.Ms[ik].idnum-100]);
            kof:= L1freq/SL;
            end;
        // QZSS
        if (pc0F.Ms[ik].idnum in[193..197]) then
            begin
            StNKA := 'J'+Format('%.2d',[pc0F.Ms[ik].idnum-192]);
            kof:= L1freq/SL;
            end;
        // LLI flag check
        if (((pc0F.Ms[ik].tdt shr 6) and 3)=0) then StLLI:=' ' else StLLI:='1';
        // Filter by UM if mode is selected - only SVs used in position fix
        FlagElMask:=false;
        if ((RGSV.ItemIndex=1))and(EditElMask.Text<>'') then
            begin
            if (EL[pc0F.Ms[ik].idnum] < strtoint(EditElMask.Text)) then
            FlagElMask:=true; // skip this spacecraft
            end;
        // rejection of anomalous measurement values
        if ((pc0F.Ms[ik].prng > 1)and(pc0F.Ms[ik].prng < 90000000)and
            (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng > 2)and
            (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng < 10)and
            (pc0F.Ms[ik].ampsg > 25)and (FlagElMask=false)) then
            Begin
            // if checked &quot;only SVs used in position fix&quot;
            if (((RGSV.ItemIndex=1)and((pc0F.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                begin
                // line forming
                St:= StNKA+
                     Format('%14.3f',[pc0F.Ms[ik].prng],formatSettings)+Format('%2s',[''])+      // pseudorange
                     Format('%14.3f',[pc0F.Ms[ik].pvel*kof],formatSettings)+Format('%2s',[''])+  // pseudo speed
                     Format('%14.3f%s',[pc0F.Ms[ik].pphi,StLLI],formatSettings)+Format('%1s',[''])+  // phase + LLI
                     Format('%14.3f',[pc0F.Ms[ik].ampsg],formatSettings);
                ArSVsOBS[SVsON]:= St;
                SVsON := SVsON + 1;
                end
            End
        else
            Begin
            if ((pc0F.Ms[ik].ampsg > 25)and(StLLI <> '1')) then
                begin
                if (FlagElMask=false) then
                    begin
                    StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
                    Writeln(LOG,StPac10+': ERROR! отбраковка аномальных значений измерений КА '+StNKA);
                    Writeln(LOG,'(1 > pc10.Ms[ik].prng < 90000000) = '+floattostrF(pc0F.Ms[ik].prng,ffFixed,8,3)+';');
                    Writeln(LOG,'(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = '+floattostrF((pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng),ffFixed,8,3)+';');
                    Writeln(LOG,'(pc10.Ms[ik].ampsg > 25) = '+floattostrF(pc0F.Ms[ik].ampsg,ffFixed,8,3)+';');
                    end
                else
                    begin
// StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s% 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10, StMinute10, StSecond10], formatSettings);
// Writeln (LOG, StPac10 + &#39;: ERROR! Sorting by spacecraft elevation&#39; + StNKA);
// Writeln (LOG, &#39;(pc10.Ms [ik] .ampsg&gt; 25) =&#39; + floattostrF (pc0F.Ms [ik] .ampsg, ffFixed, 8.3) + &#39;;&#39;);
                    end;
                end;
            End;
            end; // if ichn in [0..43] then
     end; // for ik: = 0 to pc0F.NMSat-1 do

 StKA10 := inttostr(SVsON);

 // write to file - the beginning of an era
 StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s%3s%3s%21.12f',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10,'0',StKA10,ClockOffset/SL],formatSettings);
 Writeln(F_RinMO,StPac10);
 // write to file - each spacecraft
 for ik := 0 to Length(ArSVsOBS)-1 do if (ArSVsOBS[ik]<>'') then Writeln(F_RinMO, ArSVsOBS[ik]);

 except
  StKA10 := inttostr(SVsON);
  // OknoTP.Lines.Add (&#39;error messages 0x10!&#39;);
  exit;
 end;
End;
// receiving a packet with measuring information 1st file recording
Procedure TFormRNX.ExCm10;
Var pc10: ^TPac10;
    ik, ichn: integer;
    dttm, DbSecMSec: double;
    St: string;
    TimeEposh: time_t;
    StNKA: string;
    kof: double;
    StLLI: string;
    SVsON: integer;
    iz: integer;
    FlagElMask: boolean;
    ArSVsOBS_GPS: array[0..42] of integer;  // array of measurements
Begin
 errCS10:=false;
 // if (errCS13 = true) then exit;

 try
 kof:=1;
 pc10:= @ArDat[0];

 if (ChBoxClockOFF.Checked = false) then
     begin
     ClockOffset:=pc10.ClockOffGPS; // ShV receiver shift relative to ShV GPS in meters
     end
 else ClockOffset:= 0;

 LeapSec:= pc10.LeapSec;

 if (FirstAnalysis=1) then
    begin
    if pr0x10=false then
       begin
       if ((ChDT=true)and(pc10.NMSat>0)) then
          begin
          if ((LeapSec<>0)) then // filter like in package 0x13 during recording era in OBS file
              begin
              // save the time of the first measurement
              // (pc10.dtUTC-1) since decision flag comes after 0x10 packet
              // getting the first measurement time for the file names MO and MN - GPS time
              dttm:= ((pc10.dtUTC-1)+LeapSec)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
              DateTimeToSystemTime(dttm,FTime);
              pr0x10:=true;
              end;
          end;
       end;
    // to form an array of GPS measurements over the entire file in the first pass
    if pr0x10 then
       begin
       for ik := 0 to pc10.NMSat-1 do
           begin
           if ((pc10.Ms[ik].idChn in [0..42]) and (pc10.Ms[ik].idnum in[1..197])) then
              begin
              // we fix from which systems the measurements were
              if (pc10.Ms[ik].idnum in[1..32])  then G_OBS:= true; // GPS OBS
              if (pc10.Ms[ik].idnum in[65..88]) then R_OBS:= true; // GLN
              if (pc10.Ms[ik].idnum in[101..136]) then E_OBS:= true; // Gal
              if (pc10.Ms[ik].idnum in[193..197]) then J_OBS:= true; // QZSS

              // update the array of correspondence of the spacecraft number and the letter
              if ((pc10.Ms[ik].idnum>64) and (pc10.Ms[ik].idnum<89)) then
                   ArLitNumKA[pc10.Ms[ik].idnum-64]:= pc10.Ms[ik].lit;
              // 
		          if ((pc10.Ms[ik].prng > 1)and(pc10.Ms[ik].prng < 90000000)and
                  (pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 2)and
                  (pc10.Ms[ik].pphi/pc10.Ms[ik].prng < 10)and
                  (pc10.Ms[ik].ampsg > 27)) then
                   Begin
                   // if checked &quot;only SVs used in position fix&quot;
                   if (((RGSV.ItemIndex=1)and((pc10.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                       begin
                       // there are measurements from this GPS satellite in this file
		                   if (pc10.Ms[ik].idnum in[1..32]) then ArSVsOBS_GPS[pc10.Ms[ik].idnum-1]:= 1;
                       end
			             End;
              end;
           end;
       end;
    exit;  // if (FirstAnalysis = 1) then
    end;

 if (ClockOffset < -400) and (ClockOffset > 400) then  // take with a margin, and should be from -300m to 300m
     begin
     // ClockOffset range error
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ClockOffGPS = '+floattostrF(ClockOffset,ffFixed,5,1)+';');
     end;

 // sign of highlighting date and time and spacecraft&gt; 0 in communication
 if ((ChDT=false)or(pc10.NMSat=0)) then
     begin
     // skip era
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ChDT=false or pc10.NMSat = '+inttostr(pc10.NMSat)+';');
     exit;
     end;

 // sign of highlighting date and time and spacecraft&gt; 0 in communication
// if ((pc10.dtUTC = 0) or (pc10.NMSat = 0)) then
// begin
// // skip era
// StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s% 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10, StMinute10, StSecond10], formatSettings);
// Writeln (LOG, StPac10 + &#39;: ERROR! ChDT = false or pc10.NMSat =&#39; + inttostr (pc10.NMSat) + &#39;;&#39;);
// exit
// end;

 // GPS time
 dttm := (pc10.dtUTC+LeapSec)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
 DateTimeToSystemTime(dttm,TimeEposh);

 // to write the coordinate file ROVER
 TimeEposh1:= TimeEposh;

 StYear10:=inttostr(TimeEposh.wYear);
 if TimeEposh.wMonth<10 then StMonth10:='0'+inttostr(TimeEposh.wMonth) else StMonth10:=inttostr(TimeEposh.wMonth);
 if TimeEposh.wDay<10   then StDay10:='0'+inttostr(TimeEposh.wDay) else StDay10:=inttostr(TimeEposh.wDay);
 if (TimeEposh.wHour)<10 then StHour10:='0'+inttostr(TimeEposh.wHour) else StHour10:=inttostr(TimeEposh.wHour);
 if (TimeEposh.wMinute)<10 then StMinute10:='0'+inttostr(TimeEposh.wMinute) else StMinute10:=inttostr(TimeEposh.wMinute);
 DbSecMSec:= TimeEposh.wSecond+(TimeEposh.wMilliseconds)*0.001;
 if (DbSecMSec)<10 then StSecond10:='0'+FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings)
                   else StSecond10:= FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings);

 // verification of the monotony of eras
 if (pc10.dtUTC <= pc10dtUTC_OLD) then
     begin
     // skip era
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! Повтор эпохи!;');
     exit;
     end;
 pc10dtUTC_OLD:= pc10.dtUTC;

// // trap of a specific era
// if (TimeEposh.wHour = 5) and (TimeEposh.wMinute = 41) and (TimeEposh.wSecond = 18) then
// begin
// TimeEposh1: = TimeEposh;
// end;

 SVsON := 0;
 FillChar(ArSVsOBS,sizeof(ArSVsOBS),0);
 // SetLength (ArSVsOBS, 0);
 // SetLength (ArSVsOBS, pc10.NMSat);
 // Further, the formation of measurements in this era for each spacecraft
 for ik := 0 to pc10.NMSat-1 do
 // for ik: = 0 to 42 do
     begin
 // ArSVsOBS [ik]: = &#39;&#39;;
     if ((pc10.Ms[ik].idChn in [0..42]) and (pc10.Ms[ik].idnum in[1..197])) then
        begin
        // add spacecraft number to point
        // GPS
        if (pc10.Ms[ik].idnum in[1..32])  then
            begin
            StNKA := 'G'+Format('%.2d',[pc10.Ms[ik].idnum]);
            kof:= L1freq/SL;
            end;
        // GLN
        if (pc10.Ms[ik].idnum in[65..88]) then
            begin
            StNKA := 'R'+Format('%.2d',[pc10.Ms[ik].idnum-64]);
            kof:=(1602e6+(pc10.Ms[ik].lit*562.5e3))/SL;
            end;
        // Gal
        if (pc10.Ms[ik].idnum in[101..136]) then
            begin
            StNKA := 'E'+Format('%.2d',[pc10.Ms[ik].idnum-100]);
            kof:= L1freq/SL;
            end;
        // QZSS
        if (pc10.Ms[ik].idnum in[193..197]) then
            begin
            StNKA := 'J'+Format('%.2d',[pc10.Ms[ik].idnum-192]);
            kof:= L1freq/SL;
            end;
        // LLI flag check
        if (((pc10.Ms[ik].tdt shr 6) and 3)=0) then StLLI:=' ' else StLLI:='1';
        // Filter by UM if mode is selected - only SVs used in position fix
        FlagElMask:=false;
        if ((RGSV.ItemIndex=1))and(EditElMask.Text<>'') then
            begin
            if (EL[pc10.Ms[ik].idnum] < strtoint(EditElMask.Text)) then
            FlagElMask:=true; // skip this spacecraft
            end;
        // rejection of anomalous measurement values
        if ((pc10.Ms[ik].prng > 1)and(pc10.Ms[ik].prng < 90000000)and
            (pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 2)and
            (pc10.Ms[ik].pphi/pc10.Ms[ik].prng < 10)and
            (pc10.Ms[ik].ampsg > 25)and (FlagElMask=false)) then
            Begin
            // if checked &quot;only SVs used in position fix&quot;
            if (((RGSV.ItemIndex=1)and((pc10.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                begin
                // line forming
                St:= StNKA+
                     Format('%14.3f',[pc10.Ms[ik].prng],formatSettings)+Format('%2s',[''])+      // pseudorange
                     Format('%14.3f',[pc10.Ms[ik].pvel*kof],formatSettings)+Format('%2s',[''])+  // pseudo speed
                     Format('%14.3f%s',[pc10.Ms[ik].pphi,StLLI],formatSettings)+Format('%1s',[''])+  // phase + LLI
                     Format('%14.3f',[pc10.Ms[ik].ampsg],formatSettings);
                ArSVsOBS[SVsON]:= St;
                SVsON := SVsON + 1;
                end
            End
        else
            Begin
            if ((pc10.Ms[ik].ampsg > 25)and(StLLI <> '1')) then
                begin
                if (FlagElMask=false) then
                    begin
                    StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
                    Writeln(LOG,StPac10+': ERROR! отбраковка аномальных значений измерений КА '+StNKA);
                    Writeln(LOG,'(1 > pc10.Ms[ik].prng < 90000000) = '+floattostrF(pc10.Ms[ik].prng,ffFixed,8,3)+';');
                    Writeln(LOG,'(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = '+floattostrF((pc10.Ms[ik].pphi/pc10.Ms[ik].prng),ffFixed,8,3)+';');
                    Writeln(LOG,'(pc10.Ms[ik].ampsg > 25) = '+floattostrF(pc10.Ms[ik].ampsg,ffFixed,8,3)+';');
                    end
                else
                    begin
// StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s% 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10, StMinute10, StSecond10], formatSettings);
// Writeln (LOG, StPac10 + &#39;: ERROR! Sorting by spacecraft elevation&#39; + StNKA);
// Writeln (LOG, &#39;(pc10.Ms [ik] .ampsg&gt; 25) =&#39; + floattostrF (pc10.Ms [ik] .ampsg, ffFixed, 8.3) + &#39;;&#39;);
                    end;
                end;
            End;
            end; // if ichn in [0..43] then
     end; // for ik: = 0 to pc10.NMSat-1 do

 StKA10 := inttostr(SVsON);

 // write to file - the beginning of an era
 StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s%3s%3s%21.12f',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10,'0',StKA10,ClockOffset/SL],formatSettings);
 Writeln(F_RinMO,StPac10);
 // write to file - each spacecraft
 for ik := 0 to Length(ArSVsOBS)-1 do if (ArSVsOBS[ik]<>'') then Writeln(F_RinMO, ArSVsOBS[ik]);

 except
  StKA10 := inttostr(SVsON);
  // OknoTP.Lines.Add (&#39;error messages 0x10!&#39;);
  exit;
 end;
End;
(*----------------------------------------------------------------------------*)
// receiving a packet according to the decision of the NC
Procedure TFormRNX.ExCm13;
Var pc13: ^TPac13;
    ik,iSt: integer;
    nKA10: integer;
    dttm: double;
Begin
 errCS13:=false;
 if (errCS10=true) then exit;

 try
 nKA10:=0;
 if (FirstAnalysis=1) and GinFix then
    begin
    pc13 := @ArDat[0];
    if (ChBoxClockOFF.Checked = false) then
        begin
        ClockOffset:=pc13^.dt[3];   // ShV receiver shift relative to ShV GPS in meters
        end
    else ClockOffset:= 0;
    coorX:= pc13^.dt[0];
    coorY:= pc13^.dt[1];
    coorZ:= pc13^.dt[2];
    if ((ClockOffset>-400)and(ClockOffset<400)) then pr0x13:=true;
    exit;
    end;

 except
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// packet reception time parameters
Procedure TFormRNX.ExCm14;
Var pc14: ^TPac14;
rty: integer;
Begin
 try
  pc14 := @ArDat[0];
  if (pr0x14=false) then
      begin
      WRollOver:=pc14^.RlWk;
      pr0x14:=true;
      end;
 except
  // 
 end;

End;
(*----------------------------------------------------------------------------*)
// receiving a packet updating the values of the GLONASS spacecraft almanac
Procedure TFormRNX.ExCm19;
Var aGLN: ^TAlmnGLN;
    ik: integer;
Begin
 try
 aGLN := @ArDat[0];
 if (pr0x19=false) then
     begin
     // fill in the array of correspondence of the spacecraft number and the letter
     if ((aGLN^.num>0) and (aGLN^.num<25)) then ArLitNumKA[aGLN^.num]:= aGLN^.aLt;
     // checking the filling of the entire almanac
     pr0x19:=true;
     for ik := 1 to 24 do if (ArLitNumKA[ik]=100) then pr0x19:=false;
     end;
 except
  // 
 end;

End;
(*----------------------------------------------------------------------------*)
// receiving a packet updating the values of the ephemeris KA GALILEO record 2nd file
Procedure TFormRNX.ExCm0A;
Var eGAL: ^TEphGPS;
    ik: byte;
    dttm: double;
    TimeEposh: time_t;
    TimeReal: time_t;
    StYear,StMonth,StDay,StHour,StMinute,StSecond: string;
    StNKA: string;
    Staf0,Staf1,Staf2: string;
    StIODE,StC_rs,Stdn,StM: string;
    StC_uc,Ste,StC_us,StA: string;
    Stt_oe,StC_ic,StOMEGA_0,StC_is: string;
    StI0,StC_rc,Stw,StOMEGADOT: string;
    StIDOT,StCodesL2,StweekN,StL2PdataF: string;
    StSVaccur,StSVhelth,StTgd,StIODC: string;
    Stdtrc,StFitinterval: string;
    fil1: boolean;
    Eph_Alm_E: double;

Begin
 try
 if (FirstAnalysis=1) then
    begin
    pr0x0A:=true;
    exit;
    end;

 eGAL := @ArDat[0];

 // Filter 0
 // filtering ephemeris with zero data values
 if ((eGAL.n=0)or(eGAL.I0=0)or(eGAL.SVhelth <> 0)) then
    begin
    // skip era
    Writeln(LOG,'EPH GAL ERROR! фильтрация GAL эфемерид с нулевыми значениями данных, eGAL.n = '+inttostr(eGAL.n)+'; eGAL.I0 = '+floattostr(eGAL.I0)+'; eGAL.SVhelth = '+inttostr(eGAL.SVhelth)+';');
    exit;
    end;

 // Filter 1
 // if in the OBS file there are no measurements at all from GPS PRN = N
 // (meaning measurements in the &quot;only SVs used in position fix&quot; mode),
 // then all ephemeris sets with PRN = N can be considered &quot;fake&quot; and ignored.
 if (RGSV.ItemIndex=1) then
     begin
// if ((ArSVsOBS_GPS [eGPS.n-1]) &lt;&gt; 1) then
// begin
// // skip era
// Writeln (LOG, &#39;EPH GPS ERROR! OBS file generally no measurements from eGPS.n =&#39; + inttostr (eGPS.n) + &#39;;&#39;);
// exit
// end;
     end;

 // Filter 2
 // Compare afo and eccentricity from the “tested” Geos GAL ephemeris (PRN = N)
 // with the corresponding parameters from the almanac for GAL PRN = N.
 if pr0x88 then // GAL almanac request required
    begin
// if (eGPS.n = ArTAlmnGPS [eGPS.n] .num) then // if the received ephemeris of this spacecraft is in the almanac array
// begin // then we carry out data verification
// Eph_Alm_E: = abs (ArTAlmnGPS [eGPS.n] .e - eGPS.e);
// 
// if ((abs (ArTAlmnGPS [eGPS.n] .e - eGPS.e)&gt; 0.001) or
// (abs (ArTAlmnGPS [eGPS.n] .af0 - eGPS.af0)&gt; 0.0001)) then
// begin
// // skip era
// Writeln (LOG, &#39;EPH GPS ERROR! Compare afo and eccentricity ephemeris and almanac, difference Eph_Alm_E =&#39; + floattostr (Eph_Alm_E) + &#39;;&#39;);
// exit
// end;
// end;
    end;

 // Filter 3
 // verification of received ephemeris with saved
 if ((eGAL.t_oe = ArTEphGAL[eGAL.n].t_oe) and (eGAL.t_oc = ArTEphGAL[eGAL.n].t_oc) and
     (eGAL.IODE = ArTEphGAL[eGAL.n].IODE) and (eGAL.A = ArTEphGAL[eGAL.n].A)) then
     begin
     // skip era
     Writeln(LOG,'EPH GAL ERROR! повтор эфемерид GAL от КА№' +inttostr(eGAL.n)+';');
     exit;
     end;

// IODC and IODE match check
// if (eGAL.IODC &lt;&gt; eGAL.IODE) then
// begin
// //OknoTP.Lines.Add(&#39;IODC and IODE GPS from KA№ &#39;+ inttostr (eGPS.n) are not equal;
// Writeln (LOG, &#39;EPH GAL ERROR! Unequal IODC and IODE GAL from КА№&#39; + inttostr (eGAL.n) + &#39;;&#39;);
// exit
// end;
 // =================================================== ====================================

  // further conversion to a navigation file
  if eGAL.n<10 then StNKA:='E0'+inttostr(eGAL.n) else StNKA:='E'+inttostr(eGAL.n);
  // GPS time
  // dttm: = (eGAL ^ .t_oe + (604800 * (eGAL ^ .weekN + 1024 * WRollOver))) / 86400 + RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;);
  // WRollOver in GALILEO will be equal to 1 since eGAL ^ .weekN may be 4095 (due to 12bit dimension)
  dttm := (eGAL^.t_oe+(604800*(eGAL^.weekN+1024*1)))/86400 + RegTimeGPS; // StrToDateTime (&#39;06 .01.1980 &#39;);

 // Filter 4
 // if dttm is more than 2 hours from local time, then the actual ephemeris is no longer
 if (abs(TimeUTC - dttm) > 0.1) then
     begin
     Writeln(LOG,StNKA+' error GAL dttm; old ephemeris!');
     exit;
     end;

 // preservation of more recent ephemeris in the table
 ArTEphGAL[eGAL.n].n:= eGAL.n;
 ArTEphGAL[eGAL.n].t_oe:= eGAL.t_oe;
 ArTEphGAL[eGAL.n].t_oc:= eGAL.t_oc;
 ArTEphGAL[eGAL.n].IODE:= eGAL.IODE;
 ArTEphGAL[eGAL.n].IODC:= eGAL.IODC;
 ArTEphGAL[eGAL.n].af0:= eGAL.af0;
 ArTEphGAL[eGAL.n].A:= eGAL.A;

// // cure for incorrect eGPS ^ .weekN
// if (abs (TimeUTC - dttm)&gt; (1.0 / 8.0)) then // 0.125
// begin
// dttm: = ((604800 * (eGAL ^ .weekN + 1 + 1024 * WRollOver))) / 86400 + RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;);
// Writeln (LOG, StNKA + &#39;error eGPS ^ .weekN!&#39;);
// end;
  DateTimeToSystemTime(dttm,TimeEposh);

  StYear:=inttostr(TimeEposh.wYear);
  if TimeEposh.wMonth<10 then StMonth:='0'+inttostr(TimeEposh.wMonth) else StMonth:=inttostr(TimeEposh.wMonth);
  if TimeEposh.wDay<10   then StDay:='0'+inttostr(TimeEposh.wDay) else StDay:=inttostr(TimeEposh.wDay);
  if (TimeEposh.wHour)<10 then StHour:='0'+inttostr(TimeEposh.wHour) else StHour:=inttostr(TimeEposh.wHour);
  if (TimeEposh.wMinute)<10 then StMinute:='0'+inttostr(TimeEposh.wMinute) else StMinute:=inttostr(TimeEposh.wMinute);
  if (TimeEposh.wSecond)<10 then StSecond:='0'+inttostr(TimeEposh.wSecond) else StSecond:=inttostr(TimeEposh.wSecond);

 // =================================================== ====================================

 // SV / EPOCH / SV CLK
 Staf0:= FormatFloat('0.000000000000E+00', eGAL^.af0,formatSettings);
 Staf1:= FormatFloat('0.000000000000E+00', eGAL^.af1,formatSettings);
 Staf2:= FormatFloat('0.000000000000E+00', eGAL^.af2,formatSettings);
 Writeln(F_RinMN,Format('%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s',
       [StNKA,StYear,StMonth,StDay,StHour,StMinute,StSecond,Staf0,Staf1,Staf2]));
 // BROADCAST ORBIT - 1
 StIODE:= FormatFloat('0.000000000000E+00', strtofloat(inttostr(eGAL^.IODE)),formatSettings);
 StC_rs:= FormatFloat('0.000000000000E+00', eGAL^.C_rs,formatSettings);
 Stdn:= FormatFloat('0.000000000000E+00', eGAL^.dn*pi,formatSettings);
 StM:= FormatFloat('0.000000000000E+00', eGAL^.M*pi,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StIODE,StC_rs,Stdn,StM]));
 // BROADCAST ORBIT - 2
 StC_uc:= FormatFloat('0.000000000000E+00', eGAL^.C_uc,formatSettings);
 Ste:= FormatFloat('0.000000000000E+00', eGAL^.e,formatSettings);
 StC_us:= FormatFloat('0.000000000000E+00', eGAL^.C_us,formatSettings);
 StA:= FormatFloat('0.000000000000E+00', eGAL^.A,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StC_uc,Ste,StC_us,StA]));
 // BROADCAST ORBIT - 3
 Stt_oe:= FormatFloat('0.000000000000E+00', eGAL^.t_oe,formatSettings);
 StC_ic:= FormatFloat('0.000000000000E+00', eGAL^.C_ic,formatSettings);
 StOMEGA_0:= FormatFloat('0.000000000000E+00', eGAL^.OMEGA_0*pi,formatSettings);
 StC_is:= FormatFloat('0.000000000000E+00', eGAL^.C_is,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',Stt_oe,StC_ic,StOMEGA_0,StC_is]));
 // BROADCAST ORBIT - 4
 StI0:= FormatFloat('0.000000000000E+00', eGAL^.I0*pi,formatSettings);
 StC_rc:= FormatFloat('0.000000000000E+00', eGAL^.C_rc,formatSettings);
 Stw:= FormatFloat('0.000000000000E+00', eGAL^.w*pi,formatSettings);
 StOMEGADOT:= FormatFloat('0.000000000000E+00', eGAL^.OMEGADOT*pi,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StI0,StC_rc,Stw,StOMEGADOT]));
 // BROADCAST ORBIT - 5
 StIDOT:= FormatFloat('0.000000000000E+00', eGAL^.IDOT*pi,formatSettings);
 StCodesL2:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 StweekN:= FormatFloat('0.000000000000E+00', eGAL^.weekN+1024+(0*4096),formatSettings);
 StL2PdataF:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StIDOT,StCodesL2,StweekN,StL2PdataF]));
 // BROADCAST ORBIT - 6
 StSVaccur:= FormatFloat('0.000000000000E+00', eGAL^.SVaccur,formatSettings);
 StSVhelth:= FormatFloat('0.000000000000E+00', eGAL^.SVhelth,formatSettings);
 StTgd:= FormatFloat('0.000000000000E+00', eGAL^.Tgd,formatSettings);
 StIODC:= FormatFloat('0.000000000000E+00', eGAL^.IODC,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StSVaccur,StSVhelth,StTgd,StIODC]));
 // BROADCAST ORBIT - 7
 // Stdtrc: = FormatFloat (&#39;0.000000000000E + 00&#39;, eGPS ^ .dtrc, formatSettings);
 Stdtrc:= FormatFloat('0.000000000000E+00', eGAL^.t_oe,formatSettings);  // return this entry in the standard version
 StFitinterval:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s',['    ',Stdtrc,StFitinterval]));


 except
  // OknoTP.Lines.Add (&#39;error messages 0x1A!&#39;);
 end;

 end;
(*----------------------------------------------------------------------------*)
// receiving a packet updating the ephemeris values of the GPS satellite record of the 2nd file
Procedure TFormRNX.ExCm1A;
Var eGPS: ^TEphGPS;
    ik: byte;
    dttm: double;
    TimeEpoch: time_t;
    TimeReal: time_t;
    StYear,StMonth,StDay,StHour,StMinute,StSecond: string;
    StNKA: string;
    Staf0,Staf1,Staf2: string;
    StIODE,StC_rs,Stdn,StM: string;
    StC_uc,Ste,StC_us,StA: string;
    Stt_oe,StC_ic,StOMEGA_0,StC_is: string;
    StI0,StC_rc,Stw,StOMEGADOT: string;
    StIDOT,StCodesL2,StweekN,StL2PdataF: string;
    StSVaccur,StSVhelth,StTgd,StIODC: string;
    Stdtrc,StFitinterval: string;
    fil1: boolean;
    Eph_Alm_E: double;

Begin
 try
 eGPS := @ArDat[0];

 if (FirstAnalysis=1) then
    begin
    pr0x1A:=true;
    exit;
    end;

 // Filter 0
 // filtering ephemeris with zero data values
 if ((eGPS.n=0)or(eGPS.I0=0)or(eGPS.SVhelth <> 0)) then
    begin
    // skip era
    Writeln(LOG,'EPH GPS ERROR! фильтрация GPS эфемерид с нулевыми значениями данных, eGPS.n = '+inttostr(eGPS.n)+'; eGPS.I0 = '+floattostr(eGPS.I0)+'; eGPS.SVhelth = '+inttostr(eGPS.SVhelth)+';');
    exit;
    end;

 // Filter 1
 // if in the OBS file there are no measurements at all from GPS PRN = N
 // (meaning measurements in the &quot;only SVs used in position fix&quot; mode),
 // then all ephemeris sets with PRN = N can be considered &quot;fake&quot; and ignored.
 if (RGSV.ItemIndex=1) then
     begin
// if ((ArSVsOBS_GPS [eGPS.n-1]) &lt;&gt; 1) then
// begin
// // skip era
// Writeln (LOG, &#39;EPH GPS ERROR! OBS file generally no measurements from eGPS.n =&#39; + inttostr (eGPS.n) + &#39;;&#39;);
// exit
// end;
     end;

 // Filter 2
 // Compare afo and eccentricity from the &quot;tested&quot; geosovo GPS ephemeris (PRN = N)
 // with the corresponding parameters from the almanac for GPS PRN = N.
 if pr0x88 then // GPS almanac request required
    begin
    if (eGPS.n = ArTAlmnGPS[eGPS.n].num) then // if the received ephemeris of this spacecraft is in the almanac array
        begin                                 // then we carry out data verification
        Eph_Alm_E:= abs(ArTAlmnGPS[eGPS.n].e - eGPS.e);

        if ((abs(ArTAlmnGPS[eGPS.n].e - eGPS.e) > 0.001) or
            (abs(ArTAlmnGPS[eGPS.n].af0 - eGPS.af0) > 0.0001))then
             begin
             // skip era
             Writeln(LOG,'EPH GPS ERROR! Сравниваем afo и eccentricity эфемериды и альманаха, разница Eph_Alm_E = '+floattostr(Eph_Alm_E)+';');
             exit;
             end;
        end;
    end;

 // Filter 3
 // verification of received ephemeris with saved
 if ((eGPS.t_oe = ArTEphGPS[eGPS.n].t_oe) and (eGPS.t_oc = ArTEphGPS[eGPS.n].t_oc) and
     (eGPS.IODE = ArTEphGPS[eGPS.n].IODE) and (eGPS.A = ArTEphGPS[eGPS.n].A)) then
     begin
     // skip era
     Writeln(LOG,'EPH GPS ERROR! повтор эфемерид GPS от КА№' +inttostr(eGPS.n)+';');
     exit;
     end;
 // IODC and IODE match check
 if (eGPS.IODC<>eGPS.IODE) then
     begin
     // OknoTP.Lines.Add (&#39;unequal IODC and IODE GPS from КА№&#39; + inttostr (eGPS.n));
     Writeln(LOG,'EPH GPS ERROR! неравны IODC и IODE GPS от КА№' +inttostr(eGPS.n)+';');
     exit;
     end;
 // =================================================== ====================================

  // further conversion to a navigation file
  if eGPS.n<10 then StNKA:='G0'+inttostr(eGPS.n) else StNKA:='G'+inttostr(eGPS.n);
  // GPS time
  dttm := (eGPS^.t_oe+(604800*(eGPS^.weekN+1024*WRollOver)))/86400 + RegTimeGPS; // StrToDateTime (&#39;06 .01.1980 &#39;);

  // Filter 4
  // if dttm is more than 3 hours from local time, then the actual ephemeris is no longer
  if (abs(TimeUTC - dttm) > 0.1) then // 0.125
      begin
      Writeln(LOG,StNKA+' error GPS dttm; old ephemeris!');
      exit;
      end;

  // preservation of more recent ephemeris in the table
  ArTEphGPS[eGPS.n].n:= eGPS.n;
  ArTEphGPS[eGPS.n].t_oe:= eGPS.t_oe;
  ArTEphGPS[eGPS.n].t_oc:= eGPS.t_oc;
  ArTEphGPS[eGPS.n].IODE:= eGPS.IODE;
  ArTEphGPS[eGPS.n].IODC:= eGPS.IODC;
  ArTEphGPS[eGPS.n].af0:= eGPS.af0;
  ArTEphGPS[eGPS.n].A:= eGPS.A;
  // ArTEphGPS [eGPS.n] .dtrc: = eGPS.dtrc;

  // cure for incorrect eGPS ^ .weekN
// if (abs (TimeUTC - dttm)&gt; (1.0 / 8.0)) then // 0.125
// begin
// dttm: = ((604800 * (eGPS ^ .weekN + 1 + 1024 * WRollOver))) / 86400 + RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;);
// Writeln (LOG, StNKA + &#39;error eGPS ^ .weekN!&#39;);
// // exit;
// end;
  DateTimeToSystemTime(dttm,TimeEpoch);

  StYear:=inttostr(TimeEpoch.wYear);
  if TimeEpoch.wMonth<10 then StMonth:='0'+inttostr(TimeEpoch.wMonth) else StMonth:=inttostr(TimeEpoch.wMonth);
  if TimeEpoch.wDay<10   then StDay:='0'+inttostr(TimeEpoch.wDay) else StDay:=inttostr(TimeEpoch.wDay);
  if (TimeEpoch.wHour)<10 then StHour:='0'+inttostr(TimeEpoch.wHour) else StHour:=inttostr(TimeEpoch.wHour);
  if (TimeEpoch.wMinute)<10 then StMinute:='0'+inttostr(TimeEpoch.wMinute) else StMinute:=inttostr(TimeEpoch.wMinute);
  if (TimeEpoch.wSecond)<10 then StSecond:='0'+inttostr(TimeEpoch.wSecond) else StSecond:=inttostr(TimeEpoch.wSecond);

 // =================================================== ====================================

 // SV / EPOCH / SV CLK
 Staf0:= FormatFloat('0.000000000000E+00', eGPS^.af0,formatSettings);
 Staf1:= FormatFloat('0.000000000000E+00', eGPS^.af1,formatSettings);
 Staf2:= FormatFloat('0.000000000000E+00', eGPS^.af2,formatSettings);
 Writeln(F_RinMN,Format('%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s',
       [StNKA,StYear,StMonth,StDay,StHour,StMinute,StSecond,Staf0,Staf1,Staf2]));
 // BROADCAST ORBIT - 1
 StIODE:= FormatFloat('0.000000000000E+00', strtofloat(inttostr(eGPS^.IODE)),formatSettings);
 StC_rs:= FormatFloat('0.000000000000E+00', eGPS^.C_rs,formatSettings);
 Stdn:= FormatFloat('0.000000000000E+00', eGPS^.dn*pi,formatSettings);
 StM:= FormatFloat('0.000000000000E+00', eGPS^.M*pi,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StIODE,StC_rs,Stdn,StM]));
 // BROADCAST ORBIT - 2
 StC_uc:= FormatFloat('0.000000000000E+00', eGPS^.C_uc,formatSettings);
 Ste:= FormatFloat('0.000000000000E+00', eGPS^.e,formatSettings);
 StC_us:= FormatFloat('0.000000000000E+00', eGPS^.C_us,formatSettings);
 StA:= FormatFloat('0.000000000000E+00', eGPS^.A,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StC_uc,Ste,StC_us,StA]));
 // BROADCAST ORBIT - 3
 Stt_oe:= FormatFloat('0.000000000000E+00', eGPS^.t_oe,formatSettings);
 StC_ic:= FormatFloat('0.000000000000E+00', eGPS^.C_ic,formatSettings);
 StOMEGA_0:= FormatFloat('0.000000000000E+00', eGPS^.OMEGA_0*pi,formatSettings);
 StC_is:= FormatFloat('0.000000000000E+00', eGPS^.C_is,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',Stt_oe,StC_ic,StOMEGA_0,StC_is]));
 // BROADCAST ORBIT - 4
 StI0:= FormatFloat('0.000000000000E+00', eGPS^.I0*pi,formatSettings);
 StC_rc:= FormatFloat('0.000000000000E+00', eGPS^.C_rc,formatSettings);
 Stw:= FormatFloat('0.000000000000E+00', eGPS^.w*pi,formatSettings);
 StOMEGADOT:= FormatFloat('0.000000000000E+00', eGPS^.OMEGADOT*pi,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StI0,StC_rc,Stw,StOMEGADOT]));
 // BROADCAST ORBIT - 5
 StIDOT:= FormatFloat('0.000000000000E+00', eGPS^.IDOT*pi,formatSettings);
 StCodesL2:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 StweekN:= FormatFloat('0.000000000000E+00', eGPS^.weekN+1024*WRollOver,formatSettings); // + 1024 * wRollOver!
 StL2PdataF:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StIDOT,StCodesL2,StweekN,StL2PdataF]));
 // BROADCAST ORBIT - 6
 StSVaccur:= FormatFloat('0.000000000000E+00', eGPS^.SVaccur,formatSettings);
 StSVhelth:= FormatFloat('0.000000000000E+00', eGPS^.SVhelth,formatSettings);
 StTgd:= FormatFloat('0.000000000000E+00', eGPS^.Tgd,formatSettings);
 StIODC:= FormatFloat('0.000000000000E+00', eGPS^.IODC,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StSVaccur,StSVhelth,StTgd,StIODC]));
 // BROADCAST ORBIT - 7
 // Stdtrc: = FormatFloat (&#39;0.000000000000E + 00&#39;, eGPS ^ .dtrc, formatSettings);
 Stdtrc:= FormatFloat('0.000000000000E+00', eGPS^.t_oe,formatSettings);  // return this entry in the standard version
 StFitinterval:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s',['    ',Stdtrc,StFitinterval]));


 except
  // OknoTP.Lines.Add (&#39;error messages 0x1A!&#39;);
 end;

 end;
(*----------------------------------------------------------------------------*)
// receiving a packet; updating GLONASS ephemeris values; recording a 2nd file
Procedure TFormRNX.ExCm1B;
Var ik: integer;
    eGLN: ^TEphGLN;
    dttm: double;
    TimeEposh: time_t;
    StYear,StMonth,StDay,StHour,StMinute,StSecond: string;
    StNKA: string;
    Sttau,Stgm,StMesFTime: string;
    StpX,StvX,StaX,StFt: string;
    StpY,StvY,StaY,StLit: string;
    StpZ,StvZ,StaZ,StenE: string;
    Date1: double;
Begin
 try
 if (FirstAnalysis=1) then
    begin
    pr0x1B:=true;
    exit;
    end;

 eGLN := @ArDat[0];

 // Filter 1
 // if the time of reception of the ephemeris PRN = N is equal to 2008, then they can be considered &quot;fake&quot; and ignored.
 // Ephemeris Time (UTC)
 dttm := (eGLN^.dtrc)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
 DateTimeToSystemTime(dttm,TimeEposh); // translate time from TDateTime to time_t
 if (TimeEposh.wYear=2008) then
     begin
     Writeln(LOG,'EPH GLN ERROR! ошибка выделения времени TimeEposh.wYear = ' +inttostr(TimeEposh.wYear)+';');
     exit;
     end;

 // Filter 2
 // if dttm is more than half an hour from local time, then the actual ephemeris is no longer
 if (abs(TimeUTC - dttm) > 0.02) then
     begin
     Writeln(LOG,'R'+inttostr(eGLN.n)+' error GLN dttm; old ephemeris!');
     exit;
     end;

 if (eGLN.n=0)or(eGLN.prNNK>0) then
     begin
     Writeln(LOG,'EPH GLN ERROR! фильтрация GLN эфемерид с нулевыми значениями данных, eGLN.n = '+inttostr(eGLN.n)+'; eGLN.prNNK = '+inttostr(eGLN.prNNK)+';');
     exit;
     end;

 // verification of received ephemeris with saved
 if ((eGLN.t_b = ArTEphGLN[eGLN.n].t_b) and (eGLN.pX = ArTEphGLN[eGLN.n].pX) and
     (eGLN.pY = ArTEphGLN[eGLN.n].pY) and (eGLN.pZ = ArTEphGLN[eGLN.n].pZ)) then
     begin
     // skip era
     Writeln(LOG,'EPH GLN ERROR! повтор эфемерид GLN от КА№' +inttostr(eGLN.n)+';');
     exit;
     end;

 if (RecEph_od=false) then
 Begin
 if (ChDT=false) then
    begin
    // saving ephemeris WITHOUT DATE in an array
    ArTEphGLN_od[eGLN.n].dtrc:= eGLN.dtrc;
    ArTEphGLN_od[eGLN.n].N4t:= eGLN.N4t;
    ArTEphGLN_od[eGLN.n].NA:= eGLN.NA;
    ArTEphGLN_od[eGLN.n].pX:= eGLN.pX;
    ArTEphGLN_od[eGLN.n].pY:= eGLN.pY;
    ArTEphGLN_od[eGLN.n].pZ:= eGLN.pZ;
    ArTEphGLN_od[eGLN.n].vX:= eGLN.vX;
    ArTEphGLN_od[eGLN.n].vY:= eGLN.vY;
    ArTEphGLN_od[eGLN.n].vY:= eGLN.vZ;
    ArTEphGLN_od[eGLN.n].aX:= eGLN.aX;
    ArTEphGLN_od[eGLN.n].aY:= eGLN.aZ;
    ArTEphGLN_od[eGLN.n].aZ:= eGLN.aZ;
    ArTEphGLN_od[eGLN.n].gm:= eGLN.gm;
    ArTEphGLN_od[eGLN.n].tau:= eGLN.tau;
    ArTEphGLN_od[eGLN.n].tauc:= eGLN.tauc;
    ArTEphGLN_od[eGLN.n].taugps:= eGLN.taugps;
    ArTEphGLN_od[eGLN.n].prNNK:= eGLN.prNNK;
    ArTEphGLN_od[eGLN.n].pss:= eGLN.pss;
    ArTEphGLN_od[eGLN.n].t_b:= eGLN.t_b;
    ArTEphGLN_od[eGLN.n].enE:= eGLN.enE;
    ArTEphGLN_od[eGLN.n].Ft:= eGLN.Ft;
    ArTEphGLN_od[eGLN.n].n:= eGLN.n;
    exit;
    end
 else
    begin
    // there was a sign of time allocation
    // then you need to rewrite all the entries in the array into a navigation file

    // Ephemeris Time (UTC)
    dttm := (eGLN^.dtrc)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
    DateTimeToSystemTime(dttm,TimeEposh); // translate time from TDateTime to time_t
    // nullify wHour, wMinute, wSecond, wMSecond
    TimeEposh.wHour:=0; TimeEposh.wMinute:=0; TimeEposh.wSecond:=0; TimeEposh.wMilliseconds:=0;
    // we only get the date; translate the date from time_t to TDateTime
    Date1:=SystemTimeToDateTime(TimeEposh);

    for ik := 1 to 24 do
        begin
        if (ArTEphGLN_od[ik].n<>0) then
            begin
            // preservation of more recent ephemeris in the table
            ArTEphGLN[ik].t_b:= ArTEphGLN_od[ik].t_b;
            ArTEphGLN[ik].pX:= ArTEphGLN_od[ik].pX;
            ArTEphGLN[ik].pY:= ArTEphGLN_od[ik].pY;
            ArTEphGLN[ik].pZ:= ArTEphGLN_od[ik].pZ;

            // further conversion to a navigation file
            if ArTEphGLN_od[ik].n<10 then StNKA:='R0'+inttostr(ArTEphGLN_od[ik].n) else StNKA:='R'+inttostr(ArTEphGLN_od[ik].n);

            // Epoch: Toc - Time of Clock (UTC)
            dttm := ((ArTEphGLN_od[ik].t_b*900 + 21*60*60) mod 86400)/86400 + Date1;
            DateTimeToSystemTime(dttm,TimeEposh);

            StYear:=inttostr(TimeEposh.wYear);
            if TimeEposh.wMonth<10 then StMonth:='0'+inttostr(TimeEposh.wMonth) else StMonth:=inttostr(TimeEposh.wMonth);
            if TimeEposh.wDay<10   then StDay:='0'+inttostr(TimeEposh.wDay) else StDay:=inttostr(TimeEposh.wDay);
            if (TimeEposh.wHour)<10 then StHour:='0'+inttostr(TimeEposh.wHour) else StHour:=inttostr(TimeEposh.wHour);
            if (TimeEposh.wMinute)<10 then StMinute:='0'+inttostr(TimeEposh.wMinute) else StMinute:=inttostr(TimeEposh.wMinute);
            if (TimeEposh.wSecond)<10 then StSecond:='0'+inttostr(TimeEposh.wSecond) else StSecond:=inttostr(TimeEposh.wSecond);

            // SV / EPOCH / SV CLK
            Sttau:= FormatFloat('0.000000000000E+00', -ArTEphGLN_od[ik].tau,formatSettings);
            Stgm:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].gm,formatSettings);
            StMesFTime:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].t_b*900,formatSettings); // Message frame time? tk = tb * 900
            Writeln(F_RinMN,Format('%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s',
                   [StNKA,StYear,StMonth,StDay,StHour,StMinute,StSecond,Sttau,Stgm,StMesFTime]));
            // BROADCAST ORBIT - 1
            StpX:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].pX*0.001,formatSettings);
            StvX:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].vX*0.001,formatSettings);
            StaX:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].aX*0.001,formatSettings);
            StFt:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].prNNK,formatSettings);  // health (0 = OK) (Bn)
            Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpX,StvX,StaX,StFt]));
            // BROADCAST ORBIT - 2
            StpY:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].pY*0.001,formatSettings);
            StvY:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].vY*0.001,formatSettings);
            StaY:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].aY*0.001,formatSettings);
            StLit:= FormatFloat('0.000000000000E+00', ArLitNumKA[ArTEphGLN_od[ik].n],formatSettings);  // frequency number (-7 ... + 13)
            Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpY,StvY,StaY,StLit]));
            // BROADCAST ORBIT - 3
            StpZ:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].pZ*0.001,formatSettings);
            StvZ:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].vZ*0.001,formatSettings);
            StaZ:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].aZ*0.001,formatSettings);
            StenE:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].enE,formatSettings); // Age of oper. information (days) (E)
            Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpZ,StvZ,StaZ,StenE]));
            end;
        end;
    RecEph_od:=true;
    end;
 End;
 

 // preservation of more recent ephemeris in the table
 ArTEphGLN[eGLN.n].t_b:= eGLN.t_b;
 ArTEphGLN[eGLN.n].pX:= eGLN.pX;
 ArTEphGLN[eGLN.n].pY:= eGLN.pY;
 ArTEphGLN[eGLN.n].pZ:= eGLN.pZ;

 // further conversion to a navigation file
 if eGLN.n<10 then StNKA:='R0'+inttostr(eGLN.n) else StNKA:='R'+inttostr(eGLN.n);

 // Ephemeris Time (UTC)
 dttm := (eGLN^.dtrc)/86400 + RegTime; // StrToDateTime (&#39;01 .01.2008.2008 &#39;);

 DateTimeToSystemTime(dttm,TimeEposh); // translate time from TDateTime to time_t
 // nullify wHour, wMinute, wSecond, wMSecond
 TimeEposh.wHour:=0; TimeEposh.wMinute:=0; TimeEposh.wSecond:=0; TimeEposh.wMilliseconds:=0;
 // we only get the date; translate the date from time_t to TDateTime
 Date1:=SystemTimeToDateTime(TimeEposh);
 // Epoch: Toc - Time of Clock (UTC)
 dttm := ((eGLN^.t_b*900 + 21*60*60) mod 86400)/86400 + Date1;
 DateTimeToSystemTime(dttm,TimeEposh);

 StYear:=inttostr(TimeEposh.wYear);
 if TimeEposh.wMonth<10 then StMonth:='0'+inttostr(TimeEposh.wMonth) else StMonth:=inttostr(TimeEposh.wMonth);
 if TimeEposh.wDay<10   then StDay:='0'+inttostr(TimeEposh.wDay) else StDay:=inttostr(TimeEposh.wDay);
 if (TimeEposh.wHour)<10 then StHour:='0'+inttostr(TimeEposh.wHour) else StHour:=inttostr(TimeEposh.wHour);
 if (TimeEposh.wMinute)<10 then StMinute:='0'+inttostr(TimeEposh.wMinute) else StMinute:=inttostr(TimeEposh.wMinute);
 if (TimeEposh.wSecond)<10 then StSecond:='0'+inttostr(TimeEposh.wSecond) else StSecond:=inttostr(TimeEposh.wSecond);

 // SV / EPOCH / SV CLK
 Sttau:= FormatFloat('0.000000000000E+00', -eGLN^.tau,formatSettings);
 Stgm:= FormatFloat('0.000000000000E+00', eGLN^.gm,formatSettings);
 StMesFTime:= FormatFloat('0.000000000000E+00', eGLN^.t_b*900,formatSettings); // Message frame time? tk = tb * 900
 Writeln(F_RinMN,Format('%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s',
       [StNKA,StYear,StMonth,StDay,StHour,StMinute,StSecond,Sttau,Stgm,StMesFTime]));
 // BROADCAST ORBIT - 1
 StpX:= FormatFloat('0.000000000000E+00', eGLN^.pX*0.001,formatSettings);
 StvX:= FormatFloat('0.000000000000E+00', eGLN^.vX*0.001,formatSettings);
 StaX:= FormatFloat('0.000000000000E+00', eGLN^.aX*0.001,formatSettings);
 StFt:= FormatFloat('0.000000000000E+00', eGLN^.prNNK,formatSettings);  // health (0 = OK) (Bn)
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpX,StvX,StaX,StFt]));
 // BROADCAST ORBIT - 2
 StpY:= FormatFloat('0.000000000000E+00', eGLN^.pY*0.001,formatSettings);
 StvY:= FormatFloat('0.000000000000E+00', eGLN^.vY*0.001,formatSettings);
 StaY:= FormatFloat('0.000000000000E+00', eGLN^.aY*0.001,formatSettings);
 StLit:= FormatFloat('0.000000000000E+00', ArLitNumKA[eGLN^.n],formatSettings);  // frequency number (-7 ... + 13)
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpY,StvY,StaY,StLit]));
 // BROADCAST ORBIT - 3
 StpZ:= FormatFloat('0.000000000000E+00', eGLN^.pZ*0.001,formatSettings);
 StvZ:= FormatFloat('0.000000000000E+00', eGLN^.vZ*0.001,formatSettings);
 StaZ:= FormatFloat('0.000000000000E+00', eGLN^.aZ*0.001,formatSettings);
 StenE:= FormatFloat('0.000000000000E+00', eGLN^.enE,formatSettings); // Age of oper. information (days) (E)
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpZ,StvZ,StaZ,StenE]));

 // end;
 except
  // OknoTP.Lines.Add (&#39;error messages 0x1B!&#39;);
 end;

End;
(*----------------------------------------------------------------------------*)
// receiving the GPS ionosphere parameters packet
Procedure TFormRNX.ExCm1C;
Var ion: ^TIonParam;
Begin
 try
 ion := @ArDat[0];
 if (pr0x1C=false) then
    begin
    ArMesg1C[0]:= FormatFloat('0.0000E+00', ion^.a0,formatSettings);
    ArMesg1C[1]:= FormatFloat('0.0000E+00', ion^.a1,formatSettings);
    ArMesg1C[2]:= FormatFloat('0.0000E+00', ion^.a2,formatSettings);
    ArMesg1C[3]:= FormatFloat('0.0000E+00', ion^.a3,formatSettings);
    ArMesg1C[4]:= FormatFloat('0.0000E+00', ion^.b0,formatSettings);
    ArMesg1C[5]:= FormatFloat('0.0000E+00', ion^.b1,formatSettings);
    ArMesg1C[6]:= FormatFloat('0.0000E+00', ion^.b2,formatSettings);
    ArMesg1C[7]:= FormatFloat('0.0000E+00', ion^.b3,formatSettings);
    pr0x1C:=true;
    end;
 except
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// packet reception GPS time reference parameters
Procedure TFormRNX.ExCm1D;
Var utc: ^TUTCParam;
Begin
 try
 utc := @ArDat[0];
 if (pr0x1D=false) then
    begin
    ArMesg1D[0]:= FormatFloat('0.0000000000E+00', utc^.a0,formatSettings);
    ArMesg1D[1]:= FormatFloat('0.000000000E+00', utc^.a1,formatSettings);
    ArMesg1D[2]:= inttostr(utc^.tot);
    ArMesg1D[3]:= inttostr(utc^.wnt);
    pr0x1D:=true;
    end;
 except
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// packet reception GLO time reference parameters
Procedure TFormRNX.ExCm1E;
Var gln25: ^T25GlnParam;
Begin
 try
 gln25 := @ArDat[0];
 if (pr0x1E=false) then
    begin
    ArMesg1E[0]:= '';
    ArMesg1E[0]:= FormatFloat('0.0000000000E+00', gln25^.tauc,formatSettings);
    ArMesg1E[1]:= FormatFloat('0.000000000E+00', 0.0,formatSettings);
    ArMesg1E[2]:= FormatFloat('0.0000000000E+00', gln25^.taugps,formatSettings);
    pr0x1E:=true;
    end;
 except
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// receipt of a receipt for the package &quot;0xC1&quot;
Procedure TFormRNX.ExCmC1;
Var
    snChar:char;
    snDig:integer;
    snStr:string;
Begin
 try
  // serial number
  snChar:=Chr(ArDat[2] shr 24);
  snDig:= (ArDat[2] and $FFFFFF);
  // CS firmware
  // CS: = ArDat [3];

  snStr:=inttostr(snDig);
  while (length(snStr)<9) do snStr:='0'+snStr;
  // snStr: = snChar + snStr;

  // if no number
  if (ArDat[2] = -1) then snStr:= ' N/A     ';

  // we get only the number
  verSt:= IntToStr(ArDat[0] shr 16) + '.' +IntToStr(ArDat[0] and $FFFF);

  StSN:=' S/N:'+snStr;
 except
  // 
 end;
end;
(*----------------------------------------------------------------------------*)
// GPS almanac reception
Procedure TFormRNX.ExCm88;
Var aGPS: ^TAlmnGPS;
Begin
 try
 if (FirstAnalysis=1) then
    begin
    pr0x88:=true;
    end;

 aGPS := @ArDat[0];
 if (aGPS.num<>0)and(aGPS.num<33) then ArTAlmnGPS[aGPS.num]:= aGPS^;

 except  // wrong package
  // 
 end;
End;
(*----------------------------------------------------------------------------*)
// receiving a packet to turn on the receiver
Procedure TFormRNX.ExCm3E;
Begin
 inc(num3E);
 if (num3E > 2) then
     begin
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! RESTART G5!!!');
     num3E:= 0;
     end;

End;


(*----------------------------------------------------------------------------*)
procedure TFormRNX.FormClose(Sender: TObject; var Action: TCloseAction);
begin
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_CURRENT_USER;
 Reg.OpenKey('Software\GeostarNavigation\GEOS5ToRNX',true);
 Reg.WriteInteger('Top',Top);
 Reg.WriteInteger('Left',Left);
 Reg.WriteInteger('RGSV',RGSV.ItemIndex);
 Reg.CloseKey;
 Reg.Free;
end;
procedure TFormRNX.FormCreate(Sender: TObject);
var
 ik: integer;
begin
 formatSettings.DecimalSeparator:='.';
 AppDir := Application.ExeName;
 FReg := TRegistry.Create;
 FReg.RootKey := HKEY_CURRENT_USER;
 if (FReg.KeyExists('Software\GeostarNavigation\GEOS5ToRNX')=true) then
     begin
     FReg.OpenKey('Software\GeostarNavigation\GEOS5ToRNX',true);
     FormRNX.Top:=FReg.ReadInteger('Top');
     FormRNX.Left:=FReg.ReadInteger('Left');
     try RGSV.ItemIndex:= FReg.ReadInteger('RGSV'); except RGSV.ItemIndex:= 0; end;
     RGSVClick(nil);
     FReg.CloseKey;
     FReg.Free;
     end;
 for ik := 1 to 24 do ArLitNumKA[ik]:=100;
 FillChar(ArTEphGPS,sizeof(ArTEphGPS),0);
 FillChar(ArTEphGLN,sizeof(ArTEphGLN),0);
 FillChar(EL,sizeof(EL),0);
end;

procedure TFormRNX.RGSVClick(Sender: TObject);
begin
  if (RGSV.ItemIndex=0) then
     begin
     Label1.Visible:=false;
     EditElMask.Visible:=false;
     end
 else
     begin
     Label1.Visible:=true;
     EditElMask.Visible:=true;
     end;
end;

(*----------------------------------------------------------------------------*)
end.
