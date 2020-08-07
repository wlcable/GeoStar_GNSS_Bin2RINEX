/**   @file
     @brief
*/
#include "UnitRINEX_en_copy.h"

/*$R *.dfm*/

/*----------------------------------------------------------------------------*/
// gdt file creation function
bool FileGdtCreate(std::string FileName, u_int8_t Ng, AnsiString StHead,
                   int PrOrd) {
  bool result;
  file Fgdt;  // of Ansichar
  int ik;
  u_int16_t VerFDt;
  u_int16_t Nh;
  u_int16_t MgWr;
  u_int8_t NGr;
  u_int8_t Resb;
  Ansichar Ch1;
  Ansichar Ch2;

  AssignFile(Fgdt, FileName);
  Rewrite(Fgdt);
  VerFDt = 0x100 * 5 + 0;  // SG version 5.0
  Resb = PrOrd;
  MgWr = 0x18B7;
  NGr = Ng;
  Nh = 8 + Length(StHead);
  Ch1 = Ansichar(VerFDt % 256);
  Ch2 = Ansichar(VerFDt /*div*/ / 256);
  Write(Fgdt, Ch1, Ch2);
  Ch1 = Ansichar(Resb);
  Ch2 = Ansichar(NGr);
  Write(Fgdt, Ch1, Ch2);
  Ch1 = Ansichar(Nh % 256);
  Ch2 = Ansichar(Nh /*div*/ / 256);
  Write(Fgdt, Ch1, Ch2);
  {
    long ik_end = Nh - 8 + 1;
    for (ik = 1; ik < ik_end; ++ik) Write(Fgdt, StHead[ik]);
  }
  Ch1 = Ansichar(MgWr % 256);
  Ch2 = Ansichar(MgWr /*div*/ / 256);
  Write(Fgdt, Ch1, Ch2);
  result = FileSize(Fgdt) == Nh;
  CloseFile(Fgdt);
  return result;
}
/*----------------------------------------------------------------------------*/
void TFormRNX::ConvComplete() {
  // Writeln (F_RinMO, Format (&#39;% s% 56s&#39;, [&#39;END OF FILE&#39;,
  // &#39;COMMENT&#39;])); OknoTP.Lines.Add (&#39;bytes wr =&#39; + inttostr
  // (kol));
  OknoTP.Lines.Add("Conversion completed!");
  PBarRNX.Visible = false;
  Button2.Enabled = true;
  CloseFile(LOG);
  CloseFile(F_RinMO);
  CloseFile(F_RinMN);
  Screen.Cursor = crDefault;
}
/*----------------------------------------------------------------------------*/
// verification of spacecraft number as accepted. spacecraft number, letter and
// visibility
int CheckNum(u_int8_t pNum) {
  int result;
  u_int8_t ik;

  try {
    result = 0;
    {
      long ik_end = 25;
      for (ik = 1; ik < ik_end; ++ik)
        if ((pNum == ArLitNumKA[ik])) result = ik;
    }  // get the spacecraft number

    // FormMain.OknoTP.Lines.Add (&#39;ERR letter request =&#39; + inttostr
    // (plit));
  } catch (...) {
  }
  return result;
}
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void TFormRNX::Button2Click(TObject Sender) {
  int ik;
  int dl;
  int DY;
  std::string St;
  std::string StGreateFileTime;
  std::string StDY;
  std::string StYear;
  std::string StMonth;
  std::string StDay;
  std::string StHour;
  std::string StMinute;
  std::string StSecond;
  std::string StSecF;
  File FGeosBin;  // of Char
  char StF;
  double DbSecMSec;
  int count;
  char Ch;
  int iSt;
  int lnPc;
  int lnPw;
  int iXt;
  u_int16_t ncmd;
  u_int16_t ndat;
  int CSpac;
  int dtlw;

  std::string ASD;
  std::string PathLOGGeoS3;
  std::string NameFGeos3;

  int sizRead;
  int NumRead;
  int LengthStrHalfFile;
  std::string StrHalfFile;
  int loppp;
  int I;

  formatSettings.DecimalSeparator = ".";
  // initialization of variables
  FillChar(ArSVsOBS_GPS, sizeof(ArSVsOBS_GPS), 0);
  FillChar(ArTAlmnGPS, sizeof(ArTAlmnGPS), 0);

  FillChar(ArTEphGPS, sizeof(ArTEphGPS), 0);
  FillChar(ArTEphGLN, sizeof(ArTEphGLN), 0);
  FillChar(ArTEphGAL, sizeof(ArTEphGAL), 0);

  FillChar(ArTEphGPS_od, sizeof(ArTEphGPS_od), 0);
  FillChar(ArTEphGLN_od, sizeof(ArTEphGLN_od), 0);

  {
    long ik_end = 25;
    for (ik = 1; ik < ik_end; ++ik) ArLitNumKA[ik] = 100;
  }

  {
    long ik_end = 8;
    for (ik = 0; ik < ik_end; ++ik) ArMesg1C[ik] = "0";
  }
  {
    long ik_end = 4;
    for (ik = 0; ik < ik_end; ++ik) ArMesg1D[ik] = "0";
  }
  {
    long ik_end = 3;
    for (ik = 0; ik < ik_end; ++ik) ArMesg1D[ik] = "0";
  }

  pr0x14 = false;
  pr0x19 = false;
  pr0x1C = false;
  pr0x1D = false;
  pr0x1E = false;

  pr0x10 = false;
  pr0x0F = false;
  pr0x13 = false;
  pr0x0A = false;
  pr0x1A = false;
  pr0x1B = false;
  pr0x88 = false;

  errCS10 = false;
  errCS13 = false;

  pc10dtUTC_OLD = 0;

  LeapSec = 0;
  ClockOffset = 0;
  ClockOffset1 = 0;
  WRollOver = 0;

  FEnter = false;
  GinFix = false;  // sign of decision
  verSt = "";
  StSN = "";

  ChDT = false;
  RecEph_od = false;
  GGFix = false;

  OknoTP.Clear;
  FdtUTC = 0;

  if (FNEdit.FileName == "") {
    OknoTP.Lines.Add("Input file error!");
    return;
  }

  // first scroll of the file to record headers
  FirstAnalysis = 1;
  Button2.Enabled = false;
  Screen.Cursor = crHourGlass;

  // File Designation (Activation)
  AssignFile(F1, FNEdit.FileName);
  Reset(F1);
  siz = FileSize(F1);
  sizRead = 0;
  StrFile = "";

  PBarRNX.Position = 0;
  kol = 0;
  PBarRNX.max = trunc(siz);
  PBarRNX.Visible = true;

  // reading data from a file
  while ((sizRead < siz)) {
    // if the required packages are accepted, then go to the second scroll of
    // the file
    if ((pr0x10 || pr0x0F)) {
      if ((pr0x14 && pr0x1C && pr0x1D && pr0x1E && pr0x13 && pr0x1A &&
           pr0x1B /* and pr0x19*/))
        break;
    }

    // determine the size of how much to read from the file
    StrHalfFile = "";
    if (((siz - sizRead) > 500000000))
      SetLength(StrHalfFile, 500000000);
    else
      SetLength(StrHalfFile, (siz - sizRead));

    // we read a block of a certain size from a file
    BlockRead(F1, StrHalfFile[1], Length(StrHalfFile), NumRead);
    // StrFile: = StrFile + StrHalfFile;
    StrFile = StrHalfFile;
    sizRead = sizRead + NumRead;

    // line processing
    while (Length(StrFile) > 0) {
      // sync word search
      if (FEnter == false) {
        if (Length(StrFile) < 20) break;  // not enough data to analyze
        iSt = 0;
        iSt = Pos("GEOSr3PS", StrFile);
        if ((iSt == 0)) break;                  // sync word not found
        if (Length(StrFile) - iSt < 19) break;  // not enough data to analyze
        // sync word found
        FEnter = true;
      } else {
        if (Length(StrFile) - iSt < 19)  // not enough data to analyze
        {
          Delete(StrFile, 1, iSt - 1);  // delete processed row
          FEnter = false;
          break;
        }
        ncmd = ord(StrFile[iSt + 9]) << 8 + ord(StrFile[iSt + 8]);
        ndat = ord(StrFile[iSt + 11]) << 8 + ord(StrFile[iSt + 10]);
        lnPc = 4 * ndat + 16;  // length of the entire packet in bytes
        lnPw = ndat + 4;       // longword packet length
        if (Length(StrFile) < iSt + lnPc - 1)  // not enough data to analyze
        {                                      // ConvComplete;
          Delete(StrFile, 1, iSt - 1);         // delete processed row
          FEnter = false;
          break;
        }
        CSpac = 0;
        SetLength(ArDat, ndat);
        Application.ProcessMessages;
        // CS calculation
        {
          long ik_end = lnPw;
          for (ik = 0; ik < ik_end; ++ik) {
            dtlw = ord(StrFile[iSt + 4 * ik + 3])
                   << 24 + ord(StrFile[iSt + 4 * ik + 2])
                   << 16 + ord(StrFile[iSt + 4 * ik + 1])
                   << 8 + ord(StrFile[iSt + 4 * ik]);
            CSpac = CSpac xor dtlw;
            if ((ik > 2) && (ik < lnPw - 1)) ArDat[ik - 3] = dtlw;
          }
        }
        if (CSpac != 0)  // counter. package amount did not match
        {
          // Delete (StrFile, 1, iSt + lnPc-1); // delete the processed string
          Delete(StrFile, 1, iSt + 3);  // deletion of processed data to the
                                        // start position of the bad line + GEOS
          iXt = 0;
          iXt = Pos("GEOSr3PS", StrFile);  // looking for geos
          Delete(StrFile, 1, iXt - 1);     // delete leftover bad string

          FEnter = false;
          // break;
        }
        // copying data and working out the team
        switch (ncmd) {
          case 0x0F:
            if (ndat == 20 * (ArDat[3] >> 16) + 6)
              ExCm0F;
            else
              CSpac = 1;
          case 0x10:
            if (ndat == 14 * (ArDat[3] >> 16) + 6)
              ExCm10;
            else
              CSpac = 1;
          case 0x13:
            if (ndat == 32)
              ExCm13;
            else
              CSpac = 1;
          case 0x14:
            if (ndat == 12)
              ExCm14;
            else
              CSpac = 1;
          case 0x0A:
          case 0x9A:
            if (ndat == 32)
              ExCm0A;
            else
              CSpac = 1;
          case 0x18:
          case 0x88:
            if (ndat == 20)
              ExCm88;
            else
              CSpac = 1;
          case 0x19:
          case 0x89:
            if (ndat == 18)
              ExCm19;
            else
              CSpac = 1;
          case 0x1A:
          case 0x8A:
            if (ndat == 32)
              ExCm1A;
            else
              CSpac = 1;
          case 0x1B:
          case 0x8B:
            if (ndat == 30)
              ExCm1B;
            else
              CSpac = 1;
          case 0x1C:
          case 0x9C:
            if (ndat == 8)
              ExCm1C;
            else
              CSpac = 1;
          case 0x1D:
          case 0x9D:
            if (ndat == 8)
              ExCm1D;
            else
              CSpac = 1;
          case 0x1E:
          case 0x9E:
            if (ndat == 10)
              ExCm1E;
            else
              CSpac = 1;
          case 0x20:
            if (ndat == 28)
              ExCm20;
            else
              CSpac = 1;
          case 0x23:
            if (ndat == 29)
              ExCm23;
            else
              CSpac = 1;
          case 0x24:
            if (ndat == 34)
              ExCm24;
            else
              CSpac = 1;
          case 0xC1:
            if (ndat == 4)
              ExCmC1;
            else
              CSpac = 1;
        }
        // if CSpac = 1 then OknoTP.Lines.Add (&#39;Wrong number of package
        // parameters&#39; + IntToHex (ncmd, 2));
        iSt = iSt + lnPc;
        // OknoTP.Lines.Add (&#39;iSt =&#39; + inttostr (iSt));
        kol = kol + lnPc;
        PBarRNX.Position = kol;
      }
      Application.ProcessMessages;
    }  // WHILE Length (StrFile)&gt; 0 DO
    Application.ProcessMessages;
  }  // WHILE (sizRead <siz) DO

  // checking the content of important messages in the log file
  if (pr0x14 == false)
    OknoTP.Lines.Add("No messages 0x14 (Timing parameters)!");  // Leap seconds
  if (pr0x19 == false)
    OknoTP.Lines.Add(
        "No messages 0х19, 0x89 (GLONASS almanac)!");  // for letters
  if (pr0x1C == false)
    OknoTP.Lines.Add("No messages 0x1C, 0x9C (GPS ionospheric parameters)!");
  if (pr0x1D == false)
    OknoTP.Lines.Add(
        "No messages 0x1D, 0x9D (GPS time to UTC conversion parameters)!");
  if (pr0x1E == false)
    OknoTP.Lines.Add(
        "No messages 0x1E, 0x9E (GLONASS time to UTC conversion parameters)!");
  // 0x1F, 0x9F: GST-UTC Conversion Parameters

  if (((pr0x10 != true) && (pr0x0F != true)))
    OknoTP.Lines.Add("No messages 0x10 (Raw measurements)!");
  if (pr0x13 == false)
    OknoTP.Lines.Add("No messages 0x13 (Navigation solution state vector)!");
  if (pr0x0A == false)
    OknoTP.Lines.Add("No messages 0x0A, 0x9A (GALILEO ephemeris)!");
  if (pr0x1A == false)
    OknoTP.Lines.Add("No messages 0x1A, 0x8A (GPS/QZSS ephemeris)!");
  if (pr0x1B == false)
    OknoTP.Lines.Add("No messages 0x1B, 0x8B (GLONASS ephemeris)!");

  if (pr0x88 == false)
    OknoTP.Lines.Add(
        "No messages 0x18, 0x88 (GPS/QZSS almanac)!");  // for filter 2
  // OknoTP.Lines.Add (&#39;GPS almanac is not available. Filter # 2 will be
  // disabled!&#39;);

  // generating a file creation time line
  // day of the year 1-366
  try {
    DY = DayOfTheYear(SystemTimeToDateTime(FTime));
    if ((DY < 10))
      StDY = "00" + inttostr(DY);
    else if ((DY < 100))
      StDY = "0" + inttostr(DY);
    else if ((DY < 1000))
      StDY = inttostr(DY);

    StYear = inttostr(FTime.wYear);
    if (FTime.wMonth < 10)
      StMonth = "0" + inttostr(FTime.wMonth);
    else
      StMonth = inttostr(FTime.wMonth);
    if (FTime.wDay < 10)
      StDay = "0" + inttostr(FTime.wDay);
    else
      StDay = inttostr(FTime.wDay);
    if ((FTime.wHour) < 10)
      StHour = "0" + inttostr(FTime.wHour);
    else
      StHour = inttostr(FTime.wHour);
    if ((FTime.wMinute) < 10)
      StMinute = "0" + inttostr(FTime.wMinute);
    else
      StMinute = inttostr(FTime.wMinute);
    if ((FTime.wSecond) < 10)
      StSecond = "0" + inttostr(FTime.wSecond);
    else
      StSecond = inttostr(FTime.wSecond);
    StGreateFileTime =
        StYear + StMonth + StDay + " " + StHour + StMinute + StSecond + " GPS";

    DbSecMSec = FTime.wSecond + (FTime.wMilliseconds) * 0.001;
    if ((DbSecMSec) < 10)
      StSecF = "0" + FloatToStrF(DbSecMSec, ffFixed, 7, 7, formatSettings);
    else
      StSecF = FloatToStrF(DbSecMSec, ffFixed, 7, 7, formatSettings);

    // creating rinex files F_CoorROVER
    AssignFile(F_RinMO, ExtractFilePath(FNEdit.FileName) + "xxxx00RUS_R_" +
                            StYear + StDY + StHour + StMinute + "_00U_xxU_MO" +
                            ".rnx");
    Rewrite(F_RinMO);
    AssignFile(F_RinMN, ExtractFilePath(FNEdit.FileName) + "xxxx00RUS_R_" +
                            StYear + StDY + StHour + StMinute + "_00U_xxU_MN" +
                            ".rnx");
    Rewrite(F_RinMN);
    // creating a LOG_ERRORS file
    AssignFile(LOG, ExtractFilePath(FNEdit.FileName) + "LOG_ERRORS.txt");
    Rewrite(LOG);

  } catch (...) {
    OknoTP.Lines.Add("Cannot create Rinex file!");
    Screen.Cursor = crDefault;
    PBarRNX.Visible = false;
    Button2.Enabled = true;
    return;
  }
  // HEADER - GNSS Navigation Message File -
  Writeln(F_RinMN,
          "     3.02           N: GNSS NAV DATA    M: MIXED            RINEX "
          "VERSION / TYPE");
  Writeln(F_RinMN, "GEOS5toRNX          Geostar             " +
                       StGreateFileTime + " PGM / RUN BY / DATE ");
  Writeln(F_RinMN, Format("%4s%13s%12s%12s%12s%23s",
                          [
                            "GPSA", ArMesg1C[0], ArMesg1C[1], ArMesg1C[2],
                            ArMesg1C[3], "IONOSPHERIC CORR"
                          ],
                          formatSettings));
  Writeln(F_RinMN, Format("%4s%13s%12s%12s%12s%23s",
                          [
                            "GPSB", ArMesg1C[4], ArMesg1C[5], ArMesg1C[6],
                            ArMesg1C[7], "IONOSPHERIC CORR"
                          ],
                          formatSettings));
  Writeln(F_RinMN, Format("%4s%18s%16s%7s%5s%26s",
                          [
                            "GPUT", ArMesg1D[0], ArMesg1D[1], ArMesg1D[2],
                            ArMesg1D[3], "TIME SYSTEM CORR"
                          ],
                          formatSettings));
  // Writeln (F_RinMN, Format (&#39;% 4s% 18s% 16s% 7d% 5d% 26s&#39;,
  // [&#39;GLUT&#39;, ArMesg1E [0], ArMesg1E [1], 0,0, &#39;TIME SYSTEM
  // CORR&#39;], formatSettings)) ; Writeln (F_RinMN, Format (&#39;% 4s% 18s%
  // 16s% 7d% 5d% 26s&#39;, [&#39;GLGP&#39;, ArMesg1E [2], ArMesg1E [1], 0,0,
  // &#39;TIME SYSTEM CORR&#39;], formatSettings)) ;
  Writeln(F_RinMN, Format("%6d%66s", [ LeapSec, "LEAP SECONDS" ]));
  Writeln(F_RinMN, Format("%73s", ["END OF HEADER"]));
  //

  // HEADER - GNSS Observation Data File
  Writeln(F_RinMO,
          "     3.02           OBSERVATION DATA    M                   RINEX "
          "VERSION / TYPE");
  Writeln(F_RinMO, "GEOS5toRNX          Geostar             " +
                       StGreateFileTime + " PGM / RUN BY / DATE ");
  Writeln(F_RinMO,
          "Name of antenna marker                                      MARKER "
          "NAME");
  Writeln(F_RinMO,
          "Number of antenna marker                                    MARKER "
          "NUMBER");
  Writeln(F_RinMO,
          "Type of the marker                                          MARKER "
          "TYPE");
  Writeln(F_RinMO,
          Format("%s%20s%50s", [ "GEOSTAR", "Geostar", "OBSERVER / AGENCY" ]));
  if (verSt == "")
    Writeln(F_RinMO, Format("%s%22s%20s%32s",
                            [ "GeoS5", "precise", "", "REC # / TYPE / VERS" ]));
  else
    Writeln(F_RinMO, Format("%s%8s%18s%34s", [
              "GeoS5" + StSN, "precise", verSt, "REC # / TYPE / VERS"
            ]));
  Writeln(F_RinMO, Format("%14.4f%14.4f%14.4f%37s",
                          [ coorX, coorY, coorZ, "APPROX POSITION XYZ" ],
                          formatSettings));
  Writeln(F_RinMO,
          Format("%14.4f%14.4f%14.4f%38s",
                 [ 0.0, 0.0, 0.0, "ANTENNA: DELTA H/E/N" ], formatSettings));
  if (G_OBS)
    Writeln(F_RinMO,
            "G    4 C1C D1C L1C S1C                                      SYS / "
            "# / OBS TYPES");
  if (R_OBS)
    Writeln(F_RinMO,
            "R    4 C1C D1C L1C S1C                                      SYS / "
            "# / OBS TYPES");
  if (E_OBS)
    Writeln(F_RinMO,
            "E    4 C1X D1X L1X S1X                                      SYS / "
            "# / OBS TYPES");
  if (J_OBS)
    Writeln(F_RinMO,
            "J    4 C1C D1C L1C S1C                                      SYS / "
            "# / OBS TYPES");
  // Writeln (F_RinO, &#39;18.000 INTERVAL&#39;);
  // Writeln (F_RinO, &#39;G APPL_DCB xyz.uvw.abc//pub/dcb_gps.dat SYS / DCBS
  // APPLIED&#39;);
  Writeln(F_RinMO,
          "DBHZ                                                        SIGNAL "
          "STRENGTH UNIT");
  Writeln(F_RinMO, Format("%6s%6s%6s%6s%6s%14s%7s%26s",
                          [
                            StYear, StMonth, StDay, StHour, StMinute, StSecF,
                            "GPS", "TIME OF FIRST OBS"
                          ],
                          formatSettings));
  // Writeln (F_RinO, &#39;2014 03 24 13 10 36.0000000 GPS TIME OF FIRST
  // OBS&#39;); Writeln (F_RinO, &#39;18 R01 1 R02 2 R03 3 R04 4 R05 5 R06 -6
  // R07 -5 R08 -4 GLONASS SLOT / FRQ #&#39;); Writeln (F_RinO, &#39;R09 -3 R10
  // -2 R11 -1 R12 0 R13 1 R14 2 R15 3 R16 4 GLONASS SLOT / FRQ #&#39;); Writeln
  // (F_RinO, &#39;R17 5 R18 -5 GLONASS SLOT / FRQ #&#39;); Writeln (F_RinO,
  // &#39;C1C -10.000 C1P -10.123 C2C -10.432 C2P -10.634 GLONASS COD / PHS /
  // BIS&#39;);
  Writeln(F_RinMO, Format("%6d%66s", [ LeapSec, "LEAP SECONDS" ]));
  Writeln(F_RinMO, Format("%73s", ["END OF HEADER"]));
  //

  // second file scroll to write data
  FirstAnalysis = 2;
  PBarRNX.Position = 0;
  kol = 0;
  FEnter = false;

  Reset(F1);
  siz = FileSize(F1);
  sizRead = 0;
  StrFile = "";
  TimeUTC = SystemTimeToDateTime(FTime);

  // reading data from a file
  while ((sizRead < siz)) {
    // determine the size of how much to read from the file
    StrHalfFile = "";
    if (((siz - sizRead) > 200000000))
      SetLength(StrHalfFile, 200000000);
    else
      SetLength(StrHalfFile, (siz - sizRead));

    // we read a block of a certain size from a file
    BlockRead(F1, StrHalfFile[1], Length(StrHalfFile), NumRead);
    // StrFile: = StrFile + StrHalfFile;
    StrFile = StrHalfFile;
    sizRead = sizRead + NumRead;

    while (Length(StrFile) > 0) {
      if (FEnter == false) {
        if (Length(StrFile) < 20) break;  // not enough data to analyze
        iSt = 0;
        iSt = Pos("GEOSr3PS", StrFile);
        if ((iSt == 0)) break;                  // sync word not found
        if (Length(StrFile) - iSt < 19) break;  // not enough data to analyze
        // sync word found
        FEnter = true;
      } else {
        if (Length(StrFile) - iSt < 19)  // not enough data to analyze
        {
          // Memo1.Lines.Add (&#39;err msg!&#39; + StrFile);
          // Delete (StrFile, 1, iSt-1); // delete the processed string
          FEnter = false;
          break;
        }
        // if iSt&gt; 1 then Memo1.Lines.Add (&#39;Sync Word Position&#39; +
        // IntToStr (iSt));
        ncmd = ord(StrFile[iSt + 9]) << 8 + ord(StrFile[iSt + 8]);
        ndat = ord(StrFile[iSt + 11]) << 8 + ord(StrFile[iSt + 10]);
        lnPc = 4 * ndat + 16;  // length of the entire packet in bytes
        lnPw = ndat + 4;       // longword packet length
        if (Length(StrFile) < iSt + lnPc - 1)  // not enough data to analyze
        {
          // Memo1.Lines.Add (&#39;err msg&#39; + inttohex (ncmd, 2) +
          // &#39;&#39; + StrFile); Delete (StrFile, 1, iSt-1); // delete the
          // processed string
          FEnter = false;
          break;
        }
        CSpac = 0;
        SetLength(ArDat, ndat);
        Application.ProcessMessages;
        // CS calculation
        {
          long ik_end = lnPw;
          for (ik = 0; ik < ik_end; ++ik) {
            dtlw = ord(StrFile[iSt + 4 * ik + 3])
                   << 24 + ord(StrFile[iSt + 4 * ik + 2])
                   << 16 + ord(StrFile[iSt + 4 * ik + 1])
                   << 8 + ord(StrFile[iSt + 4 * ik]);
            CSpac = CSpac xor dtlw;
            if ((ik > 2) && (ik < lnPw - 1)) ArDat[ik - 3] = dtlw;
          }
        }
        if (CSpac != 0)  // counter. package amount did not match
        {
          // skip era
          // DateTimeToSystemTime (now, TimeNow);
          StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                                  [
                                    StYear10, StMonth10, StDay10, StHour10,
                                    StMinute10, StSecond10
                                  ],
                                  formatSettings);
          Writeln(LOG,
                  StPac10 + ": ERROR! CS msg = " + inttohex(ncmd, 2) + ";");

          // if (ncmd = $ 10) then errCS10: = true;
          // if (ncmd = $ 13) then errCS13: = true;
          // OknoTP.Lines.Add (&#39;err CS msg&#39; + inttohex (ncmd, 2));
          // Delete (StrFile, 1, iSt + lnPc-1); // delete the processed string

          Delete(StrFile, 1, iSt + 3);  // deletion of processed data to the
                                        // start position of the bad line + GEOS
          // Delete (StrFile, 1, iSt-1); // delete processed data to the start
          // position of the bad line + GEOS
          iXt = 0;
          iXt = Pos("GEOSr3PS", StrFile);  // looking for geos
          Delete(StrFile, 1, iXt - 1);     // delete leftover bad string

          FEnter = false;
          // break;
        } else {
          // copying data and working out the team
          switch (ncmd) {
            case 0x0F:
              if (ndat == 20 * (ArDat[3] >> 16) + 6)
                ExCm0F;
              else
                CSpac = 1;
            case 0x10:
              if (ndat == 14 * (ArDat[3] >> 16) + 6)
                ExCm10;
              else
                CSpac = 1;
            case 0x13:
              if (ndat == 32)
                ExCm13;
              else
                CSpac = 1;
              // $ 14: if ndat = 12 then ExCm14 else CSpac: = 1;
            case 0x0A:
            case 0x9A:
              if (ndat == 32)
                ExCm0A;
              else
                CSpac = 1;
            case 0x1A:
            case 0x8A:
              if (ndat == 32)
                ExCm1A;
              else
                CSpac = 1;
            case 0x1B:
            case 0x8B:
              if (ndat == 30)
                ExCm1B;
              else
                CSpac = 1;
              // $ 1C: if ndat = 8 then ExCm1C else CSpac: = 1;
              // $ 1D: if ndat = 8 then ExCm1D else CSpac: = 1;
              // $ 1E: if ndat = 10 then ExCm1E else CSpac: = 1;
            case 0x20:
              if (ndat == 28)
                ExCm20;
              else
                CSpac = 1;
            case 0x22:
              if (ndat == 5 * ArDat[0] + 1)
                ExCm22;
              else
                CSpac = 1;
            case 0x24:
              if (ndat == 34)
                ExCm24;
              else
                CSpac = 1;

            case 0x3E:
              if (ndat == 3)
                ExCm3E;
              else
                CSpac = 1;
          }
          // if CSpac = 1 then OknoTP.Lines.Add (&#39;Wrong number of package
          // parameters&#39; + IntToHex (ncmd, 2));
          iSt = iSt + lnPc;
          // OknoTP.Lines.Add (&#39;iSt =&#39; + inttostr (iSt));
          kol = kol + lnPc;
          PBarRNX.Position = kol;
        }  // if CSpac &lt;&gt; 0
      }
      Application.ProcessMessages;
    }  // WHILE Length (StrFile)&gt; 0 DO
    Application.ProcessMessages;
  }  // WHILE (sizRead <siz) DO

  ConvComplete;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
// receiving the package &quot;consumer coordinates&quot;
void TFormRNX::ExCm20() {
  TCoorUser *cr20;

  time_t TimeEpochUTC;

  try {
    cr20 = &ArDat[0];
    // a sign of the decision of the NC and a sign of the reliability of the
    // decision and a sign of allocating time and date
    if (((((cr20->mdFix >> 19) && 1) == 1) && (cr20->PrValNT == 0)))  // Fix
    {
      GinFix = true;
      TimeUTC = ((cr20->DtTmUTC + LeapSec) * 1.0 / 86400 + 39448);
      ChDT = true;
    } else {
      GinFix = false;
      ChDT = false;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving the consumer coordinates RTK packet
void TFormRNX::ExCm24() {
  TPac24 *pc24;

  try {
    pc24 = &ArDat[0];
    // sign of decision
    if ((pc24.TypeSolve > 0))  // Fix
    {
      GinFix = true;
      TimeUTC = ((pc24->DtTmUTC + LeapSec) * 1.0 / 86400 + 39448);
      ChDT = true;
    } else {
      GinFix = false;
      ChDT = false;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
//
void TFormRNX::ExCm23() {
  TPac23 *pc23;
  std::string St;
  double a1;

  try {
    pc23 = &ArDat[0];
    // FileSeek (hgups, 0.2);
    // a1: = ((pc23 ^ .DtTmUTC + LeapSec) / 86400 + 39448) ;; FileWrite (hgups,
    // a1.8); // number of restarts a1: = pc23.TypeSolve; FileWrite (hgups,
    // a1.8);

    // wrong package
    // OknoTP.Lines.Add (&#39;Msg (23): error&#39;);
  } catch (...) {
    return;
  }
}
/*----------------------------------------------------------------------------*/
// reception of visible spacecraft
void TFormRNX::ExCm22() {
  TPac22 *pc22;
  int ik;
  double tel;
  double taz;
  int num;
  std::string StNum;
  u_int8_t Syst;
  int infix;

  try {
    pc22 = &ArDat[0];
    {
      long ik_end = (pc22->NSat);
      for (ik = 0; ik < ik_end; ++ik) {
        // Syst: = (pc22 ^ .vs [ik] .ss and $ 700) shr 8; // GPS = 1, GLN = 0
        num = pc22->vs[ik].idnm;  // receiving spacecraft number
        tel = 180.0 * (pc22->vs[ik].el) * 1.0 / pi;  // select mind
        taz = 180.0 * (pc22->vs[ik].aZ) * 1.0 / pi;  // we select AZ

        // infix: = (pc22.vs [ik] .ss and $ 20,000,000); // sign of using the
        // spacecraft in the solution
        if ((num > 0) && (num < 200) && (tel > 0) && (taz > 0)) {
          el[num] = tel;
        }
      }
    }  // end of cycle for all spacecraft

    // wrong package
    // OknoTP.Lines.Add (&#39;Msg (22): error&#39;);
  } catch (...) {
    return;
  }
}
/*----------------------------------------------------------------------------*/
// receiving a packet with measuring information 1st file recording
void TFormRNX::ExCm0F() {
  TPac0F *pc0F;
  int ik;
  int ichn;
  double dttm;
  double DbSecMSec;
  std::string St;
  time_t TimeEposh;
  std::string StNKA;
  double kof;
  std::string StLLI;
  int SVsON;
  int iz;
  bool FlagElMask;

  int numm;
  TDateTime tim;
  std::string StAll;
  std::string StNumm;
  std::string StdtUTC;
  std::string StPR;
  std::string StAF0;
  std::string StTn;
  std::string StX;
  std::string StY;
  std::string StZ;
  std::string StvX;
  std::string StvY;
  std::string StvZ;

  errCS10 = false;
  if ((pr0x10 == true)) return;

  try {
    kof = 1;
    pc0F = &ArDat[0];
    if ((ChBoxClockOFF.Checked == false)) {
      ClockOffset =
          pc0F.ClockOffGPS;  // ShV receiver shift relative to ShV GPS in meters
    } else
      ClockOffset = 0;
    LeapSec = pc0F.LeapSec;

    if ((FirstAnalysis == 1)) {
      if (pr0x0F == false) {
        if (((ChDT == true) && (pc0F.NMSat > 0))) {
          if (((LeapSec != 0) && (ClockOffset > -400) &&
               (ClockOffset < 400)))  // filter like in package 0x13 during
                                      // recording era in OBS file
          {
            // save the time of the first measurement
            // (pc0F.dtUTC-1) since decision flag comes after 0x10 packet
            // getting the first measurement time for the file names MO and MN -
            // GPS time
            dttm = ((pc0F.dtUTC - 1) + LeapSec) * 1.0 / 86400 +
                   RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
            DateTimeToSystemTime(dttm, FTime);
            pr0x0F = true;
          }
        }
      }
      // to form an array of GPS measurements over the entire file in the first
      // pass
      if (pr0x0F) {
        {
          long ik_end = pc0F.NMSat;
          for (ik = 0; ik < ik_end; ++ik) {
            if (((pc0F.Ms[ik].idChn in[0..42]) &&
                 (pc0F.Ms[ik].idnum in[1..197]))) {
              // we fix from which systems the measurements were
              if ((pc0F.Ms[ik].idnum in[1..32])) G_OBS = true;     // GPS OBS
              if ((pc0F.Ms[ik].idnum in[65..88])) R_OBS = true;    // GLN
              if ((pc0F.Ms[ik].idnum in[101..136])) E_OBS = true;  // Gal
              if ((pc0F.Ms[ik].idnum in[193..197])) J_OBS = true;  // QZSS

              // update the array of correspondence of the spacecraft number and
              // the letter
              if (((pc0F.Ms[ik].idnum > 64) && (pc0F.Ms[ik].idnum < 89)))
                ArLitNumKA[pc0F.Ms[ik].idnum - 64] = pc0F.Ms[ik].lit;
              //
              if (((pc0F.Ms[ik].prng > 1) && (pc0F.Ms[ik].prng < 90000000) &&
                   (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng > 2) &&
                   (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng < 10) &&
                   (pc0F.Ms[ik].ampsg > 27))) {
                // if checked &quot;only SVs used in position fix&quot;
                if ((((RGSV.ItemIndex == 1) &&
                      ((pc0F.Ms[ik].tdt && 0x20) == 0x20)) ||
                     (RGSV.ItemIndex == 0))) {
                  // there are measurements from this GPS satellite in this file
                  if ((pc0F.Ms[ik].idnum in[1..32]))
                    ArSVsOBS_GPS[pc0F.Ms[ik].idnum - 1] = 1;
                }
              }
            }
          }
        }
      }
      return;
    }

    if ((ClockOffset < -400) &&
        (ClockOffset >
         400))  // take with a margin, and should be from -300m to 300m
    {
      // skip era
      StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                              [
                                StYear10, StMonth10, StDay10, StHour10,
                                StMinute10, StSecond10
                              ],
                              formatSettings);
      Writeln(LOG, StPac10 + ": ERROR! ClockOffGPS = " +
                       FloatToStrF(ClockOffset, ffFixed, 5, 1) + ";");
      return;
    }

    // sign of highlighting date and time and spacecraft&gt; 0 in communication
    if (((ChDT == false) || (pc0F.NMSat == 0))) {
      // skip era
      StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                              [
                                StYear10, StMonth10, StDay10, StHour10,
                                StMinute10, StSecond10
                              ],
                              formatSettings);
      Writeln(LOG, StPac10 + ": ERROR! ChDT=false or pc0F.NMSat = " +
                       inttostr(pc0F.NMSat) + ";");
      return;
    }

    // GPS time
    dttm = (pc0F.dtUTC + LeapSec) * 1.0 / 86400 +
           RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
    DateTimeToSystemTime(dttm, TimeEposh);

    // to write the coordinate file ROVER
    TimeEposh1 = TimeEposh;

    StYear10 = inttostr(TimeEposh.wYear);
    if (TimeEposh.wMonth < 10)
      StMonth10 = "0" + inttostr(TimeEposh.wMonth);
    else
      StMonth10 = inttostr(TimeEposh.wMonth);
    if (TimeEposh.wDay < 10)
      StDay10 = "0" + inttostr(TimeEposh.wDay);
    else
      StDay10 = inttostr(TimeEposh.wDay);
    if ((TimeEposh.wHour) < 10)
      StHour10 = "0" + inttostr(TimeEposh.wHour);
    else
      StHour10 = inttostr(TimeEposh.wHour);
    if ((TimeEposh.wMinute) < 10)
      StMinute10 = "0" + inttostr(TimeEposh.wMinute);
    else
      StMinute10 = inttostr(TimeEposh.wMinute);
    DbSecMSec = TimeEposh.wSecond + (TimeEposh.wMilliseconds) * 0.001;
    if ((DbSecMSec) < 10)
      StSecond10 = "0" + FloatToStrF(DbSecMSec, ffFixed, 7, 7, formatSettings);
    else
      StSecond10 = FloatToStrF(DbSecMSec, ffFixed, 7, 7, formatSettings);

    // verification of the monotony of eras
    if ((pc0F.dtUTC <= pc10dtUTC_OLD)) {
      // skip era
      StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                              [
                                StYear10, StMonth10, StDay10, StHour10,
                                StMinute10, StSecond10
                              ],
                              formatSettings);
      Writeln(LOG, StPac10 + ": ERROR! Повтор эпохи!;");
      return;
    }
    pc10dtUTC_OLD = pc0F.dtUTC;

    // era-specific trap
    if ((TimeEposh.wHour == 19) && (TimeEposh.wMinute == 51) &&
        (TimeEposh.wSecond == 58)) {
      TimeEposh1 = TimeEposh;
    }

    SVsON = 0;
    FillChar(ArSVsOBS, sizeof(ArSVsOBS), 0);
    // SetLength (ArSVsOBS, 0);
    // SetLength (ArSVsOBS, pc0F.NMSat);
    // Further, the formation of measurements in this era for each spacecraft
    {
      long ik_end = pc0F.NMSat;
      for (ik = 0; ik < ik_end; ++ik)
      // for ik: = 0 to 42 do
      {
        // ArSVsOBS [ik]: = &#39;&#39;;
        if (((pc0F.Ms[ik].idChn in[0..42]) && (pc0F.Ms[ik].idnum in[1..197]))) {
          // add spacecraft number to point
          // GPS
          if ((pc0F.Ms[ik].idnum in[1..32])) {
            StNKA = "G" + Format("%.2d", [pc0F.Ms[ik].idnum]);
            kof = L1freq * 1.0 / SL;
          }
          // GLN
          if ((pc0F.Ms[ik].idnum in[65..88])) {
            StNKA = "R" + Format("%.2d", [pc0F.Ms[ik].idnum - 64]);
            kof = (1602e6 + (pc0F.Ms[ik].lit * 562.5e3)) * 1.0 / SL;
          }
          // Gal
          if ((pc0F.Ms[ik].idnum in[101..136])) {
            StNKA = "E" + Format("%.2d", [pc0F.Ms[ik].idnum - 100]);
            kof = L1freq * 1.0 / SL;
          }
          // QZSS
          if ((pc0F.Ms[ik].idnum in[193..197])) {
            StNKA = "J" + Format("%.2d", [pc0F.Ms[ik].idnum - 192]);
            kof = L1freq * 1.0 / SL;
          }
          // LLI flag check
          if ((((pc0F.Ms[ik].tdt >> 6) && 3) == 0))
            StLLI = " ";
          else
            StLLI = "1";
          // Filter by UM if mode is selected - only SVs used in position fix
          FlagElMask = false;
          if (((RGSV.ItemIndex == 1)) && (EditElMask.Text != "")) {
            if ((el[pc0F.Ms[ik].idnum] < strtoint(EditElMask.Text)))
              FlagElMask = true;  // skip this spacecraft
          }
          // rejection of anomalous measurement values
          if (((pc0F.Ms[ik].prng > 1) && (pc0F.Ms[ik].prng < 90000000) &&
               (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng > 2) &&
               (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng < 10) &&
               (pc0F.Ms[ik].ampsg > 25) && (FlagElMask == false))) {
            // if checked &quot;only SVs used in position fix&quot;
            if ((((RGSV.ItemIndex == 1) &&
                  ((pc0F.Ms[ik].tdt && 0x20) == 0x20)) ||
                 (RGSV.ItemIndex == 0))) {
              // line forming
              St = StNKA +
                   Format("%14.3f", [pc0F.Ms[ik].prng], formatSettings) +
                   Format("%2s", [""]) +  // pseudorange
                   Format("%14.3f", [pc0F.Ms[ik].pvel * kof], formatSettings) +
                   Format("%2s", [""]) +  // pseudo speed
                   Format("%14.3f%s", [ pc0F.Ms[ik].pphi, StLLI ],
                          formatSettings) +
                   Format("%1s", [""]) +  // phase + LLI
                   Format("%14.3f", [pc0F.Ms[ik].ampsg], formatSettings);
              ArSVsOBS[SVsON] = St;
              SVsON = SVsON + 1;
            }
          } else {
            if (((pc0F.Ms[ik].ampsg > 25) && (StLLI != "1"))) {
              if ((FlagElMask == false)) {
                StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                                        [
                                          StYear10, StMonth10, StDay10,
                                          StHour10, StMinute10, StSecond10
                                        ],
                                        formatSettings);
                Writeln(LOG, StPac10 +
                                 ": ERROR! отбраковка аномальных значений "
                                 "измерений КА " +
                                 StNKA);
                Writeln(LOG, "(1 > pc10.Ms[ik].prng < 90000000) = " +
                                 FloatToStrF(pc0F.Ms[ik].prng, ffFixed, 8, 3) +
                                 ";");
                Writeln(LOG, "(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = " +
                                 FloatToStrF((pc0F.Ms[ik].pphi * 1.0 /
                                              pc0F.Ms[ik].prng),
                                             ffFixed, 8, 3) +
                                 ";");
                Writeln(LOG, "(pc10.Ms[ik].ampsg > 25) = " +
                                 FloatToStrF(pc0F.Ms[ik].ampsg, ffFixed, 8, 3) +
                                 ";");
              } else {
                // StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s%
                // 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10,
                // StMinute10, StSecond10], formatSettings); Writeln (LOG,
                // StPac10 + &#39;: ERROR! Sorting by spacecraft elevation&#39;
                // + StNKA); Writeln (LOG, &#39;(pc10.Ms [ik] .ampsg&gt; 25)
                // =&#39; + floattostrF (pc0F.Ms [ik] .ampsg, ffFixed, 8.3) +
                // &#39;;&#39;);
              }
            }
          }
        }  // if ichn in [0..43] then
      }
    }  // for ik: = 0 to pc0F.NMSat-1 do

    StKA10 = inttostr(SVsON);

    // write to file - the beginning of an era
    StPac10 =
        "> " + Format("%4s%3s%3s%3s%3s%11s%3s%3s%21.12f",
                      [
                        StYear10, StMonth10, StDay10, StHour10, StMinute10,
                        StSecond10, "0", StKA10, ClockOffset * 1.0 / SL
                      ],
                      formatSettings);
    Writeln(F_RinMO, StPac10);
    // write to file - each spacecraft
    {
      long ik_end = Length(ArSVsOBS);
      for (ik = 0; ik < ik_end; ++ik)
        if ((ArSVsOBS[ik] != "")) Writeln(F_RinMO, ArSVsOBS[ik]);
    }

  } catch (...) {
    StKA10 = inttostr(SVsON);
    // OknoTP.Lines.Add (&#39;error messages 0x10!&#39;);
    return;
  }
}

// receiving a packet with measuring information 1st file recording
void TFormRNX::ExCm10() {
  TPac10 *pc10;
  int ik;
  int ichn;
  double dttm;
  double DbSecMSec;
  std::string St;
  time_t TimeEposh;
  std::string StNKA;
  double kof;
  std::string StLLI;
  int SVsON;
  int iz;
  bool FlagElMask;
  int ArSVsOBS_GPS[43];  // array of measurements

  errCS10 = false;
  // if (errCS13 = true) then exit;

  try {
    kof = 1;
    pc10 = &ArDat[0];

    if ((ChBoxClockOFF.Checked == false)) {
      ClockOffset =
          pc10.ClockOffGPS;  // ShV receiver shift relative to ShV GPS in meters
    } else
      ClockOffset = 0;

    LeapSec = pc10.LeapSec;

    if ((FirstAnalysis == 1)) {
      if (pr0x10 == false) {
        if (((ChDT == true) && (pc10.NMSat > 0))) {
          if (((LeapSec != 0)))  // filter like in package 0x13 during recording
                                 // era in OBS file
          {
            // save the time of the first measurement
            // (pc10.dtUTC-1) since decision flag comes after 0x10 packet
            // getting the first measurement time for the file names MO and MN -
            // GPS time
            dttm = ((pc10.dtUTC - 1) + LeapSec) * 1.0 / 86400 +
                   RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
            DateTimeToSystemTime(dttm, FTime);
            pr0x10 = true;
          }
        }
      }
      // to form an array of GPS measurements over the entire file in the first
      // pass
      if (pr0x10) {
        {
          long ik_end = pc10.NMSat;
          for (ik = 0; ik < ik_end; ++ik) {
            if (((pc10.Ms[ik].idChn in[0..42]) &&
                 (pc10.Ms[ik].idnum in[1..197]))) {
              // we fix from which systems the measurements were
              if ((pc10.Ms[ik].idnum in[1..32])) G_OBS = true;     // GPS OBS
              if ((pc10.Ms[ik].idnum in[65..88])) R_OBS = true;    // GLN
              if ((pc10.Ms[ik].idnum in[101..136])) E_OBS = true;  // Gal
              if ((pc10.Ms[ik].idnum in[193..197])) J_OBS = true;  // QZSS

              // update the array of correspondence of the spacecraft number and
              // the letter
              if (((pc10.Ms[ik].idnum > 64) && (pc10.Ms[ik].idnum < 89)))
                ArLitNumKA[pc10.Ms[ik].idnum - 64] = pc10.Ms[ik].lit;
              //
              if (((pc10.Ms[ik].prng > 1) && (pc10.Ms[ik].prng < 90000000) &&
                   (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng > 2) &&
                   (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng < 10) &&
                   (pc10.Ms[ik].ampsg > 27))) {
                // if checked &quot;only SVs used in position fix&quot;
                if ((((RGSV.ItemIndex == 1) &&
                      ((pc10.Ms[ik].tdt && 0x20) == 0x20)) ||
                     (RGSV.ItemIndex == 0))) {
                  // there are measurements from this GPS satellite in this file
                  if ((pc10.Ms[ik].idnum in[1..32]))
                    ArSVsOBS_GPS[pc10.Ms[ik].idnum - 1] = 1;
                }
              }
            }
          }
        }
      }
      return;  // if (FirstAnalysis = 1) then
    }

    if ((ClockOffset < -400) &&
        (ClockOffset >
         400))  // take with a margin, and should be from -300m to 300m
    {
      // ClockOffset range error
      StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                              [
                                StYear10, StMonth10, StDay10, StHour10,
                                StMinute10, StSecond10
                              ],
                              formatSettings);
      Writeln(LOG, StPac10 + ": ERROR! ClockOffGPS = " +
                       FloatToStrF(ClockOffset, ffFixed, 5, 1) + ";");
    }

    // sign of highlighting date and time and spacecraft&gt; 0 in communication
    if (((ChDT == false) || (pc10.NMSat == 0))) {
      // skip era
      StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                              [
                                StYear10, StMonth10, StDay10, StHour10,
                                StMinute10, StSecond10
                              ],
                              formatSettings);
      Writeln(LOG, StPac10 + ": ERROR! ChDT=false or pc10.NMSat = " +
                       inttostr(pc10.NMSat) + ";");
      return;
    }

    // sign of highlighting date and time and spacecraft&gt; 0 in communication
    // if ((pc10.dtUTC = 0) or (pc10.NMSat = 0)) then
    // begin
    // // skip era
    // StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s% 3s% 11s&#39;,
    // [StYear10, StMonth10, StDay10, StHour10, StMinute10, StSecond10],
    // formatSettings); Writeln (LOG, StPac10 + &#39;: ERROR! ChDT = false or
    // pc10.NMSat =&#39; + inttostr (pc10.NMSat) + &#39;;&#39;); exit end;

    // GPS time
    dttm = (pc10.dtUTC + LeapSec) * 1.0 / 86400 +
           RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
    DateTimeToSystemTime(dttm, TimeEposh);

    // to write the coordinate file ROVER
    TimeEposh1 = TimeEposh;

    StYear10 = inttostr(TimeEposh.wYear);
    if (TimeEposh.wMonth < 10)
      StMonth10 = "0" + inttostr(TimeEposh.wMonth);
    else
      StMonth10 = inttostr(TimeEposh.wMonth);
    if (TimeEposh.wDay < 10)
      StDay10 = "0" + inttostr(TimeEposh.wDay);
    else
      StDay10 = inttostr(TimeEposh.wDay);
    if ((TimeEposh.wHour) < 10)
      StHour10 = "0" + inttostr(TimeEposh.wHour);
    else
      StHour10 = inttostr(TimeEposh.wHour);
    if ((TimeEposh.wMinute) < 10)
      StMinute10 = "0" + inttostr(TimeEposh.wMinute);
    else
      StMinute10 = inttostr(TimeEposh.wMinute);
    DbSecMSec = TimeEposh.wSecond + (TimeEposh.wMilliseconds) * 0.001;
    if ((DbSecMSec) < 10)
      StSecond10 = "0" + FloatToStrF(DbSecMSec, ffFixed, 7, 7, formatSettings);
    else
      StSecond10 = FloatToStrF(DbSecMSec, ffFixed, 7, 7, formatSettings);

    // verification of the monotony of eras
    if ((pc10.dtUTC <= pc10dtUTC_OLD)) {
      // skip era
      StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                              [
                                StYear10, StMonth10, StDay10, StHour10,
                                StMinute10, StSecond10
                              ],
                              formatSettings);
      Writeln(LOG, StPac10 + ": ERROR! Повтор эпохи!;");
      return;
    }
    pc10dtUTC_OLD = pc10.dtUTC;

    // // trap of a specific era
    // if (TimeEposh.wHour = 5) and (TimeEposh.wMinute = 41) and
    // (TimeEposh.wSecond = 18) then begin TimeEposh1: = TimeEposh; end;

    SVsON = 0;
    FillChar(ArSVsOBS, sizeof(ArSVsOBS), 0);
    // SetLength (ArSVsOBS, 0);
    // SetLength (ArSVsOBS, pc10.NMSat);
    // Further, the formation of measurements in this era for each spacecraft
    {
      long ik_end = pc10.NMSat;
      for (ik = 0; ik < ik_end; ++ik)
      // for ik: = 0 to 42 do
      {
        // ArSVsOBS [ik]: = &#39;&#39;;
        if (((pc10.Ms[ik].idChn in[0..42]) && (pc10.Ms[ik].idnum in[1..197]))) {
          // add spacecraft number to point
          // GPS
          if ((pc10.Ms[ik].idnum in[1..32])) {
            StNKA = "G" + Format("%.2d", [pc10.Ms[ik].idnum]);
            kof = L1freq * 1.0 / SL;
          }
          // GLN
          if ((pc10.Ms[ik].idnum in[65..88])) {
            StNKA = "R" + Format("%.2d", [pc10.Ms[ik].idnum - 64]);
            kof = (1602e6 + (pc10.Ms[ik].lit * 562.5e3)) * 1.0 / SL;
          }
          // Gal
          if ((pc10.Ms[ik].idnum in[101..136])) {
            StNKA = "E" + Format("%.2d", [pc10.Ms[ik].idnum - 100]);
            kof = L1freq * 1.0 / SL;
          }
          // QZSS
          if ((pc10.Ms[ik].idnum in[193..197])) {
            StNKA = "J" + Format("%.2d", [pc10.Ms[ik].idnum - 192]);
            kof = L1freq * 1.0 / SL;
          }
          // LLI flag check
          if ((((pc10.Ms[ik].tdt >> 6) && 3) == 0))
            StLLI = " ";
          else
            StLLI = "1";
          // Filter by UM if mode is selected - only SVs used in position fix
          FlagElMask = false;
          if (((RGSV.ItemIndex == 1)) && (EditElMask.Text != "")) {
            if ((el[pc10.Ms[ik].idnum] < strtoint(EditElMask.Text)))
              FlagElMask = true;  // skip this spacecraft
          }
          // rejection of anomalous measurement values
          if (((pc10.Ms[ik].prng > 1) && (pc10.Ms[ik].prng < 90000000) &&
               (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng > 2) &&
               (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng < 10) &&
               (pc10.Ms[ik].ampsg > 25) && (FlagElMask == false))) {
            // if checked &quot;only SVs used in position fix&quot;
            if ((((RGSV.ItemIndex == 1) &&
                  ((pc10.Ms[ik].tdt && 0x20) == 0x20)) ||
                 (RGSV.ItemIndex == 0))) {
              // line forming
              St = StNKA +
                   Format("%14.3f", [pc10.Ms[ik].prng], formatSettings) +
                   Format("%2s", [""]) +  // pseudorange
                   Format("%14.3f", [pc10.Ms[ik].pvel * kof], formatSettings) +
                   Format("%2s", [""]) +  // pseudo speed
                   Format("%14.3f%s", [ pc10.Ms[ik].pphi, StLLI ],
                          formatSettings) +
                   Format("%1s", [""]) +  // phase + LLI
                   Format("%14.3f", [pc10.Ms[ik].ampsg], formatSettings);
              ArSVsOBS[SVsON] = St;
              SVsON = SVsON + 1;
            }
          } else {
            if (((pc10.Ms[ik].ampsg > 25) && (StLLI != "1"))) {
              if ((FlagElMask == false)) {
                StPac10 = "> " + Format("%4s%3s%3s%3s%3s%11s",
                                        [
                                          StYear10, StMonth10, StDay10,
                                          StHour10, StMinute10, StSecond10
                                        ],
                                        formatSettings);
                Writeln(LOG, StPac10 +
                                 ": ERROR! отбраковка аномальных значений "
                                 "измерений КА " +
                                 StNKA);
                Writeln(LOG, "(1 > pc10.Ms[ik].prng < 90000000) = " +
                                 FloatToStrF(pc10.Ms[ik].prng, ffFixed, 8, 3) +
                                 ";");
                Writeln(LOG, "(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = " +
                                 FloatToStrF((pc10.Ms[ik].pphi * 1.0 /
                                              pc10.Ms[ik].prng),
                                             ffFixed, 8, 3) +
                                 ";");
                Writeln(LOG, "(pc10.Ms[ik].ampsg > 25) = " +
                                 FloatToStrF(pc10.Ms[ik].ampsg, ffFixed, 8, 3) +
                                 ";");
              } else {
                // StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s%
                // 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10,
                // StMinute10, StSecond10], formatSettings); Writeln (LOG,
                // StPac10 + &#39;: ERROR! Sorting by spacecraft elevation&#39;
                // + StNKA); Writeln (LOG, &#39;(pc10.Ms [ik] .ampsg&gt; 25)
                // =&#39; + floattostrF (pc10.Ms [ik] .ampsg, ffFixed, 8.3) +
                // &#39;;&#39;);
              }
            }
          }
        }  // if ichn in [0..43] then
      }
    }  // for ik: = 0 to pc10.NMSat-1 do

    StKA10 = inttostr(SVsON);

    // write to file - the beginning of an era
    StPac10 =
        "> " + Format("%4s%3s%3s%3s%3s%11s%3s%3s%21.12f",
                      [
                        StYear10, StMonth10, StDay10, StHour10, StMinute10,
                        StSecond10, "0", StKA10, ClockOffset * 1.0 / SL
                      ],
                      formatSettings);
    Writeln(F_RinMO, StPac10);
    // write to file - each spacecraft
    {
      long ik_end = Length(ArSVsOBS);
      for (ik = 0; ik < ik_end; ++ik)
        if ((ArSVsOBS[ik] != "")) Writeln(F_RinMO, ArSVsOBS[ik]);
    }

  } catch (...) {
    StKA10 = inttostr(SVsON);
    // OknoTP.Lines.Add (&#39;error messages 0x10!&#39;);
    return;
  }
}

/*----------------------------------------------------------------------------*/
// receiving a packet according to the decision of the NC
void TFormRNX::ExCm13() {
  TPac13 *pc13;
  int ik;
  int iSt;
  int nKA10;
  double dttm;

  errCS13 = false;
  if ((errCS10 == true)) return;

  try {
    nKA10 = 0;
    if ((FirstAnalysis == 1) && GinFix) {
      pc13 = &ArDat[0];
      if ((ChBoxClockOFF.Checked == false)) {
        ClockOffset =
            pc13->dt[3];  // ShV receiver shift relative to ShV GPS in meters
      } else
        ClockOffset = 0;
      coorX = pc13->dt[0];
      coorY = pc13->dt[1];
      coorZ = pc13->dt[2];
      if (((ClockOffset > -400) && (ClockOffset < 400))) pr0x13 = true;
      return;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// packet reception time parameters
void TFormRNX::ExCm14() {
  TPac14 *pc14;
  int rty;

  try {
    pc14 = &ArDat[0];
    if ((pr0x14 == false)) {
      WRollOver = pc14->RlWk;
      pr0x14 = true;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving a packet updating the values of the GLONASS spacecraft almanac
void TFormRNX::ExCm19() {
  TAlmnGLN *aGLN;
  int ik;

  try {
    aGLN = &ArDat[0];
    if ((pr0x19 == false)) {
      // fill in the array of correspondence of the spacecraft number and the
      // letter
      if (((aGLN->num > 0) && (aGLN->num < 25)))
        ArLitNumKA[aGLN->num] = aGLN->Alt;
      // checking the filling of the entire almanac
      pr0x19 = true;
      {
        long ik_end = 25;
        for (ik = 1; ik < ik_end; ++ik)
          if ((ArLitNumKA[ik] == 100)) pr0x19 = false;
      }
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving a packet updating the values of the ephemeris KA GALILEO record 2nd
// file
void TFormRNX::ExCm0A() {
  TEphGPS *eGAL;
  u_int8_t ik;
  double dttm;
  time_t TimeEposh;
  time_t TimeReal;
  std::string StYear;
  std::string StMonth;
  std::string StDay;
  std::string StHour;
  std::string StMinute;
  std::string StSecond;
  std::string StNKA;
  std::string StAF0;
  std::string Staf1;
  std::string Staf2;
  std::string StIODE;
  std::string StC_rs;
  std::string Stdn;
  std::string StM;
  std::string StC_uc;
  std::string Ste;
  std::string StC_us;
  std::string StA;
  std::string Stt_oe;
  std::string StC_ic;
  std::string StOMEGA_0;
  std::string StC_is;
  std::string StI0;
  std::string StC_rc;
  std::string Stw;
  std::string StOMEGADOT;
  std::string StIDOT;
  std::string StCodesL2;
  std::string StweekN;
  std::string StL2PdataF;
  std::string StSVaccur;
  std::string StSVhelth;
  std::string StTgd;
  std::string StIODC;
  std::string Stdtrc;
  std::string StFitinterval;
  bool fil1;
  double Eph_Alm_E;

  try {
    if ((FirstAnalysis == 1)) {
      pr0x0A = true;
      return;
    }

    eGAL = &ArDat[0];

    // Filter 0
    // filtering ephemeris with zero data values
    if (((eGAL.n == 0) || (eGAL.I0 == 0) || (eGAL.SVhelth != 0))) {
      // skip era
      Writeln(LOG,
              "EPH GAL ERROR! фильтрация GAL эфемерид с нулевыми значениями "
              "данных, eGAL.n = " +
                  inttostr(eGAL.n) + "; eGAL.I0 = " + floattostr(eGAL.I0) +
                  "; eGAL.SVhelth = " + inttostr(eGAL.SVhelth) + ";");
      return;
    }

    // Filter 1
    // if in the OBS file there are no measurements at all from GPS PRN = N
    // (meaning measurements in the &quot;only SVs used in position fix&quot;
    // mode), then all ephemeris sets with PRN = N can be considered
    // &quot;fake&quot; and ignored.
    if ((RGSV.ItemIndex == 1)) {
      // if ((ArSVsOBS_GPS [eGPS.n-1]) &lt;&gt; 1) then
      // begin
      // // skip era
      // Writeln (LOG, &#39;EPH GPS ERROR! OBS file generally no measurements
      // from eGPS.n =&#39; + inttostr (eGPS.n) + &#39;;&#39;); exit end;
    }

    // Filter 2
    // Compare afo and eccentricity from the “tested” Geos GAL ephemeris (PRN =
    // N) with the corresponding parameters from the almanac for GAL PRN = N.
    if (pr0x88)  // GAL almanac request required
    {
      // if (eGPS.n = ArTAlmnGPS [eGPS.n] .num) then // if the received
      // ephemeris of this spacecraft is in the almanac array begin // then we
      // carry out data verification Eph_Alm_E: = abs (ArTAlmnGPS [eGPS.n] .e -
      // eGPS.e);
      //
      // if ((abs (ArTAlmnGPS [eGPS.n] .e - eGPS.e)&gt; 0.001) or
      // (abs (ArTAlmnGPS [eGPS.n] .af0 - eGPS.af0)&gt; 0.0001)) then
      // begin
      // // skip era
      // Writeln (LOG, &#39;EPH GPS ERROR! Compare afo and eccentricity
      // ephemeris and almanac, difference Eph_Alm_E =&#39; + floattostr
      // (Eph_Alm_E) + &#39;;&#39;); exit end; end;
    }

    // Filter 3
    // verification of received ephemeris with saved
    if (((eGAL.t_oe == ArTEphGAL[eGAL.n].t_oe) &&
         (eGAL.t_oc == ArTEphGAL[eGAL.n].t_oc) &&
         (eGAL.IODE == ArTEphGAL[eGAL.n].IODE) &&
         (eGAL.A == ArTEphGAL[eGAL.n].A))) {
      // skip era
      Writeln(LOG, "EPH GAL ERROR! повтор эфемерид GAL от КА№" +
                       inttostr(eGAL.n) + ";");
      return;
    }

    // IODC and IODE match check
    // if (eGAL.IODC &lt;&gt; eGAL.IODE) then
    // begin
    // //OknoTP.Lines.Add(&#39;IODC and IODE GPS from KA№ &#39;+ inttostr
    // (eGPS.n) are not equal; Writeln (LOG, &#39;EPH GAL ERROR! Unequal IODC
    // and IODE GAL from КА№&#39; + inttostr (eGAL.n) + &#39;;&#39;); exit end;
    // ===================================================
    // ====================================

    // further conversion to a navigation file
    if (eGAL.n < 10)
      StNKA = "E0" + inttostr(eGAL.n);
    else
      StNKA = "E" + inttostr(eGAL.n);
    // GPS time
    // dttm: = (eGAL ^ .t_oe + (604800 * (eGAL ^ .weekN + 1024 * WRollOver))) /
    // 86400 + RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;); WRollOver in
    // GALILEO will be equal to 1 since eGAL ^ .weekN may be 4095 (due to 12bit
    // dimension)
    dttm = (eGAL->t_oe + (604800 * (eGAL->weekN + 1024 * 1))) * 1.0 / 86400 +
           RegTimeGPS;  // StrToDateTime (&#39;06 .01.1980 &#39;);

    // Filter 4
    // if dttm is more than 2 hours from local time, then the actual ephemeris
    // is no longer
    if ((abs(TimeUTC - dttm) > 0.1)) {
      Writeln(LOG, StNKA + " error GAL dttm; old ephemeris!");
      return;
    }

    // preservation of more recent ephemeris in the table
    ArTEphGAL[eGAL.n].n = eGAL.n;
    ArTEphGAL[eGAL.n].t_oe = eGAL.t_oe;
    ArTEphGAL[eGAL.n].t_oc = eGAL.t_oc;
    ArTEphGAL[eGAL.n].IODE = eGAL.IODE;
    ArTEphGAL[eGAL.n].IODC = eGAL.IODC;
    ArTEphGAL[eGAL.n].af0 = eGAL.af0;
    ArTEphGAL[eGAL.n].A = eGAL.A;

    // // cure for incorrect eGPS ^ .weekN
    // if (abs (TimeUTC - dttm)&gt; (1.0 / 8.0)) then // 0.125
    // begin
    // dttm: = ((604800 * (eGAL ^ .weekN + 1 + 1024 * WRollOver))) / 86400 +
    // RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;); Writeln (LOG, StNKA +
    // &#39;error eGPS ^ .weekN!&#39;); end;
    DateTimeToSystemTime(dttm, TimeEposh);

    StYear = inttostr(TimeEposh.wYear);
    if (TimeEposh.wMonth < 10)
      StMonth = "0" + inttostr(TimeEposh.wMonth);
    else
      StMonth = inttostr(TimeEposh.wMonth);
    if (TimeEposh.wDay < 10)
      StDay = "0" + inttostr(TimeEposh.wDay);
    else
      StDay = inttostr(TimeEposh.wDay);
    if ((TimeEposh.wHour) < 10)
      StHour = "0" + inttostr(TimeEposh.wHour);
    else
      StHour = inttostr(TimeEposh.wHour);
    if ((TimeEposh.wMinute) < 10)
      StMinute = "0" + inttostr(TimeEposh.wMinute);
    else
      StMinute = inttostr(TimeEposh.wMinute);
    if ((TimeEposh.wSecond) < 10)
      StSecond = "0" + inttostr(TimeEposh.wSecond);
    else
      StSecond = inttostr(TimeEposh.wSecond);

    // ===================================================
    // ====================================

    // SV / EPOCH / SV CLK
    StAF0 = FormatFloat("0.000000000000E+00", eGAL->af0, formatSettings);
    Staf1 = FormatFloat("0.000000000000E+00", eGAL->af1, formatSettings);
    Staf2 = FormatFloat("0.000000000000E+00", eGAL->af2, formatSettings);
    Writeln(F_RinMN, Format("%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s", [
              StNKA, StYear, StMonth, StDay, StHour, StMinute, StSecond, StAF0,
              Staf1, Staf2
            ]));
    // BROADCAST ORBIT - 1
    StIODE = FormatFloat("0.000000000000E+00", atof(inttostr(eGAL->IODE)),
                         formatSettings);
    StC_rs = FormatFloat("0.000000000000E+00", eGAL->C_rs, formatSettings);
    Stdn = FormatFloat("0.000000000000E+00", eGAL->dn * pi, formatSettings);
    StM = FormatFloat("0.000000000000E+00", eGAL->M * pi, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StIODE, StC_rs, Stdn, StM ]));
    // BROADCAST ORBIT - 2
    StC_uc = FormatFloat("0.000000000000E+00", eGAL->C_uc, formatSettings);
    Ste = FormatFloat("0.000000000000E+00", eGAL->e, formatSettings);
    StC_us = FormatFloat("0.000000000000E+00", eGAL->C_us, formatSettings);
    StA = FormatFloat("0.000000000000E+00", eGAL->A, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StC_uc, Ste, StC_us, StA ]));
    // BROADCAST ORBIT - 3
    Stt_oe = FormatFloat("0.000000000000E+00", eGAL->t_oe, formatSettings);
    StC_ic = FormatFloat("0.000000000000E+00", eGAL->C_ic, formatSettings);
    StOMEGA_0 =
        FormatFloat("0.000000000000E+00", eGAL->OMEGA_0 * pi, formatSettings);
    StC_is = FormatFloat("0.000000000000E+00", eGAL->C_is, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", Stt_oe, StC_ic, StOMEGA_0, StC_is ]));
    // BROADCAST ORBIT - 4
    StI0 = FormatFloat("0.000000000000E+00", eGAL->I0 * pi, formatSettings);
    StC_rc = FormatFloat("0.000000000000E+00", eGAL->C_rc, formatSettings);
    Stw = FormatFloat("0.000000000000E+00", eGAL->w * pi, formatSettings);
    StOMEGADOT =
        FormatFloat("0.000000000000E+00", eGAL->OMEGADOT * pi, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StI0, StC_rc, Stw, StOMEGADOT ]));
    // BROADCAST ORBIT - 5
    StIDOT = FormatFloat("0.000000000000E+00", eGAL->IDOT * pi, formatSettings);
    StCodesL2 = FormatFloat("0.000000000000E+00", 0.0, formatSettings);
    StweekN = FormatFloat("0.000000000000E+00", eGAL->weekN + 1024 + (0 * 4096),
                          formatSettings);
    StL2PdataF = FormatFloat("0.000000000000E+00", 0.0, formatSettings);
    Writeln(F_RinMN,
            Format("%4s%19s%19s%19s%19s",
                   [ "    ", StIDOT, StCodesL2, StweekN, StL2PdataF ]));
    // BROADCAST ORBIT - 6
    StSVaccur =
        FormatFloat("0.000000000000E+00", eGAL->SVaccur, formatSettings);
    StSVhelth =
        FormatFloat("0.000000000000E+00", eGAL->SVhelth, formatSettings);
    StTgd = FormatFloat("0.000000000000E+00", eGAL->Tgd, formatSettings);
    StIODC = FormatFloat("0.000000000000E+00", eGAL->IODC, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StSVaccur, StSVhelth, StTgd, StIODC ]));
    // BROADCAST ORBIT - 7
    // Stdtrc: = FormatFloat (&#39;0.000000000000E + 00&#39;, eGPS ^ .dtrc,
    // formatSettings);
    Stdtrc = FormatFloat(
        "0.000000000000E+00", eGAL->t_oe,
        formatSettings);  // return this entry in the standard version
    StFitinterval = FormatFloat("0.000000000000E+00", 0.0, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s", [ "    ", Stdtrc, StFitinterval ]));

    // OknoTP.Lines.Add (&#39;error messages 0x1A!&#39;);
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving a packet updating the ephemeris values of the GPS satellite record
// of the 2nd file
void TFormRNX::ExCm1A() {
  TEphGPS *eGPS;
  u_int8_t ik;
  double dttm;
  time_t TimeEpoch;
  time_t TimeReal;
  std::string StYear;
  std::string StMonth;
  std::string StDay;
  std::string StHour;
  std::string StMinute;
  std::string StSecond;
  std::string StNKA;
  std::string StAF0;
  std::string Staf1;
  std::string Staf2;
  std::string StIODE;
  std::string StC_rs;
  std::string Stdn;
  std::string StM;
  std::string StC_uc;
  std::string Ste;
  std::string StC_us;
  std::string StA;
  std::string Stt_oe;
  std::string StC_ic;
  std::string StOMEGA_0;
  std::string StC_is;
  std::string StI0;
  std::string StC_rc;
  std::string Stw;
  std::string StOMEGADOT;
  std::string StIDOT;
  std::string StCodesL2;
  std::string StweekN;
  std::string StL2PdataF;
  std::string StSVaccur;
  std::string StSVhelth;
  std::string StTgd;
  std::string StIODC;
  std::string Stdtrc;
  std::string StFitinterval;
  bool fil1;
  double Eph_Alm_E;

  try {
    eGPS = &ArDat[0];

    if ((FirstAnalysis == 1)) {
      pr0x1A = true;
      return;
    }

    // Filter 0
    // filtering ephemeris with zero data values
    if (((eGPS.n == 0) || (eGPS.I0 == 0) || (eGPS.SVhelth != 0))) {
      // skip era
      Writeln(LOG,
              "EPH GPS ERROR! фильтрация GPS эфемерид с нулевыми значениями "
              "данных, eGPS.n = " +
                  inttostr(eGPS.n) + "; eGPS.I0 = " + floattostr(eGPS.I0) +
                  "; eGPS.SVhelth = " + inttostr(eGPS.SVhelth) + ";");
      return;
    }

    // Filter 1
    // if in the OBS file there are no measurements at all from GPS PRN = N
    // (meaning measurements in the &quot;only SVs used in position fix&quot;
    // mode), then all ephemeris sets with PRN = N can be considered
    // &quot;fake&quot; and ignored.
    if ((RGSV.ItemIndex == 1)) {
      // if ((ArSVsOBS_GPS [eGPS.n-1]) &lt;&gt; 1) then
      // begin
      // // skip era
      // Writeln (LOG, &#39;EPH GPS ERROR! OBS file generally no measurements
      // from eGPS.n =&#39; + inttostr (eGPS.n) + &#39;;&#39;); exit end;
    }

    // Filter 2
    // Compare afo and eccentricity from the &quot;tested&quot; geosovo GPS
    // ephemeris (PRN = N) with the corresponding parameters from the almanac
    // for GPS PRN = N.
    if (pr0x88)  // GPS almanac request required
    {
      if ((eGPS.n ==
           ArTAlmnGPS[eGPS.n].num))  // if the received ephemeris of this
                                     // spacecraft is in the almanac array
      {                              // then we carry out data verification
        Eph_Alm_E = abs(ArTAlmnGPS[eGPS.n].e - eGPS.e);

        if (((abs(ArTAlmnGPS[eGPS.n].e - eGPS.e) > 0.001) ||
             (abs(ArTAlmnGPS[eGPS.n].af0 - eGPS.af0) > 0.0001))) {
          // skip era
          Writeln(LOG,
                  "EPH GPS ERROR! Сравниваем afo и eccentricity эфемериды и "
                  "альманаха, разница Eph_Alm_E = " +
                      floattostr(Eph_Alm_E) + ";");
          return;
        }
      }
    }

    // Filter 3
    // verification of received ephemeris with saved
    if (((eGPS.t_oe == ArTEphGPS[eGPS.n].t_oe) &&
         (eGPS.t_oc == ArTEphGPS[eGPS.n].t_oc) &&
         (eGPS.IODE == ArTEphGPS[eGPS.n].IODE) &&
         (eGPS.A == ArTEphGPS[eGPS.n].A))) {
      // skip era
      Writeln(LOG, "EPH GPS ERROR! повтор эфемерид GPS от КА№" +
                       inttostr(eGPS.n) + ";");
      return;
    }
    // IODC and IODE match check
    if ((eGPS.IODC != eGPS.IODE)) {
      // OknoTP.Lines.Add (&#39;unequal IODC and IODE GPS from КА№&#39; +
      // inttostr (eGPS.n));
      Writeln(LOG, "EPH GPS ERROR! неравны IODC и IODE GPS от КА№" +
                       inttostr(eGPS.n) + ";");
      return;
    }
    // ===================================================
    // ====================================

    // further conversion to a navigation file
    if (eGPS.n < 10)
      StNKA = "G0" + inttostr(eGPS.n);
    else
      StNKA = "G" + inttostr(eGPS.n);
    // GPS time
    dttm = (eGPS->t_oe + (604800 * (eGPS->weekN + 1024 * WRollOver))) * 1.0 /
               86400 +
           RegTimeGPS;  // StrToDateTime (&#39;06 .01.1980 &#39;);

    // Filter 4
    // if dttm is more than 3 hours from local time, then the actual ephemeris
    // is no longer
    if ((abs(TimeUTC - dttm) > 0.1))  // 0.125
    {
      Writeln(LOG, StNKA + " error GPS dttm; old ephemeris!");
      return;
    }

    // preservation of more recent ephemeris in the table
    ArTEphGPS[eGPS.n].n = eGPS.n;
    ArTEphGPS[eGPS.n].t_oe = eGPS.t_oe;
    ArTEphGPS[eGPS.n].t_oc = eGPS.t_oc;
    ArTEphGPS[eGPS.n].IODE = eGPS.IODE;
    ArTEphGPS[eGPS.n].IODC = eGPS.IODC;
    ArTEphGPS[eGPS.n].af0 = eGPS.af0;
    ArTEphGPS[eGPS.n].A = eGPS.A;
    // ArTEphGPS [eGPS.n] .dtrc: = eGPS.dtrc;

    // cure for incorrect eGPS ^ .weekN
    // if (abs (TimeUTC - dttm)&gt; (1.0 / 8.0)) then // 0.125
    // begin
    // dttm: = ((604800 * (eGPS ^ .weekN + 1 + 1024 * WRollOver))) / 86400 +
    // RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;); Writeln (LOG, StNKA +
    // &#39;error eGPS ^ .weekN!&#39;);
    // // exit;
    // end;
    DateTimeToSystemTime(dttm, TimeEpoch);

    StYear = inttostr(TimeEpoch.wYear);
    if (TimeEpoch.wMonth < 10)
      StMonth = "0" + inttostr(TimeEpoch.wMonth);
    else
      StMonth = inttostr(TimeEpoch.wMonth);
    if (TimeEpoch.wDay < 10)
      StDay = "0" + inttostr(TimeEpoch.wDay);
    else
      StDay = inttostr(TimeEpoch.wDay);
    if ((TimeEpoch.wHour) < 10)
      StHour = "0" + inttostr(TimeEpoch.wHour);
    else
      StHour = inttostr(TimeEpoch.wHour);
    if ((TimeEpoch.wMinute) < 10)
      StMinute = "0" + inttostr(TimeEpoch.wMinute);
    else
      StMinute = inttostr(TimeEpoch.wMinute);
    if ((TimeEpoch.wSecond) < 10)
      StSecond = "0" + inttostr(TimeEpoch.wSecond);
    else
      StSecond = inttostr(TimeEpoch.wSecond);

    // ===================================================
    // ====================================

    // SV / EPOCH / SV CLK
    StAF0 = FormatFloat("0.000000000000E+00", eGPS->af0, formatSettings);
    Staf1 = FormatFloat("0.000000000000E+00", eGPS->af1, formatSettings);
    Staf2 = FormatFloat("0.000000000000E+00", eGPS->af2, formatSettings);
    Writeln(F_RinMN, Format("%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s", [
              StNKA, StYear, StMonth, StDay, StHour, StMinute, StSecond, StAF0,
              Staf1, Staf2
            ]));
    // BROADCAST ORBIT - 1
    StIODE = FormatFloat("0.000000000000E+00", atof(inttostr(eGPS->IODE)),
                         formatSettings);
    StC_rs = FormatFloat("0.000000000000E+00", eGPS->C_rs, formatSettings);
    Stdn = FormatFloat("0.000000000000E+00", eGPS->dn * pi, formatSettings);
    StM = FormatFloat("0.000000000000E+00", eGPS->M * pi, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StIODE, StC_rs, Stdn, StM ]));
    // BROADCAST ORBIT - 2
    StC_uc = FormatFloat("0.000000000000E+00", eGPS->C_uc, formatSettings);
    Ste = FormatFloat("0.000000000000E+00", eGPS->e, formatSettings);
    StC_us = FormatFloat("0.000000000000E+00", eGPS->C_us, formatSettings);
    StA = FormatFloat("0.000000000000E+00", eGPS->A, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StC_uc, Ste, StC_us, StA ]));
    // BROADCAST ORBIT - 3
    Stt_oe = FormatFloat("0.000000000000E+00", eGPS->t_oe, formatSettings);
    StC_ic = FormatFloat("0.000000000000E+00", eGPS->C_ic, formatSettings);
    StOMEGA_0 =
        FormatFloat("0.000000000000E+00", eGPS->OMEGA_0 * pi, formatSettings);
    StC_is = FormatFloat("0.000000000000E+00", eGPS->C_is, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", Stt_oe, StC_ic, StOMEGA_0, StC_is ]));
    // BROADCAST ORBIT - 4
    StI0 = FormatFloat("0.000000000000E+00", eGPS->I0 * pi, formatSettings);
    StC_rc = FormatFloat("0.000000000000E+00", eGPS->C_rc, formatSettings);
    Stw = FormatFloat("0.000000000000E+00", eGPS->w * pi, formatSettings);
    StOMEGADOT =
        FormatFloat("0.000000000000E+00", eGPS->OMEGADOT * pi, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StI0, StC_rc, Stw, StOMEGADOT ]));
    // BROADCAST ORBIT - 5
    StIDOT = FormatFloat("0.000000000000E+00", eGPS->IDOT * pi, formatSettings);
    StCodesL2 = FormatFloat("0.000000000000E+00", 0.0, formatSettings);
    StweekN = FormatFloat("0.000000000000E+00", eGPS->weekN + 1024 * WRollOver,
                          formatSettings);  // + 1024 * wRollOver!
    StL2PdataF = FormatFloat("0.000000000000E+00", 0.0, formatSettings);
    Writeln(F_RinMN,
            Format("%4s%19s%19s%19s%19s",
                   [ "    ", StIDOT, StCodesL2, StweekN, StL2PdataF ]));
    // BROADCAST ORBIT - 6
    StSVaccur =
        FormatFloat("0.000000000000E+00", eGPS->SVaccur, formatSettings);
    StSVhelth =
        FormatFloat("0.000000000000E+00", eGPS->SVhelth, formatSettings);
    StTgd = FormatFloat("0.000000000000E+00", eGPS->Tgd, formatSettings);
    StIODC = FormatFloat("0.000000000000E+00", eGPS->IODC, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                            [ "    ", StSVaccur, StSVhelth, StTgd, StIODC ]));
    // BROADCAST ORBIT - 7
    // Stdtrc: = FormatFloat (&#39;0.000000000000E + 00&#39;, eGPS ^ .dtrc,
    // formatSettings);
    Stdtrc = FormatFloat(
        "0.000000000000E+00", eGPS->t_oe,
        formatSettings);  // return this entry in the standard version
    StFitinterval = FormatFloat("0.000000000000E+00", 0.0, formatSettings);
    Writeln(F_RinMN, Format("%4s%19s%19s", [ "    ", Stdtrc, StFitinterval ]));

    // OknoTP.Lines.Add (&#39;error messages 0x1A!&#39;);
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving a packet; updating GLONASS ephemeris values; recording a 2nd file
void TFormRNX::ExCm1B() {
  int ik;
  TEphGLN *eGLN;
  double dttm;
  time_t TimeEposh;
  std::string StYear;
  std::string StMonth;
  std::string StDay;
  std::string StHour;
  std::string StMinute;
  std::string StSecond;
  std::string StNKA;
  std::string Sttau;
  std::string Stgm;
  std::string StMesFTime;
  std::string StpX;
  std::string StvX;
  std::string StaX;
  std::string StFt;
  std::string StpY;
  std::string StvY;
  std::string StaY;
  std::string StLit;
  std::string StpZ;
  std::string StvZ;
  std::string StaZ;
  std::string StenE;
  double Date1;

  try {
    if ((FirstAnalysis == 1)) {
      pr0x1B = true;
      return;
    }

    eGLN = &ArDat[0];

    // Filter 1
    // if the time of reception of the ephemeris PRN = N is equal to 2008, then
    // they can be considered &quot;fake&quot; and ignored. Ephemeris Time (UTC)
    dttm = (eGLN->dtrc) * 1.0 / 86400 +
           RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
    DateTimeToSystemTime(
        dttm, TimeEposh);  // translate time from TDateTime to time_t
    if ((TimeEposh.wYear == 2008)) {
      Writeln(LOG,
              "EPH GLN ERROR! ошибка выделения времени TimeEposh.wYear = " +
                  inttostr(TimeEposh.wYear) + ";");
      return;
    }

    // Filter 2
    // if dttm is more than half an hour from local time, then the actual
    // ephemeris is no longer
    if ((abs(TimeUTC - dttm) > 0.02)) {
      Writeln(LOG, "R" + inttostr(eGLN.n) + " error GLN dttm; old ephemeris!");
      return;
    }

    if ((eGLN.n == 0) || (eGLN.prNNK > 0)) {
      Writeln(LOG,
              "EPH GLN ERROR! фильтрация GLN эфемерид с нулевыми значениями "
              "данных, eGLN.n = " +
                  inttostr(eGLN.n) + "; eGLN.prNNK = " + inttostr(eGLN.prNNK) +
                  ";");
      return;
    }

    // verification of received ephemeris with saved
    if (((eGLN.t_b == ArTEphGLN[eGLN.n].t_b) &&
         (eGLN.pX == ArTEphGLN[eGLN.n].pX) &&
         (eGLN.pY == ArTEphGLN[eGLN.n].pY) &&
         (eGLN.pZ == ArTEphGLN[eGLN.n].pZ))) {
      // skip era
      Writeln(LOG, "EPH GLN ERROR! повтор эфемерид GLN от КА№" +
                       inttostr(eGLN.n) + ";");
      return;
    }

    if ((RecEph_od == false)) {
      if ((ChDT == false)) {
        // saving ephemeris WITHOUT DATE in an array
        ArTEphGLN_od[eGLN.n].dtrc = eGLN.dtrc;
        ArTEphGLN_od[eGLN.n].N4t = eGLN.N4t;
        ArTEphGLN_od[eGLN.n].NA = eGLN.NA;
        ArTEphGLN_od[eGLN.n].pX = eGLN.pX;
        ArTEphGLN_od[eGLN.n].pY = eGLN.pY;
        ArTEphGLN_od[eGLN.n].pZ = eGLN.pZ;
        ArTEphGLN_od[eGLN.n].vX = eGLN.vX;
        ArTEphGLN_od[eGLN.n].vY = eGLN.vY;
        ArTEphGLN_od[eGLN.n].vY = eGLN.vZ;
        ArTEphGLN_od[eGLN.n].aX = eGLN.aX;
        ArTEphGLN_od[eGLN.n].aY = eGLN.aZ;
        ArTEphGLN_od[eGLN.n].aZ = eGLN.aZ;
        ArTEphGLN_od[eGLN.n].gm = eGLN.gm;
        ArTEphGLN_od[eGLN.n].tau = eGLN.tau;
        ArTEphGLN_od[eGLN.n].tauc = eGLN.tauc;
        ArTEphGLN_od[eGLN.n].taugps = eGLN.taugps;
        ArTEphGLN_od[eGLN.n].prNNK = eGLN.prNNK;
        ArTEphGLN_od[eGLN.n].pss = eGLN.pss;
        ArTEphGLN_od[eGLN.n].t_b = eGLN.t_b;
        ArTEphGLN_od[eGLN.n].enE = eGLN.enE;
        ArTEphGLN_od[eGLN.n].Ft = eGLN.Ft;
        ArTEphGLN_od[eGLN.n].n = eGLN.n;
        return;
      } else {
        // there was a sign of time allocation
        // then you need to rewrite all the entries in the array into a
        // navigation file

        // Ephemeris Time (UTC)
        dttm = (eGLN->dtrc) * 1.0 / 86400 +
               RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);
        DateTimeToSystemTime(
            dttm, TimeEposh);  // translate time from TDateTime to time_t
        // nullify wHour, wMinute, wSecond, wMSecond
        TimeEposh.wHour = 0;
        TimeEposh.wMinute = 0;
        TimeEposh.wSecond = 0;
        TimeEposh.wMilliseconds = 0;
        // we only get the date; translate the date from time_t to
        // TDateTime
        Date1 = SystemTimeToDateTime(TimeEposh);

        {
          long ik_end = 25;
          for (ik = 1; ik < ik_end; ++ik) {
            if ((ArTEphGLN_od[ik].n != 0)) {
              // preservation of more recent ephemeris in the table
              ArTEphGLN[ik].t_b = ArTEphGLN_od[ik].t_b;
              ArTEphGLN[ik].pX = ArTEphGLN_od[ik].pX;
              ArTEphGLN[ik].pY = ArTEphGLN_od[ik].pY;
              ArTEphGLN[ik].pZ = ArTEphGLN_od[ik].pZ;

              // further conversion to a navigation file
              if (ArTEphGLN_od[ik].n < 10)
                StNKA = "R0" + inttostr(ArTEphGLN_od[ik].n);
              else
                StNKA = "R" + inttostr(ArTEphGLN_od[ik].n);

              // Epoch: Toc - Time of Clock (UTC)
              dttm = ((ArTEphGLN_od[ik].t_b * 900 + 21 * 60 * 60) % 86400) *
                         1.0 / 86400 +
                     Date1;
              DateTimeToSystemTime(dttm, TimeEposh);

              StYear = inttostr(TimeEposh.wYear);
              if (TimeEposh.wMonth < 10)
                StMonth = "0" + inttostr(TimeEposh.wMonth);
              else
                StMonth = inttostr(TimeEposh.wMonth);
              if (TimeEposh.wDay < 10)
                StDay = "0" + inttostr(TimeEposh.wDay);
              else
                StDay = inttostr(TimeEposh.wDay);
              if ((TimeEposh.wHour) < 10)
                StHour = "0" + inttostr(TimeEposh.wHour);
              else
                StHour = inttostr(TimeEposh.wHour);
              if ((TimeEposh.wMinute) < 10)
                StMinute = "0" + inttostr(TimeEposh.wMinute);
              else
                StMinute = inttostr(TimeEposh.wMinute);
              if ((TimeEposh.wSecond) < 10)
                StSecond = "0" + inttostr(TimeEposh.wSecond);
              else
                StSecond = inttostr(TimeEposh.wSecond);

              // SV / EPOCH / SV CLK
              Sttau = FormatFloat("0.000000000000E+00", -ArTEphGLN_od[ik].tau,
                                  formatSettings);
              Stgm = FormatFloat("0.000000000000E+00", ArTEphGLN_od[ik].gm,
                                 formatSettings);
              StMesFTime = FormatFloat(
                  "0.000000000000E+00", ArTEphGLN_od[ik].t_b * 900,
                  formatSettings);  // Message frame time? tk = tb * 900
              Writeln(F_RinMN, Format("%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s", [
                        StNKA, StYear, StMonth, StDay, StHour, StMinute,
                        StSecond, Sttau, Stgm, StMesFTime
                      ]));
              // BROADCAST ORBIT - 1
              StpX = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].pX * 0.001, formatSettings);
              StvX = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].vX * 0.001, formatSettings);
              StaX = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].aX * 0.001, formatSettings);
              StFt = FormatFloat("0.000000000000E+00", ArTEphGLN_od[ik].prNNK,
                                 formatSettings);  // health (0 = OK) (Bn)
              Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                                      [ "    ", StpX, StvX, StaX, StFt ]));
              // BROADCAST ORBIT - 2
              StpY = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].pY * 0.001, formatSettings);
              StvY = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].vY * 0.001, formatSettings);
              StaY = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].aY * 0.001, formatSettings);
              StLit = FormatFloat(
                  "0.000000000000E+00", ArLitNumKA[ArTEphGLN_od[ik].n],
                  formatSettings);  // frequency number (-7 ... + 13)
              Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                                      [ "    ", StpY, StvY, StaY, StLit ]));
              // BROADCAST ORBIT - 3
              StpZ = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].pZ * 0.001, formatSettings);
              StvZ = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].vZ * 0.001, formatSettings);
              StaZ = FormatFloat("0.000000000000E+00",
                                 ArTEphGLN_od[ik].aZ * 0.001, formatSettings);
              StenE = FormatFloat(
                  "0.000000000000E+00", ArTEphGLN_od[ik].enE,
                  formatSettings);  // Age of oper. information (days) (E)
              Writeln(F_RinMN, Format("%4s%19s%19s%19s%19s",
                                      [ "    ", StpZ, StvZ, StaZ, StenE ]));
            }
          }
        }
        RecEph_od = true;
      }
    }

    // preservation of more recent ephemeris in the table
    ArTEphGLN[eGLN.n].t_b = eGLN.t_b;
    ArTEphGLN[eGLN.n].pX = eGLN.pX;
    ArTEphGLN[eGLN.n].pY = eGLN.pY;
    ArTEphGLN[eGLN.n].pZ = eGLN.pZ;

    // further conversion to a navigation file
    if (eGLN.n < 10)
      StNKA = "R0" + inttostr(eGLN.n);
    else
      StNKA = "R" + inttostr(eGLN.n);

    // Ephemeris Time (UTC)
    dttm = (eGLN->dtrc) * 1.0 / 86400 +
           RegTime;  // StrToDateTime (&#39;01 .01.2008.2008 &#39;);

    DateTimeToSystemTime(
        dttm, TimeEposh);  // translate time from TDateTime to time_t
    // nullify wHour, wMinute, wSecond, wMSecond
    TimeEposh.wHour = 0;
    TimeEposh.wMinute = 0;
    TimeEposh.wSecond = 0;
    TimeEposh.wMilliseconds = 0;
    // we only get the date; translate the date from time_t to TDateTime
    Date1 = SystemTimeToDateTime(TimeEposh);
    // Epoch: Toc - Time of Clock (UTC)
    dttm = ((eGLN->t_b * 900 + 21 * 60 * 60) % 86400) * 1.0 / 86400 + Date1;
    DateTimeToSystemTime(dttm, TimeEposh);

    StYear = inttostr(TimeEposh.wYear);
    if (TimeEposh.wMonth < 10)
      StMonth = "0" + inttostr(TimeEposh.wMonth);
    else
      StMonth = inttostr(TimeEposh.wMonth);
    if (TimeEposh.wDay < 10)
      StDay = "0" + inttostr(TimeEposh.wDay);
    else
      StDay = inttostr(TimeEposh.wDay);
    if ((TimeEposh.wHour) < 10)
      StHour = "0" + inttostr(TimeEposh.wHour);
    else
      StHour = inttostr(TimeEposh.wHour);
    if ((TimeEposh.wMinute) < 10)
      StMinute = "0" + inttostr(TimeEposh.wMinute);
    else
      StMinute = inttostr(TimeEposh.wMinute);
    if ((TimeEposh.wSecond) < 10)
      StSecond = "0" + inttostr(TimeEposh.wSecond);
    else
      StSecond = inttostr(TimeEposh.wSecond);

    // SV / EPOCH / SV CLK
    Sttau = FormatFloat("0.000000000000E+00", -eGLN->tau, formatSettings);
    Stgm = FormatFloat("0.000000000000E+00", eGLN->gm, formatSettings);
    StMesFTime =
        FormatFloat("0.000000000000E+00", eGLN->t_b * 900,
                    formatSettings);  // Message frame time? tk = tb * 900
    Writeln(F_RinMN, Format("%3s%5s%3s%3s%3s%3s%3s%19s%19s%19s", [
              StNKA, StYear, StMonth, StDay, StHour, StMinute, StSecond, Sttau,
              Stgm, StMesFTime
            ]));
    // BROADCAST ORBIT - 1
    StpX = FormatFloat("0.000000000000E+00", eGLN->pX * 0.001, formatSettings);
    StvX = FormatFloat("0.000000000000E+00", eGLN->vX * 0.001, formatSettings);
    StaX = FormatFloat("0.000000000000E+00", eGLN->aX * 0.001, formatSettings);
    StFt = FormatFloat("0.000000000000E+00", eGLN->prNNK,
                       formatSettings);  // health (0 = OK) (Bn)
    Writeln(F_RinMN,
            Format("%4s%19s%19s%19s%19s", [ "    ", StpX, StvX, StaX, StFt ]));
    // BROADCAST ORBIT - 2
    StpY = FormatFloat("0.000000000000E+00", eGLN->pY * 0.001, formatSettings);
    StvY = FormatFloat("0.000000000000E+00", eGLN->vY * 0.001, formatSettings);
    StaY = FormatFloat("0.000000000000E+00", eGLN->aY * 0.001, formatSettings);
    StLit = FormatFloat("0.000000000000E+00", ArLitNumKA[eGLN->n],
                        formatSettings);  // frequency number (-7 ... + 13)
    Writeln(F_RinMN,
            Format("%4s%19s%19s%19s%19s", [ "    ", StpY, StvY, StaY, StLit ]));
    // BROADCAST ORBIT - 3
    StpZ = FormatFloat("0.000000000000E+00", eGLN->pZ * 0.001, formatSettings);
    StvZ = FormatFloat("0.000000000000E+00", eGLN->vZ * 0.001, formatSettings);
    StaZ = FormatFloat("0.000000000000E+00", eGLN->aZ * 0.001, formatSettings);
    StenE = FormatFloat("0.000000000000E+00", eGLN->enE,
                        formatSettings);  // Age of oper. information (days) (E)
    Writeln(F_RinMN,
            Format("%4s%19s%19s%19s%19s", [ "    ", StpZ, StvZ, StaZ, StenE ]));

    // end;

    // OknoTP.Lines.Add (&#39;error messages 0x1B!&#39;);
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving the GPS ionosphere parameters packet
void TFormRNX::ExCm1C() {
  TIonParam *ion;

  try {
    ion = &ArDat[0];
    if ((pr0x1C == false)) {
      ArMesg1C[0] = FormatFloat("0.0000E+00", ion->a0, formatSettings);
      ArMesg1C[1] = FormatFloat("0.0000E+00", ion->a1, formatSettings);
      ArMesg1C[2] = FormatFloat("0.0000E+00", ion->a2, formatSettings);
      ArMesg1C[3] = FormatFloat("0.0000E+00", ion->a3, formatSettings);
      ArMesg1C[4] = FormatFloat("0.0000E+00", ion->b0, formatSettings);
      ArMesg1C[5] = FormatFloat("0.0000E+00", ion->b1, formatSettings);
      ArMesg1C[6] = FormatFloat("0.0000E+00", ion->b2, formatSettings);
      ArMesg1C[7] = FormatFloat("0.0000E+00", ion->b3, formatSettings);
      pr0x1C = true;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// packet reception GPS time reference parameters
void TFormRNX::ExCm1D() {
  TUTCParam *utc;

  try {
    utc = &ArDat[0];
    if ((pr0x1D == false)) {
      ArMesg1D[0] = FormatFloat("0.0000000000E+00", utc->a0, formatSettings);
      ArMesg1D[1] = FormatFloat("0.000000000E+00", utc->a1, formatSettings);
      ArMesg1D[2] = inttostr(utc->tot);
      ArMesg1D[3] = inttostr(utc->wnt);
      pr0x1D = true;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// packet reception GLO time reference parameters
void TFormRNX::ExCm1E() {
  T25GlnParam *gln25;

  try {
    gln25 = &ArDat[0];
    if ((pr0x1E == false)) {
      ArMesg1E[0] = "";
      ArMesg1E[0] =
          FormatFloat("0.0000000000E+00", gln25->tauc, formatSettings);
      ArMesg1E[1] = FormatFloat("0.000000000E+00", 0.0, formatSettings);
      ArMesg1E[2] =
          FormatFloat("0.0000000000E+00", gln25->taugps, formatSettings);
      pr0x1E = true;
    }

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receipt of a receipt for the package &quot;0xC1&quot;
void TFormRNX::ExCmC1() {
  char snChar;
  int snDig;
  std::string snStr;

  try {
    // serial number
    snChar = Chr(ArDat[2] >> 24);
    snDig = (ArDat[2] && 0xFFFFFF);
    // CS firmware
    // CS: = ArDat [3];

    snStr = inttostr(snDig);
    while ((Length(snStr) < 9)) snStr = "0" + snStr;
    // snStr: = snChar + snStr;

    // if no number
    if ((ArDat[2] == -1)) snStr = " N/A     ";

    // we get only the number
    verSt = inttostr(ArDat[0] >> 16) + "." + inttostr(ArDat[0] && 0xFFFF);

    StSN = " S/N:" + snStr;

    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// GPS almanac reception
void TFormRNX::ExCm88() {
  TAlmnGPS *aGPS;

  try {
    if ((FirstAnalysis == 1)) {
      pr0x88 = true;
    }

    aGPS = &ArDat[0];
    if ((aGPS.num != 0) && (aGPS.num < 33))
      ArTAlmnGPS[aGPS.num] = aGPS /*?*/ ^ ;

    // wrong package
    //
  } catch (...) {
  }
}
/*----------------------------------------------------------------------------*/
// receiving a packet to turn on the receiver
void TFormRNX::ExCm3E() {
  ++num3E;
  if ((num3E > 2)) {
    StPac10 =
        "> " +
        Format(
            "%4s%3s%3s%3s%3s%11s",
            [ StYear10, StMonth10, StDay10, StHour10, StMinute10, StSecond10 ],
            formatSettings);
    Writeln(LOG, StPac10 + ": ERROR! RESTART G5!!!");
    num3E = 0;
  }
}

/*----------------------------------------------------------------------------*/
void TFormRNX::FormClose(TObject Sender, TCloseAction &Action) {
  Reg = TRegistry.Create;
  Reg.RootKey = HKEY_CURRENT_USER;
  Reg.OpenKey("Software\\GeostarNavigation\\GEOS5ToRNX", true);
  Reg.WriteInteger("Top", Top);
  Reg.WriteInteger("Left", Left);
  Reg.WriteInteger("RGSV", RGSV.ItemIndex);
  Reg.CloseKey;
  Reg.Free;
}
void TFormRNX::FormCreate(TObject Sender) {
  int ik;

  formatSettings.DecimalSeparator = ".";
  AppDir = Application.ExeName;
  FReg = TRegistry.Create;
  FReg.RootKey = HKEY_CURRENT_USER;
  if ((FReg.KeyExists("Software\\GeostarNavigation\\GEOS5ToRNX") == true)) {
    FReg.OpenKey("Software\\GeostarNavigation\\GEOS5ToRNX", true);
    FormRNX.Top = FReg.ReadInteger("Top");
    FormRNX.Left = FReg.ReadInteger("Left");
    try {
      RGSV.ItemIndex = FReg.ReadInteger("RGSV");
    } catch (...) {
      RGSV.ItemIndex = 0;
    }
    RGSVClick(00);
    FReg.CloseKey;
    FReg.Free;
  }
  {
    long ik_end = 25;
    for (ik = 1; ik < ik_end; ++ik) ArLitNumKA[ik] = 100;
  }
  FillChar(ArTEphGPS, sizeof(ArTEphGPS), 0);
  FillChar(ArTEphGLN, sizeof(ArTEphGLN), 0);
  FillChar(el, sizeof(el), 0);
}

void TFormRNX::RGSVClick(TObject Sender) {
  if ((RGSV.ItemIndex == 0)) {
    Label1.Visible = false;
    EditElMask.Visible = false;
  } else {
    Label1.Visible = true;
    EditElMask.Visible = true;
  }
}

/*----------------------------------------------------------------------------*/

// END
