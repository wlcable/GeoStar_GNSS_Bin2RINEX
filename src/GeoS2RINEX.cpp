// Example program to convert a binary file from a GeoStar GNSS receiver into RINEX format

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "GeoStar.cpp"

using namespace GeoStar;

int main(int argc, char *argv[])
{
    u_int8_t c = 0;
    char *filename;
    int pathend = 0; //end to the path in filename

    if (argc < 2)
    { //no arguments given
        cout << "GeoS Binary to RINEX\n";
        cout << "Usage:\n";
        cout << "\tGeoS2RINEX path/input_file [options]\n";
        cout << "\tOptions:\n";
        cout << "\t\t-StationID XXXX, default=XXXX, limit 4 characters\n";
        cout << "\t\t-SVlog, enable generation of a csv file with Satellite Vehicle Info (CNR, El, Az)\n";
        cout << "\t\t-Outpath /path/path/, default is same as input\n";
        cout << "\t\t-CreationAgency XXXX, Name of Agency that creates this file (20)\n";
        cout << "\t\t-MarkerName XXXX, Name of the Marker (60)\n";
        cout << "\t\t-MarkerNumber XXXX, Number of the Marker (20)\n";
        cout << "\t\t-MarkerType XXXX, default=NON_GEODETIC, Name of the Marker (20)\n";
        cout << "\t\t-NameObserver XXXX, Name of the Observer (21)\n";
        cout << "\t\t-NameAgency XXXX, Name of the Agency (41)\n";
        return 0;
    }
    else
    { //at least filename given
        for (c = 1; c < argc; c++)
        {
            if (c == 1) //input file
            {
                filename = argv[c];
                //get the path of file
                for (int i = 0; i < strlen(filename); i++)
                {
                    if (filename[i] == '/')
                    {
                        pathend = i;
                    }
                }
                if (pathend > 0)
                {
                    memcpy(&outpath, filename, pathend + 1);
                }
                continue;
            }
            if ((strcmp(argv[c], "-StationID") == 0) & (argc > c))
            {
                memcpy(&StationID, argv[c + 1], strlen(argv[c+1])+1);
                StationID[4] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }
            if (strcmp(argv[c], "-SVlog") == 0)
            {
                SVinfoLOGGING = true;
                continue;
            }
            if (strcmp(argv[c], "-Outpath") == 0)
            {
                memcpy(&outpath, argv[c + 1], strlen(argv[c+1])+1);
                c++;
                continue;
            }
            if ((strcmp(argv[c], "-CreationAgency") == 0) & (argc > c))
            {
                memcpy(&CreationAgency, argv[c + 1], strlen(argv[c+1])+1);
                CreationAgency[20] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }
            if ((strcmp(argv[c], "-MarkerName") == 0) & (argc > c))
            {
                memcpy(&MarkerName, argv[c + 1], strlen(argv[c+1])+1);
                MarkerName[20] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }
            if ((strcmp(argv[c], "-MarkerNumber") == 0) & (argc > c))
            {
                memcpy(&MarkerNumber, argv[c + 1], strlen(argv[c+1])+1);
                MarkerNumber[20] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }     
            if ((strcmp(argv[c], "-MarkerType") == 0) & (argc > c))
            {
                memcpy(&MarkerType, argv[c + 1], strlen(argv[c+1])+1);
                MarkerType[20] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }              
            if ((strcmp(argv[c], "-NameObserver") == 0) & (argc > c))
            {
                memcpy(&NameObserver, argv[c + 1], strlen(argv[c+1])+1);
                NameObserver[20] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }   
            if ((strcmp(argv[c], "-NameAgency") == 0) & (argc > c))
            {
                memcpy(&NameAgency, argv[c + 1], strlen(argv[c+1])+1);
                NameAgency[40] = '\0'; //null terminated
                c++;              //to skip the station name
                continue;
            }                          
            //Invalid Option
            cout << "option not recognized: " << argv[c] << "\n";
            return 0;
        }
    }

    //call the file conversion
    BINfile2RINEX(filename);
    return 0;
}