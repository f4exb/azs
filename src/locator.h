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

class LocatorInvalidException
{
  public:
    LocatorInvalidException(std::string locator_str) : _locator_str(locator_str) {};
    std::string getString();
  protected:
    std::string _locator_str;
};

class Locator
{
  public:
    Locator(std::string locator_str);
    Locator(float lat, float lon);
    std::string toString();
    float latitude();
    float longitude();
  protected:
    int _lat_index1;
    int _lat_index2;
    int _lat_index3;
    int _lon_index1;
    int _lon_index2;
    int _lon_index3;
    static const std::string _lon_array1;
    static const std::string _lat_array1;
    static const std::string _lon_array2;
    static const std::string _lat_array2;
    static const std::string _lon_array3;
    static const std::string _lat_array3;
    float _lat;
    float _lon;
};

class LocPoint
{
  public:
    LocPoint(Locator& locator) : _locator(locator) {};
    LocPoint(float lat, float lon) : _locator(lat, lon) {};
    float latitude();
    float longitude();
    float bearingTo(LocPoint& distant_point);
    float distanceTo(LocPoint& distant_point);
  protected:
    Locator _locator;
};

