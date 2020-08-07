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

  TArLw = array of longword;  // ��������� ������ ��� ������� ��� PSGG

  TCoorUser = record
    DtTmUTC: double;		            // ���� � ����� UTC
	  Lat: double;			              // ������
	  Lon: double;			              // �������
	  Alt: double;			              // ������ ��� ����������
	  dEG: double;			              // ���������� ��������� �� ������
	  Nfix: integer;			            // ���-�� �� � �������
	  mdFix: integer;			            // ��� ������� ��
	  GDOP: double; 	   	            // GDOP
	  PDOP: double;		                // PDOP
	  TDOP: double;		                // TDOP
	  HDOP: double;		                // HDOP
	  VDOP: double;		                // VDOP
    PrValNT: integer;		            // �������� ������������� ������� ��
	  NValNT: longword;			          // ���-�� ����������� ����������� ������� ��
    speed: double;		              // �������� � �����
	  heading: double;		            // ����
  end;

  // ��������� �������� �� �������
  TEphGLN = record
	  dtrc: longword; 	  // ���� � ����� ������ ������ � ����						1
	  N4t: word;          // �����. ����� ������������ � ����� 1461*N4 + Nt
    NA: word; 	        // ����� ����� �� ������� ���������� ��������				1
    pX, pY, pZ: double; // ���������, ��������, ��������� �� �� ������ t_b			6
    vX, vY, vZ: double;	//															6
	  aX, aY, aZ: double;	//															6
	  gm: double;       	// �������. ���������� ������� �������						2
    tau: double;      	// ����� ��� ������������ ���								2
    tauc: double;     	// ����� ��� ������������ UTC(SU)							2
    taugps: double;   	// ����������� ��� � �� GPS									2
    prNNK: word;        // ������� ��������������� �����
                      	// 0 ��� - ln �� ������ 2 ��
                      	// 1 ��� - ln �� ������ 3 ��
                      	// 2 ��� - ln �� ������ 5 ��
                      	// 4 ��� - ln �� ������ 7 ��
                      	// 5 ��� - ln �� ������ 9 ��
                      	// 6 ��� - ln �� ������ 11 ��
                      	// 7 ��� - ln �� ������ 13 ��
                      	// 8 ��� - ln �� ������ 15 ��
                      	// 9 ��� - ��������� �������� CnA ��� �� �� ���������
    pss: word;        	// ������ �������� ��										1
                      	// 0..1  - ������� P1
                      	// 2..3  - ������� Bn
                      	// 4     - ������� P2
                      	// 5     - ������� P3
                      	// 6..7  - ������� P
                      	// 8     - ������� P4
                      	// 9..11 - ������� ��&��
                      	// 12.13 - ������� M
	  t_b: byte;          // �������� �� ������� ��������� ��������� (1..95)
    enE: byte; 	        // ������� �� ����� (1..31)
    Ft: byte;           // �������������� ������ ��������� (1..15)
   	n: byte;            // ����� ��������� ����� ��	(1..24)      					1
  end;

// ��������� �������� �� GPS
  TEphGPS = record
	  dtrc: longword; 	  // ���� � ����� ������ ������ � ����						1
	  dn: single;       	// ������� ������������ �������� � �������. ����������		1
 	  M: double;        	// ������� �������� �� �������� ������ �������				2
	  e: double;       	  // ��������������											2
	  A: double;        	// ���������� ������ �� ������� �������						2
	  OMEGA_0: double;  	// ������� ����������� ���� � ����������� ���������			2
	  I0: double;       	// ���������� ������ � �������� ������ �������				2
	  w: double;        	// �������� (����) �������									2
	  OMEGADOT: double; 	// �������� ������� �����������								2
	  af0: double;		    // ����������� ������������ ����� �� ��						2
	  IDOT: single;     	// ����������� ���������� ������							1
	  C_uc: single;     	// ��������� �����.������.������.����� � ��������� ������	1
	  C_us: single;     	// ��������� �����.������.������.����� � ��������� ������	1
	  C_rc: single;     	// ��������� �����.������.������.����� � ������� ������		1
	  C_rs: single;     	// ��������� �����.������.������.����� � ������� ������		1
	  C_ic: single;     	// ��������� �����.������.������.����� � ���� ����������	1
	  C_is: single;     	// ��������� �����.������.������.����� � ���� ����������	1
	  Tgd: single;			  // �������� ��������� ��������								1
	  af2: single;      	// ����������� ������������ ����� �� ��						1
	  af1: single;			  //															1
	  t_oe: integer;     	// ��������� ������ ������� ��������						1
	  t_oc: integer;     	// clock data reference time								1
   	weekN: word;	      // ����� ������
    IODC: word; 	      //
	  SVaccur: byte;	    // ������ �������� 	(0..15)
    SVhelth: byte; 	    // �������� ��������	(6 ���)
	  n: byte;		        // ����� ��	(1..32)										1
    IODE: byte; 	      // ������� ������ (��������)							1
  end;

// ��������� ������ ������� ��
  TVisSA = record
  	lit: smallint; 	// ����� ��������� ������ ��� ���. ���� �� ���, �� ���������� 0
	  idnm: byte;	    // ����� �� � ������������ � Satellite ID number (NMEA)
  	idch: byte;	    // ����� ������ �������� (0..31). ���� �� �� �� �����, �� ���������� ��� 0xFF
	  ss: longword;	  // ����� ��������� ������. ���� �� �� �� �����, �� ���������� 0
  	Amp: single;		// ��������� ������/���, ����
	  el: single;			// ���� ����� ��, ���
  	az: single;			// ������ ��, ���
  end;

// ��������� ������ 0x22
  TPac22 = record
	  NSat: integer;			// ���������� ��������� �� ������� ���������� ����������
    vs: array [0..42] of TVisSA;		// ��������� ������ ������� ��
  end;

  TPac23 = record
   DtTmUTC: double;		        // ���� � ����� UTC
	 X: double;			            // X: ���������� ��������� �� ��� X, WGS-84
	 Y: double;			            // Y: ���������� ��������� �� ��� Y, WGS-84
	 Z: double;			            // Z: ���������� ��������� �� ��� Z, WGS-84
	 Vx: double;			          // VX: ���������� ������� �������� �� ��� X
   Vy: double;		            // VY: ���������� ������� �������� �� ��� Y
   Vz: double;                // VZ: ���������� ������� �������� �� ��� Z
   acPos: double;             // ��� ������ 3D ���������
   acVel: double;             // ��� ������ 3D ��������
   NumEffSV: integer;			    // ���-�� �� � �������
   useExtrapolate: word;      // ������� ������������� RTK �������
   TypeSolve: word;           // ��� �������
   ClockOffGPS: double;       // ����� �� ��������� ������������ �� GPS
   lps: double;               // �������� ����� �� ���������
   ClockOffRaB: double;       // ����� �� ������ ������������ �� ����
   ageDiff: double;           // ������� ���������������� ��������
   reserv: double;
  end;

  // ��������� ������ 0x24
  TPac24 = record
    DtTmUTC: double;		            // ���� � ����� UTC
	  Lat: double;			              // ������
	  Lon: double;			              // �������
	  Alt: double;			              // ������ ��� ����������
	  dEG: double;			              // ���������� ��������� �� ������
    speed: double;		              // �������� � �����
    absVecSpeed: double;
    heading: double;		            // ����
    GDOP: double; 	   	            // GDOP
	  PDOP: double;		                // PDOP
	  TDOP: double;		                // TDOP
	  HDOP: double;		                // HDOP
	  VDOP: double;		                // VDOP
    acPos: double;                // ��� ������ 3D ���������
    acVel: double;                // ��� ������ 3D ��������
    NumEffSV: integer;			         // ���-�� �� � �������
    ModeRTK: word;
    TypeSolve: word;
    SurveyTime: integer;
    reserv: integer;
  end;

  // ��������� ������ 0x13
  TPac13 = record
    dt: array[0..15] of double;
  end;

  // ��������� ������ 0x14
  TPac14 = record
  	dtUTC: double;		// ������� ���� � ����� (UTC), ��� (0 - 00:00 1 ������ 2008 �. UTC)
	  dtLOC: double;		// ������� ���� � ����� (���������), ���
	  ac1PPS: double;		// ������ �������� ������� 1PPS, ����
  	TmGPS: longword;	// ����� GPS � ������ ������, ���
	  TmGLN: longword;	// ����� ��� � ������ �����, ���
	  NmWk: word;       // ����� ������ GPS
  	RlWk: word;       // ���������� ������������ �������� ������ GPS
	  Nt: word;	        // ����� ����� ������ ������������ ���
  	N4: word;	        // ����� ������������ ���
	  revb: byte;	      // ������
  	KP: byte;	        // ������� ����������� ��������� �� ��� (0 - �� �����������, 1 - ��������� �� "+1���", 2 - ������� �� �������, 3 - ��������� �� "-1���"
	  flp: byte;	      // ����������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
  	lps: byte;	      // ������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
  	rev14: longword;	// ������
  end;

  // ��������� ��������� �� �������
  TAlmnGLN = record
   	e: double;           	// �������������� ������ (0 ... 0,03)
    ddT: double;         	// �������� ��������� ������. ������� (+-2e-8)
    Omg: double;         	// �������� ������� ������ (+-1)
    dT: double;          	// �������� � ����. �������� ����. ������� (+-3,6e+3)
    tl: double;          	// ��������� ����������� ������� ������. ���� (0 ... 44100)
    Lmd: double;         	// ������� ������� ������. ���� (+-1)
    di: double;          	// �������� � �����. �������� ���������� ������ (+-0,067)
    ta: double;          	// ������ ����� ��� � ��� (+-1,9�-3)
    num: word;            // ����� �� �� ���������
    day: word;		        // ���� �� ������� ���������� ��������
    Cn_mdf: word;         // ������� Cn (15 ���) ����������� �� (0,1 ���)
    aLt: SmallInt;        // ����� ������ �� ���������
  end;

  // ��������� ��������� �� GPS
  TAlmnGPS = record
  	M: double;		   	    // ������� ��������
    e: double;			      // ��������������
    A: double;		        // ������ �� ������� �������
    OMEGA_0: double;		  // ������� ����������� ���� �� 00�.00���.00���. ������� ����
    dI0: double;			    // ���������� ���������� ������
    w: double;		        // �������� �������
    OMEGADOT: double;	    // �������� ��������� ����������� ���� ������
    af0: double;		      // �������� � �������� �����
    af1: double;			    // �������� ��������� af0
	  t_oa: integer;			  // ����� �� ������� ���� (���.)
	  num: word;	          // ����� ��������� �����
    stSv: word;			      // ��������� ��������
  end;

  // ��������� ���������� ���������
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

  // ��������� ��������� �������� GPS
  TUTCParam = record
    a0: double;
    a1: double;
    tot: integer;   // ��������� ����� ��� ������ UTC, ���
    dtls: word;     // ������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
    wnt: word;      // ��������� ����� ������ UTC
    wnlsf: word;    // ����� ������ � �������� ��������� ����������� ����������� UTC � GPS �� ����� ����� ������
    dn: word;       // ����� ���, � ��������� �������� ��������� ����������� ����������� UTC � GPS �� ����� ����� ������
    dtlsf: word;    // ����������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
    wn: word;       // ������� ����� ������
  end;

  // ��������� ��������� �������� �������
  T25GlnParam = record
    tauc: double;
    taugps: double;
	  B1, B2: double;
    N4: word;
    Nt: word;
    KP: word;
    NA: word;
  end;

  // ��������� ������������� ���������� ��� ������ 0�10
  TMeasOut = record
    tdt: byte;        // ������������ �������������� ������������� ������
    lit: Shortint;    // ����� ��������� ������ ��� ������� � �������������� ����
 	  idnum: byte;		  // ����� �� � ������������ � Satellite ID number (NMEA)
	  idChn: byte;		  // ����� ������ ��������
    ampsg: single;    // ��������� �������
	  prng: double;		  // ���������������, �
	  pvel: double;	    // ��������������, �/���
    padr: double;   	// ���.��������� ���������������
    pphi: double;		  // ���� �������, ��� (????)
    acpr: single;	    // ������ ��� ��������� ���������������, �
    acvl: single;	    // ������ ��� ��������� ��������������, �/���
    erpr: single;	    // ������ ������ ���������������, �
    ervl: single;	    // ������ ������ ��������������, �/���
  end;  
  // ��������� ������ 0x10
  TPac10 = record
   dtUTC: double;		    // ���� � ����� ������������� ����������, ��� (0 � 00:00 1 ������ 2008 �. UTC)
	 NFs: integer;			  // ���-�� ������ �� ������ ���������
   LeapSec: word;		    // ������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
   NMSat: word;         // ���������� ���������, �� ������� ���������� ������������� ���������� (NSat)
   ClockOffGPS: double; // ����� �� ��������� ������������ �� GPS � ������
   Ms: array [0..42] of TMeasOut;  // ��������� ������������� ����������
  end;

  // ��������� ������������� ���������� ��� ������ 0�0F
  TMeasContOut = record
   tdt: byte;         // ������������ �������������� ������������� ������
   lit: Shortint;     // ����� ��������� ������ ��� ������� � �������������� ����
   idnum: byte;		    // ����� �� � ������������ � Satellite ID number (NMEA)
   idChn: byte;		    // ����� ������ ��������
   ampsg: single;     // ��������� �������
   prng: double;		  // ���������������, �
   pvel: double;	    // ��������������, �/���
   pphi: double;		  // ���� �������, ��� (????)
   X: double;	        // ���������� �������� X, �
   Y: double;	        // ���������� �������� Y, �
   Z: double;	        // ���������� �������� Z, �
   vX: double;	      // �������� �������� �� ��� X, �/���
   vY: double;	      // �������� �������� �� ��� Y, �/���
   vZ: double;	      // �������� �������� �� ��� Z, �/���
  end;
  // ��������� ������ 0x0F
  TPac0F = record
   dtUTC: double;		    // ���� � ����� ������������� ����������, ��� (0 � 00:00 1 ������ 2008 �. UTC)
	 NFs: integer;			  // ���-�� ������ �� ������ ���������
   LeapSec: word;		    // ������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
   NMSat: word;         // ���������� ���������, �� ������� ���������� ������������� ���������� (NSat)
   ClockOffGPS: double; // ����� �� ��������� ������������ �� GPS � ������
   Ms: array [0..42] of TMeasContOut;  // ��������� ������������� ����������
  end;

const
  L1freq = 1575.42e6;
  SL = 299792458;
  pi = 3.1415926535897932384626433832795;

var
  FormRNX: TFormRNX;
  formatSettings : TFormatSettings;
  Reg,FReg: TRegistry;
  AppDir: string;                 // ������� ��������� �������
  Fnam,Nkml1:string;

  ArDat: array of integer;            // ������ ������ ������. ������
  RegTime:double = 39448;             // 01.01.2008
  RegTimeGPS:double = 29226;          // 06.01.1980
  StrFile1: string;
  StrFile: AnsiString;
  F_RinMO,F_RinMN: TextFile;    // ���� Rinex MO Mixed Obs. and  ���� Rinex MN- Nav. All GNSS
  LOG: TextFile;                // log ���� ��������� ������ ���������
  F1: File of char;             // ���� BIN
  F2: File of char;
  hnd,siz,kol: integer;         // ������ ����� � ������������ ���-��
  NameFrnx:string;

  indGPS,indGLN: integer;
  FEnter:boolean;  // ������� �������������
  Hold:boolean;

  LeapSec: integer=0;     // ������� �������� ����������� UTC � GPS �� ����� ����� ������, ���
  ClockOffset,ClockOffset1: double;
  WRollOver: integer=0; // ���-�� ������������ �������� ������ GPS
  StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10,StKA10: string;
  TimeEposh1: time_t;  // ����� ����� ��� ������ ����� ��������� ROVER
  pc10dtUTC_OLD: double; // ���������� ����� ��� �������� ������������ ����
  StPac10: string;          // ������ ������ 0�10
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

  ArLitNumKA: array [1..24] of integer; // ������ ������������ ������ �� � ������
  EL: array [1..199] of single;         // ������ �� ��� ���� �� � �������
  GinFix: boolean=false; // ������� ������� ��
  verSt:string = '';
  StSN:string = '';

  ArTEphGPS: array [1..32] of TEphGPS;
  ArTEphGLN: array [1..24] of TEphGLN;
  ArTEphGAL: array [1..36] of TEphGPS;

  ArTEphGPS_od: array [1..32] of TEphGPS; // ������ �������� �������� ��� ����
  ArTEphGLN_od: array [1..24] of TEphGLN; // ������ �������� �������� ��� ����

  ArMesg1C: array[0..7] of string; // ������ ������ ���������� 0x1C ��������� ��������� GPS
  ArMesg1D: array[0..3] of string; // ������ ������ ���������� 0x1D ��������� �������� GPS
  ArMesg1E: array[0..2] of string; // ������ ������ ���������� 0x1E ��������� �������� GLO

  FTime: time_t;       // ����� ������� ��������� � ���� ����������
  FdtUTC: double=0;         // ����� ������� ��������� � ������� ��������� 0�10
  ChDT: boolean=false;      // ������� ��������� ���� � ������� �� �������������� ���������
  RecEph_od: boolean=false; // ������� ������ ������ ������� �������� ��� ����
  GGFix: boolean=false;     // ������� ������������ �������

  coorX,coorY,coorZ: double;
  errCS10,errCS13: boolean;

  ArSVsOBS_GPS: array[0..42] of integer;  // ������ ��������� �� ���� �� GPS �� ���� �����; 1- ��������� ����; 0-��������� ������
  ArTAlmnGPS: array [1..32] of TAlmnGPS;

  TimeUTC: double; // ������� ����� ������� ��

  num3E: word = 0;                    // ���������� ��������� �� �������� G5 (������ 4��)

implementation

{$R *.dfm}

(*----------------------------------------------------------------------------*)
// ������� �������� ����� gdt
Function FileGdtCreate(FileName: string; Ng: byte; StHead: Ansistring; PrOrd: integer): boolean;
Var Fgdt: file of Ansichar;
    ik: integer;
    VerFDt, Nh, MgWr: word;
    NGr, Resb: byte;
    Ch1, Ch2: Ansichar;
Begin
 AssignFile(Fgdt,FileName);
 Rewrite(Fgdt);
 VerFDt:=$100*5+0;      // ������ ��������� SG 5.0
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
// Writeln(F_RinMO,Format('%s%56s',['END OF FILE','COMMENT']));
// OknoTP.Lines.Add('bytes wr = '+inttostr(kol));
 OknoTP.Lines.Add('Conversion completed!');
 PBarRNX.Visible:=false;
 Button2.Enabled:=true;
 CloseFile(LOG);
 CloseFile(F_RinMO);
 CloseFile(F_RinMN);
 Screen.Cursor:=crDefault;
end;
(*----------------------------------------------------------------------------*)
// �������� ������ �� �� ������. ������, ������ � ��������� ��
Function CheckNum(pNum: byte): integer;
Var ik: byte;
Begin
 try
 Result := 0;
 for ik := 1 to 24 do if (pNum = ArLitNumKA[ik]) then Result := ik; // �������� ����� ��
 except
  //FormMain.OknoTP.Lines.Add('ERR ������ ������ = '+inttostr(plit));
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
 // ������������� ����������
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
 GinFix:=false; // ������� ������� ��
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

 // ������ ��������� �����, ��� ������ ����������
 FirstAnalysis:=1;
 Button2.Enabled:=false;
 Screen.Cursor:=crHourGlass;

 // ����������� �����(���������)
 AssignFile(F1,FNEdit.FileName);
 Reset(F1) ;
 siz:= filesize(F1);
 sizRead:=0;
 StrFile:='';

 PBarRNX.Position:=0; kol:=0;
 PBarRNX.Max:=trunc(siz);
 PBarRNX.Visible:=true;

 // ������ ������ �� �����
 WHILE (sizRead<siz) DO
   BEGIN
   // ���� ��������� ������ �������, �� ��������� �� ������ ��������� �����
   if (pr0x10 or pr0x0F) then
       begin
       if (pr0x14 and pr0x1C and pr0x1D and pr0x1E and
           pr0x13 and pr0x1A and pr0x1B{ and pr0x19}) then break;
       end;

   //  ���������� ������ ������� ����� ������� �� �����
   StrHalfFile:='';
   if ((siz-sizRead)>500000000) then
        SetLength(StrHalfFile, 500000000)
   else SetLength(StrHalfFile, (siz-sizRead));

   // ������ �� ����� ���� ������������� �������
   BlockRead(F1,StrHalfFile[1],Length(StrHalfFile),NumRead);
   //StrFile:=StrFile+StrHalfFile;
   StrFile:=StrHalfFile;
   sizRead:= sizRead+NumRead;

   // ��������� ������
   WHILE Length(StrFile)>0 DO
     BEGIN
     // ����� �����������
     if FEnter=false then
        begin
        if Length(StrFile) < 20 then break;// ������������ ������ ��� �������
        iSt:=0; iSt:= Pos('GEOSr3PS',StrFile);
        if (iSt=0) then break; // ����������� �� �������
        if Length(StrFile)-iSt < 19 then break;// ������������ ������ ��� �������
        // ����������� �������
        FEnter:=true;
        end
     else
        begin
        if Length(StrFile)-iSt < 19 then // ������������ ������ ��� �������
           begin
           Delete(StrFile,1,iSt-1);   // �������� ������������ ������
           FEnter:=false;
           break; end;
        ncmd := ord(StrFile[iSt+9]) shl 8 + ord(StrFile[iSt+8]);
        ndat := ord(StrFile[iSt+11]) shl 8 + ord(StrFile[iSt+10]);
        lnPc := 4*ndat+16;                        // ����� ����� ������ � ������
        lnPw := ndat+4;                           // ����� ������ � longword
        if Length(StrFile) < iSt+lnPc-1 then // ������������ ������ ��� �������
           begin //ConvComplete;
           Delete(StrFile,1,iSt-1);   // �������� ������������ ������
           FEnter:=false;
           break;
           end;
        CSpac := 0;
        SetLength(ArDat,ndat);
        Application.ProcessMessages;
        // ������ CS
        for ik := 0 to lnPw-1 do
            begin
            dtlw := ord(StrFile[iSt+4*ik+3]) shl 24 + ord(StrFile[iSt+4*ik+2]) shl 16 +
                    ord(StrFile[iSt+4*ik+1]) shl 8 + ord(StrFile[iSt+4*ik]);
            CSpac := CSpac xor dtlw;
            if (ik > 2)and(ik < lnPw-1) then ArDat[ik-3] := dtlw;
            end;
        if CSpac <> 0 then          //�����. ����� ������ �� �������
           begin
           //Delete(StrFile,1,iSt+lnPc-1);   // �������� ������������ ������
           Delete(StrFile,1,iSt+3);   // �������� ������������ ������ �� ������� ������ ������� ������ + GEOS
           iXt:=0; iXt:= Pos('GEOSr3PS',StrFile); // ���� GEOS
           Delete(StrFile,1,iXt-1);   // �������� ������� ������� ������

           FEnter:=false;
           //break;
           end;
        // ����������� ������ � ��������� �������
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
        //if CSpac = 1 then OknoTP.Lines.Add('�������� ���-�� ���������� ������ '+IntToHex(ncmd,2));
        iSt:=iSt+lnPc;
        //OknoTP.Lines.Add('iSt = '+inttostr(iSt));
        kol:=kol+lnPc; PBarRNX.Position:=kol;
        end;
     Application.ProcessMessages;
     END; // WHILE Length(StrFile)>0 DO
   Application.ProcessMessages;
   END; // WHILE (sizRead<siz) DO

 // �������� �� ���������� ������ ��������� � ��� �����
 if pr0x14=false then OknoTP.Lines.Add('No messages 0x14 (Timing parameters)!');  // Leap seconds
 if pr0x19=false then OknoTP.Lines.Add('No messages 0�19, 0x89 (GLONASS almanac)!');  // ��� �����
 if pr0x1C=false then OknoTP.Lines.Add('No messages 0x1C, 0x9C (GPS ionospheric parameters)!');
 if pr0x1D=false then OknoTP.Lines.Add('No messages 0x1D, 0x9D (GPS time to UTC conversion parameters)!');
 if pr0x1E=false then OknoTP.Lines.Add('No messages 0x1E, 0x9E (GLONASS time to UTC conversion parameters)!');
                                      //0x1F, 0x9F: GST-UTC Conversion Parameters

 if ((pr0x10<>true) and (pr0x0F<>true)) then OknoTP.Lines.Add('No messages 0x10 (Raw measurements)!');
 if pr0x13=false then OknoTP.Lines.Add('No messages 0x13 (Navigation solution state vector)!');
 if pr0x0A=false then OknoTP.Lines.Add('No messages 0x0A, 0x9A (GALILEO ephemeris)!');
 if pr0x1A=false then OknoTP.Lines.Add('No messages 0x1A, 0x8A (GPS/QZSS ephemeris)!');
 if pr0x1B=false then OknoTP.Lines.Add('No messages 0x1B, 0x8B (GLONASS ephemeris)!');

 if pr0x88=false then OknoTP.Lines.Add('No messages 0x18, 0x88 (GPS/QZSS almanac)!');  // ��� ������� 2
 // OknoTP.Lines.Add('GPS almanac is not available. Filter #2 will be disabled!');


 // ������������ ������ ������� �������� �����
 // ���� ���� 1-366
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

  //�������� ������ rinex   F_CoorROVER
  AssignFile(F_RinMO,ExtractFilePath(FNEdit.FileName)+'xxxx00RUS_R_'+StYear+StDY+StHour+StMinute+'_00U_xxU_MO'+'.rnx'); Rewrite(F_RinMO);
  AssignFile(F_RinMN,ExtractFilePath(FNEdit.FileName)+'xxxx00RUS_R_'+StYear+StDY+StHour+StMinute+'_00U_xxU_MN'+'.rnx'); Rewrite(F_RinMN);
  //�������� ����� LOG_ERRORS
  AssignFile(LOG,ExtractFilePath(FNEdit.FileName)+'LOG_ERRORS.txt'); Rewrite(LOG);

 except
  OknoTP.Lines.Add('Cannot create Rinex file!');
  Screen.Cursor:=crDefault;
  PBarRNX.Visible:=false;
  Button2.Enabled:=true;
  exit;
 end;
   // HEADER - GNSS Navigation Message File �
   Writeln(F_RinMN,'     3.02           N: GNSS NAV DATA    M: MIXED            RINEX VERSION / TYPE');
   Writeln(F_RinMN,'GEOS5toRNX          Geostar             '+StGreateFileTime+' PGM / RUN BY / DATE ');
   Writeln(F_RinMN,Format('%4s%13s%12s%12s%12s%23s',['GPSA',ArMesg1C[0],ArMesg1C[1],ArMesg1C[2],ArMesg1C[3],'IONOSPHERIC CORR'],formatSettings));
   Writeln(F_RinMN,Format('%4s%13s%12s%12s%12s%23s',['GPSB',ArMesg1C[4],ArMesg1C[5],ArMesg1C[6],ArMesg1C[7],'IONOSPHERIC CORR'],formatSettings));
   Writeln(F_RinMN,Format('%4s%18s%16s%7s%5s%26s',['GPUT',ArMesg1D[0],ArMesg1D[1],ArMesg1D[2],ArMesg1D[3],'TIME SYSTEM CORR'],formatSettings));
   //Writeln(F_RinMN,Format('%4s%18s%16s%7d%5d%26s',['GLUT',ArMesg1E[0],ArMesg1E[1],0,0,'TIME SYSTEM CORR'],formatSettings));
   //Writeln(F_RinMN,Format('%4s%18s%16s%7d%5d%26s',['GLGP',ArMesg1E[2],ArMesg1E[1],0,0,'TIME SYSTEM CORR'],formatSettings));
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
   //Writeln(F_RinO,'     18.000                                                 INTERVAL');
   //Writeln(F_RinO,'G  APPL_DCB         xyz.uvw.abc//pub/dcb_gps.dat            SYS / DCBS APPLIED');
   Writeln(F_RinMO,'DBHZ                                                        SIGNAL STRENGTH UNIT');
   Writeln(F_RinMO,Format('%6s%6s%6s%6s%6s%14s%7s%26s',
   [StYear,StMonth,StDay,StHour,StMinute,StSecF,'GPS','TIME OF FIRST OBS'],formatSettings));
   //Writeln(F_RinO,'  2014    03    24    13    10    36.0000000     GPS        TIME OF FIRST OBS');
   //Writeln(F_RinO,' 18 R01  1 R02  2 R03  3 R04  4 R05  5 R06 -6 R07 -5 R08 -4 GLONASS SLOT / FRQ #');
   //Writeln(F_RinO,'    R09 -3 R10 -2 R11 -1 R12  0 R13  1 R14  2 R15  3 R16  4 GLONASS SLOT / FRQ #');
   //Writeln(F_RinO,'    R17  5 R18 -5                                           GLONASS SLOT / FRQ #');
   //Writeln(F_RinO,' C1C  -10.000 C1P  -10.123 C2C  -10.432 C2P  -10.634        GLONASS COD/PHS/BIS');
   Writeln(F_RinMO,Format('%6d%66s',[LeapSec,'LEAP SECONDS']));
   Writeln(F_RinMO,Format('%73s',['END OF HEADER']));
   //


 // ������ ��������� �����, ��� ������ ������
 FirstAnalysis:=2;
 PBarRNX.Position:=0; kol:=0; FEnter:=false;

 Reset(F1);
 siz:= filesize(F1);
 sizRead:=0;
 StrFile:='';
 TimeUTC:=SystemTimeToDateTime(FTime);

 // ������ ������ �� �����
 WHILE (sizRead<siz) DO
   BEGIN
   //  ���������� ������ ������� ����� ������� �� �����
   StrHalfFile:='';
   if ((siz-sizRead)>200000000) then
        SetLength(StrHalfFile, 200000000)
   else SetLength(StrHalfFile, (siz-sizRead));

   // ������ �� ����� ���� ������������� �������
   BlockRead(F1,StrHalfFile[1],Length(StrHalfFile),NumRead);
   //StrFile:=StrFile+StrHalfFile;
   StrFile:=StrHalfFile;
   sizRead:=sizRead+NumRead;

   WHILE Length(StrFile)>0 DO
     BEGIN
     if FEnter=false then
        begin
        if Length(StrFile) < 20 then break;// ������������ ������ ��� �������
        iSt:=0; iSt:= Pos('GEOSr3PS',StrFile);
        if (iSt=0) then break; // ����������� �� �������
        if Length(StrFile)-iSt < 19 then break;// ������������ ������ ��� �������
        // ����������� �������
        FEnter:=true;
        end
     else
        begin
        if Length(StrFile)-iSt < 19 then // ������������ ������ ��� �������
           begin
           //Memo1.Lines.Add('err msg!'+StrFile);
           //Delete(StrFile,1,iSt-1);   // �������� ������������ ������
           FEnter:=false;
           break; end;
        //if iSt > 1 then Memo1.Lines.Add('������� ����������� '+IntToStr(iSt));
        ncmd := ord(StrFile[iSt+9]) shl 8 + ord(StrFile[iSt+8]);
        ndat := ord(StrFile[iSt+11]) shl 8 + ord(StrFile[iSt+10]);
        lnPc := 4*ndat+16;                        // ����� ����� ������ � ������
        lnPw := ndat+4;                           // ����� ������ � longword
        if Length(StrFile) < iSt+lnPc-1 then // ������������ ������ ��� �������
           begin
           //Memo1.Lines.Add('err msg '+inttohex(ncmd,2)+' '+StrFile);
           //Delete(StrFile,1,iSt-1);   // �������� ������������ ������
           FEnter:=false;
           break;
           end;
        CSpac := 0;
        SetLength(ArDat,ndat);
        Application.ProcessMessages;
        // ������ CS
        for ik := 0 to lnPw-1 do
            begin
            dtlw := ord(StrFile[iSt+4*ik+3]) shl 24 + ord(StrFile[iSt+4*ik+2]) shl 16 +
                    ord(StrFile[iSt+4*ik+1]) shl 8 + ord(StrFile[iSt+4*ik]);
            CSpac := CSpac xor dtlw;
            if (ik > 2)and(ik < lnPw-1) then ArDat[ik-3] := dtlw;
            end;
        if CSpac <> 0 then          //�����. ����� ������ �� �������
           begin
           // ������� �����
           //DateTimeToSystemTime(now,TimeNow);
           StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
           Writeln(LOG,StPac10+': ERROR! CS msg = '+inttohex(ncmd,2)+';');

           //if (ncmd=$10) then errCS10:=true;
           //if (ncmd=$13) then errCS13:=true;
           //OknoTP.Lines.Add('err CS msg '+inttohex(ncmd,2));
           //Delete(StrFile,1,iSt+lnPc-1);   // �������� ������������ ������

           Delete(StrFile,1,iSt+3);   // �������� ������������ ������ �� ������� ������ ������� ������ + GEOS
           //Delete(StrFile,1,iSt-1);   // �������� ������������ ������ �� ������� ������ ������� ������ + GEOS
           iXt:=0; iXt:= Pos('GEOSr3PS',StrFile); // ���� GEOS
           Delete(StrFile,1,iXt-1);   // �������� ������� ������� ������

           FEnter:=false;
           //break;
           end
        else
           begin
           // ����������� ������ � ��������� �������
           case ncmd of
           $0F: if ndat = 20*(ArDat[3] shr 16)+6 then ExCm0F else CSpac := 1;
           $10: if ndat = 14*(ArDat[3] shr 16)+6 then ExCm10 else CSpac := 1;
           $13: if ndat = 32   then ExCm13 else CSpac := 1;
         //$14: if ndat = 12   then ExCm14 else CSpac := 1;
           $0A,$9A: if ndat = 32   then ExCm0A else CSpac := 1;
           $1A,$8A: if ndat = 32   then ExCm1A else CSpac := 1;
           $1B,$8B: if ndat = 30   then ExCm1B else CSpac := 1;
         //$1C: if ndat = 8    then ExCm1C else CSpac := 1;
         //$1D: if ndat = 8    then ExCm1D else CSpac := 1;
         //$1E: if ndat = 10   then ExCm1E else CSpac := 1;
           $20: if ndat = 28   then ExCm20 else CSpac := 1;
           $22: if ndat = 5*ArDat[0]+1 then ExCm22 else CSpac := 1;
           $24: if ndat = 34   then ExCm24 else CSpac := 1;

           $3E: if ndat = 3    then ExCm3E else CSpac:= 1;
           end;
           //if CSpac = 1 then OknoTP.Lines.Add('�������� ���-�� ���������� ������ '+IntToHex(ncmd,2));
           iSt:=iSt+lnPc;
           //OknoTP.Lines.Add('iSt = '+inttostr(iSt));
           kol:=kol+lnPc; PBarRNX.Position:=kol;
           end; //if CSpac <> 0
        end;
     Application.ProcessMessages;
     END; // WHILE Length(StrFile)>0 DO
   Application.ProcessMessages;
   END; // WHILE (sizRead<siz) DO

ConvComplete;
end;
(*----------------------------------------------------------------------------*)
(*----------------------------------------------------------------------------*)
// ����� ������ "���������� �����������"
Procedure TFormRNX.ExCm20;
Var cr20: ^TCoorUser;

TimeEpochUTC : time_t;
Begin
 try
 cr20 := @ArDat[0];
 // ������� ������� �� � ������� ������������� ������� � ������� ��������� ������� � ����
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
// ����� ������ "���������� �����������" RTK
Procedure TFormRNX.ExCm24;
Var pc24:^TPac24;
Begin
 try
 pc24 := @ArDat[0];
 // ������� ������� ��
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
// FileSeek(hgups,0,2);
// a1 := ((pc23^.DtTmUTC+LeapSec)/86400 + 39448);; FileWrite(hgups,a1,8); // ���������� �����������
// a1 := pc23.TypeSolve; FileWrite(hgups,a1,8);

 except // ������������ �������
  //OknoTP.Lines.Add('Msg(23): error');
  exit;
 end;
End;
(*----------------------------------------------------------------------------*)
// ����� ������� ��
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
     //Syst:=(pc22^.vs[ik].ss and $700) shr 8; // GPS=1, GLN=0
     num:= pc22^.vs[ik].idnm;           // ��������� ������ ��
     tel := 180.0*(pc22^.vs[ik].el)/pi; // �������� ��
     taz := 180.0*(pc22^.vs[ik].az)/pi; // �������� ��

     //infix := (pc22.vs[ik].ss and $20000000);	// ������� ������������� �� � �������
     If (num>0)and(num<200)and(tel>0)and(taz>0) then
         begin
         EL[num]:= tel;
         end;
     End; // ����� ����� �� ���� ��

 except // ������������ �������
  //OknoTP.Lines.Add('Msg(22): error');
  exit;
 end;
End;
(*----------------------------------------------------------------------------*)
// ����� ������ � ������������� �����������    ������ 1�� �����
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
     ClockOffset:=pc0F.ClockOffGPS;   // ����� �� ��������� ������������ �� GPS � ������
     end
 else ClockOffset:= 0;
 LeapSec:= pc0F.LeapSec;

 if (FirstAnalysis=1) then
    begin
    if pr0x0F=false then
       begin
       if ((ChDT=true)and(pc0F.NMSat>0)) then
          begin
          if ((LeapSec<>0)and(ClockOffset > -400)and(ClockOffset < 400)) then // ������ ��� � ������ 0x13 �� ����� ������ ����� � OBS ����
              begin
              // ��������� ����� ������� ���������
              // (pc0F.dtUTC-1) �.�. ������� ������� �������� ����� ������ 0x10
              // ��������� ������� ������� ��������� ��� ����� ������ MO � MN - ����� GPS
              dttm:= ((pc0F.dtUTC-1)+LeapSec)/86400 + RegTime; //StrToDateTime('01.01.2008');
              DateTimeToSystemTime(dttm,FTime);
              pr0x0F:=true;
              end;
          end;
       end;
    // ��� ������������ ������� ��������� GPS �� ����� ����� �� ������ �������
    if pr0x0F then
       begin
       for ik := 0 to pc0F.NMSat-1 do
           begin
           if ((pc0F.Ms[ik].idChn in [0..42]) and (pc0F.Ms[ik].idnum in[1..197])) then
              begin
              // ��������� �� ����� ������ ���� ���������
              if (pc0F.Ms[ik].idnum in[1..32])  then G_OBS:= true; // GPS OBS
              if (pc0F.Ms[ik].idnum in[65..88]) then R_OBS:= true; // GLN
              if (pc0F.Ms[ik].idnum in[101..136]) then E_OBS:= true; // GAL
              if (pc0F.Ms[ik].idnum in[193..197]) then J_OBS:= true; // QZSS

              // ��������� ������ ������������ ������ �� � ������
              if ((pc0F.Ms[ik].idnum>64) and (pc0F.Ms[ik].idnum<89)) then
                   ArLitNumKA[pc0F.Ms[ik].idnum-64]:= pc0F.Ms[ik].lit;
              //
		          if ((pc0F.Ms[ik].prng > 1)and(pc0F.Ms[ik].prng < 90000000)and
                  (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng > 2)and
                  (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng < 10)and
                  (pc0F.Ms[ik].ampsg > 27)) then
                   Begin
                   // ���� ����������� ������� "only SVs used in position fix"
                   if (((RGSV.ItemIndex=1)and((pc0F.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                       begin
                       // ���� ��������� �� ����� �� GPS � ������ �����
		                   if (pc0F.Ms[ik].idnum in[1..32]) then ArSVsOBS_GPS[pc0F.Ms[ik].idnum-1]:= 1;
                       end
			             End;
              end;
           end;
       end;
    exit;
    end;

 if (ClockOffset < -400) and (ClockOffset > 400) then  // ����� � �������, � ������ ���� �� -300� �� 300�
     begin
     // ������� �����
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ClockOffGPS = '+floattostrF(ClockOffset,ffFixed,5,1)+';');
     exit;
     end;

 // ������� ��������� ���� � ������� � ��>0 �� �����
 if ((ChDT=false)or(pc0F.NMSat=0)) then
     begin
     // ������� �����
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ChDT=false or pc0F.NMSat = '+inttostr(pc0F.NMSat)+';');
     exit;
     end;

 // ����� GPS
 dttm := (pc0F.dtUTC+LeapSec)/86400 + RegTime; //StrToDateTime('01.01.2008');
 DateTimeToSystemTime(dttm,TimeEposh);

 // ��� ������ ����� ��������� ROVER
 TimeEposh1:= TimeEposh;

 StYear10:=inttostr(TimeEposh.wYear);
 if TimeEposh.wMonth<10 then StMonth10:='0'+inttostr(TimeEposh.wMonth) else StMonth10:=inttostr(TimeEposh.wMonth);
 if TimeEposh.wDay<10   then StDay10:='0'+inttostr(TimeEposh.wDay) else StDay10:=inttostr(TimeEposh.wDay);
 if (TimeEposh.wHour)<10 then StHour10:='0'+inttostr(TimeEposh.wHour) else StHour10:=inttostr(TimeEposh.wHour);
 if (TimeEposh.wMinute)<10 then StMinute10:='0'+inttostr(TimeEposh.wMinute) else StMinute10:=inttostr(TimeEposh.wMinute);
 DbSecMSec:= TimeEposh.wSecond+(TimeEposh.wMilliseconds)*0.001;
 if (DbSecMSec)<10 then StSecond10:='0'+FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings)
                   else StSecond10:= FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings);

 // �������� ������������ ����
 if (pc0F.dtUTC <= pc10dtUTC_OLD) then
     begin
     // ������� �����
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ������ �����!;');
     exit;
     end;
 pc10dtUTC_OLD:= pc0F.dtUTC;

 // ������� ���������� �����
 if (TimeEposh.wHour = 19) and (TimeEposh.wMinute=51) and (TimeEposh.wSecond =58)  then
     begin
     TimeEposh1:= TimeEposh;
     end;

 SVsON := 0;
 FillChar(ArSVsOBS,sizeof(ArSVsOBS),0);
 //SetLength(ArSVsOBS,0);
 //SetLength(ArSVsOBS,pc0F.NMSat);
 // ����� ������������ ��������� � ���� ����� �� ������� ��
 for ik := 0 to pc0F.NMSat-1 do
 //for ik := 0 to 42 do
     begin
 //    ArSVsOBS[ik]:= '';
     if ((pc0F.Ms[ik].idChn in [0..42]) and (pc0F.Ms[ik].idnum in[1..197])) then
        begin
        // ���������� ������ �� � �����
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
        // GAL
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
        // �������� ����� LLI
        if (((pc0F.Ms[ik].tdt shr 6) and 3)=0) then StLLI:=' ' else StLLI:='1';
        // ������ �� �� ���� ������ ����� - only SVs used in position fix
        FlagElMask:=false;
        if ((RGSV.ItemIndex=1))and(EditElMask.Text<>'') then
            begin
            if (EL[pc0F.Ms[ik].idnum] < strtoint(EditElMask.Text)) then
            FlagElMask:=true; // ���������� ���� ��
            end;
        // ���������� ���������� �������� ���������
        if ((pc0F.Ms[ik].prng > 1)and(pc0F.Ms[ik].prng < 90000000)and
            (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng > 2)and
            (pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng < 10)and
            (pc0F.Ms[ik].ampsg > 25)and (FlagElMask=false)) then
            Begin
            // ���� ����������� ������� "only SVs used in position fix"
            if (((RGSV.ItemIndex=1)and((pc0F.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                begin
                // ������������ ������
                St:= StNKA+
                     Format('%14.3f',[pc0F.Ms[ik].prng],formatSettings)+Format('%2s',[''])+      // ���������������
                     Format('%14.3f',[pc0F.Ms[ik].pvel*kof],formatSettings)+Format('%2s',[''])+  // ��������������
                     Format('%14.3f%s',[pc0F.Ms[ik].pphi,StLLI],formatSettings)+Format('%1s',[''])+  // ���� + LLI
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
                    Writeln(LOG,StPac10+': ERROR! ���������� ���������� �������� ��������� �� '+StNKA);
                    Writeln(LOG,'(1 > pc10.Ms[ik].prng < 90000000) = '+floattostrF(pc0F.Ms[ik].prng,ffFixed,8,3)+';');
                    Writeln(LOG,'(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = '+floattostrF((pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng),ffFixed,8,3)+';');
                    Writeln(LOG,'(pc10.Ms[ik].ampsg > 25) = '+floattostrF(pc0F.Ms[ik].ampsg,ffFixed,8,3)+';');
                    end
                else
                    begin
//                    StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
//                    Writeln(LOG,StPac10+': ERROR! ���������� �� ���� ����� �� '+StNKA);
//                    Writeln(LOG,'(pc10.Ms[ik].ampsg > 25) = '+floattostrF(pc0F.Ms[ik].ampsg,ffFixed,8,3)+';');
                    end;
                end;
            End;
            end; // if ichn in [0..43] then
     end; // for ik := 0 to pc0F.NMSat-1 do

 StKA10 := inttostr(SVsON);

 // ������ � ���� - ������ �����
 StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s%3s%3s%21.12f',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10,'0',StKA10,ClockOffset/SL],formatSettings);
 Writeln(F_RinMO,StPac10);
 // ������ � ���� - ������ ��
 for ik := 0 to Length(ArSVsOBS)-1 do if (ArSVsOBS[ik]<>'') then Writeln(F_RinMO, ArSVsOBS[ik]);

 except
  StKA10 := inttostr(SVsON);
  //OknoTP.Lines.Add('error messages 0�10!');
  exit;
 end;
End;
// ����� ������ � ������������� �����������    ������ 1�� �����
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
    ArSVsOBS_GPS: array[0..42] of integer;  // ������ ���������
Begin
 errCS10:=false;
 //if (errCS13=true) then exit;

 try
 kof:=1;
 pc10:= @ArDat[0];

 if (ChBoxClockOFF.Checked = false) then
     begin
     ClockOffset:=pc10.ClockOffGPS; // ����� �� ��������� ������������ �� GPS � ������
     end
 else ClockOffset:= 0;

 LeapSec:= pc10.LeapSec;

 if (FirstAnalysis=1) then
    begin
    if pr0x10=false then
       begin
       if ((ChDT=true)and(pc10.NMSat>0)) then
          begin
          if ((LeapSec<>0)) then // ������ ��� � ������ 0x13 �� ����� ������ ����� � OBS ����
              begin
              // ��������� ����� ������� ���������
              // (pc10.dtUTC-1) �.�. ������� ������� �������� ����� ������ 0x10
              // ��������� ������� ������� ��������� ��� ����� ������ MO � MN - ����� GPS
              dttm:= ((pc10.dtUTC-1)+LeapSec)/86400 + RegTime; //StrToDateTime('01.01.2008');
              DateTimeToSystemTime(dttm,FTime);
              pr0x10:=true;
              end;
          end;
       end;
    // ��� ������������ ������� ��������� GPS �� ����� ����� �� ������ �������
    if pr0x10 then
       begin
       for ik := 0 to pc10.NMSat-1 do
           begin
           if ((pc10.Ms[ik].idChn in [0..42]) and (pc10.Ms[ik].idnum in[1..197])) then
              begin
              // ��������� �� ����� ������ ���� ���������
              if (pc10.Ms[ik].idnum in[1..32])  then G_OBS:= true; // GPS OBS
              if (pc10.Ms[ik].idnum in[65..88]) then R_OBS:= true; // GLN
              if (pc10.Ms[ik].idnum in[101..136]) then E_OBS:= true; // GAL
              if (pc10.Ms[ik].idnum in[193..197]) then J_OBS:= true; // QZSS

              // ��������� ������ ������������ ������ �� � ������
              if ((pc10.Ms[ik].idnum>64) and (pc10.Ms[ik].idnum<89)) then
                   ArLitNumKA[pc10.Ms[ik].idnum-64]:= pc10.Ms[ik].lit;
              //
		          if ((pc10.Ms[ik].prng > 1)and(pc10.Ms[ik].prng < 90000000)and
                  (pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 2)and
                  (pc10.Ms[ik].pphi/pc10.Ms[ik].prng < 10)and
                  (pc10.Ms[ik].ampsg > 27)) then
                   Begin
                   // ���� ����������� ������� "only SVs used in position fix"
                   if (((RGSV.ItemIndex=1)and((pc10.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                       begin
                       // ���� ��������� �� ����� �� GPS � ������ �����
		                   if (pc10.Ms[ik].idnum in[1..32]) then ArSVsOBS_GPS[pc10.Ms[ik].idnum-1]:= 1;
                       end
			             End;
              end;
           end;
       end;
    exit;  // if (FirstAnalysis=1) then
    end;

 if (ClockOffset < -400) and (ClockOffset > 400) then  // ����� � �������, � ������ ���� �� -300� �� 300�
     begin
     // ������ ��������� ClockOffset
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ClockOffGPS = '+floattostrF(ClockOffset,ffFixed,5,1)+';');
     end;

 // ������� ��������� ���� � ������� � ��>0 �� �����
 if ((ChDT=false)or(pc10.NMSat=0)) then
     begin
     // ������� �����
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ChDT=false or pc10.NMSat = '+inttostr(pc10.NMSat)+';');
     exit;
     end;

 // ������� ��������� ���� � ������� � ��>0 �� �����
// if ((pc10.dtUTC=0)or(pc10.NMSat=0)) then
//     begin
//     // ������� �����
//     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
//     Writeln(LOG,StPac10+': ERROR! ChDT=false or pc10.NMSat = '+inttostr(pc10.NMSat)+';');
//     exit;
//     end;

 // ����� GPS
 dttm := (pc10.dtUTC+LeapSec)/86400 + RegTime; //StrToDateTime('01.01.2008');
 DateTimeToSystemTime(dttm,TimeEposh);

 // ��� ������ ����� ��������� ROVER
 TimeEposh1:= TimeEposh;

 StYear10:=inttostr(TimeEposh.wYear);
 if TimeEposh.wMonth<10 then StMonth10:='0'+inttostr(TimeEposh.wMonth) else StMonth10:=inttostr(TimeEposh.wMonth);
 if TimeEposh.wDay<10   then StDay10:='0'+inttostr(TimeEposh.wDay) else StDay10:=inttostr(TimeEposh.wDay);
 if (TimeEposh.wHour)<10 then StHour10:='0'+inttostr(TimeEposh.wHour) else StHour10:=inttostr(TimeEposh.wHour);
 if (TimeEposh.wMinute)<10 then StMinute10:='0'+inttostr(TimeEposh.wMinute) else StMinute10:=inttostr(TimeEposh.wMinute);
 DbSecMSec:= TimeEposh.wSecond+(TimeEposh.wMilliseconds)*0.001;
 if (DbSecMSec)<10 then StSecond10:='0'+FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings)
                   else StSecond10:= FloatToStrF(DbSecMSec,ffFixed,7,7,formatSettings);

 // �������� ������������ ����
 if (pc10.dtUTC <= pc10dtUTC_OLD) then
     begin
     // ������� �����
     StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
     Writeln(LOG,StPac10+': ERROR! ������ �����!;');
     exit;
     end;
 pc10dtUTC_OLD:= pc10.dtUTC;

// // ������� ���������� �����
// if (TimeEposh.wHour = 5) and (TimeEposh.wMinute=41) and (TimeEposh.wSecond =18)  then
//     begin
//     TimeEposh1:= TimeEposh;
//     end;

 SVsON := 0;
 FillChar(ArSVsOBS,sizeof(ArSVsOBS),0);
 //SetLength(ArSVsOBS,0);
 //SetLength(ArSVsOBS,pc10.NMSat);
 // ����� ������������ ��������� � ���� ����� �� ������� ��
 for ik := 0 to pc10.NMSat-1 do
 //for ik := 0 to 42 do
     begin
 //    ArSVsOBS[ik]:= '';
     if ((pc10.Ms[ik].idChn in [0..42]) and (pc10.Ms[ik].idnum in[1..197])) then
        begin
        // ���������� ������ �� � �����
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
        // GAL
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
        // �������� ����� LLI
        if (((pc10.Ms[ik].tdt shr 6) and 3)=0) then StLLI:=' ' else StLLI:='1';
        // ������ �� �� ���� ������ ����� - only SVs used in position fix
        FlagElMask:=false;
        if ((RGSV.ItemIndex=1))and(EditElMask.Text<>'') then
            begin
            if (EL[pc10.Ms[ik].idnum] < strtoint(EditElMask.Text)) then
            FlagElMask:=true; // ���������� ���� ��
            end;
        // ���������� ���������� �������� ���������
        if ((pc10.Ms[ik].prng > 1)and(pc10.Ms[ik].prng < 90000000)and
            (pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 2)and
            (pc10.Ms[ik].pphi/pc10.Ms[ik].prng < 10)and
            (pc10.Ms[ik].ampsg > 25)and (FlagElMask=false)) then
            Begin
            // ���� ����������� ������� "only SVs used in position fix"
            if (((RGSV.ItemIndex=1)and((pc10.Ms[ik].tdt and $20)=$20)) or (RGSV.ItemIndex=0)) then
                begin
                // ������������ ������
                St:= StNKA+
                     Format('%14.3f',[pc10.Ms[ik].prng],formatSettings)+Format('%2s',[''])+      // ���������������
                     Format('%14.3f',[pc10.Ms[ik].pvel*kof],formatSettings)+Format('%2s',[''])+  // ��������������
                     Format('%14.3f%s',[pc10.Ms[ik].pphi,StLLI],formatSettings)+Format('%1s',[''])+  // ���� + LLI
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
                    Writeln(LOG,StPac10+': ERROR! ���������� ���������� �������� ��������� �� '+StNKA);
                    Writeln(LOG,'(1 > pc10.Ms[ik].prng < 90000000) = '+floattostrF(pc10.Ms[ik].prng,ffFixed,8,3)+';');
                    Writeln(LOG,'(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = '+floattostrF((pc10.Ms[ik].pphi/pc10.Ms[ik].prng),ffFixed,8,3)+';');
                    Writeln(LOG,'(pc10.Ms[ik].ampsg > 25) = '+floattostrF(pc10.Ms[ik].ampsg,ffFixed,8,3)+';');
                    end
                else
                    begin
//                    StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10],formatSettings);
//                    Writeln(LOG,StPac10+': ERROR! ���������� �� ���� ����� �� '+StNKA);
//                    Writeln(LOG,'(pc10.Ms[ik].ampsg > 25) = '+floattostrF(pc10.Ms[ik].ampsg,ffFixed,8,3)+';');
                    end;
                end;
            End;
            end; // if ichn in [0..43] then
     end; // for ik := 0 to pc10.NMSat-1 do

 StKA10 := inttostr(SVsON);

 // ������ � ���� - ������ �����
 StPac10 := '> '+Format('%4s%3s%3s%3s%3s%11s%3s%3s%21.12f',[StYear10,StMonth10,StDay10,StHour10,StMinute10,StSecond10,'0',StKA10,ClockOffset/SL],formatSettings);
 Writeln(F_RinMO,StPac10);
 // ������ � ���� - ������ ��
 for ik := 0 to Length(ArSVsOBS)-1 do if (ArSVsOBS[ik]<>'') then Writeln(F_RinMO, ArSVsOBS[ik]);

 except
  StKA10 := inttostr(SVsON);
  //OknoTP.Lines.Add('error messages 0�10!');
  exit;
 end;
End;
(*----------------------------------------------------------------------------*)
// ����� ������ �� c �������� ��
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
        ClockOffset:=pc13^.dt[3];   // ����� �� ��������� ������������ �� GPS � ������
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
// ����� ������ ��������� ���������
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
// ����� ������ ���������� �������� ��������� �� �������
Procedure TFormRNX.ExCm19;
Var aGLN: ^TAlmnGLN;
    ik: integer;
Begin
 try
 aGLN := @ArDat[0];
 if (pr0x19=false) then
     begin
     // ��������� ������ ������������ ������ �� � ������
     if ((aGLN^.num>0) and (aGLN^.num<25)) then ArLitNumKA[aGLN^.num]:= aGLN^.aLt;
     // �������� ���������� ����� ���������
     pr0x19:=true;
     for ik := 1 to 24 do if (ArLitNumKA[ik]=100) then pr0x19:=false;
     end;
 except
  //
 end;

End;
(*----------------------------------------------------------------------------*)
// ����� ������ ���������� �������� �������� �� GALILEO       ������ 2�� �����
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

 // ������ 0
 // ���������� �������� � �������� ���������� ������
 if ((eGAL.n=0)or(eGAL.I0=0)or(eGAL.SVhelth <> 0)) then
    begin
    // ������� �����
    Writeln(LOG,'EPH GAL ERROR! ���������� GAL �������� � �������� ���������� ������, eGAL.n = '+inttostr(eGAL.n)+'; eGAL.I0 = '+floattostr(eGAL.I0)+'; eGAL.SVhelth = '+inttostr(eGAL.SVhelth)+';');
    exit;
    end;

 // ������ 1
 // ���� � OBS ����� ������ ��� ��������� �� GPS PRN=N
 // (������� � ���� ��������� � ������ "only SVs used in position fix"),
 // �� ��� ����������� ������ � PRN=N ����� ������� "���������" � ���������������.
 if (RGSV.ItemIndex=1) then
     begin
//     if ((ArSVsOBS_GPS[eGPS.n-1]) <> 1) then
//         begin
//         // ������� �����
//         Writeln(LOG,'EPH GPS ERROR! OBS ����� ������ ��� ��������� �� eGPS.n = '+inttostr(eGPS.n)+';');
//         exit;
//         end;
     end;

 // ������ 2
 // ���������� afo � eccentricity �� "�����������" ���������� GAL ��������� (PRN=N)
 // � ���������������� ����������� �� ��������� ��� GAL PRN=N.
 if pr0x88 then // ��������� ������ ��������� GAL
    begin
//    if (eGPS.n = ArTAlmnGPS[eGPS.n].num) then // ���� �������� ��������� ������� �� ���� � ������� ���������
//        begin                                 // �� �������� ����������� ������
//        Eph_Alm_E:= abs(ArTAlmnGPS[eGPS.n].e - eGPS.e);
//
//        if ((abs(ArTAlmnGPS[eGPS.n].e - eGPS.e) > 0.001) or
//            (abs(ArTAlmnGPS[eGPS.n].af0 - eGPS.af0) > 0.0001))then
//             begin
//             // ������� �����
//             Writeln(LOG,'EPH GPS ERROR! ���������� afo � eccentricity ��������� � ���������, ������� Eph_Alm_E = '+floattostr(Eph_Alm_E)+';');
//             exit;
//             end;
//        end;
    end;

 // ������ 3
 // �������� ���������� �������� � ������������
 if ((eGAL.t_oe = ArTEphGAL[eGAL.n].t_oe) and (eGAL.t_oc = ArTEphGAL[eGAL.n].t_oc) and
     (eGAL.IODE = ArTEphGAL[eGAL.n].IODE) and (eGAL.A = ArTEphGAL[eGAL.n].A)) then
     begin
     // ������� �����
     Writeln(LOG,'EPH GAL ERROR! ������ �������� GAL �� ���' +inttostr(eGAL.n)+';');
     exit;
     end;

// �������� �� ���������� IODC � IODE
// if (eGAL.IODC<>eGAL.IODE) then
//     begin
//     //OknoTP.Lines.Add('������� IODC � IODE GPS �� ���' +inttostr(eGPS.n));
//     Writeln(LOG,'EPH GAL ERROR! ������� IODC � IODE GAL �� ���' +inttostr(eGAL.n)+';');
//     exit;
//     end;
 //====================================================================================

  // ����� ����������� � ������������� ����
  if eGAL.n<10 then StNKA:='E0'+inttostr(eGAL.n) else StNKA:='E'+inttostr(eGAL.n);
  // ����� GPS
  //dttm := (eGAL^.t_oe+(604800*(eGAL^.weekN+1024*WRollOver)))/86400 + RegTimeGPS; //StrToDateTime('06.01.1980');
  // WRollOver � GALILEO ����� ����� 1 ��� ��� eGAL^.weekN ����� ���� 4095 (��-�� ����������� 12���)
  dttm := (eGAL^.t_oe+(604800*(eGAL^.weekN+1024*1)))/86400 + RegTimeGPS; //StrToDateTime('06.01.1980');

 // ������ 4
 // ���� dttm ������ 2 ����� �� ���������� �������, �� ��� �� ���������� ���������
 if (abs(TimeUTC - dttm) > 0.1) then
     begin
     Writeln(LOG,StNKA+' error GAL dttm; old ephemeris!');
     exit;
     end;

 // ���������� ����� ������ �������� � �������
 ArTEphGAL[eGAL.n].n:= eGAL.n;
 ArTEphGAL[eGAL.n].t_oe:= eGAL.t_oe;
 ArTEphGAL[eGAL.n].t_oc:= eGAL.t_oc;
 ArTEphGAL[eGAL.n].IODE:= eGAL.IODE;
 ArTEphGAL[eGAL.n].IODC:= eGAL.IODC;
 ArTEphGAL[eGAL.n].af0:= eGAL.af0;
 ArTEphGAL[eGAL.n].A:= eGAL.A;

//  // ��������� �� ��������� eGPS^.weekN
//  if (abs(TimeUTC - dttm) > (1.0/8.0)) then // 0.125
//      begin
//      dttm := ((604800*(eGAL^.weekN+1+1024*WRollOver)))/86400 + RegTimeGPS; //StrToDateTime('06.01.1980');
//      Writeln(LOG,StNKA+' error eGPS^.weekN!');
//      end;
  DateTimeToSystemTime(dttm,TimeEposh);

  StYear:=inttostr(TimeEposh.wYear);
  if TimeEposh.wMonth<10 then StMonth:='0'+inttostr(TimeEposh.wMonth) else StMonth:=inttostr(TimeEposh.wMonth);
  if TimeEposh.wDay<10   then StDay:='0'+inttostr(TimeEposh.wDay) else StDay:=inttostr(TimeEposh.wDay);
  if (TimeEposh.wHour)<10 then StHour:='0'+inttostr(TimeEposh.wHour) else StHour:=inttostr(TimeEposh.wHour);
  if (TimeEposh.wMinute)<10 then StMinute:='0'+inttostr(TimeEposh.wMinute) else StMinute:=inttostr(TimeEposh.wMinute);
  if (TimeEposh.wSecond)<10 then StSecond:='0'+inttostr(TimeEposh.wSecond) else StSecond:=inttostr(TimeEposh.wSecond);

 //====================================================================================

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
 //Stdtrc:= FormatFloat('0.000000000000E+00', eGPS^.dtrc,formatSettings);
 Stdtrc:= FormatFloat('0.000000000000E+00', eGAL^.t_oe,formatSettings);  //������� ��� ������ � ������� ��������
 StFitinterval:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s',['    ',Stdtrc,StFitinterval]));


 except
  //OknoTP.Lines.Add('error messages 0�1A!');
 end;

 end;
(*----------------------------------------------------------------------------*)
// ����� ������ ���������� �������� �������� �� GPS       ������ 2�� �����
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

 // ������ 0
 // ���������� �������� � �������� ���������� ������
 if ((eGPS.n=0)or(eGPS.I0=0)or(eGPS.SVhelth <> 0)) then
    begin
    // ������� �����
    Writeln(LOG,'EPH GPS ERROR! ���������� GPS �������� � �������� ���������� ������, eGPS.n = '+inttostr(eGPS.n)+'; eGPS.I0 = '+floattostr(eGPS.I0)+'; eGPS.SVhelth = '+inttostr(eGPS.SVhelth)+';');
    exit;
    end;

 // ������ 1
 // ���� � OBS ����� ������ ��� ��������� �� GPS PRN=N
 // (������� � ���� ��������� � ������ "only SVs used in position fix"),
 // �� ��� ����������� ������ � PRN=N ����� ������� "���������" � ���������������.
 if (RGSV.ItemIndex=1) then
     begin
//     if ((ArSVsOBS_GPS[eGPS.n-1]) <> 1) then
//         begin
//         // ������� �����
//         Writeln(LOG,'EPH GPS ERROR! OBS ����� ������ ��� ��������� �� eGPS.n = '+inttostr(eGPS.n)+';');
//         exit;
//         end;
     end;

 // ������ 2
 // ���������� afo � eccentricity �� "�����������" ���������� GPS ��������� (PRN=N)
 // � ���������������� ����������� �� ��������� ��� GPS PRN=N.
 if pr0x88 then // ��������� ������ ��������� GPS
    begin
    if (eGPS.n = ArTAlmnGPS[eGPS.n].num) then // ���� �������� ��������� ������� �� ���� � ������� ���������
        begin                                 // �� �������� ����������� ������
        Eph_Alm_E:= abs(ArTAlmnGPS[eGPS.n].e - eGPS.e);

        if ((abs(ArTAlmnGPS[eGPS.n].e - eGPS.e) > 0.001) or
            (abs(ArTAlmnGPS[eGPS.n].af0 - eGPS.af0) > 0.0001))then
             begin
             // ������� �����
             Writeln(LOG,'EPH GPS ERROR! ���������� afo � eccentricity ��������� � ���������, ������� Eph_Alm_E = '+floattostr(Eph_Alm_E)+';');
             exit;
             end;
        end;
    end;

 // ������ 3
 // �������� ���������� �������� � ������������
 if ((eGPS.t_oe = ArTEphGPS[eGPS.n].t_oe) and (eGPS.t_oc = ArTEphGPS[eGPS.n].t_oc) and
     (eGPS.IODE = ArTEphGPS[eGPS.n].IODE) and (eGPS.A = ArTEphGPS[eGPS.n].A)) then
     begin
     // ������� �����
     Writeln(LOG,'EPH GPS ERROR! ������ �������� GPS �� ���' +inttostr(eGPS.n)+';');
     exit;
     end;
 // �������� �� ���������� IODC � IODE
 if (eGPS.IODC<>eGPS.IODE) then
     begin
     //OknoTP.Lines.Add('������� IODC � IODE GPS �� ���' +inttostr(eGPS.n));
     Writeln(LOG,'EPH GPS ERROR! ������� IODC � IODE GPS �� ���' +inttostr(eGPS.n)+';');
     exit;
     end;
 //====================================================================================

  // ����� ����������� � ������������� ����
  if eGPS.n<10 then StNKA:='G0'+inttostr(eGPS.n) else StNKA:='G'+inttostr(eGPS.n);
  // ����� GPS
  dttm := (eGPS^.t_oe+(604800*(eGPS^.weekN+1024*WRollOver)))/86400 + RegTimeGPS; //StrToDateTime('06.01.1980');

  // ������ 4
  // ���� dttm ������ 3 ����� �� ���������� �������, �� ��� �� ���������� ���������
  if (abs(TimeUTC - dttm) > 0.1) then // 0.125
      begin
      Writeln(LOG,StNKA+' error GPS dttm; old ephemeris!');
      exit;
      end;

  // ���������� ����� ������ �������� � �������
  ArTEphGPS[eGPS.n].n:= eGPS.n;
  ArTEphGPS[eGPS.n].t_oe:= eGPS.t_oe;
  ArTEphGPS[eGPS.n].t_oc:= eGPS.t_oc;
  ArTEphGPS[eGPS.n].IODE:= eGPS.IODE;
  ArTEphGPS[eGPS.n].IODC:= eGPS.IODC;
  ArTEphGPS[eGPS.n].af0:= eGPS.af0;
  ArTEphGPS[eGPS.n].A:= eGPS.A;
  //ArTEphGPS[eGPS.n].dtrc:= eGPS.dtrc;

  // ��������� �� ��������� eGPS^.weekN
//  if (abs(TimeUTC - dttm) > (1.0/8.0)) then // 0.125
//      begin
//      dttm := ((604800*(eGPS^.weekN+1+1024*WRollOver)))/86400 + RegTimeGPS; //StrToDateTime('06.01.1980');
//      Writeln(LOG,StNKA+' error eGPS^.weekN!');
//      //exit;
//      end;
  DateTimeToSystemTime(dttm,TimeEpoch);

  StYear:=inttostr(TimeEpoch.wYear);
  if TimeEpoch.wMonth<10 then StMonth:='0'+inttostr(TimeEpoch.wMonth) else StMonth:=inttostr(TimeEpoch.wMonth);
  if TimeEpoch.wDay<10   then StDay:='0'+inttostr(TimeEpoch.wDay) else StDay:=inttostr(TimeEpoch.wDay);
  if (TimeEpoch.wHour)<10 then StHour:='0'+inttostr(TimeEpoch.wHour) else StHour:=inttostr(TimeEpoch.wHour);
  if (TimeEpoch.wMinute)<10 then StMinute:='0'+inttostr(TimeEpoch.wMinute) else StMinute:=inttostr(TimeEpoch.wMinute);
  if (TimeEpoch.wSecond)<10 then StSecond:='0'+inttostr(TimeEpoch.wSecond) else StSecond:=inttostr(TimeEpoch.wSecond);

 //====================================================================================

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
 StweekN:= FormatFloat('0.000000000000E+00', eGPS^.weekN+1024*WRollOver,formatSettings); //////////////+1024*wRollOver!
 StL2PdataF:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StIDOT,StCodesL2,StweekN,StL2PdataF]));
 // BROADCAST ORBIT - 6
 StSVaccur:= FormatFloat('0.000000000000E+00', eGPS^.SVaccur,formatSettings);
 StSVhelth:= FormatFloat('0.000000000000E+00', eGPS^.SVhelth,formatSettings);
 StTgd:= FormatFloat('0.000000000000E+00', eGPS^.Tgd,formatSettings);
 StIODC:= FormatFloat('0.000000000000E+00', eGPS^.IODC,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StSVaccur,StSVhelth,StTgd,StIODC]));
 // BROADCAST ORBIT - 7
 //Stdtrc:= FormatFloat('0.000000000000E+00', eGPS^.dtrc,formatSettings);
 Stdtrc:= FormatFloat('0.000000000000E+00', eGPS^.t_oe,formatSettings);  //������� ��� ������ � ������� ��������
 StFitinterval:= FormatFloat('0.000000000000E+00', 0.0,formatSettings);
 Writeln(F_RinMN,Format('%4s%19s%19s',['    ',Stdtrc,StFitinterval]));


 except
  //OknoTP.Lines.Add('error messages 0�1A!');
 end;

 end;
(*----------------------------------------------------------------------------*)
// ����� ������ ���������� �������� �������� �� �������   ������ 2�� �����
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

 // ������ 1
 // ���� ����� ������ �������� PRN=N ����� 2008���� �� �� ����� ������� "���������" � ���������������.
 // ����� ������ ��������(UTC)
 dttm := (eGLN^.dtrc)/86400 + RegTime; //StrToDateTime('01.01.2008');
 DateTimeToSystemTime(dttm,TimeEposh); // ��������� �time_tteTime � TSystemTime
 if (TimeEposh.wYear=2008) then
     begin
     Writeln(LOG,'EPH GLN ERROR! ������ ��������� ������� TimeEposh.wYear = ' +inttostr(TimeEposh.wYear)+';');
     exit;
     end;

 // ������ 2
 // ���� dttm ������ ������� �� ���������� �������, �� ��� �� ���������� ���������
 if (abs(TimeUTC - dttm) > 0.02) then
     begin
     Writeln(LOG,'R'+inttostr(eGLN.n)+' error GLN dttm; old ephemeris!');
     exit;
     end;

 if (eGLN.n=0)or(eGLN.prNNK>0) then
     begin
     Writeln(LOG,'EPH GLN ERROR! ���������� GLN �������� � �������� ���������� ������, eGLN.n = '+inttostr(eGLN.n)+'; eGLN.prNNK = '+inttostr(eGLN.prNNK)+';');
     exit;
     end;

 // �������� ���������� �������� � ������������
 if ((eGLN.t_b = ArTEphGLN[eGLN.n].t_b) and (eGLN.pX = ArTEphGLN[eGLN.n].pX) and
     (eGLN.pY = ArTEphGLN[eGLN.n].pY) and (eGLN.pZ = ArTEphGLN[eGLN.n].pZ)) then
     begin
     // ������� �����
     Writeln(LOG,'EPH GLN ERROR! ������ �������� GLN �� ���' +inttostr(eGLN.n)+';');
     exit;
     end;

 if (RecEph_od=false) then
 Begin
 if (ChDT=false) then
    begin
    // ���������� �������� ��� ���� � ������
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
    // �������� ������� ��������� �������
    // ������ ����� ��� ������ � ������� ���������� � ������������� ����

    // ����� ������ ��������(UTC)
    dttm := (eGLN^.dtrc)/86400 + RegTime; //StrToDateTime('01.01.2008');
    DateTimeToSystemTime(dttm,TimeEposh); // ��������� �time_tteTime � TSystemTime
    // �������� wHour, wMinute, wSecond, wMSecond
    TimeEposh.wHour:=0; TimeEposh.wMinute:=0; TimeEposh.wSecond:=0; TimeEposh.wMilliseconds:=0;
    // �������� �����time_t����� ���� �� TSystemTime � TDateTime
    Date1:=SystemTimeToDateTime(TimeEposh);

    for ik := 1 to 24 do
        begin
        if (ArTEphGLN_od[ik].n<>0) then
            begin
            // ���������� ����� ������ �������� � �������
            ArTEphGLN[ik].t_b:= ArTEphGLN_od[ik].t_b;
            ArTEphGLN[ik].pX:= ArTEphGLN_od[ik].pX;
            ArTEphGLN[ik].pY:= ArTEphGLN_od[ik].pY;
            ArTEphGLN[ik].pZ:= ArTEphGLN_od[ik].pZ;

            // ����� ����������� � ������������� ����
            if ArTEphGLN_od[ik].n<10 then StNKA:='R0'+inttostr(ArTEphGLN_od[ik].n) else StNKA:='R'+inttostr(ArTEphGLN_od[ik].n);

            // Epoch: Toc - Time of Clock(UTC)
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
            StMesFTime:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].t_b*900,formatSettings); // Message frame time? tk=tb*900
            Writeln(F_RinMN,Format('%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s',
                   [StNKA,StYear,StMonth,StDay,StHour,StMinute,StSecond,Sttau,Stgm,StMesFTime]));
            // BROADCAST ORBIT - 1
            StpX:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].pX*0.001,formatSettings);
            StvX:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].vX*0.001,formatSettings);
            StaX:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].aX*0.001,formatSettings);
            StFt:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].prNNK,formatSettings);  // health (0=OK) (Bn)
            Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpX,StvX,StaX,StFt]));
            // BROADCAST ORBIT - 2
            StpY:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].pY*0.001,formatSettings);
            StvY:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].vY*0.001,formatSettings);
            StaY:= FormatFloat('0.000000000000E+00', ArTEphGLN_od[ik].aY*0.001,formatSettings);
            StLit:= FormatFloat('0.000000000000E+00', ArLitNumKA[ArTEphGLN_od[ik].n],formatSettings);  // frequency number(-7...+13)
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
 

 // ���������� ����� ������ �������� � �������
 ArTEphGLN[eGLN.n].t_b:= eGLN.t_b;
 ArTEphGLN[eGLN.n].pX:= eGLN.pX;
 ArTEphGLN[eGLN.n].pY:= eGLN.pY;
 ArTEphGLN[eGLN.n].pZ:= eGLN.pZ;

 // ����� ����������� � ������������� ����
 if eGLN.n<10 then StNKA:='R0'+inttostr(eGLN.n) else StNKA:='R'+inttostr(eGLN.n);

 // ����� ������ ��������(UTC)
 dttm := (eGLN^.dtrc)/86400 + RegTime; //StrToDateTime('01.01.2008');

 DateTimeToSystemTime(dttm,TimeEposh); // ��������� �time_tteTime � TSystemTime
 // �������� wHour, wMinute, wSecond, wMSecond
 TimeEposh.wHour:=0; TimeEposh.wMinute:=0; TimeEposh.wSecond:=0; TimeEposh.wMilliseconds:=0;
 // �������� �����time_t����� ���� �� TSystemTime � TDateTime
 Date1:=SystemTimeToDateTime(TimeEposh);
 // Epoch: Toc - Time of Clock(UTC)
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
 StMesFTime:= FormatFloat('0.000000000000E+00', eGLN^.t_b*900,formatSettings); // Message frame time? tk=tb*900
 Writeln(F_RinMN,Format('%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s',
       [StNKA,StYear,StMonth,StDay,StHour,StMinute,StSecond,Sttau,Stgm,StMesFTime]));
 // BROADCAST ORBIT - 1
 StpX:= FormatFloat('0.000000000000E+00', eGLN^.pX*0.001,formatSettings);
 StvX:= FormatFloat('0.000000000000E+00', eGLN^.vX*0.001,formatSettings);
 StaX:= FormatFloat('0.000000000000E+00', eGLN^.aX*0.001,formatSettings);
 StFt:= FormatFloat('0.000000000000E+00', eGLN^.prNNK,formatSettings);  // health (0=OK) (Bn)
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpX,StvX,StaX,StFt]));
 // BROADCAST ORBIT - 2
 StpY:= FormatFloat('0.000000000000E+00', eGLN^.pY*0.001,formatSettings);
 StvY:= FormatFloat('0.000000000000E+00', eGLN^.vY*0.001,formatSettings);
 StaY:= FormatFloat('0.000000000000E+00', eGLN^.aY*0.001,formatSettings);
 StLit:= FormatFloat('0.000000000000E+00', ArLitNumKA[eGLN^.n],formatSettings);  // frequency number(-7...+13)
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpY,StvY,StaY,StLit]));
 // BROADCAST ORBIT - 3
 StpZ:= FormatFloat('0.000000000000E+00', eGLN^.pZ*0.001,formatSettings);
 StvZ:= FormatFloat('0.000000000000E+00', eGLN^.vZ*0.001,formatSettings);
 StaZ:= FormatFloat('0.000000000000E+00', eGLN^.aZ*0.001,formatSettings);
 StenE:= FormatFloat('0.000000000000E+00', eGLN^.enE,formatSettings); // Age of oper. information (days) (E)
 Writeln(F_RinMN,Format('%4s%19s%19s%19s%19s',['    ',StpZ,StvZ,StaZ,StenE]));

 //   end;
 except
  //OknoTP.Lines.Add('error messages 0�1B!');
 end;

End;
(*----------------------------------------------------------------------------*)
// ����� ������ ��������� ��������� GPS
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
// ����� ������ ��������� ��������� �������� GPS
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
// ����� ������ ��������� ��������� �������� GLO
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
// ����� ��������� �� ����� �0xC1�
Procedure TFormRNX.ExCmC1;
Var
    snChar:char;
    snDig:integer;
    snStr:string;
Begin
 try
  // �������� �����
  snChar:=Chr(ArDat[2] shr 24);
  snDig:= (ArDat[2] and $FFFFFF);
  // CS ��������
  //CS:=ArDat[3];

  snStr:=inttostr(snDig);
  while (length(snStr)<9) do snStr:='0'+snStr;
  //snStr:=snChar+snStr;

  // ���� ������ ���
  if (ArDat[2] = -1) then snStr:= ' N/A     ';

  // �������� ������ �����
  verSt:= IntToStr(ArDat[0] shr 16) + '.' +IntToStr(ArDat[0] and $FFFF);

  StSN:=' S/N:'+snStr;
 except
  //
 end;
end;
(*----------------------------------------------------------------------------*)
// ����� ��������� GPS
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

 except  // ������������ �������
  //
 end;
End;
(*----------------------------------------------------------------------------*)
// ����� ������ �� ��������� ���������
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
