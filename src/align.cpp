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

//-----------------------------bibliothèques------------------------------------
//------------------------------------------------------------------------------

#include <iostream>
#include <math.h>
#include <iomanip>
#include <sys/time.h>
#include <cstdlib>
#include <getopt.h>

//-----------------------------programme principal------------------------------
//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  int real_angle_calibration = 0;
  int disc_angle_calibration = 0; 
  int bearing = 0;

  //-----------------------------traitement des options---------------------------
  //------------------------------------------------------------------------------
 
  int index; 
  opterr = 0;
  int c;
     
  while ((c = getopt (argc, argv, "r:d:b:")) != -1)
  {
    switch (c)
    {
      case 'r':
        real_angle_calibration = atoi(optarg);
        break;
      case 'd':
        disc_angle_calibration = atoi(optarg);
        break;
      case 'b':
        bearing = atoi(optarg);
        break;
      case '?':
        std::cerr << "Unknown option " << optopt << std::endl;
        std::cerr << "Usage is " << argv[0] << " options" << std::endl;
        std::cerr << "  options are:" << std::endl;
        std::cerr << "  -r: Real angle of calibration (degrees)" << std::endl;
        std::cerr << "  -d: Disc angle of calibration (degrees)" << std::endl;
        std::cerr << "  -b: Bearing expected or read (degrees)" << std::endl;
        std::cerr << "  returns the bearing (degrees) expected vs read or vice versa" << std::endl;
        return 1;
      default:
        abort ();
    }
  }
     
  for (index = optind; index < argc; index++) {} // non option arguments

  //-----------------------------boucle de calculs--------------------------------
  //------------------------------------------------------------------------------

  std::cout << (disc_angle_calibration - bearing + real_angle_calibration) % 360 << std::endl;
  
  return 0;
}
