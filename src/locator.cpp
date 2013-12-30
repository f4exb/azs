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

#include <cctype> // for toupper
#include <algorithm>
#include <math.h>
#include "locator.h"

// LocatorInvalidException class

std::string LocatorInvalidException::getString()
{ return _locator_str; }


// Locator class

std::string const Locator::_lon_array1 = "ABCDEFGHIJKLMNOPQR";
std::string const Locator::_lat_array1 = "ABCDEFGHIJKLMNOPQR";
std::string const Locator::_lon_array2 = "0123456789";
std::string const Locator::_lat_array2 = "0123456789";
std::string const Locator::_lon_array3 = "ABCDEFGHIJKLMNOPQRSTUVWX";
std::string const Locator::_lat_array3 = "ABCDEFGHIJKLMNOPQRSTUVWX";

Locator::Locator(std::string loc_string)
{
  // locator must be exactly 6 characters
  if (loc_string.length() != 6)
    throw LocatorInvalidException(loc_string);
  // convert to upper case
  std::transform(loc_string.begin(), loc_string.end(), loc_string.begin(), toupper);
  // retrieve latitude and longitude indexes
  if ((_lon_index1 = _lon_array1.find(loc_string[0])) == std::string::npos)
    throw LocatorInvalidException(loc_string);
  if ((_lat_index1 = _lat_array1.find(loc_string[1])) == std::string::npos)
    throw LocatorInvalidException(loc_string);
  if ((_lon_index2 = _lon_array2.find(loc_string[2])) == std::string::npos)
    throw LocatorInvalidException(loc_string);
  if ((_lat_index2 = _lat_array2.find(loc_string[3])) == std::string::npos)
    throw LocatorInvalidException(loc_string);
  if ((_lon_index3 = _lon_array3.find(loc_string[4])) == std::string::npos)
    throw LocatorInvalidException(loc_string);
  if ((_lat_index3 = _lat_array3.find(loc_string[5])) == std::string::npos)
    throw LocatorInvalidException(loc_string);
  // convert to decimal degrees for lower corner of locator square
  _lat  = (_lat_index1 * 10.0) - 90.0;
  _lon  = (_lon_index1 * 20.0) - 180.0;
  _lat += _lat_index2;
  _lon += _lon_index2 * 2.0;
  _lat += _lat_index3 / 24.0;
  _lon += _lon_index3 / 12.0;
  // adjust to center of locator square
  _lat += 1.25 / 60.0;
  _lon += 2.5 / 60.0;
  
}

Locator::Locator(float lat, float lon) : _lat(lat), _lon(lon)
{
  float lat_rem, lon_rem;
  
  _lat_index1 = int((lat + 90.0)  / 10.0);
  _lon_index1 = int((lon + 180.0) / 20.0);
  lat_rem = lat + 90.0 - (_lat_index1 * 10.0);
  lon_rem = lon + 180.0 - (_lon_index1 * 20.0);
  _lat_index2 = int(lat_rem);
  _lon_index2 = int(lon_rem / 2.0);
  lat_rem = lat_rem - _lat_index2;
  lon_rem = lon_rem - (_lon_index2 * 2);
  _lat_index3 = int(lat_rem * 24.0);
  _lon_index3 = int(lon_rem * 12.0);
}

std::string Locator::toString()
{
  std::string returned = "";
  
  returned.append(1,_lon_array1[_lon_index1]);
  returned.append(1,_lat_array1[_lat_index1]);
  returned.append(1,_lon_array2[_lon_index2]);
  returned.append(1,_lat_array2[_lat_index2]);
  returned.append(1,_lon_array3[_lon_index3]);
  returned.append(1,_lat_array3[_lat_index3]);
  
  return returned;
}

float Locator::latitude()
{ return _lat; }

float Locator::longitude()
{ return _lon; }


// LocPoint class

float LocPoint::bearingTo(LocPoint& distant_point)
{
  double lat1 = _locator.latitude() * (M_PI / 180.0);
  double lon1 = _locator.longitude() * (M_PI / 180.0);
  double lat2 = distant_point.latitude() * (M_PI / 180.0);
  double lon2 = distant_point.longitude() * (M_PI / 180.0);  
  double dLat = lat2 - lat1;
  double dLon = lon2 - lon1;
  double y = sin(dLon) * cos(lat2);
  double x = (cos(lat1)*sin(lat2)) - 
             (sin(lat1)*cos(lat2)*cos(dLon));
  double bear_rad = atan2(y,x);
  if (bear_rad > 0)
    return bear_rad * (180.0 / M_PI); 
  else
    return 360.0 + (bear_rad * (180.0 / M_PI)); 
}

float LocPoint::distanceTo(LocPoint& distant_point)
{
  double lat1 = _locator.latitude() * (M_PI / 180.0);
  double lon1 = _locator.longitude() * (M_PI / 180.0);
  double lat2 = distant_point.latitude() * (M_PI / 180.0);
  double lon2 = distant_point.longitude() * (M_PI / 180.0);
  return acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon2-lon1))*6371.0;
}

float LocPoint::latitude()
{ return _locator.latitude(); }

float LocPoint::longitude()
{ return _locator.longitude(); }

