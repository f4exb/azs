//-----------------------------bibliothèques------------------------------------
//------------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <getopt.h>
#include <sys/time.h>
#include "locator.h"

// conversions
#define DEG2RAD(x)  (x) * (M_PI / 180.0)
#define RAD2DEG(x)  (x) * (180.0 / M_PI)
#define DEG2HOUR(x) (x) * (24.0 / 360.0)
#define HOUR2RAD(x) (x) * (M_PI / 12.0)

//-----------------------------déclaration--------------------------------------
//------------------------------------------------------------------------------

double SunDia = 0.53;  // Sunradius degrees
double AirRefr = 34.0/60.0; // athmospheric refraction degrees //
bool verboseMode = false;

typedef struct lever_midi_coucher_s 
{
    double daylen;
    double riset;
    double settm;
    double noont;
    double altmax;
    double twam;
    double twpm;
} lever_midi_coucher_t;

typedef struct sol_data_s
{
    double sol_mano;
    double sol_elong;
    double sol_mlong;
} sol_data_t;

typedef struct sol_pos_s
{
    double asc_droite;
    double declination;
} sol_pos_t;

typedef struct location_s
{
    double latitude;
    double longitude;
} location_t;


//-----------------------------fonction d'attente-------------------------------
//------------------------------------------------------------------------------

void to_xms(double input, int *intpart, int *minutes, double *seconds)
{
	*intpart = int(input);
	*minutes = int((input-*intpart)*60);
	*seconds = ((input-*intpart)*60-*minutes)*60;
}

// the function below returns an angle in the range
// 0 to 2*M_PI

double FNrange (double x) {
    double tpi = 2 * M_PI;
    double b = x / tpi;
    double a = tpi * (b - (long)(b));
    
    if (a < 0) 
        a = tpi + a;
        
    return a;
};


//-----------------------------calcul du jour julien----------------------------
//------------------------------------------------------------------------------

double calcul_jour_julien(double jour, double mois, double annee, double heure, double minute, double seconde)
{
  double month, year, day, a, b, jour_julien;

  day=jour+heure/24.0+minute/1440.0+seconde/86400.0;
  year=annee;
  month=mois;
 
  if(month==1 || month==2)
  {
    year=year-1.0;
    month=month+12.0;
  }

  a=int(year/100.0);
  b=2-a+int(a/4.0);

  jour_julien=int(365.25*(year+4716.0))+int(30.6001*(month+1.0))+day+b-1524.5;
  return jour_julien;
}


//------------calcul de donnees solaires de base: ------------------------------
// location->longitude moyenne du soleil
// Anomalie moyenne du soleil
// location->longitude ecliptique du soleil
//------------------------------------------------------------------------------
void calcul_data_solaires(double jour_nouveau, sol_data_t *sol_data)
{
  double g, q, l;
  
  sol_data->sol_mano = FNrange(DEG2RAD(357.529 + 0.98560028*jour_nouveau)); //g
  sol_data->sol_mlong = FNrange(DEG2RAD(280.459 + 0.98564736*jour_nouveau)); //q or L
  sol_data->sol_elong = FNrange(sol_data->sol_mlong + DEG2RAD(1.915*sin(sol_data->sol_mano) + 0.020*sin(2*(sol_data->sol_mano))));
  
  if (verboseMode)
  {
      std::cout << "sol_mano (g): " << sol_data->sol_mano << std::endl;
      std::cout << "sol_mlong (L): " << sol_data->sol_mlong << std::endl;
      std::cout << "sol_elong (lambda): " << sol_data->sol_elong << std::endl;
  }
 
  
}

//------------calcul de l'ascenscion droite et de la declinaison ---------------
//------------------------------------------------------------------------------
void calcul_asc_decl(double jour_nouveau, double sol_elong, sol_pos_t *sol_pos)
{
  double e = DEG2RAD(23.439 - 0.00000036*jour_nouveau);

  if (verboseMode)
  {
    std::cout << "obliq (e): " << e << std::endl << std::endl;
  }
  
  //sol_pos->asc_droite=RAD2DEG(atan(cos(DEG2RAD(e))*sin(DEG2RAD(sol_elong))/cos(DEG2RAD(sol_elong))))/15.0;
  sol_pos->asc_droite=RAD2DEG(atan2(cos(e)*sin(sol_elong),cos(sol_elong)))/15.0;
  
  /*
  if(cos(DEG2RAD(sol_elong))<0)
    sol_pos->asc_droite=12.0+sol_pos->asc_droite;

  if(cos(DEG2RAD(sol_elong))>0 && sin(DEG2RAD(sol_elong))<0)
    sol_pos->asc_droite=sol_pos->asc_droite+24.0;
  */
  sol_pos->declination=RAD2DEG(asin(sin(e)*sin(sol_elong)));
}


//-------------------------calcul de l'angle horaire ---------------------------
//------------------------------------------------------------------------------

double calcul_angle_horaire(double jour_nouveau, int heure, int minute, int seconde, double ascension_droite, double longitude)
{
  double heure_siderale1, heure_siderale2, nb_siecle;
  double angleH, angleT, angle;
  
  nb_siecle=jour_nouveau/36525.0;
  heure_siderale1=(24110.54841+(8640184.812866*nb_siecle)+(0.093104*(nb_siecle*nb_siecle))-(0.0000062*(nb_siecle*nb_siecle*nb_siecle)))/3600.0;
  heure_siderale2=((heure_siderale1/24.0)-int(heure_siderale1/24.0))*24.0;
 
  angleH=360.0*heure_siderale2/23.9344;
  //angleT=(heure2-correction_heure-12.0+minute2/60.0+seconde2/3600.0)*360.0/23.9344;
  angleT=(heure-12.0+minute/60.0+seconde/3600.0)*360.0/23.9344;
  angle=angleT+angleH;
 
  return angle-ascension_droite*15.0+longitude;
}


//------------calcul de l'alititude et l'azimuth solaires-----------------------
//------------------------------------------------------------------------------
void calcul_alt_az(double declinaison, double latitude, double angle_horaire, double *altitude, double *azimuth)
{
  *altitude=RAD2DEG(asin(sin(DEG2RAD(declinaison))*sin(DEG2RAD(latitude))-cos(DEG2RAD(declinaison))*cos(DEG2RAD(latitude))*cos(DEG2RAD(angle_horaire))));
  *azimuth=RAD2DEG(acos((sin(DEG2RAD(declinaison))-sin(DEG2RAD(latitude))*sin(DEG2RAD(*altitude)))/(cos(DEG2RAD(latitude))*cos(DEG2RAD(*altitude)))));
  
  double sinazimuth=(cos(DEG2RAD(declinaison))*sin(DEG2RAD(angle_horaire)))/cos(DEG2RAD(*altitude));
  
  if(sinazimuth<0)
    *azimuth=360-*azimuth;
}


double calcul_angle_horaire_zero_rad(double declinaison, double latitude)
{
    double fo,dfo;
    
    // Correction: different sign at S HS
    dfo = DEG2RAD(0.5*SunDia + AirRefr); 
    
    if (latitude < 0.0) 
        dfo = -dfo;
        
    fo = tan(DEG2RAD(declinaison) + dfo) * tan(DEG2RAD(latitude));

    if (fo > 0.99999) 
        fo=1.0; // to avoid overflow //
        
    fo = asin(fo) + M_PI/2.0;
    
    return fo;    
}


double calcul_angle_horaire_crepusculaire_rad(double declinaison, double latitude)
{
    double fi,df1;

    // Correction: different sign at S HS
    df1 = DEG2RAD(6.0); 
    
    if (latitude < 0.0) 
        df1 = -df1;
    
    fi = tan(DEG2RAD(declinaison) + df1) * tan(DEG2RAD(latitude));

    if (fi > 0.99999) 
        fi=1.0; // to avoid overflow //
        
    fi = asin(fi) + M_PI/2.0;
    
    return fi;    
}


void calcul_lever_midi_coucher(double sol_mlong, sol_pos_t *sol_pos, location_t *location, lever_midi_coucher_t *lmc)
{
    if (verboseMode)
    {
        std::cout << "L: " << sol_mlong << std::endl;
        std::cout << "alpha: " << HOUR2RAD(sol_pos->asc_droite) << std::endl;
        std::cout << "delta: " << DEG2RAD(sol_pos->declination) << std::endl;
    }
    
    double LL = sol_mlong - HOUR2RAD(sol_pos->asc_droite);
    
    if (sol_mlong < M_PI) 
        LL += 2*M_PI;
        
    double equation = 1440.0 * (1.0 - LL / (2*M_PI));

    double ha = calcul_angle_horaire_zero_rad(sol_pos->declination, location->latitude);
    double hb = calcul_angle_horaire_crepusculaire_rad(sol_pos->declination, location->latitude);
    
    if (verboseMode)
    {
        std::cout << "ha: " << ha << std::endl;
        std::cout << "hb: " << hb << std::endl << std::endl;
    }
    
    double twx = hb - ha;   // length of twilight in radians
    twx = 12.0*twx/M_PI;      // length of twilight in degrees

    // Conversion of angle to hours and minutes //
    lmc->daylen = RAD2DEG(ha) / 7.5;
    
    if (lmc->daylen<0.0001) 
        lmc->daylen = 0.0;
    // arctic winter   //

    lmc->riset = 12.0 - 12.0 * ha/M_PI - location->longitude/15.0 + equation/60.0;
    lmc->settm = 12.0 + 12.0 * ha/M_PI - location->longitude/15.0 + equation/60.0;
    lmc->noont = lmc->riset + 12.0 * ha/M_PI;
    lmc->altmax = 90.0 + sol_pos->declination - location->latitude;
    // Correction suggested by David Smith
    // to express as degrees from the N horizon

    if (sol_pos->declination > location->latitude ) 
        lmc->altmax = 90.0 + location->latitude - sol_pos->declination;

    lmc->twam = lmc->riset - twx;      // morning twilight begin
    lmc->twpm = lmc->settm + twx;      // evening twilight end

    if (lmc->riset > 24.0) 
        lmc->riset-= 24.0;
        
    if (lmc->settm > 24.0) 
        lmc->settm-= 24.0;
}


//-----------------calcul de l'angle horaire a zero altitude--------------------
// calcul approché utilisant les valeurs d'ascenscion droite et de déclinaison 
// calculés pour l'heure courante
//------------------------------------------------------------------------------

double calcul_zero_alt_angle_horaire(double declinaison, double latitude)
{
	// resolution de asin(A - B*cos(angle_horaire)) = 0 (altitude)
	double A = sin(DEG2RAD(declinaison))*sin(DEG2RAD(latitude));
	double B = cos(DEG2RAD(declinaison))*cos(DEG2RAD(latitude));
	return RAD2DEG(acos(A/B)); // solution positive avec n=0 dans {2PIn+acos(A/B), -(2PIn+acos(A/B))}
}

//-----------------calcul de l'heure pour un angle horaire----------------------
// calcul approché utilisant la valeur du jour julien et ascenscion droite
// calculés pour l'heure courante
//------------------------------------------------------------------------------
double calcul_heure_pour_angle_horaire(double jour_nouveau, double angle_horaire, double ascension_droite, double longitude)
{
    // angle_horaire = angleT + angleH - ascension_droite*15.0 + longitude 
	// => angleT = angle_horaire + ascension_droite*15.0 - longitude - angleH
	// angleT = (hdec - 12.0)* (360.0/23.9344)
	// => (hdec - 12.0)* (360.0/23.9344) = angle_horaire + ascension_droite*15.0 - longitude - angleH
	// => hdec - 12.0 = (angle_horaire + ascension_droite*15.0 - longitude - angleH)*(23.9344/360.0)
	// => hdec = (angle_horaire + ascension_droite*15.0 - longitude - angleH)*(23.9344/360.0) + 12.0

	double nb_siecle=jour_nouveau/36525.0;
    double heure_siderale1=(24110.54841+(8640184.812866*nb_siecle)+(0.093104*(nb_siecle*nb_siecle))-(0.0000062*(nb_siecle*nb_siecle*nb_siecle)))/3600.0;
    double heure_siderale2=((heure_siderale1/24.0)-int(heure_siderale1/24.0))*24.0;
 
    double angleH = 360.0*heure_siderale2/23.9344;
	
	double A = angle_horaire + ascension_droite*15.0 - longitude - angleH - 360.0; // abracadabra subtract 360
	
	//std::cout << angleH << std::endl;
	
	return (A*(23.9344/360.0))+12.0;
}


//-----------------------------programme principal------------------------------
//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  time_t t;
  struct tm *t_tm;
  double sd, jour2, mois2, heure2, minute2, annee2, seconde2, correction_heure=0.0, heure_donnee=-1.0;
  double jour_nouveau, heure_siderale1, heure_siderale2, nb_siecle;
  double angle, angleT, angleH, angle_horaire, altitude, azimuth, sinazimuth;
  bool use_loctime;
  bool locator_given = false;
  std::string loc_string;
  int intpart, minutes;
  double seconds;
  int jour_donne = 0;
  int mois_donne = 0;
  int annee_donnee = 0;
  sol_data_t sol_data;
  sol_pos_t sol_pos;
  location_t location;

  location.latitude=0.0;
  location.longitude=0.0;
  
  //-----------------------------avertissement------------------------------------
  //------------------------------------------------------------------------------
 
  std::cout << "------------------------------------------------------------------------------" << std::endl;
  std::cout << " Attention! Si vous utilisez ce programme, je souhaiterais recevoir un mail : " << std::endl;
  std::cout << " rusty55@caramail.com Merci d'avoir choisi AltAz Solaire !" << std::endl;
  std::cout << "------------------------------------------------------------------------------" << std::endl;

  //-----------------------------traitement des options---------------------------
  //------------------------------------------------------------------------------
 
  int index; 
  float f_val;
  opterr = 0;
  int c;
     
  while ((c = getopt (argc, argv, "a:l:L:t:T:D:M:Y:hv")) != -1)
  {
    switch (c)
    {
      case 'a':
        loc_string = optarg;
        locator_given = true;
        break;
      case 'l':
        f_val = atof(optarg);
        location.longitude = f_val;
        break;
      case 'L':
        f_val = atof(optarg);
        location.latitude = f_val;
        break;
      case 't':
        f_val = atof(optarg);
        correction_heure = f_val;
        break;
      case 'T':
        f_val = atof(optarg);
        heure_donnee = f_val;
        /*
        if ((f_val > 12.0) || (f_val < -12.0))
          correction_heure = 0.0;
        else
          correction_heure = f_val;
          */
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
      case 'v':
        verboseMode = true;
        break;
      case '?':
        std::cerr << "Unknown option " << (char) optopt << std::endl;
	  case 'h':
        std::cerr << "Usage is " << argv[0] << " options" << std::endl;
        std::cerr << "  options are:" << std::endl;
        std::cerr << "  -L: location->latitude in decimal degrees, positive towards north" << std::endl;
        std::cerr << "  -l: location->longitude in decimal degrees, positive towards east" << std::endl;
        std::cerr << "  -a: Maidenhead locator, if correct it overrides any -l or -L option given" << std::endl;
        std::cerr << "  -t: Delta time to GMT in integer hours" << std::endl;
        std::cerr << "  -T: Override GMT in integer hours" << std::endl;
        std::cerr << "  -D: Override day" << std::endl;
        std::cerr << "  -M: Override month" << std::endl;
        std::cerr << "  -Y: Override year" << std::endl;
        std::cerr << "  -v: Verbose (shows some intermediate results)" << std::endl;
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
      location.latitude = loc.latitude();
      location.longitude = loc.longitude();
	  to_xms(location.latitude, &intpart, &minutes, &seconds);
      std::cout << "Place of observation:" << std::endl;
      std::cout << "  latitude.........: " << location.latitude << " deg" << std::endl;
      std::cout << "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;
	  to_xms(location.longitude, &intpart, &minutes, &seconds);
      std::cout << "  longitude........: " << location.longitude << " deg" << std::endl;
      std::cout << "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl << std::endl;
    }
    catch (LocatorInvalidException ex)
    {
      std::cerr << "Warning: invalid locator " << ex.getString() << " using -l and -L options or their defaults" << std::endl;
    }
  }
    
  //-----------------------------boucle de calculs--------------------------------
  //------------------------------------------------------------------------------

  t = time(NULL);
    
  t += (time_t) (3600.0 * correction_heure);
  t_tm = gmtime(&t);
  
  jour2 = t_tm->tm_mday;
  mois2 = t_tm->tm_mon+1;
  annee2 = t_tm->tm_year+1900;
  heure2 = t_tm->tm_hour;
  minute2 = t_tm->tm_min;
  seconde2 = t_tm->tm_sec;
  
  if ((heure_donnee >= 0.0) && (heure_donnee < 24.0))
  {
	to_xms(heure_donnee, &intpart, &minutes, &seconds);
	heure2 = intpart;
	minute2 = minutes;
	seconde2 = seconds;
  }
  
  if ((jour_donne > 0) and (jour_donne < 32))
  {
	jour2 = jour_donne;
  }
  
  if ((mois_donne > 0) and (mois_donne < 13))
  {
	mois2 = mois_donne;
  }
  
  if (annee_donnee > 0)
  {
	annee2 = annee_donnee;
  }

# //-----------------------------affichages date et heure-------------------------
# //------------------------------------------------------------------------------

  std::cout <<     "Time of observation:" << std::endl;
  std::cout <<     "  Date.............: " << int(jour2) << " " << int(mois2) << " " << int(annee2) << std::endl;
  std::cout <<     "  GMT Time.........: " << heure2 + minute2/60.0 + seconde2/3600.0 << std::endl;
  std::cout <<     "                   : " << int(heure2)<<":";
 
  if (minute2<10) 
    std::cout << "0";
  
  std::cout << int(minute2) <<":";
  
  if (seconde2<10) 
    std::cout << "0";
 
  std::cout << int(seconde2) << std::endl;

# //-----------------------------affichages jour julien et jour julien nouveau----
# //------------------------------------------------------------------------------
 
  std::cout <<     "  Julian day.......: " << std::setiosflags(std::ios::fixed) << calcul_jour_julien(jour2, mois2, annee2, heure2, minute2, seconde2) << std::endl;
  jour_nouveau = calcul_jour_julien(jour2, mois2, annee2, heure2, minute2, seconde2) -2451545.0;
  std::cout <<     "  New Julian day...: " << jour_nouveau << std::endl << std::endl;

# //-------------calculs et affichages ascension droite et délinaison-------------
# //------------------------------------------------------------------------------

  calcul_data_solaires(jour_nouveau, &sol_data);
  calcul_asc_decl(jour_nouveau, sol_data.sol_elong, &sol_pos);

  to_xms(sol_pos.asc_droite, &intpart, &minutes, &seconds);
  std::cout <<     "Sun's absolute position:" << std::endl;
  std::cout <<     "  Right ascension..: " << sol_pos.asc_droite << " h" << std::endl;
  std::cout <<     "                   : " << intpart << " h " << minutes << " min " << seconds << " s" << std::endl;

  to_xms(sol_pos.declination, &intpart, &minutes, &seconds);
  std::cout <<     "  Declination......: " << sol_pos.declination << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl << std::endl;

# //-----------------------------calculs heure sidérale et angle horaire----------
# //------------------------------------------------------------------------------
  
  angle_horaire = calcul_angle_horaire(jour_nouveau, heure2, minute2, seconde2, sol_pos.asc_droite, location.longitude);

 
# //-------------calculs et affichages altitude et azimuth------------------------
# //------------------------------------------------------------------------------

  calcul_alt_az(sol_pos.declination, location.latitude, angle_horaire, &altitude, &azimuth);

  to_xms(altitude, &intpart, &minutes, &seconds);
  std::cout <<     "Sun's relative position at the place of observation:" << std::endl;
  std::cout <<     "  Altitude.........: " << altitude << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;

  to_xms(azimuth, &intpart, &minutes, &seconds);
  std::cout <<     "  Azimuth..........: " << azimuth << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl << std::endl;

  
# //-------------calculs et affichages heure de lever et coucher------------------
# //------------------------------------------------------------------------------

  /*
  double angle_h_a_zero_alt = calcul_zero_alt_angle_horaire(declinaison, location->latitude);
  double heure_lever = calcul_heure_pour_angle_horaire(jour_nouveau, angle_h_a_zero_alt, ascension_droite, location->longitude);
  double heure_coucher = calcul_heure_pour_angle_horaire(jour_nouveau, 360.0-angle_h_a_zero_alt, ascension_droite, location->longitude);
  */
  double azimuth_lever, azimuth_coucher, azimuth_apex, alt_lever, alt_coucher, alt_apex;
  lever_midi_coucher_t lmc;
  //sol_data_t sol_data_lmc;
  //sol_pos_t sol_pos_lmc;
  //double jour_nouveau_lmc = calcul_jour_julien(jour2, mois2, annee2, 12.0, 0.0, 0.0) - 2451545.0;
  
  //calcul_data_solaires(jour_nouveau_lmc, &sol_data_lmc);
  //calcul_asc_decl(jour_nouveau_lmc, sol_data_lmc.sol_elong, &sol_pos_lmc);
  //calcul_lever_midi_coucher(sol_data_lmc.sol_mlong, &sol_pos_lmc, &location, &lmc); // calculation at midday (pretty useless)
  calcul_lever_midi_coucher(sol_data.sol_mlong, &sol_pos, &location, &lmc); 
  
  to_xms(lmc.riset, &intpart, &minutes, &seconds);
  std::cout <<     "Sun's rising/apex/setting times at the place of observation:" << std::endl;
  std::cout <<     "  Rising time......: " << lmc.riset << " h GMT" << std::endl;
  std::cout <<     "                   : " << intpart << " h " << minutes << " min " << seconds << " s" << std::endl;
  
  double angle_horaire_lever = calcul_angle_horaire(jour_nouveau, intpart, minutes, seconds, sol_pos.asc_droite, location.longitude);
  calcul_alt_az(sol_pos.declination, location.latitude, angle_horaire_lever, &alt_lever, &azimuth_lever);
  
  to_xms(azimuth_lever, &intpart, &minutes, &seconds);
  std::cout <<     "        azimuth....: " << azimuth_lever << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;

  to_xms(lmc.noont, &intpart, &minutes, &seconds);
  std::cout <<     "  Apex time........: " << lmc.noont << " h GMT" << std::endl;
  std::cout <<     "                   : " << intpart << " h " << minutes << " min " << seconds << " s" << std::endl;
  
  double angle_horaire_apex = calcul_angle_horaire(jour_nouveau, intpart, minutes, seconds, sol_pos.asc_droite, location.longitude);
  calcul_alt_az(sol_pos.declination, location.latitude, angle_horaire_apex, &alt_apex, &azimuth_apex);

  to_xms(azimuth_apex, &intpart, &minutes, &seconds);
  std::cout <<     "        azimuth....: " << azimuth_apex << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;

  to_xms(lmc.altmax, &intpart, &minutes, &seconds);
  std::cout <<     "        altitude...: " << lmc.altmax << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;
  
  to_xms(lmc.settm, &intpart, &minutes, &seconds);
  std::cout <<     "  Setting time.....: " << lmc.settm << " h GMT" << std::endl;
  std::cout <<     "                   : " << intpart << " h " << minutes << " min " << seconds << " s" << std::endl;
  
  double angle_horaire_coucher = calcul_angle_horaire(jour_nouveau, intpart, minutes, seconds, sol_pos.asc_droite, location.longitude);
  calcul_alt_az(sol_pos.declination, location.latitude, angle_horaire_coucher, &alt_coucher, &azimuth_coucher);
  
  to_xms(azimuth_coucher, &intpart, &minutes, &seconds);
  std::cout <<     "        azimuth....: " << azimuth_coucher << " deg" << std::endl;
  std::cout <<     "                   : " << intpart << " deg " << minutes << " min " << seconds << " s" << std::endl;
  
  to_xms(lmc.daylen, &intpart, &minutes, &seconds);
  std::cout <<     "  Day duration.....: " << lmc.daylen << " h" << std::endl;
  std::cout <<     "                   : " << intpart << " h " << minutes << " min " << seconds << " s" << std::endl;
  
  std::cout << "------------------------------------------------------------------------------" << std::endl;

  return 0;
}
