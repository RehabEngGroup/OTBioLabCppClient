/*
    OTBioLabClient - a C++ client for OTBioLab Connector
    Copyright (C) 2013  Monica Reggiani <monica.reggiani@gmail.com>

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
*/


#include <cstdlib>
#include <cstring>
#include <fstream>
using std::ofstream;
#include <iostream>
using std::ostream;
#include <boost/concept_check.hpp>
using std::cerr;
using std::cout;
using std::endl;
#include <cstdlib>
#include <vector>
using std::vector;

#include <boost/thread/thread.hpp>

#include "OTBioLabClient.h"

#define LOG

#define STORE_DATA

ostream& operator<<(ostream& out, const vector<short> & data) {
    for (unsigned j = 0; j < data.size(); ++j)
        out << data.at(j) << "  ";

    return out;
}


int main(int argc, char* argv[])
{
  cout << "----------------------------------\n";
  cout << "   C++ Socket Client for OTLab\n";
  cout << "----------------------------------\n";

  if (argc != 2)
  {
    std::cerr << "Usage: clientOtLab <host> \n";
    exit(EXIT_FAILURE);
  } 
  
  
#ifdef STORE_DATA
  ofstream sampleFile("Sample.dat"); 
  if (!sampleFile)
  { 
     cerr << "Uh oh, Sample.dat could not be opened for writing!" << endl;
     exit(EXIT_FAILURE);
  }
  
  
  ofstream configFile("Config.dat"); 
  if (!configFile)
  { 
     cerr << "Uh oh, Config.dat could not be opened for writing!" << endl;
     exit(EXIT_FAILURE);
  }
  
#endif  
  
  OTBioLabClient otBioLabClient(argv[1]);
  
  
#ifdef LOG
  cout << "OTBioLab configuration: \n";
  cout << otBioLabClient << endl;
  //otBioLabClient.printChannelConfiguration(cout);
#endif
  

#ifdef STORE_DATA
  configFile << otBioLabClient << endl;
  otBioLabClient.printChannelConfiguration(configFile);
#endif
  
  otBioLabClient.start();
  int i = 0;
  while(1)
  {
    vector<short> newData;
    otBioLabClient.readChannels(newData);
    
#ifdef LOG    
    cout << i++ << ": " << newData << endl;
#endif
#ifdef STORE_DATA
    sampleFile << newData << endl;
#endif
  }
  
  boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
  otBioLabClient.stop();
  
  
#ifdef STORE_DATA
    sampleFile.close();
#endif
  
  exit(EXIT_SUCCESS);
}