/*
    Copyright 2008 Edouard Griffiths, F4EXB.

   
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    See gpl.txt for the details of the licensing terms.

 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <getopt.h>
#include "math.h"

#define actual_earth_radius 6371.0

int main(int argc, char *argv[])
{
  float distance=10.0, height=5.0, station_height=0.0, curvature_correction=1.0;
  double earth_radius = actual_earth_radius;

  //-----------------------------traitement des options---------------------------
  //------------------------------------------------------------------------------
 
  int index; 
  opterr = 0;
  int c;
     
  while ((c = getopt (argc, argv, "d:h:H:c:")) != -1)
  {
    switch (c)
    {
      case 'd':
        distance = atof(optarg);
        break;
      case 'H':
        station_height = atof(optarg);
        break;
      case 'h':
        height = atof(optarg);
        break;
      case 'c':
        curvature_correction = atof(optarg);
        break;
      case '?':
        std::cerr << "Unknown option " << optopt << std::endl;
        std::cerr << "Usage is " << argv[0] << " options" << std::endl;
        std::cerr << "  options are:" << std::endl;
        std::cerr << "  -d: Distance to SCP in km" << std::endl;
        std::cerr << "  -H: Height my station above ground in km" << std::endl;
        std::cerr << "  -h: Height of SCP above ground in km" << std::endl;
        std::cerr << "  -c: Earth curvature correction (default 1.0)" << std::endl;
        std::cerr << "  returns the elevation angle in decimal degrees" << std::endl;
        return 1;
      default:
        abort ();
    }
  }
     
  for (index = optind; index < argc; index++) {} // non option arguments

  //-----------------------------calculate----------------------------------------
  //------------------------------------------------------------------------------

  std::cout << "------------------------------------------------------------------------------" << std::endl;  

  earth_radius *= curvature_correction;
    
  double earth_angle = distance / earth_radius;
  double zero_angle_height = ((earth_radius+station_height) / cos(earth_angle))-earth_radius;
  double angle_rad = atan(((height-zero_angle_height)*cos(earth_angle))/((zero_angle_height+earth_radius)*sin(earth_angle)));
  double max_angle = acos(earth_radius/(height+earth_radius));
  double max_distance = max_angle * earth_radius; 
  std::cout << "elevation angle......................: " << angle_rad * (180.0 / M_PI) << " deg" << std::endl;
  std::cout << "zero angle height from my station....: " << zero_angle_height << " km" << std::endl;
  std::cout << "maximum horizon distance from scatter: " << max_distance << " km" << std::endl;
    
  std::cout << "------------------------------------------------------------------------------" << std::endl;  

  return 0;
}
