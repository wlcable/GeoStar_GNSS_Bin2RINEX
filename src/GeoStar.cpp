/**   @file
     @brief
*/
#include "GeoStar.h"

namespace GeoStar
{

  /*----------------------------------------------------------------------------*/
  // verification of spacecraft number as accepted. spacecraft number, letter and
  // visibility
  int CheckNum(u_int8_t pNum)
  {
    int result;
    u_int8_t ik;

    try
    {
      result = 0;
      {
        long ik_end = 25;
        for (ik = 1; ik < ik_end; ++ik)
          if ((pNum == ArLitNumKA[ik]))
            result = ik;
      } // get the spacecraft number
    }
    catch (...)
    {
    }
    return result;
  }
  /*---------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------*/
  //Init Routine
  // sets GeoStar & GPS Base Times
  // resets flags
  // resets a few variables to default
  void Init()
  {
    int ik;
    setenv("TZ","UTC",true); //This fixes the 1 hour offset I was having on my Windows system!

    // initialization of variables
    // 01/01/2008 GeoStar Base Time
    BTGeo.tm_year = 2008 - 1900;
    BTGeo.tm_mon = 0;
    BTGeo.tm_mday = 1;
    BTGeo.tm_hour = 0; 
    BTGeo.tm_min = 0;
    BTGeo.tm_sec = 0;
    BTGeo.tm_gmtoff = 0;
    BTGeo.tm_isdst = 0;
    BaseTimeGeo = mktime(&BTGeo);

    // 06/01/1980 GPS Base Time
    BTGPS.tm_year = 1980 - 1900;
    BTGPS.tm_mon = 0;
    BTGPS.tm_mday = 6;
    BTGPS.tm_hour = 0; 
    BTGPS.tm_min = 0;
    BTGPS.tm_sec = 0;
    BTGPS.tm_gmtoff = 0;
    BTGPS.tm_isdst = 0;
    BaseTimeGPS = mktime(&BTGPS);

    for (ik = 0; ik < GLNprn + 1; ++ik)
      ArLitNumKA[ik] = 100;
    for (ik = 0; ik < GPSprn + 1; ++ik)
      ArTAlmnGPS[ik].num = 100;

    pr0x14 = false;
    pr0x18 = false;
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
    pr0x21 = false;
    pr0x20 = false;
    pr0x22 = false;
    pr0x23 = false;
    pr0x24 = false;

    errCS10 = false;
    errCS13 = false;

    LeapSec = 0;
    ClockOffset = 0;
    WRollOver = 2; //default is 2 until 2038

    RecEph_od = false;
    return;
  }

  /*---------------------------------------------------------------------------*/
  //Init Logging File
  void InitLogging()
  {
    //Get System time UTC and format string
    time(&systime);
    struct tm SysTimeUTC = *gmtime(&systime);
    char StSysTimeUTC[21];
    sprintf(StSysTimeUTC, "%04u%02u%02u %02u%02u%02u UTC", SysTimeUTC.tm_year + 1900, SysTimeUTC.tm_mon + 1, SysTimeUTC.tm_mday,
            SysTimeUTC.tm_hour, SysTimeUTC.tm_min, SysTimeUTC.tm_sec);

    // Open the Log File
    sprintf(buff, "%s%s", outpath, "GeoRINEX.log");
    FLog.open(buff, ios::out | ios::app);
    FLog << "GeoSbin2RINEX Log File " << StSysTimeUTC << "\n";
    return;
  }

  /*---------------------------------------------------------------------------*/
  //Init Logging File
  void InitOutFiles()
  {
    // generating a file creation time line and filename base
    int8_t year = (FTime.tm_year + 1900 - 2000);
    if (year < 0)
      year = 0;

    strftime(StCreateFileTime, 22, "%Y%m%d %H%M%S GPS", &FTime);

    // creating rinex files for Obs. and Nav.
    sprintf(buff, "%s%s_R_%04u%03u%02u%02u_%cO.rnx",
            outpath, StationID, FTime.tm_year+1900, FTime.tm_yday + 1, FTime.tm_hour, FTime.tm_min, obsType);
    F_RinMO.open(buff, ios::out | ios::trunc);
    sprintf(buff, "%s%s_R_%04u%03u%02u%02u_%cN.rnx",
            outpath, StationID, FTime.tm_year+1900, FTime.tm_yday + 1, FTime.tm_hour, FTime.tm_min, obsType);
    F_RinMN.open(buff, ios::out | ios::trunc);

    //Prep a file of the In View / Active SVs
    if (SVinfoLOGGING)
    {
      sprintf(buff, "%s%.4s%03u0.%02u.snr%02u",
            outpath, StationID, FTime.tm_yday + 1, FTime.tm_year+1900-2000, 88);
      F_SVinfo.open(buff, ios::out | ios::trunc);
      //F_SVinfo << "GNSS In View / Active Satellite Vehicles\n";
      // sprintf(buff, "TIME OF FIRST OBS: %4u-%02u-%02u %02u:%02u:%02.4f%4s\n",
      //         (FTime.tm_year + 1900), FTime.tm_mon + 1, FTime.tm_mday, FTime.tm_hour, FTime.tm_min, (float)FTime.tm_sec,
      //         "GPS");
      // F_SVinfo << buff;
      // sprintf(buff, "%s,%s,%s,%s,%s\n",
      //         "seconds", "SVN", "Azimuth[deg]", "ElevationAngle[deg]", "CNR[dBHz]");
      // F_SVinfo << buff;
    }

    //Get System time UTC and format string
    time(&systime);
    struct tm SysTimeUTC = *gmtime(&systime);
    char StSysTimeUTC[21];
    sprintf(StSysTimeUTC, "%04u%02u%02u %02u%02u%02u UTC", SysTimeUTC.tm_year + 1900, SysTimeUTC.tm_mon + 1, SysTimeUTC.tm_mday,
            SysTimeUTC.tm_hour, SysTimeUTC.tm_min, SysTimeUTC.tm_sec);

    // HEADER - GNSS Navigation Message File -
    sprintf(buff, "%9.2f           N: GNSS NAV DATA    %c                   RINEX VERSION / TYPE\n",
            verRINEX, obsType);
    F_RinMN << buff;
    sprintf(buff, "%-20s%-20s%-20sPGM / RUN BY / DATE\n", CreationProgram, CreationAgency, StSysTimeUTC);
    F_RinMN << buff;
    if (pr0x1C)
    {
      sprintf(buff, "%4s %12.4E%12.4E%12.4E%12.4E%23s\n", "GPSA", ion.a0, ion.a1, ion.a2,
              ion.a3, "IONOSPHERIC CORR");
      F_RinMN << buff;
      sprintf(buff, "%4s %12.4E%12.4E%12.4E%12.4E%23s\n", "GPSB", ion.b0, ion.b1, ion.b2,
              ion.b3, "IONOSPHERIC CORR");
      F_RinMN << buff;
    }
    if (pr0x1D)
    {
      sprintf(buff, "%4s %17.10E%16.9E%7i%5i %5s %2d %s\n", "GPUT", utc.a0, utc.a1, utc.tot,
              utc.wnt, "", 0, "TIME SYSTEM CORR");
      F_RinMN << buff;
      sprintf(buff, "%6d%6d%6d%6d%35s LEAP SECONDS\n",
              LeapSec, utc.dtlsf, utc.wnlsf, utc.dn, "");
    }
    F_RinMN << buff;
    sprintf(buff, "%73s\n", "END OF HEADER");
    F_RinMN << buff;
    F_RinMN.flush();
    //end header Nav

    // HEADER - GNSS Observation Data File
    sprintf(buff, "%9.2f           OBSERVATION DATA    %c                   RINEX VERSION / TYPE\n",
            verRINEX, obsType);
    F_RinMO << buff;
    sprintf(buff, "%-20s%-20s%-20sPGM / RUN BY / DATE\n", CreationProgram, CreationAgency, StSysTimeUTC);
    F_RinMO << buff;
    sprintf(buff, "%-60sMARKER NAME\n", MarkerName);
    F_RinMO << buff;
    if (MarkerNumber[0] != '\0')
    {
      sprintf(buff, "%-60sMARKER NUMBER\n", MarkerNumber);
      F_RinMO << buff;
    }
    sprintf(buff, "%-60sMARKER TYPE\n", MarkerType);
    F_RinMO << buff;
    sprintf(buff, "%-20s%-40sOBSERVER / AGENCY\n", NameObserver, NameAgency);
    F_RinMO << buff;
    sprintf(buff, "%-20s%-20s%-20sREC # / TYPE / VERS\n", StSN, StRecType, verSt);
    F_RinMO << buff;
    sprintf(buff, "%-20s%-20s%-20sANT # / TYPE\n", Antenna.num, Antenna.type, "");
    F_RinMO << buff;
    sprintf(buff, "%14.4f%14.4f%14.4f%37s\n", coorX, coorY, coorZ, "APPROX POSITION XYZ");
    F_RinMO << buff;
    sprintf(buff, "%14.4f%14.4f%14.4f%38s\n", Antenna.dH, Antenna.dE, Antenna.dN, "ANTENNA: DELTA H/E/N");
    F_RinMO << buff;
    if (G_OBS)
      F_RinMO << "G    4 C1C D1C L1C S1C                                      SYS / "
              << "# / OBS TYPES\n";
    if (R_OBS)
      F_RinMO << "R    4 C1C D1C L1C S1C                                      SYS / "
              << "# / OBS TYPES\n";
    if (E_OBS)
      F_RinMO << "E    4 C1X D1X L1X S1X                                      SYS / "
              << "# / OBS TYPES\n";
    if (J_OBS)
      F_RinMO << "J    4 C1C D1C L1C S1C                                      SYS / "
              << "# / OBS TYPES\n";
    F_RinMO << "DBHZ                                                        SIGNAL "
            << "STRENGTH UNIT\n";
    sprintf(buff, "%6u    %02u    %02u    %02u    %02u    %02.7f%7s%26s\n", (FTime.tm_year + 1900), FTime.tm_mon + 1, FTime.tm_mday, FTime.tm_hour, FTime.tm_min, (float)FTime.tm_sec,
            "GPS", "TIME OF FIRST OBS");
    F_RinMO << buff;
    if (pr0x1D)
    {
      sprintf(buff, "%6d%6d%6d%6d%35s LEAP SECONDS\n",
              LeapSec, utc.dtlsf, utc.wnlsf, utc.dn, "");
      F_RinMO << buff;
    }
    sprintf(buff, "%73s\n", "END OF HEADER");
    F_RinMO << buff;
    F_RinMO.flush();
    //end header Obs
  }

  /*---------------------------------------------------------------------------*/
  //Close the output files
  void CloseOutFiles()
  {
    if (F_SVinfo.is_open())
      F_SVinfo.close();
    F_RinMO.close();
    F_RinMN.close();
  }

  /*---------------------------------------------------------------------------*/
  //Checks the Receiver Status Word and outputs errors to log file
  void RSWcheck()
  {
    if (FLog.is_open())
    {
      struct tm TimeEposh = *gmtime(&TimeUTC);
      if ((RSW.SRAMtest == false) && (RSWout.SRAMtest == false))
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": Backup SRAM test failed");
        FLog << buff;
        RSWout.SRAMtest = true;
      }
      if ((RSW.RTCtest == false) && (RSWout.RTCtest == false))
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": RTC test failed");
        FLog << buff;
        RSWout.RTCtest = true;
      }
      if ((RSW.rfPLLstatus == false) && (RSWout.rfPLLstatus == false))
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": RF PLL status failed");
        FLog << buff;
        RSWout.rfPLLstatus = true;
      }
      if ((RSW.GPS_AGC == false) && (RSWout.GPS_AGC == false))
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": GPS AGC failed");
        FLog << buff;
        RSWout.GPS_AGC = true;
      }
      if ((RSW.GLN_AGC == false) && (RSWout.GLN_AGC == false))
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": GLONASS AGC failed");
        FLog << buff;
        RSWout.GLN_AGC = true;
      }
      if ((RSW.AntStatus < 3) && (RSWout.AntStatus == 0))
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s %i\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": Antenna status =", RSW.AntStatus);
        FLog << buff;
        RSWout.AntStatus = 1;
      }
    }
    return;
  }

  /*----------------------------------------------------------------------------*/
  // Calculate the checksum
  u_int32_t CScalc(u_int32_t *dat, u_int16_t ncmd, u_int16_t ndat)
  {
    //CS calculation
    u_int32_t CSpac = preamble[0] ^ preamble[1] ^ ((ndat << 16) + ncmd); //init the CS
    for (int i = 0; i < ndat; ++i)
    {
      CSpac = CSpac ^ dat[i];
    }
    return CSpac;
  }

  /*----------------------------------------------------------------------------*/
  //This function accepts a binary file from the GeoS GNSS and decodes all the messages,
  //  saving them into RINEX files
  void BINfile2RINEX(char *bfName)
  {
    int ik;

    Init(); //sets and resets flags and some variables

    //Open the Log file if Logging is enabled
    if (LOGGING)
      InitLogging();

    //loop through the file twice, first to get enough info to init
    //then a second time to write the RINEX files
    for (FirstAnalysis = 1; FirstAnalysis < 3; FirstAnalysis++)
    {
      // Open the Binary File
      FGeosBin.open(bfName, ios::binary | ios::in); //Open file with pointer at end, AssignFile(F1, FNEdit.FileName);
      if (!FGeosBin.is_open())
      {
        FLog << "Could not open file " << bfName << "\n";
        return;
      }
      if (FLog.is_open() && (FirstAnalysis == 2))
      {
        FLog << "Opened file " << bfName << "\n";
      }

      GinFix = false;
      ChDT = false;
      pc10dtUTC_OLD = 0;

      // reading data from a file
      while (!FGeosBin.eof())
      {
        // if the required packages are accepted, then go to the second scroll of
        // the file
        if ((pr0x10 || pr0x0F) && (FirstAnalysis == 1))
        { //this should be adjusted so not everything is required if not all sat. systems in use
          if ((pr0x14 && pr0x1C && pr0x1D && pr0x1E && pr0x13 && pr0x1A &&
               pr0x1B))
          {
            break; //go to second scroll of file
          }
        }

        //Universial search / read preamble
        for (ik = 0; ik < 2; ik++)
        {
          if (!FGeosBin.read((char *)&word, 4)) //fill word buffer
            break;                              //EOF or read error
          while ((word != preamble[ik]) && !FGeosBin.eof())
          {
            //read byte by byte looking for match
            if (!FGeosBin.read((char *)&Ch, 1))
              break;
            word = word >> 8 | Ch << 24; //shift in the new char
          }
          if (FGeosBin.eof())
            break;
        }

        if (FGeosBin.eof()) //check for EOF
          break;

        { //file synced, preamble read
          if (FGeosBin.read((char *)&word, 4))
          { //read the message ID and message length
            ncmd = word;
            ndat = word >> 16;
          }
          else
          {
            break; //EOF or read error
          }

          //Read the message and calc CS within each message specific function
          CSpass = true;
          mLength = true;
          switch (ncmd)
          {
          case 0x0F:
          {
            ExCm0F();
            break;
          }
          case 0x10:
          {
            ExCm10();
            break;
          }
          case 0x13:
          {
            if (ndat == 32)
            {
              ExCm13();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x14:
          {
            if (ndat == 12)
            {
              ExCm14();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x0A:
          {
            if (ndat == 32)
            {
              ExCm0A();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x18:
          {
            if (ndat == 20)
            {
              ExCm18();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x19:
          {
            if (ndat == 18)
            {
              ExCm19();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x1A:
          {
            if (ndat == 32)
            {
              ExCm1A();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x1B:
          {
            if (ndat == 30)
            {
              ExCm1B();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x1C:
          {
            if (ndat == 8)
            {
              ExCm1C();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x1D:
          {
            if (ndat == 8)
            {
              ExCm1D();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x1E:
          {
            if (ndat == 10)
            {
              ExCm1E();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x20:
          {
            if (ndat == 28)
            {
              ExCm20();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x21:
          {
            if (ndat == 8)
            {
              ExCm21();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x22:
          {
            ExCm22();
            break;
          }
          case 0x23:
          {
            if (ndat == 29)
            {
              ExCm23();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x24:
          {
            if (ndat == 34)
            {
              ExCm24();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0xC1:
          {
            if (ndat == 4)
            {
              ExCmC1();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0xC3:
          {
            if (ndat == 1)
            {
              ExCmC3();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x3E:
          {
            if (ndat == 3)
            {
              ExCm3E();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          case 0x3F:
          {
            if (ndat == 2)
            {
              ExCm3F();
            }
            else
            {
              mLength = false;
            }
            break;
          }
          default:
          {
            if (FLog.is_open())
            {
              FLog << "MSG not recognized 0x" << std::hex << ncmd << "\n";
            }
            break;
          }
          }

          //Write to log file
          if (!CSpass & FLog.is_open())
          {
            FLog << "CS failed 0x" << std::hex << ncmd << "\n";
          }
          //Write to log file
          if (!mLength & FLog.is_open())
          {
            FLog << "Wrong number of package parameters 0x" << hex << ncmd << ", len = " << dec << ndat << "\n";
          }
        } //preable found
      }   //end file read loop

      FGeosBin.close(); //close the BIN file

      if (FirstAnalysis == 2)
      { //finish up, close files, skip the rest of the file
        //Write error messages for missing messages
        if (FLog.is_open())
        {
          // checking the content of important messages in the log file
          if (pr0x14 == false)
            FLog << "No messages 0x14 (Timing parameters)!\n"; // Leap seconds
          if (pr0x19 == false && pc21.GLNen)
            FLog << "No messages 0х19, 0x89 (GLONASS almanac)!\n"; // for letters
          if (pr0x1C == false && pc21.GPSen)
            FLog << "No messages 0x1C, 0x9C (GPS ionospheric parameters)!\n";
          if (pr0x1D == false && pc21.GPSen)
            FLog << "No messages 0x1D, 0x9D (GPS time to UTC conversion parameters)!\n";
          if (pr0x1E == false && pc21.GLNen)
            FLog << "No messages 0x1E, 0x9E (GLONASS time to UTC conversion parameters)!\n";
          // 0x1F, 0x9F: GST-UTC Conversion Parameters
          if (((pr0x10 != true) && (pr0x0F != true)))
            FLog << "No messages 0x10 (Raw measurements)!\n";
          if (pr0x13 == false)
            FLog << "No messages 0x13 (Navigation solution state vector)!\n";
          if (pr0x0A == false && pc21.GALen)
            FLog << "No messages 0x0A, 0x9A (GALILEO ephemeris)!\n";
          if (pr0x1A == false && pc21.GPSen)
            FLog << "No messages 0x1A, 0x8A (GPS/QZSS ephemeris)!\n";
          if (pr0x1B == false && pc21.GLNen)
            FLog << "No messages 0x1B, 0x8B (GLONASS ephemeris)!\n";
          if (pr0x18 == false && pc21.GPSen)
            FLog << "No messages 0x18, 0x88 (GPS/QZSS almanac)!\n"; // for filter 2
          FLog.flush();
        }
        if (FLog.is_open())
        {
          FLog << "Closed file " << bfName << "\n";
          FLog << "----------------------------------------\n";
          FLog.close();
        }
        CloseOutFiles();
        break; //skip the rest
      }

      //Prep. for the Second Scroll through the file

      //  opening files, writing headers
      InitOutFiles();

    } //end FirstAnalysis Loop, 2 times through
    return;
  } //Bin2Rinex

  //Some simple tests to see which Sat System the measurements are from
  //GPS measurements?
  bool svGPS(uint8_t *idnum)
  {
    if ((0 < *idnum) && (*idnum < 33))
      return true;
    else
      return false;
  }
  //GLONASS Measurements?
  bool svGLN(uint8_t *idnum)
  {
    if ((64 < *idnum) && (*idnum < 89))
      return true;
    else
      return false;
  }
  //Gal Measurements?
  bool svGAL(uint8_t *idnum)
  {
    if ((100 < *idnum) && (*idnum < 137))
      return true;
    else
      return false;
  }
  //QZSS Measurements?
  bool svQZSS(uint8_t *idnum)
  {
    if ((192 < *idnum) && (*idnum < 198))
      return true;
    else
      return false;
  }

  /*----------------------------------------------------------------------------*/
  // SVcalc
  // -Creates the String SV name
  // -calculates conversion from Pseudorange rate to Doppler, kof
  void SVcalc(uint8_t *idnum, char *StSatID, double *kof, int8_t *Hn)
  {
    // add spacecraft number to point
    // GPS
    if (svGPS(idnum))
    {
      sprintf(StSatID, "G%02u", *idnum);
      *kof = Fn_GPS * 1.0 / SL;
      G_OBS = true; // GPS OBS
    }
    // GLN
    if (svGLN(idnum))
    {
      sprintf(StSatID, "R%02u", *idnum - 64);
      *kof = (Fn_GLN + (*Hn * dFn_GLN)) * 1.0 / SL;
      R_OBS = true; // GLN
    }
    // Gal
    if (svGAL(idnum))
    {
      sprintf(StSatID, "E%02u", *idnum - 100);
      *kof = Fn_GPS * 1.0 / SL;
      E_OBS = true; // Gal
    }
    // QZSS
    if (svQZSS(idnum))
    {
      sprintf(StSatID, "J%02u", *idnum - 192);
      *kof = Fn_GPS * 1.0 / SL;
      J_OBS = true; // QZSS
    }
    return;
  }

  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  // 0x20: Geographic Coordinates: Basic Dataset
  // receiving the package &quot;consumer coordinates&quot;
  bool ExCm20()
  {
    //time_t TimeEpochUTC;
    CSpass = true;

    if (FGeosBin.read((char *)&cr20, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&cr20, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }
    pr0x20 = true;

    //Copy the RSW
    RSW = cr20.RSW;
    RSWcheck();

    // a sign of the decision of the NC and a sign of the reliability of the
    // decision and a sign of allocating time and date
    if (cr20.RSW.FIX && (cr20.PosFixVal == 0)) // Fix
    {
      GinFix = true;
      TimeUTC = ((cr20.DtTmUTC + LeapSec) + BaseTimeGeo);
      ChDT = true;
    }
    else
    {
      GinFix = false;
      ChDT = false;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x21: Receiver Telemetry
  //
  bool ExCm21()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&pc21, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc21, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    if (pr0x21 == false)
    { //first 0x21 message, check config and RSW
      //output config to log file
      if (FLog.is_open())
      {
        struct tm TimeEposh = *gmtime(&TimeUTC);
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": Receiver Configuration");
        FLog << buff;
        FLog << "  Systems Enabled: ";
        if (pc21.GLNen)
          FLog << "GLONASS ";
        if (pc21.GPSen)
          FLog << "GPS ";
        if (pc21.GALen)
          FLog << "GALILEO ";
        if (pc21.QZSSen)
          FLog << "QZSS ";
        if (pc21.SBASen)
          FLog << "SBAS ";
        //Output data rate
        switch (pc21.rate)
        {
        case 0:
          FLog << "10Hz ";
          break;
        case 1:
          FLog << "5Hz ";
          break;
        case 2:
          FLog << "2Hz ";
          break;
        case 3:
          FLog << "1Hz ";
          break;
        default:
          break;
        }
        if (pc21.Psy0x10)
          FLog << "0x10:smoothed ";
        else
          FLog << "0x10:unsmoothed ";
        if (pc21.AntPower)
          FLog << "AntPower:ON ";
        else
          FLog << "AntPower:OFF ";
        switch (pc21.CoorSys)
        {
        case 0:
          FLog << "CoorSys:WGS-84 ";
          break;
        case 1:
          FLog << "CoorSys:PZ-90.11 ";
          break;
        case 2:
          FLog << "CoorSys:User ";
          break;
        default:
          break;
        }

        FLog << "\n";
        //FLog << "  ReceiverType:" << hex << pc21.ReceiverType << "\n";
        switch (pc21.ReceiverType)
        {
        case 0xF7FF:
          sprintf(StRecType, "GeoS-5M");
          break;
        case 0xF7FE:
          sprintf(StRecType, "GeoS-5MR");
          break;
        case 0xF7FD:
          sprintf(StRecType, "GeoS-5MH");
          break;
        default:
          sprintf(StRecType, "unknown");
          break;
        }
      }
    }

    pr0x21 = true;

    //Copy the RSW
    RSW = pc21.RSW;
    RSWcheck();

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x24: RTK Output Dataset: Geographic Coordinates
  // receiving the consumer coordinates RTK packet
  bool ExCm24()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&pc24, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc24, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }
    pr0x24 = true;

    if ((pc24.TypeSolve > 0)) // Fix
    {
      GinFix = true;
      TimeUTC = ((pc24.DtTmUTC + LeapSec) + BaseTimeGeo);
      ChDT = true;
    }
    else
    {
      GinFix = false;
      ChDT = false;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  //  0x23: RTK Output Dataset: ECEF Coordinates
  bool ExCm23()
  {
    // std::string St;
    // double a1;
    CSpass = true;

    if (FGeosBin.read((char *)&pc23, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc23, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }
    pr0x23 = true;

    // FileSeek (hgups, 0.2);
    // a1: = ((pc23 ^ .DtTmUTC + LeapSec) / 86400 + 39448) ;; FileWrite (hgups,
    // a1.8); // number of restarts a1: = pc23.TypeSolve; FileWrite (hgups,
    // a1.8);

    // wrong package
    // OknoTP.Lines.Add (&#39;Msg (23): error&#39;);

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x22: In View/Active SVs
  // reception of visible spacecraft
  bool ExCm22()
  {
    uint16_t ik;
    double tel; //elevation in deg.
    double taz; //azmuth in deg.
    uint8_t num;
    uint16_t num2; //SV num according to K. Larson format
    double Dsec;   //seconds into the current day
    double edot;   //rate of change in elevation deg/sec

    CSpass = true;

    if (FGeosBin.read((char *)&pc22, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (ndat != ((5 * pc22.NSat) + 1)) //check message length
    {
      mLength = false;
      return false;
    }

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
    {
      return false; //read failed or EOF
    }

    if (word != CScalc((u_int32_t *)&pc22, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }
    pr0x22 = true;

    for (ik = 0; ik < pc22.NSat; ++ik)
    {
      // Syst: = (pc22 ^ .vs [ik] .ss and $ 700) shr 8; // GPS = 1, GLN = 0
      num = pc22.vs[ik].idnm;                    // receiving spacecraft number
      tel = 180.0 * (pc22.vs[ik].el) * 1.0 / pi; // select mind
      taz = 180.0 * (pc22.vs[ik].aZ) * 1.0 / pi; // we select AZ

      // infix: = (pc22.vs [ik] .ss and $ 20,000,000); // sign of using the
      // spacecraft in the solution
      if ((num > 0) && (num < 200) && (tel > 0) && (tel < 90) &&
          (taz > 0) && (taz < 360) && (pc22.vs[ik].CNR > 0.1) &&
          (pc22.vs[ik].ss != 0))
      {
        //calculate the rate of change in elevation, deg/sec
        edot = (el[num] - tel) / (difftime(Last22, TimeUTC));

        el[num] = tel; //save el value

        //output SV info to file
        //changing the format here to match that of M. Larson
        // https://github.com/kristinemlarson/gnssSNR
        /* 
        1 Satellite number (see **)
        2 Elevation angle, degrees
        3 Azimuth angle, degrees
        4 Seconds of the day, GPS time
        5 elevation angle rate of change, degrees/sec.
        6  S6
        7  S1
        8  S2
        9  S5
        10 S7
        11 S8

        *columns 8-11 were truncated to save space because they are always 0 

        Units of the SNR observables are the same as the native RINEX file

        **
        Satellites are named as follows:
        GPS 1-99
        Glonass 101-199
        Galileo 201-299
        Beidou 301-399
 */
        if (F_SVinfo.is_open() && ChDT)
        {
          //translate the SV num
          if (svGPS(&num))
          { //GPS is the same
            num2 = num;
          }
          else if (svGLN(&num))
          { //GLONASS
            num2 = num - 64 + 100;
          }
          else if (svGAL(&num))
          { //GALILEO
            num2 = num + 100;
          }
          else
          { // everything else is skipped
            continue;
          }
          Dsec = difftime(TimeUTC, DaySec); //seconds into the current day
          //I have changed the format slightly to removed unused observables from the last columns, to make the files smaller
          sprintf(buff, "%3u %10.4f %10.4f %10.1f %10.6f %1u %7.2f\n",
                  num2, tel, taz, Dsec, edot, 0, pc22.vs[ik].CNR);
          F_SVinfo << buff;
        }
      }
    }                 // end of cycle for all spacecraft
    Last22 = TimeUTC; //save the time of this message

    // wrong package
    // OknoTP.Lines.Add (&#39;Msg (22): error&#39;);
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // receiving a packet with measuring information 1st file recording
  bool ExCm0F()
  {
    int ik;
    time_t dttm;
    //double DbSecMSec; //milliseconds
    struct tm TimeEposh;
    char StNKA[5];
    double kof;
    bool LLI = false;           //loss-of-lock indicator
    char StLLI[2];              //string loss-of-lock indicator
    uint8_t SVsON;              //Num SV in obs
    bool SVpass[maxChan] = {0}; //should the SV obs be output?
    bool FlagElMask;

    // Reading the data from the stream
    CSpass = true;

    if (FGeosBin.read((char *)&pc0F, ndat * 4)) //read the message
    {
    }
    else
      return false; //read failed or EOF

    if (ndat != ((20 * pc0F.NMSat) + 6)) //check message length
    {
      mLength = false;
      return false;
    }

    if (FGeosBin.read((char *)&word, 4)) //read the CS
    {
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc0F, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    errCS10 = false;
    if (pr0x10)
      return true;

    kof = 1;

    ClockOffset = pc0F.ClockOffGPS; // ShV receiver shift relative to ShV GPS in meters

    LeapSec = pc0F.LeapSec;

    if ((FirstAnalysis == 1))
    {
      if (pr0x0F == false)
      {
        if (((ChDT == true) && (pc0F.NMSat > 0)))
        {
          if (((LeapSec != 0) && (ClockOffset > -400) &&
               (ClockOffset < 400))) // filter like in package 0x13 during
                                     // recording era in OBS file
          {
            // save the time of the first measurement
            // (pc0F.dtUTC-1) since decision flag comes after 0x10 packet
            // getting the first measurement time for the file names MO and MN -
            // GPS time
            FdtUTC = ((pc0F.dtUTC - 1) + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
            FTime = *gmtime(&FdtUTC);                            //DateTimeToSystemTime(dttm, FTime)
            DSec = FTime;
            DSec.tm_hour = 0; //zero out hours, min, sec, for base seconds of the day
            DSec.tm_min = 0;
            DSec.tm_sec = 0;
            DaySec = mktime(&DSec);
            pr0x0F = true;
          }
        }
      }
      // to form an array of GPS measurements over the entire file in the first
      // pass
      if (pr0x0F)
      {
        for (ik = 0; ik < pc0F.NMSat; ++ik)
        {
          if ((pc0F.Ms[ik].ChNum < maxChan) &&
              ((0 < pc0F.Ms[ik].idnum) && (pc0F.Ms[ik].idnum < 198)))
          { // determin which systems the measurements were from
            if (svGPS(&pc0F.Ms[ik].idnum))
              G_OBS = true; // GPS OBS
            if (svGLN(&pc0F.Ms[ik].idnum))
              R_OBS = true; // GLN
            if (svGAL(&pc0F.Ms[ik].idnum))
              E_OBS = true; // Gal
            if (svQZSS(&pc0F.Ms[ik].idnum))
              J_OBS = true; // QZSS
            // update the array of correspondence of the spacecraft number and
            // the letter
            if (svGLN(&pc0F.Ms[ik].idnum)) //GLN
              ArLitNumKA[pc0F.Ms[ik].idnum - 64] = pc0F.Ms[ik].Hn;
            //
            if ((pc0F.Ms[ik].prng > 1) && (pc0F.Ms[ik].prng < 90000000) &&
                (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng > 2) &&
                (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng < 10) &&
                (pc0F.Ms[ik].CNR > 27))
            {
              // there are measurements from this GPS satellite in this file
              if (svGPS(&pc0F.Ms[ik].idnum))
                ArSVsOBS_GPS[pc0F.Ms[ik].idnum - 1] = 1;
            }
          }
        }
      }
      return true;
    } //end First analysis

    // GPS time
    dttm = (pc0F.dtUTC + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
    TimeEposh = *gmtime(&dttm);

    // to write the coordinate file ROVER
    TimeEposh1 = TimeEposh;

    if ((ClockOffset < -400) && (ClockOffset > 400)) // take with a margin, should be from -300m to 300m
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%5.1f;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": ERROR! ClockOffGPS = ", ClockOffset);
        FLog << buff;
      }
      return true;
    }

    // sign of highlighting date and time and spacecraft&gt; 0 in communication
    if (((ChDT == false) || (pc0F.NMSat == 0)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%i;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": ERROR! ChDT=false or pc0F.NMSat = ", pc0F.NMSat);
        FLog << buff;
      }
      return true;
    }

    // verification of the monotony of eras
    if ((pc0F.dtUTC <= pc10dtUTC_OLD))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": ERROR! Repeat era!");
        FLog << buff;
      }
      return true;
    }
    pc10dtUTC_OLD = pc0F.dtUTC;

    // era-specific trap
    if ((TimeEposh.tm_hour == 19) && (TimeEposh.tm_min == 51) &&
        (TimeEposh.tm_sec == 58))
    {
      TimeEposh1 = TimeEposh;
    }

    SVsON = 0;
    // Further, the formation of measurements in this era for each spacecraft
    for (ik = 0; ik < pc0F.NMSat; ++ik)
    {
      if ((pc0F.Ms[ik].ChNum < maxChan) &&
          ((0 < pc0F.Ms[ik].idnum) && (pc0F.Ms[ik].idnum < 198)))
      {
        SVcalc(&pc0F.Ms[ik].idnum, StNKA, &kof, &pc0F.Ms[ik].Hn);

        // LLI flag check
        if (pc0F.Ms[ik].LLI == 0)
        {
          LLI = false;
          sprintf(StLLI, " ");
        }
        else
        {
          LLI = true;
          sprintf(StLLI, "1");
        }
        FlagElMask = false;
        // rejection of anomalous measurement values
        if (((pc0F.Ms[ik].prng > 1) && (pc0F.Ms[ik].prng < 90000000) &&
             (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng > 2) &&
             (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng < 10) &&
             (pc0F.Ms[ik].CNR > 25) && (FlagElMask == false)))
        { //measurement check passes
          // if checked &quot;only SVs used in position fix&quot;
          /* if ((((RGSV.ItemIndex == 1) &&
            ((pc0F.Ms[ik].tdt && 0x20) == 0x20)) ||
            (RGSV.ItemIndex == 0))) */
          // // line forming
          // sprintf(buff,"%s%14.3f  %14.3f  %14.3f%s %14.3f\n", StNKA,
          //   pc0F.Ms[ik].prng,(pc0F.Ms[ik].pvel * kof),pc0F.Ms[ik].pphi,StLLI,pc0F.Ms[ik].CNR);
          // St = StNKA +
          //      Format("%14.3f", [pc0F.Ms[ik].prng], formatSettings) +
          //      Format("%2s", [""]) +  // pseudorange
          //      Format("%14.3f", [pc0F.Ms[ik].pvel * kof], formatSettings) +
          //      Format("%2s", [""]) +  // pseudo speed
          //      Format("%14.3f%s", [ pc0F.Ms[ik].pphi, StLLI ],
          //             formatSettings) +
          //      Format("%1s", [""]) +  // phase + LLI
          //      Format("%14.3f", [pc0F.Ms[ik].CNR], formatSettings);
          //ArSVsOBS[SVsON] = St;
          SVpass[ik] = true;
          SVsON++;
        }
        else
        { //measurement check failed
          SVpass[ik] = false;
          if (((pc0F.Ms[ik].CNR > 25) && (LLI == false)))
          {
            if ((FlagElMask == false))
            {
              if (FLog.is_open())
              {
                sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%s;\n",
                        TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                        (float)TimeEposh.tm_sec, ": ERROR! rejection of anomalous values spacecraft measurements KA ", StNKA);
                FLog << buff;
                sprintf(buff, "%s%8.3f;\n", "(1 > pc0F.Ms[ik].prng < 90000000) = ", pc0F.Ms[ik].prng);
                FLog << buff;
                sprintf(buff, "%s%8.3f;\n", "(2 > pc0F.Ms[ik].pphi/pc0F.Ms[ik].prng > 10) = ", (pc0F.Ms[ik].pphi * 1.0 / pc0F.Ms[ik].prng));
                FLog << buff;
                sprintf(buff, "%s%8.3f;\n", "(pc0F.Ms[ik].CNR > 25) = ", pc0F.Ms[ik].CNR);
                FLog << buff;
              }
            }
            else
            {
              // StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s%
              // 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10,
              // StMinute10, StSecond10], formatSettings); Writeln (LOG,
              // StPac10 + &#39;: ERROR! Sorting by spacecraft elevation&#39;
              // + StNKA); Writeln (LOG, &#39;(pc10.Ms [ik] .CNR&gt; 25)
              // =&#39; + floattostrF (pc0F.Ms [ik] .CNR, ffFixed, 8.3) +
              // &#39;;&#39;);
            }
          }
        } // end rejection of anomalous measurement values
      }   // if ichn in [0..43] then
    }     // for (ik = 0; ik < pc0F.NMSat; ++ik)

    // write to file
    if (SVsON > 0)
    {
      //write the date/time for obs
      sprintf(buff, "> %04u %02u %02u %02u %02u %11.7f%3s%3u%21.12f\n",
              TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
              (float)TimeEposh.tm_sec, "0", SVsON, ClockOffset * 1.0 / SL);
      F_RinMO << buff;
      // write to file - each spacecraft
      for (ik = 0; ik < pc0F.NMSat; ++ik)
      {
        if (SVpass[ik])
        {
          // line forming & output
          //C1C - Code/Pseudorange (m)
          //D1C - Doppler (Hz)
          //L1C - Phase (full cycles)
          //S1C - CNR
          sprintf(buff, "%s%14.3f  %14.3f  %14.3f%s %14.3f\n", StNKA,
                  pc0F.Ms[ik].prng, (pc0F.Ms[ik].pvel * kof), pc0F.Ms[ik].pphi, StLLI, pc0F.Ms[ik].CNR);
          F_RinMO << buff;
        }
      }
    }
    return true;
  }

  // receiving a packet with measuring information 1st file recording
  bool ExCm10()
  {
    int ik;
    //int ichn;
    time_t dttm;
    //double DbSecMSec; //milliseconds
    //std::string St;
    struct tm TimeEposh;
    char StNKA[5]; //string of the SV Char&Num
    double kof;
    bool LLI = false;           //loss-of-lock indicator
    char StLLI[2];              //string loss-of-lock indicator
    uint8_t SVsON;              //Num SV in obs
    bool SVpass[maxChan] = {0}; //should the SV obs be output?
    //int iz;
    bool FlagElMask;
    //int ArSVsOBS_GPS[maxChan]; // array of measurements

    // Reading the data from the stream
    CSpass = true;

    if (FGeosBin.read((char *)&pc10, ndat * 4)) //read the message
    {
    }
    else
      return false; //read failed or EOF

    if (ndat != ((14 * pc10.NMSat) + 6)) //check message length
    {
      mLength = false;
      return false;
    }

    if (FGeosBin.read((char *)&word, 4)) //read the CS
    {
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc10, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    errCS10 = false;
    // if (errCS13 = true) then exit;

    kof = 1;

    ClockOffset = pc10.ClockOffGPS; // ShV receiver shift relative to ShV GPS in meters

    LeapSec = pc10.LeapSec;

    if ((FirstAnalysis == 1))
    {
      if (pr0x10 == false)
      {
        if (((ChDT == true) && (pc10.NMSat > 0)))
        {
          if (((LeapSec != 0))) // filter like in package 0x13 during recording
                                // era in OBS file
          {
            // save the time of the first measurement
            // (pc10.dtUTC-1) since decision flag comes after 0x10 packet
            // getting the first measurement time for the file names MO and MN -
            // GPS time
            FdtUTC = ((pc10.dtUTC - 1) + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
            FTime = *gmtime(&FdtUTC);                            //DateTimeToSystemTime(dttm, FTime);
            DSec = FTime;
            DSec.tm_hour = 0; //zero out hours, min, sec, for base seconds of the day
            DSec.tm_min = 0;
            DSec.tm_sec = 0;
            DaySec = mktime(&DSec);
            pr0x10 = true;
          }
        }
      }
      // to form an array of GPS measurements over the entire file in the first
      // pass
      if (pr0x10)
      {
        for (ik = 0; ik < pc10.NMSat; ++ik)
        {
          if ((pc10.Ms[ik].ChNum < maxChan) &&
              ((0 < pc10.Ms[ik].idnum) && (pc10.Ms[ik].idnum < 198)))
          { // determin which systems the measurements were from
            if (svGPS(&pc10.Ms[ik].idnum))
              G_OBS = true; // GPS OBS
            if (svGLN(&pc10.Ms[ik].idnum))
              R_OBS = true; // GLN
            if (svGAL(&pc10.Ms[ik].idnum))
              E_OBS = true; // Gal
            if (svQZSS(&pc10.Ms[ik].idnum))
              J_OBS = true; // QZSS
            // update the array of correspondence of the spacecraft number and
            // the letter
            if (svGLN(&pc10.Ms[ik].idnum)) //GLN
              ArLitNumKA[pc10.Ms[ik].idnum - 64] = pc10.Ms[ik].Hn;
            //
            if (((pc10.Ms[ik].prng > 1) && (pc10.Ms[ik].prng < 90000000) &&
                 (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng > 2) &&
                 (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng < 10) &&
                 (pc10.Ms[ik].CNR > 27)))
            {
              // there are measurements from this GPS satellite in this file
              if (svGPS(&pc10.Ms[ik].idnum))
              {
                //ArSVsOBS_GPS[pc10.Ms[ik].idnum - 1] = 1;
              }
            }
          }
        }
      }
      return true;
    } // if (FirstAnalysis = 1) then

    // GPS time
    dttm = (pc10.dtUTC + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
    TimeEposh = *gmtime(&dttm);

    // to write the coordinate file ROVER
    TimeEposh1 = TimeEposh;

    if ((ClockOffset < -400) && (ClockOffset > 400)) // take with a margin, and should be from -300m to 300m
    {                                                // ClockOffset range error
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%5.1f;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": 0x10 ERROR! ClockOffGPS = ", ClockOffset);
        FLog << buff;
      }
      return true;
    }

    // sign of highlighting date and time and spacecraft&gt; 0 in communication
    if (((ChDT == false) || (pc10.NMSat == 0)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%i;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": Ox10 ERROR! ChDT=false or pc10.NMSat = ", pc10.NMSat);
        FLog << buff;
      }
      return true;
    }

    // sign of highlighting date and time and spacecraft&gt; 0 in communication
    // if ((pc10.dtUTC = 0) or (pc10.NMSat = 0)) then
    // begin
    // // skip era
    // StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s% 3s% 11s&#39;,
    // [StYear10, StMonth10, StDay10, StHour10, StMinute10, StSecond10],
    // formatSettings); Writeln (LOG, StPac10 + &#39;: ERROR! ChDT = false or
    // pc10.NMSat =&#39; + inttostr (pc10.NMSat) + &#39;;&#39;); exit end;

    // verification of the monotony of eras
    if ((pc10.dtUTC <= pc10dtUTC_OLD))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": 0x10 ERROR! Repeat era!");
        FLog << buff;
      }
      return true;
    }
    pc10dtUTC_OLD = pc10.dtUTC;

    // // trap of a specific era
    // if (TimeEposh.wHour = 5) and (TimeEposh.wMinute = 41) and
    // (TimeEposh.wSecond = 18) then begin TimeEposh1: = TimeEposh; end;

    SVsON = 0;
    // Further, the formation of measurements in this era for each spacecraft
    for (ik = 0; ik < pc10.NMSat; ++ik)
    // for ik: = 0 to 42 do
    {
      // ArSVsOBS [ik]: = &#39;&#39;;
      if ((pc10.Ms[ik].ChNum < maxChan) &&
          ((0 < pc10.Ms[ik].idnum) && (pc10.Ms[ik].idnum < 198)))
      {

        SVcalc(&pc10.Ms[ik].idnum, StNKA, &kof, &pc10.Ms[ik].Hn);

        // LLI flag check
        if (pc10.Ms[ik].LLI == 0)
        {
          LLI = false;
          sprintf(StLLI, " ");
        }
        else
        {
          LLI = true;
          sprintf(StLLI, "1");
        }
        FlagElMask = false;
        // rejection of anomalous measurement values
        if (((pc10.Ms[ik].prng > 1) && (pc10.Ms[ik].prng < 90000000) &&
             (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng > 2) &&
             (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng < 10) &&
             (pc10.Ms[ik].CNR > 25) && (FlagElMask == false)))
        { //measurement check passes
          // if checked &quot;only SVs used in position fix&quot;
          // if ((((RGSV.ItemIndex == 1) &&
          //       ((pc10.Ms[ik].tdt && 0x20) == 0x20)) ||
          //      (RGSV.ItemIndex == 0)))
          // {
          //   // line forming
          //   St = StNKA +
          //        Format("%14.3f", [pc10.Ms[ik].prng], formatSettings) +
          //        Format("%2s", [""]) + // pseudorange
          //        Format("%14.3f", [pc10.Ms[ik].pvel * kof], formatSettings) +
          //        Format("%2s", [""]) + // pseudo speed
          //        Format("%14.3f%s", [ pc10.Ms[ik].pphi, StLLI ],
          //               formatSettings) +
          //        Format("%1s", [""]) + // phase + LLI
          //        Format("%14.3f", [pc10.Ms[ik].CNR], formatSettings);
          //   ArSVsOBS[SVsON] = St;
          SVpass[ik] = true;
          SVsON = SVsON + 1;
        }
        else
        { //measurement check failed
          SVpass[ik] = false;
          if (((pc10.Ms[ik].CNR > 25) && (LLI == false)))
          {
            if ((FlagElMask == false))
            {
              if (FLog.is_open())
              {
                sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%s;\n",
                        TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                        (float)TimeEposh.tm_sec, ": 0x10 ERROR! rejection of anomalous values spacecraft measurements KA ", StNKA);
                FLog << buff;
                sprintf(buff, "%s%8.3f;\n", "(1 > pc10.Ms[ik].prng < 90000000) = ", pc10.Ms[ik].prng);
                FLog << buff;
                sprintf(buff, "%s%8.3f;\n", "(2 > pc10.Ms[ik].pphi/pc10.Ms[ik].prng > 10) = ", (pc10.Ms[ik].pphi * 1.0 / pc10.Ms[ik].prng));
                FLog << buff;
                sprintf(buff, "%s%8.3f;\n", "(pc10.Ms[ik].CNR > 25) = ", pc10.Ms[ik].CNR);
                FLog << buff;
              }
            }
            else
            {
              // StPac10: = &#39;&gt;&#39; + Format (&#39;% 4s% 3s% 3s% 3s%
              // 3s% 11s&#39;, [StYear10, StMonth10, StDay10, StHour10,
              // StMinute10, StSecond10], formatSettings); Writeln (LOG,
              // StPac10 + &#39;: ERROR! Sorting by spacecraft elevation&#39;
              // + StNKA); Writeln (LOG, &#39;(pc10.Ms [ik] .CNR&gt; 25)
              // =&#39; + floattostrF (pc10.Ms [ik] .CNR, ffFixed, 8.3) +
              // &#39;;&#39;);
            }
          }
        } // end rejection of anomalous measurement values
      }   // if ichn in [0..43] then
    }     // for ik: = 0 to pc10.NMSat-1 do

    // write to file
    if (SVsON > 0) //more than 1 SV
    {
      //write the date/time for obs
      sprintf(buff, "> %04u %02u %02u %02u %02u %010.7f%3s%3u%21.12f\n",
              TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
              (float)TimeEposh.tm_sec, "0", SVsON, ClockOffset * 1.0 / SL);
      F_RinMO << buff;
      // write to file - each spacecraft
      for (ik = 0; ik < pc10.NMSat; ++ik)
      {
        if (SVpass[ik]) //passed check
        {
          SVcalc(&pc10.Ms[ik].idnum, StNKA, &kof, &pc10.Ms[ik].Hn);
          // LLI flag check
          if (pc10.Ms[ik].LLI == 0)
          {
            LLI = false;
            sprintf(StLLI, " ");
          }
          else
          {
            LLI = true;
            sprintf(StLLI, "1");
          }
          // line forming & output
          //C1C - Code/Pseudorange (m)
          //D1C - Doppler (Hz)
          //L1C - Phase (full cycles)
          //S1C - CNR
          sprintf(buff, "%s%14.3f  %14.3f  %14.3f%s %14.3f\n", StNKA,
                  pc10.Ms[ik].prng, (pc10.Ms[ik].pvel * kof), pc10.Ms[ik].pphi, StLLI, pc10.Ms[ik].CNR);
          F_RinMO << buff;
        }
      }
      F_RinMO.flush();
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x13: Navigation Solution State Vector
  // receiving a packet according to the decision of the NC
  bool ExCm13()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&pc13, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc13, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    errCS13 = false;
    if ((errCS10 == true))
      return true;

    if ((FirstAnalysis == 1) && GinFix)
    {
      ClockOffset = pc13.cs; // ShV receiver shift relative to ShV GPS in meters
      coorX = pc13.X;
      coorY = pc13.Y;
      coorZ = pc13.Z;
      if (((ClockOffset > -400) && (ClockOffset < 400)))
        pr0x13 = true;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x14: Timing Parameters
  // packet reception time parameters
  bool ExCm14()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&pc14, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc14, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    WRollOver = pc14.RlWk; //set this every time because I think I was getting a bad value from the first message

    if ((pr0x14 == false))
    {
      pr0x14 = true;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x18: GPS/QZSS Almanac
  // receiving a packet updating the values of the GPS spacecraft almanac
  bool ExCm18()
  {
    int ik;

    CSpass = true;

    if (FGeosBin.read((char *)&aGPS, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&aGPS, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    if ((pr0x18 == false))
    {
      // fill in the array of GPS almanac
      if (((aGPS.num > 0) && (aGPS.num < GPSprn + 1)))
        ArTAlmnGPS[aGPS.num] = aGPS;
      // checking the filling of the entire almanac
      pr0x18 = true;
      for (ik = 1; ik < GPSprn + 1; ++ik)
        if ((ArTAlmnGPS[ik].num == 100)) //default value
          pr0x18 = false;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x19, 0x89: GLONASS Almanac
  // receiving a packet updating the values of the GLONASS spacecraft almanac
  bool ExCm19()
  {
    int ik;

    CSpass = true;

    if (FGeosBin.read((char *)&aGLN, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&aGLN, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    if ((pr0x19 == false))
    {
      // fill in the array of correspondence of the spacecraft number and letter
      if (((aGLN.num > 0) && (aGLN.Alt < 25)))
        ArLitNumKA[aGLN.num] = aGLN.Alt;
      // checking the filling of the entire almanac
      pr0x19 = true;
      for (ik = 1; ik < GLNprn + 1; ++ik)
        if ((ArLitNumKA[ik] == 100)) //default value
          pr0x19 = false;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x0A, 0x9A: GALILEO Ephemeris
  // receiving a packet updating the values of the ephemeris KA GALILEO record 2nd
  // file
  bool ExCm0A()
  {
    //u_int8_t ik;
    time_t dttm;
    struct tm TimeEposh;
    //time_t TimeReal;
    char StNKA[5]; //string Spacecraft number

    CSpass = true;

    if (FGeosBin.read((char *)&eGAL, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&eGAL, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    if ((FirstAnalysis == 1))
    {
      pr0x0A = true;
      return true;
    }

    // GPS time
    dttm = (eGAL.dtrc + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
    TimeEposh = *gmtime(&dttm);

    // Filter 0
    // filtering ephemeris with zero data values
    if (((eGAL.n == 0) || (eGAL.I0 == 0) || (eGAL.SVhelth != 0)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u%s%f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GAL ERROR! filtering GAL ephemeris with zero values data, eGAL.n = ", eGAL.n, "; eGAL.I0 = ", eGAL.I0, "; eGAL.SVhelth = ", eGAL.SVhelth);
        FLog << buff;
      }
      return true;
    }

    // Filter 1
    // if in the OBS file there are no measurements at all from GPS PRN = N
    // (meaning measurements in the &quot;only SVs used in position fix&quot;
    // mode), then all ephemeris sets with PRN = N can be considered
    // &quot;fake&quot; and ignored.
    // if ((RGSV.ItemIndex == 1))
    // {
    //   // if ((ArSVsOBS_GPS [eGPS.n-1]) &lt;&gt; 1) then
    //   // begin
    //   // // skip era
    //   // Writeln (LOG, &#39;EPH GPS ERROR! OBS file generally no measurements
    //   // from eGPS.n =&#39; + inttostr (eGPS.n) + &#39;;&#39;); exit end;
    // }

    // Filter 2
    // Compare afo and eccentricity from the “tested” Geos GAL ephemeris (PRN =
    // N) with the corresponding parameters from the almanac for GAL PRN = N.
    if (pr0x88) // GAL almanac request required
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
         (eGAL.A == ArTEphGAL[eGAL.n].A)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GAL Warning! GAL ephemeris repeat, eGAL.n = ", eGAL.n);
        FLog << buff;
      }
      return true;
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
    sprintf(StNKA, "E%02u", eGAL.n);
    // GPS time
    // dttm: = (eGAL.t_oe + (604800 * (eGAL ^ .weekN + 1024 * WRollOver))) + RegTimeGPS;
    // StrToDateTime(&#39;06.01.1980 &#39;); WRollOver in
    // GALILEO will be equal to 1 since eGAL.weekN may be 4095 (due to 12bit
    // dimension)
    dttm = (eGAL.t_oe + (604800 * (eGAL.weekN + 1024 * WRollOver))) + BaseTimeGPS; // StrToDateTime (&#39;06 .01.1980 &#39;);

    // Filter 4
    // if dttm is more than 2 hours from local time, then the actual ephemeris
    // is no longer
    if ((abs(difftime(TimeUTC, dttm)) > (2 * 60 * 60)))
    {
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f %s%s;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                StNKA, " error GAL dttm; old ephemeris!");
        FLog << buff;
      }
      return true;
    }

    // preservation of more recent ephemeris in the table
    ArTEphGAL[eGAL.n].n = eGAL.n;
    ArTEphGAL[eGAL.n].t_oe = eGAL.t_oe;
    ArTEphGAL[eGAL.n].t_oc = eGAL.t_oc;
    ArTEphGAL[eGAL.n].IODE = eGAL.IODE;
    ArTEphGAL[eGAL.n].IODC = eGAL.IODC;
    ArTEphGAL[eGAL.n].af0 = eGAL.af0;
    ArTEphGAL[eGAL.n].A = eGAL.A;

    // // cure for incorrect eGPS.weekN
    // if (abs (TimeUTC - dttm)&gt; (1.0 / 8.0)) then // 0.125
    // begin
    // dttm: = ((604800 * (eGAL ^ .weekN + 1 + 1024 * WRollOver))) / 86400 +
    // RegTimeGPS; //StrToDateTime(&#39;06.01.1980 &#39;); Writeln (LOG, StNKA +
    // &#39;error eGPS ^ .weekN!&#39;); end;
    TimeEposh = *gmtime(&dttm); //Ephemerides reference Time

    // ===================================================
    // ====================================

    // SV / EPOCH / SV CLK
    sprintf(buff, "%s %04u %02u %02u %02u %02u %02u% 19.12E% 19.12E% 19.12E\n",
            StNKA, TimeEposh.tm_year + 1900 - 2000, TimeEposh.tm_mon + 1,
            TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
            TimeEposh.tm_sec, eGAL.af0, eGAL.af1, eGAL.af2);
    F_RinMN << buff;
    // BROADCAST ORBIT - 1
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", (float)eGAL.IODE, eGAL.C_rs, eGAL.dn * pi, eGAL.M * pi);
    F_RinMN << buff;
    // BROADCAST ORBIT - 2
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGAL.C_uc, eGAL.e, eGAL.C_us, eGAL.A);
    F_RinMN << buff;
    // BROADCAST ORBIT - 3
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", (float)eGAL.t_oe, eGAL.C_ic, eGAL.OMEGA_0 * pi, eGAL.C_is);
    F_RinMN << buff;
    // BROADCAST ORBIT - 4
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGAL.I0 * pi, eGAL.C_rc, eGAL.w * pi, eGAL.OMEGADOT * pi);
    F_RinMN << buff;
    // BROADCAST ORBIT - 5
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGAL.IDOT * pi, 0.0, (float)(eGAL.weekN + 1024 + (0 * 4096)), 0.0);
    F_RinMN << buff;
    // BROADCAST ORBIT - 6
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", (float)eGAL.SVaccur, (float)eGAL.SVhelth, eGAL.Tgd, (float)eGAL.IODC);
    F_RinMN << buff;
    // BROADCAST ORBIT - 7
    // Stdtrc: = FormatFloat (&#39;0.000000000000E + 00&#39;, eGPS ^ .dtrc,
    // formatSettings);
    sprintf(buff, "%s% 19.12E% 19.12E\n",
            "    ", (float)eGAL.t_oe, 0.0);
    F_RinMN << buff;
    F_RinMN.flush();

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x1A, 0x8A: GPS/QZSS Ephemeris
  // receiving a packet updating the ephemeris values of the GPS satellite record
  // of the 2nd file
  bool ExCm1A()
  {
    //u_int8_t ik;
    time_t dttm;
    struct tm TimeEposh;
    char StNKA[5]; //string Spacecraft number
    double Eph_Alm_E;

    CSpass = true;

    if (FGeosBin.read((char *)&eGPS, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&eGPS, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    if ((FirstAnalysis == 1))
    {
      pr0x1A = true;
      return true;
    }

    // GPS time
    dttm = (eGPS.dtrc + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
    TimeEposh = *gmtime(&dttm);

    // Filter 0
    // filtering ephemeris with zero data values
    if (((eGPS.n == 0) || (eGPS.I0 == 0) || (eGPS.SVhelth != 0)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u%s%f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GPS ERROR! filtering GPS ephemeris with zero values data, eGPS.n = ", eGAL.n, "; eGAL.I0 = ", eGAL.I0, "; eGAL.SVhelth = ", eGAL.SVhelth);
        FLog << buff;
      }
      return true;
    }

    // Filter 1
    // if in the OBS file there are no measurements at all from GPS PRN = N
    // (meaning measurements in the &quot;only SVs used in position fix&quot;
    // mode), then all ephemeris sets with PRN = N can be considered
    // &quot;fake&quot; and ignored.
    // if ((RGSV.ItemIndex == 1))
    // {
    //   // if ((ArSVsOBS_GPS [eGPS.n-1]) &lt;&gt; 1) then
    //   // begin
    //   // // skip era
    //   // Writeln (LOG, &#39;EPH GPS ERROR! OBS file generally no measurements
    //   // from eGPS.n =&#39; + inttostr (eGPS.n) + &#39;;&#39;); exit end;
    // }

    // Filter 2
    // Compare afo and eccentricity from the &quot;tested&quot; geosovo GPS
    // ephemeris (PRN = N) with the corresponding parameters from the almanac
    // for GPS PRN = N.
    // if (pr0x88) // GPS almanac request required
    // {
    if ((eGPS.n ==
         ArTAlmnGPS[eGPS.n].num)) // if the received ephemeris of this
                                  // spacecraft is in the almanac array
    {                             // then we carry out data verification
      Eph_Alm_E = abs(ArTAlmnGPS[eGPS.n].e - eGPS.e);

      if (((abs(ArTAlmnGPS[eGPS.n].e - eGPS.e) > 0.001) ||
           (abs(ArTAlmnGPS[eGPS.n].af0 - eGPS.af0) > 0.0001)))
      {
        // skip era
        if (FLog.is_open())
        {
          sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%f;\n",
                  TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                  (float)TimeEposh.tm_sec,
                  ": Compare afo and eccentricity ephemeris and almanac, the difference is Eph_Alm_E = ", Eph_Alm_E);
          FLog << buff;
        }
        return true;
      }
    }
    // }

    // Filter 3
    // verification of received ephemeris with saved
    if (((eGPS.t_oe == ArTEphGPS[eGPS.n].t_oe) &&
         (eGPS.t_oc == ArTEphGPS[eGPS.n].t_oc) &&
         (eGPS.IODE == ArTEphGPS[eGPS.n].IODE) &&
         (eGPS.A == ArTEphGPS[eGPS.n].A)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GPS Warning! GPS ephemeris repeat, eGPS.n = ", eGPS.n);
        FLog << buff;
      }
      return true;
    }
    // IODC and IODE match check
    if ((eGPS.IODC != eGPS.IODE))
    {
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GPS ERROR! not equal IODC and IODE GPS, eGPS.n = ", eGPS.n);
        FLog << buff;
      }
      return true;
    }
    // ===================================================
    // ====================================

    // further conversion to a navigation file
    sprintf(StNKA, "G%02u", eGPS.n);
    // GPS time
    dttm = (eGPS.t_oe + (604800 * (eGPS.weekN + 1024 * WRollOver))) + BaseTimeGPS; // StrToDateTime (&#39;06 .01.1980 &#39;);

    // Filter 4
    // if dttm is more than 3 hours from local time, then the actual ephemeris
    // is no longer
    if ((abs(difftime(TimeUTC, dttm)) > (3 * 60 * 60)))
    {
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f %s%s;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                StNKA, " error GPS dttm; old ephemeris!");
        FLog << buff;
      }
      return true;
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
    TimeEposh = *gmtime(&dttm); //Ephemerides reference Time

    // ===================================================
    // ====================================
    // SV / EPOCH / SV CLK
    sprintf(buff, "%s %04u %02u %02u %02u %02u %02u% 19.12E% 19.12E% 19.12E\n",
            StNKA, TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1,
            TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
            TimeEposh.tm_sec, eGPS.af0, eGPS.af1, eGPS.af2);
    F_RinMN << buff;
    // BROADCAST ORBIT - 1
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", (float)eGPS.IODE, eGPS.C_rs, eGPS.dn * pi, eGPS.M * pi);
    F_RinMN << buff;
    // BROADCAST ORBIT - 2
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGPS.C_uc, eGPS.e, eGPS.C_us, eGPS.A);
    F_RinMN << buff;
    // BROADCAST ORBIT - 3
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", (float)eGPS.t_oe, eGPS.C_ic, eGPS.OMEGA_0 * pi, eGPS.C_is);
    F_RinMN << buff;
    // BROADCAST ORBIT - 4
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGPS.I0 * pi, eGPS.C_rc, eGPS.w * pi, eGPS.OMEGADOT * pi);
    F_RinMN << buff;
    // BROADCAST ORBIT - 5
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGPS.IDOT * pi, 0.0, (float)(eGPS.weekN + 1024 * WRollOver), 0.0);
    F_RinMN << buff;
    // BROADCAST ORBIT - 6
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", (float)eGPS.SVaccur, (float)eGPS.SVhelth, eGPS.Tgd, (float)eGPS.IODC);
    F_RinMN << buff;
    // BROADCAST ORBIT - 7
    // Stdtrc: = FormatFloat (&#39;0.000000000000E + 00&#39;, eGPS ^ .dtrc,
    // formatSettings);
    sprintf(buff, "%s% 19.12E% 19.12E\n",
            "    ", (float)eGPS.t_oe, 0.0);
    F_RinMN << buff;
    F_RinMN.flush();

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x1B, 0x8B: GLONASS Ephemeris
  // receiving a packet; updating GLONASS ephemeris values; recording a 2nd file
  bool ExCm1B()
  {
    int ik;
    time_t dttm;
    struct tm TimeEposh;
    char StNKA[5]; //string Spacecraft number
    time_t Date1;

    CSpass = true;

    if (FGeosBin.read((char *)&eGLN, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&eGLN, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    if ((FirstAnalysis == 1))
    {
      pr0x1B = true;
      return true;
    }

    // Filter 1
    // if the time of reception of the ephemeris PRN = N is equal to 2008, then
    // they can be considered &quot;fake&quot; and ignored. Ephemeris Time (UTC)
    dttm = (eGLN.dtrc + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
    TimeEposh = *gmtime(&dttm);
    if ((TimeEposh.tm_year + 1900) == 2008)
    {
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                "EPH GLN ERROR! time allocation error TimeEposh.tm_year = ", TimeEposh.tm_year + 1900);
        FLog << buff;
      }
      return true;
    }
    // further conversion to a navigation file
    sprintf(StNKA, "R%02u", eGLN.n);

    // Filter 2
    // if dttm is more than half an hour from local time, then the actual
    // ephemeris is no longer
    if ((abs(difftime(TimeUTC, dttm)) > (0.5 * 60 * 60)))
    {
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f %s%s;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                StNKA, " error GLN dttm; old ephemeris!");
        FLog << buff;
      }
      return true;
    }

    if ((eGLN.n == 0) || (eGLN.prNNK > 0))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GLN ERROR! filtering GLN ephemeris with zero values data, eGLN.n = ", eGLN.n, "; eGLN.prNNK = ", eGLN.prNNK);
        FLog << buff;
      }
      return true;
    }

    // verification of received ephemeris with saved
    if (((eGLN.t_b == ArTEphGLN[eGLN.n].t_b) &&
         (eGLN.pX == ArTEphGLN[eGLN.n].pX) &&
         (eGLN.pY == ArTEphGLN[eGLN.n].pY) &&
         (eGLN.pZ == ArTEphGLN[eGLN.n].pZ)))
    {
      // skip era
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s%u;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec,
                ": EPH GLN Warning! GLN ephemeris repeat, eGLN.n = ", eGLN.n);
        FLog << buff;
      }
      return true;
    }

    if ((RecEph_od == false))
    {
      if ((ChDT == false))
      {
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
        return true;
      }
      else
      {
        // there was a sign of time allocation
        // then you need to rewrite all the entries in the array into a
        // navigation file

        // Ephemeris Time (UTC)
        dttm = (eGLN.dtrc + LeapSec) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
        TimeEposh = *gmtime(&dttm);

        // nullify wHour, wMinute, wSecond, wMSecond
        TimeEposh.tm_hour = 1;
        TimeEposh.tm_min = 0;
        TimeEposh.tm_sec = 0;
        //TimeEposh.wMilliseconds = 0;
        // we only get the date; translate the date from time_t to
        // TDateTime
        Date1 = mktime(&TimeEposh);

        for (ik = 1; ik < (GLNprn + 1); ++ik)
        {
          if ((ArTEphGLN_od[ik].n != 0))
          {
            // preservation of more recent ephemeris in the table
            ArTEphGLN[ik].t_b = ArTEphGLN_od[ik].t_b;
            ArTEphGLN[ik].pX = ArTEphGLN_od[ik].pX;
            ArTEphGLN[ik].pY = ArTEphGLN_od[ik].pY;
            ArTEphGLN[ik].pZ = ArTEphGLN_od[ik].pZ;

            // further conversion to a navigation file
            sprintf(StNKA, "R%02u", ArTEphGLN_od[ik].n);

            // Epoch: Toc - Time of Clock (UTC)
            dttm = ((ArTEphGLN_od[ik].t_b * 900 + 21 * 60 * 60) % 86400) + Date1;
            TimeEposh = *gmtime(&dttm);

            // StYear = inttostr(TimeEposh.wYear);
            // if (TimeEposh.wMonth < 10)
            //   StMonth = "0" + inttostr(TimeEposh.wMonth);
            // else
            //   StMonth = inttostr(TimeEposh.wMonth);
            // if (TimeEposh.wDay < 10)
            //   StDay = "0" + inttostr(TimeEposh.wDay);
            // else
            //   StDay = inttostr(TimeEposh.wDay);
            // if ((TimeEposh.wHour) < 10)
            //   StHour = "0" + inttostr(TimeEposh.wHour);
            // else
            //   StHour = inttostr(TimeEposh.wHour);
            // if ((TimeEposh.wMinute) < 10)
            //   StMinute = "0" + inttostr(TimeEposh.wMinute);
            // else
            //   StMinute = inttostr(TimeEposh.wMinute);
            // if ((TimeEposh.wSecond) < 10)
            //   StSecond = "0" + inttostr(TimeEposh.wSecond);
            // else
            //   StSecond = inttostr(TimeEposh.wSecond);

            // SV / EPOCH / SV CLK
            sprintf(buff, "%s %04u %02u %02u %02u %02u %02u% 19.12E% 19.12E% 19.12E\n",
                    StNKA, TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1,
                    TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                    TimeEposh.tm_sec, -ArTEphGLN_od[ik].tau, ArTEphGLN_od[ik].gm,
                    (float)(ArTEphGLN_od[ik].t_b * 900)); // Message frame time? tk = tb * 900
            F_RinMN << buff;
            // BROADCAST ORBIT - 1
            sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
                    "    ", ArTEphGLN_od[ik].pX * 0.001, ArTEphGLN_od[ik].vX * 0.001,
                    ArTEphGLN_od[ik].aX * 0.001, (float)ArTEphGLN_od[ik].prNNK);
            F_RinMN << buff;
            // BROADCAST ORBIT - 2
            sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
                    "    ", ArTEphGLN_od[ik].pY * 0.001, ArTEphGLN_od[ik].vY * 0.001,
                    ArTEphGLN_od[ik].aY * 0.001, (float)ArLitNumKA[ArTEphGLN_od[ik].n]);
            F_RinMN << buff;
            // BROADCAST ORBIT - 3
            sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
                    "    ", ArTEphGLN_od[ik].pZ * 0.001, ArTEphGLN_od[ik].vZ * 0.001,
                    ArTEphGLN_od[ik].aZ * 0.001, (float)ArTEphGLN_od[ik].enE); // Age of oper. information (days) (E)
            F_RinMN << buff;
            F_RinMN.flush();
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
    sprintf(StNKA, "R%02u", eGLN.n);

    // Ephemeris Time (UTC)
    dttm = (eGLN.dtrc) + BaseTimeGeo; // convert GeoStar Time to System Time, not yet dealing with decimal seconds
    TimeEposh = *gmtime(&dttm);

    // nullify wHour, wMinute, wSecond, wMSecond
    TimeEposh.tm_hour = 1;
    TimeEposh.tm_min = 0;
    TimeEposh.tm_sec = 0;
    //TimeEposh.wMilliseconds = 0;
    // we only get the date; translate the date from time_t to TDateTime
    Date1 = mktime(&TimeEposh);
    // Epoch: Toc - Time of Clock (UTC)
    dttm = ((eGLN.t_b * 900 + 21 * 60 * 60) % 86400) + Date1;
    TimeEposh = *gmtime(&dttm);

    // SV / EPOCH / SV CLK
    sprintf(buff, "%s %04u %02u %02u %02u %02u %02u% 19.12E% 19.12E% 19.12E\n",
            StNKA, TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1,
            TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
            TimeEposh.tm_sec, -eGLN.tau, eGLN.gm, (float)(eGLN.t_b * 900)); // Message frame time? tk = tb * 900
    F_RinMN << buff;
    // BROADCAST ORBIT - 1
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGLN.pX * 0.001, eGLN.vX * 0.001, eGLN.aX * 0.001, (float)eGLN.prNNK); // health (0 = OK) (Bn)
    F_RinMN << buff;
    // BROADCAST ORBIT - 2
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGLN.pY * 0.001, eGLN.vY * 0.001,
            eGLN.aY * 0.001, (float)ArLitNumKA[eGLN.n]); // frequency number (-7 ... + 13)
    F_RinMN << buff;
    // BROADCAST ORBIT - 3
    sprintf(buff, "%s% 19.12E% 19.12E% 19.12E% 19.12E\n",
            "    ", eGLN.pZ * 0.001, eGLN.vZ * 0.001, eGLN.aZ * 0.001, (float)eGLN.enE); // Age of oper. information (days) (E)
    F_RinMN << buff;
    F_RinMN.flush();
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x1C, 0x9C: GPS Ionospheric Parameters
  // receiving the GPS ionosphere parameters packet
  bool ExCm1C()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&ion, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&ion, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    pr0x1C = true;

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x1D, 0x9D: GPS Time to UTC Conversion Parameters
  // packet reception GPS time reference parameters
  bool ExCm1D()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&utc, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&utc, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    pr0x1D = true;

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0x1E, 0x9E: GLONASS Time to UTC Conversion Parameters
  // packet reception GLO time reference parameters
  bool ExCm1E()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&gln25, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&gln25, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    pr0x1E = true;

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0xC1: Receiver Type, Firmware Version
  // receipt of a receipt for the package &quot;0xC1&quot;
  bool ExCmC1()
  {
    //char snChar;
    int snDig;

    CSpass = true;

    if (FGeosBin.read((char *)&pcC1, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pcC1, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    // if no number
    if ((pcC1.ReceiverType == 0))
    {
      sprintf(StSN, "%s", " N/A     ");
    }
    else
    {
      // serial number
      //snChar = char(pcC1.ReceiverType >> 24);
      snDig = (pcC1.ReceiverType && 0xFFFFFF);
      sprintf(StSN, " %08u", snDig);
    }

    // we get only the number
    sprintf(verSt, "%u.%u", pcC1.FWver_high, pcC1.FWver_low);
    //sprintf(StSN, " S/N:%s", snStr);

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // 0xC3: Storing Data to Flash Report
  bool ExCmC3()
  {
    if (FGeosBin.read((char *)&pcC3, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pcC3, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    //Write the contents to the Log file
    if (FLog.is_open())
    {
      struct tm TimeEposh = *gmtime(&TimeUTC);
      //decode the report
      char message[55];
      switch (pcC3.flashReport)
      {
      case 0:
        sprintf(message, "saving data cannot be completed");
        break;
      case 1:
        sprintf(message, "saving almanacs by command 0xC3 successfully done");
        break;
      case 2:
        sprintf(message, "saving almanacs automatically successfully done");
        break;
      case 3:
        sprintf(message, "saving configuration/FW settings successfully done");
        break;
      default:
        sprintf(message, "report not in range, %i", pcC3.flashReport);
        break;
      }
      sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f: 0xC3 Storing Data to Flash, %s;\n",
              TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
              (float)TimeEposh.tm_sec, message);
      FLog << buff;
    }
    return true;
  }

  /*----------------------------------------------------------------------------*/
  // GPS almanac reception
  // bool ExCm88()
  // {

  //   try
  //   {
  //     if ((FirstAnalysis == 1))
  //     {
  //       pr0x88 = true;
  //     }

  //     aGPS = &ArDat[0];
  //     if ((aGPS.num != 0) && (aGPS.num < 33))
  //       ArTAlmnGPS[aGPS.num] = aGPS /*?*/ ^ ;

  //     // wrong package
  //     //
  //   }
  //   catch (...)
  //   {
  //   }
  // }

  /*----------------------------------------------------------------------------*/
  // receiving a packet to turn on the receiver
  bool ExCm3E()
  {
    struct tm TimeEposh;
    CSpass = true;

    if (FGeosBin.read((char *)&pc3E, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc3E, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    ++num3E;

    TimeEposh = *gmtime(&TimeUTC);

    if ((num3E > 2))
    {
      if (FLog.is_open())
      {
        sprintf(buff, " %04u-%02u-%02u %02u:%02u:%05.2f%s;\n",
                TimeEposh.tm_year + 1900, TimeEposh.tm_mon + 1, TimeEposh.tm_mday, TimeEposh.tm_hour, TimeEposh.tm_min,
                (float)TimeEposh.tm_sec, ": ERROR! RESTART G5!!!");
        FLog << buff;
      }
      num3E = 0;
    }

    return true;
  }

  /*----------------------------------------------------------------------------*/
  // Message Reception Acknowledgement
  bool ExCm3F()
  {
    CSpass = true;

    if (FGeosBin.read((char *)&pc3F, ndat * 4))
    { //read the message
    }
    else
      return false; //read failed or EOF

    if (FGeosBin.read((char *)&word, 4))
    { //read the CS
    }
    else
      return false; //read failed or EOF

    if (word != CScalc((u_int32_t *)&pc3F, ncmd, ndat))
    { //check the CS
      CSpass = false;
      return false;
    }

    return true;
  }

} // namespace GeoStar

// END
