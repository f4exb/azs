azs
===

Solar Azimuth and Elevation command line calculator and more for amateur radio

Solar Azimuth and Elevation calculator
--------------------------------------

*azs*

Calculates Sun's position details at the given place of observation at time given by host RTC. This can be overriden with specific options as shown by help command.

        azs -h
        ------------------------------------------------------------------------------
         Attention! Si vous utilisez ce programme, je souhaiterais recevoir un mail : 
         rusty55@caramail.com Merci d'avoir choisi AltAz Solaire !
        ------------------------------------------------------------------------------
        Usage is /opt/install/azs/bin/azs options
          options are:
          -L: location->latitude in decimal degrees, positive towards north
          -l: location->longitude in decimal degrees, positive towards east
          -a: Maidenhead locator, if correct it overrides any -l or -L option given
          -t: Delta time to GMT in integer hours
          -T: Override GMT in integer hours
          -D: Override day
          -M: Override month
          -Y: Override year
          -v: Verbose (shows some intermediate results)
          -h: This help

Example:

        azs -a jn33nn
        ------------------------------------------------------------------------------
         Attention! Si vous utilisez ce programme, je souhaiterais recevoir un mail : 
         rusty55@caramail.com Merci d'avoir choisi AltAz Solaire !
        ------------------------------------------------------------------------------
        Place of observation:
          latitude.........: 43.5625 deg
                           : 43 deg 33 min 45 s
          longitude........: 7.125 deg
                           : 7 deg 7 min 30 s

        Time of observation:
          Date.............: 30 12 2013
          GMT Time.........: 20.3506
                           : 20:21:02
          Julian day.......: 2456657.347940
          New Julian day...: 5112.347940

        Sun's absolute position:
          Right ascension..: -5.325083 h
                           : -5 h -19 min -30.299544 s
          Declination......: -23.110817 deg
                           : -23 deg -6 min -38.941347 s

        Sun's relative position at the place of observation:
          Altitude.........: -45.981167 deg
                           : -45 deg -58 min -52.200913 s
          Azimuth..........: 281.810497 deg
                           : 281 deg 48 min 37.790054 s

        Sun's rising/apex/setting times at the place of observation:
          Rising time......: 7.099997 h GMT
                           : 7 h 5 min 59.988973 s
                azimuth....: 122.148876 deg
                           : 122 deg 8 min 55.954032 s
          Apex time........: 11.571167 h GMT
                           : 11 h 34 min 16.200388 s
                azimuth....: 180.256132 deg
                           : 180 deg 15 min 22.073605 s
                altitude...: 23.326683 deg
                           : 23 deg 19 min 36.058653 s
          Setting time.....: 16.042337 h GMT
                           : 16 h 2 min 32.411804 s
                azimuth....: 238.198210 deg
                           : 238 deg 11 min 53.554646 s
          Day duration.....: 8.942340 h
                           : 8 h 56 min 32.422831 s
        ------------------------------------------------------------------------------

Distance and azimuth between two places caculator
-------------------------------------------------

*loc*

Calculates distance and azimuth between two places given either latitude and longitude or Maidenhead locators. Options are:

        -a: My locator
        -L: My latitude in decimal degrees, positive towards north
        -l: My longitude in decimal degrees, positive towards east
        -b: Distant locator
        -D: Distant latitude in decimal degrees, positive towards north
        -d: Distant longitude in decimal degrees, positive towards east
        uses lat,lon and displays the locator if -a or -b option is empty or not present
        else disregards lat,lon options
        calculates distance and bearing only if distant point info is provided

Example:

        loc -a jn33nn -b jn12kq
        ------------------------------------------------------------------------------
        My Station:
          Latitude....: 43.5625 deg
          Longitude...: 7.125 deg

        Other Station:
          Latitude....: 42.6875 deg
          Longitude...: 2.875 deg

        To other station:
          Distance....: 358.331 km
          Bearing.....: 255.707 deg

        From other station:
          Bearing.....: 72.8011 deg
        ------------------------------------------------------------------------------

Elevated scatter point calculator (Rain or Aircraft scatter)
------------------------------------------------------------

*elev*

Calculates range of signal scattered from an elevated reflexion or refraction point. This can be aircraft or rain scatter for example. Options are:

        -d: Distance to SCP in km
        -H: Height my station above ground in km
        -h: Height of SCP above ground in km
        -c: Earth curvature correction (default 1.0)
        returns the elevation angle in decimal degrees

Example:

        elev -d 250 -H 1.25 -h 10 -c 1.5
        ------------------------------------------------------------------------------
        elevation angle......................: 1.25453 deg
        zero angle height from my station....: 4.52139 km
        maximum horizon distance from scatter: 436.994 km
        ------------------------------------------------------------------------------

Test programs not for general use
---------------------------------

*srs*

Sun's position calculation with some gory details.

