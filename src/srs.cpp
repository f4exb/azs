// A simple C++ program calculating the sunrise and sunset for
// the current date and a set location (latitude,longitude)
// Jarmo Lammi © 1999 - 2000
// jjlammi@netti.fi

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include "locator.h"

double tpi = 2 * M_PI;
double degs = 180.0/M_PI;
double rads = M_PI/180.0;

double L,g,daylen;
double SunDia = 0.53;  // Sunradius degrees

double AirRefr = 34.0/60.0; // athmospheric refraction degrees //


void to_xms(double input, int *intpart, int *minutes, double *seconds)
{
	*intpart = int(input);
	*minutes = int((input-*intpart)*60);
	*seconds = ((input-*intpart)*60-*minutes)*60;
}

// Get the days to J2000
// h is UT in decimal hours
// FNday only works between 1901 to 2099 - see Meeus chapter 7


double FNday (int y, int m, int d, float h) {
    long int luku = - 7 * (y + (m + 9)/12)/4 + 275*m/9 + d;

    // Typecasting needed for TClite on PC DOS at least, to avoid product overflow
    luku+= (long int)y*367;

    return (double)luku - 730531.5 + h/24.0;
};

// the function below returns an angle in the range
// 0 to 2*M_PI

double FNrange (double x) {
    double b = x / tpi;
    double a = tpi * (b - (long)(b));
    
    if (a < 0) 
        a = tpi + a;
        
    return a;
};

// Calculating the hourangle
double f0(double lat, double declin) {
    double fo,dfo;
    // Correction: different sign at S HS
    dfo = rads*(0.5*SunDia + AirRefr); if (lat < 0.0) dfo = -dfo;
    fo = tan(declin + dfo) * tan(lat*rads);

    if (fo > 0.99999) 
        fo=1.0; // to avoid overflow //
        
    fo = asin(fo) + M_PI/2.0;
    
    return fo;
};

// Calculating the hourangle for twilight times
//
double f1(double lat, double declin) {
    double fi,df1;

    // Correction: different sign at S HS
    df1 = rads * 6.0; if (lat < 0.0) df1 = -df1;
    fi = tan(declin + df1) * tan(lat*rads);

    if (fi > 0.99999) 
        fi=1.0; // to avoid overflow //
    fi = asin(fi) + M_PI/2.0;
    
    return fi;
};

// Find the ecliptic longitude of the Sun
double FNsun (double d) {
  //sol_data->sol_mano = FNrange(DEG2RAD(357.529 + 0.98560028*jour_nouveau)); //g
  //sol_data->sol_mlong = FNrange(DEG2RAD(280.459 + 0.98564736*jour_nouveau)); //q

    // mean longitude of the Sun
    //L = FNrange(280.461 * rads + .9856474 * rads * d); //q
    L = FNrange(280.459 * rads + .98564736 * rads * d); //q
    // mean anomaly of the Sun
    //g = FNrange(357.528 * rads + .9856003 * rads * d); //g
    g = FNrange(357.529 * rads + .98560028 * rads * d); //g
    // Ecliptic longitude of the Sun
    double e = FNrange(L + 1.915 * rads * sin(g) + .02 * rads * sin(2 * g));
    std::cout << "L:" << L << std::endl;
    std::cout << "g:" << g << std::endl;
    std::cout << "e:" << e << std::endl;
    
    return e;
};

// Display decimal hours in hours and minutes
void showhrmn(double dhr) {
    int hr,mn;

    hr=(int) dhr;
    mn = (dhr - (double) hr)*60;
    
    if (hr < 10) 
         std::cout << '0';
    
    std::cout << hr << ':';
    
    if (mn < 10) 
        std::cout << '0';
        
    std::cout << mn;
};

int main(int argc, char *argv[])
{
    double y,m,day,h,latit,longit,tz_d;

    time_t sekunnit;
    struct tm *p;
    
    bool use_loctime;
    bool locator_given = false;
    std::string loc_string;
    int jour_donne = 0;
    int mois_donne = 0;
    int annee_donnee = 0;
    int intpart, minutes;
    double seconds;
    

    //-----------------------------traitement des options---------------------------
    //------------------------------------------------------------------------------
    
    int index; 
    float f_val;
    opterr = 0;
    int c;
    
    while ((c = getopt (argc, argv, "a:l:L:D:M:Y:z:h")) != -1)
    {
    switch (c)
    {
        case 'a':
            loc_string = optarg;
            locator_given = true;
            break;
        case 'l':
            f_val = atof(optarg);
            longit = f_val;
            break;
        case 'L':
            f_val = atof(optarg);
            latit = f_val;
            break;
        case 'D':
            f_val = atof(optarg);
            jour_donne = f_val;
            break;
        case 'M':
            f_val = atof(optarg);
            mois_donne = f_val;
            break;
        case 'Y':
            f_val = atof(optarg);
            annee_donnee = f_val;
            break;
        case 'z':
            f_val = atof(optarg);
            tz_d = f_val;
            break;
        case '?':
            std::cerr << "Unknown option " << (char) optopt << std::endl;
        case 'h':
            std::cerr << "Usage is " << argv[0] << " options" << std::endl;
            std::cerr << "  options are:" << std::endl;
            std::cerr << "  -L: Latitude in decimal degrees, positive towards north" << std::endl;
            std::cerr << "  -l: Longitude in decimal degrees, positive towards east" << std::endl;
            std::cerr << "  -a: Maidenhead locator, if correct it overrides any -l or -L option given" << std::endl;
            std::cerr << "  -z: Timezone in integer hours to GMT" << std::endl;
            std::cerr << "  -D: Override day" << std::endl;
            std::cerr << "  -M: Override month" << std::endl;
            std::cerr << "  -Y: Override year" << std::endl;
            std::cerr << "  -h: This help" << std::endl;
            return 0;
        default:
            abort ();
        }
    }
    
    for (index = optind; index < argc; index++) {} // non option arguments

    if (locator_given)
    {
        try
        {
            Locator loc(loc_string);
            latit = loc.latitude();
            longit = loc.longitude();
            to_xms(latit, &intpart, &minutes, &seconds);
            std::cout << "Place of observation:" << std::endl;
            std::cout << "  Latitude.........: " << latit << " deg" << std::endl;
            std::cout << "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;
            to_xms(longit, &intpart, &minutes, &seconds);
            std::cout << "  Longitude........: " << longit << " deg" << std::endl;
            std::cout << "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl << std::endl;
        }
        catch (LocatorInvalidException ex)
        {
            std::cerr << "Warning: invalid locator " << ex.getString() << " using -l and -L options or their defaults" << std::endl;
        }
    }
    
    
    // read system date and extract the year    

    /** First get current time **/
    time(&sekunnit);

    /** Next get localtime **/

    p=localtime(&sekunnit);
    // this is Y2K compliant algorithm
    y = 1900 + p->tm_year;
    m = p->tm_mon + 1;
    day = p->tm_mday;
    h = 12;

    // Timezone hours
    double tzone = tz_d;
    
    if ((jour_donne > 0) and (jour_donne < 32))
    {
        day = jour_donne;
    }

    if ((mois_donne > 0) and (mois_donne < 13))
    {
        m = mois_donne;
    }

    if (annee_donnee > 0)
    {
        y = annee_donnee;
    }

    double d = FNday(y, m, day, h);
    std::cout << "d: " << d << std::endl;

    // Use FNsun to find the ecliptic longitude of the
    // Sun

    double lambda = FNsun(d);

    // Obliquity of the ecliptic

    //double obliq = 23.439 * rads - .0000004 * rads * d;
    double obliq = 23.439 * rads - .00000036 * rads * d;

    // Find the RA and DEC of the Sun

    std::cout << "obliq: " <<  obliq << std::endl;
    std::cout << "lambda: " <<  lambda << std::endl;
    
    double alpha = atan2(cos(obliq) * sin(lambda), cos(lambda));
    double delta = asin(sin(obliq) * sin(lambda));

    std::cout << "L: " << L << std::endl;
    std::cout << "alpha: " <<  alpha << std::endl;
    std::cout << "delta: " <<  delta << std::endl;

    // Find the Equation of Time in minutes
    // Correction suggested by David Smith

    double LL = L - alpha;
    if (L < M_PI) LL += tpi;
    double equation = 1440.0 * (1.0 - LL / tpi);

    double ha = f0(latit,delta);
    double hb = f1(latit,delta);
    
    std::cout << "ha: " << ha << std::endl;
    std::cout << "hb: " << hb << std::endl;
    
    double twx = hb - ha;   // length of twilight in radians
    twx = 12.0*twx/M_PI;      // length of twilight in degrees

    // Conversion of angle to hours and minutes //
    daylen = degs * ha / 7.5;
    if (daylen<0.0001) {daylen = 0.0;}
    // arctic winter   //

    double riset = 12.0 - 12.0 * ha/M_PI + tzone - longit/15.0 + equation/60.0;
    double settm = 12.0 + 12.0 * ha/M_PI + tzone - longit/15.0 + equation/60.0;
    double noont = riset + 12.0 * ha/M_PI;
    double altmax = 90.0 + delta * degs - latit;
    // Correction suggested by David Smith
    // to express as degrees from the N horizon

    if (delta * degs > latit ) 
        altmax = 90.0 + latit - delta * degs;

    double twam = riset - twx;      // morning twilight begin
    double twpm = settm + twx;      // evening twilight end

    if (riset > 24.0) 
        riset-= 24.0;
        
    if (settm > 24.0) 
        settm-= 24.0;

    std::cout << "\n Sunrise and set\n";
    std::cout << "===============\n";

    std::cout.setf(std::ios::fixed);
    std::cout.precision(0);

    std::cout << "  year  : " << y << '\n';
    std::cout << "  month : " << m << '\n';
    std::cout << "  day   : " << day << "\n\n";
    std::cout << "Days until Y2K :  " << -d << '\n';

    std::cout.precision(2);
    std::cout << "Latitude :  " << latit << ", longitude:  " << longit << '\n';
    std::cout << "Timezone :  " << tzone << "\n\n";
    std::cout << "Declination : " << delta * degs << '\n';
    std::cout << "Daylength   : "; showhrmn(daylen); std::cout << " hours \n";

    std::cout << "Begin civil twilight: ";
    showhrmn(twam); std::cout << '\n';

    std::cout << "Sunrise     : ";
    showhrmn(riset); std::cout << '\n';

    std::cout << "Sun altitude at noontime ";

    // Correction by D. Smith
    showhrmn(noont); std::cout << " = " << altmax << " degrees" << (latit>= delta * degs ? "S" : "N")<< std::endl;

    std::cout << "Sunset      : ";
    showhrmn(settm); std::cout << '\n';
    std::cout << "Civil twilight: ";
    showhrmn(twpm); std::cout << '\n';
    
    return 0;
}

