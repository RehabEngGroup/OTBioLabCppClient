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


#ifndef OTBioLabClient_h
#define OTBioLabClient_h

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <vector>

class OTBioLabClient
{
public:
  OTBioLabClient(const std::string& host);
  void start();
  void stop();
  void readChannels(std::vector<short>& newData);
  
  void printChannelConfiguration(std::ostream& out) ;
  friend std::ostream& operator<<(std::ostream& out, const OTBioLabClient& client);
  short getEMGgain() { return channelConfig_.at(0).at(0); }; 
private:
  void getConfiguration();
  void getAcquisitionMode();
  void getChannelConfig();
  void sendCommand(const char*);
  void readData(std::vector< short >& data, unsigned numData);
  void fromBytesToShort(char* from, std::vector<short>& to, unsigned fromLength);
  
  
  std::string host_;
  std::string port_;
  unsigned short sampleRate_;
  unsigned short noEMGchannels_;
  unsigned short noAUXchannels_;
  unsigned short nGain_;
  std::vector< std::vector< short> > channelConfig_; 
  
  
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::socket socket_;
  
  std::string mode_;
  
  unsigned int blockSizeInBytes_;
};

#endif