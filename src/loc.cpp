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
#include "locator.h"

int main(int argc, char *argv[])
{
  float my_latitude = 0.0, my_longitude = 0.0, other_latitude = 0.0, other_longitude = 0.0;
  std::string my_locator, other_locator;
  Locator *my_loc, *other_loc;
  bool my_loc_present = false, other_loc_present = false, distant_data_present = false;
  
  //-----------------------------traitement des options---------------------------
  //------------------------------------------------------------------------------
 
  int index; 
  opterr = 0;
  int c;
     
  while ((c = getopt (argc, argv, "a:L:l:b:D:d:")) != -1)
  {
    switch (c)
    {
      case 'a':
        my_locator = optarg;
        my_loc_present = true;
        break;
      case 'L':
        my_latitude = atof(optarg);
        break;
      case 'l':
        my_longitude = atof(optarg);
        break;
      case 'b':
        other_locator = optarg;
        other_loc_present = true;
        distant_data_present = true;
        break;
      case 'D':
        other_latitude = atof(optarg);
        distant_data_present = true;
        break;
      case 'd':
        other_longitude = atof(optarg);
        distant_data_present = true;
        break;
      case '?':
        std::cerr << "Unknown option " << optopt << std::endl;
        std::cerr << "Usage is " << argv[0] << " options" << std::endl;
        std::cerr << "  options are:" << std::endl;
        std::cerr << "  -a: My locator" << std::endl;
        std::cerr << "  -L: My latitude in decimal degrees, positive towards north" << std::endl;
        std::cerr << "  -l: My longitude in decimal degrees, positive towards east" << std::endl;
        std::cerr << "  -b: Distant locator" << std::endl;
        std::cerr << "  -D: Distant latitude in decimal degrees, positive towards north" << std::endl;
        std::cerr << "  -d: Distant longitude in decimal degrees, positive towards east" << std::endl;
        std::cerr << "  uses lat,lon and displays the locator if -a or -b option is empty or not present" << std::endl;
        std::cerr << "  else disregards lat,lon options" << std::endl;
        std::cerr << "  calculates distance and bearing only if distant point info is provided" << std::endl;
        return 1;
      default:
        abort ();
    }
  }
     
  for (index = optind; index < argc; index++) {} // non option arguments

  try
  {
    if (my_loc_present)
      my_loc = new Locator(my_locator);
    else
      my_loc = new Locator(my_latitude, my_longitude);
  }
  catch (LocatorInvalidException ex)
  {
    std::cerr << "Locator " << ex.getString() << " is invalid" << std::endl;
    delete my_loc;
    return 1;
  }
  
  try
  {
    if (other_loc_present)
      other_loc = new Locator(other_locator);
    else
      other_loc = new Locator(other_latitude, other_longitude);
  }
  catch (LocatorInvalidException ex)
  {
    std::cerr << "Locator " << ex.getString() << " is invalid" << std::endl;
    delete other_loc;
    return 1;
  }
  
  std::cout << "------------------------------------------------------------------------------" << std::endl;  
  
  std::cout <<   "My Station:" << std::endl;
  
  if (my_loc_present)
  {
    std::cout << "  Latitude....: " << my_loc->latitude()  << " deg" << std::endl;
    std::cout << "  Longitude...: " << my_loc->longitude() << " deg" << std::endl;
  }
  else
    std::cout << "  Locator.....: " << my_loc->toString() << std::endl;
  
  if (distant_data_present)
  {
    LocPoint my_point(*my_loc);
    LocPoint other_point(*other_loc);

    std::cout << std::endl;
    std::cout <<   "Other Station:" << std::endl;
    
    if (other_loc_present)
    {
      std::cout << "  Latitude....: " << other_loc->latitude()  << " deg" << std::endl;
      std::cout << "  Longitude...: " << other_loc->longitude() << " deg" << std::endl;
    }
    else
      std::cout << "  Locator.....: " << other_loc->toString() << std::endl;

    std::cout << std::endl;
    
    std::cout << "To other station:" << std::endl;
    std::cout << "  Distance....: " << my_point.distanceTo(other_point) << " km" << std::endl;
    std::cout << "  Bearing.....: " << my_point.bearingTo(other_point) << " deg" << std::endl;

    std::cout << std::endl;

    std::cout << "From other station:" << std::endl;
    std::cout << "  Bearing.....: " << other_point.bearingTo(my_point) << " deg" << std::endl;
  }
  
  std::cout << "------------------------------------------------------------------------------" << std::endl;  

  return 0;
}

