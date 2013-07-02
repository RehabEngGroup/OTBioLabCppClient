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


#include "OTBioLabClient.h"

#include <iostream>
using std::cout;
using std::endl;
using std::ostream;
#include <cstdlib>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
#include <boost/thread/thread.hpp>


enum { max_length = 1024 };


// private - helping functions

void OTBioLabClient::sendCommand(const char* command) { 
  boost::asio::write(socket_, boost::asio::buffer(command, strlen(command))); 
}

void OTBioLabClient::readData(vector< short >& data, unsigned numData) {
  char reply[max_length]; 
  size_t reply_length = boost::asio::read(socket_,
                         boost::asio::buffer(reply, numData * 2)); 
   
  fromBytesToShort(reply, data, reply_length);
}

void OTBioLabClient::fromBytesToShort(char* from, vector<short>& to, unsigned fromLength)
{
  to.clear();
  
  if ( (fromLength % 2 ) != 0) {
    cout << "Converting an odd number (" << fromLength 
         << " Bytes to Short... impossible\n";
    exit(EXIT_FAILURE);
  }
    
  for (unsigned i = 0; i < fromLength/2; ++i) {
    to.push_back( ( ( from[i*2]&0xFF ) << 8 ) |  ( from[i*2+1]&0xFF) );
  }

}

OTBioLabClient::OTBioLabClient(const string& host)
:host_(host), port_("31000"), socket_(io_service_)
{
  try
  {
    tcp::resolver resolver(io_service_);
    tcp::resolver::query query(tcp::v4(), host_, port_, boost::asio::ip::resolver_query_base::all_matching);
    tcp::resolver::iterator iterator = resolver.resolve(query);
 
    boost::asio::connect(socket_, iterator);
   
    // When connected, OT Connector answer with "OTBiolab"
    char reply[max_length]; 
    size_t reply_length = boost::asio::read(socket_,
        boost::asio::buffer(reply, 8));
     
    std::cout << "Reply is: ";
    std::cout.write(reply, reply_length); 
    std::cout << endl;
    
    getConfiguration();
    getAcquisitionMode();
    getChannelConfig();
    
    blockSizeInBytes_ = (noEMGchannels_ + noAUXchannels_) * 2;
    
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  
}


void OTBioLabClient::getConfiguration() 
{
  sendCommand("config");
  
  vector<short> configurationParameters;
  readData(configurationParameters, 4);
    
  sampleRate_  = configurationParameters.at(0);
  noEMGchannels_ = configurationParameters.at(1);
  noAUXchannels_ = configurationParameters.at(2);
  nGain_       = configurationParameters.at(3);
   
}


void OTBioLabClient::getAcquisitionMode() 
{
  sendCommand("mode"); 
  
  while (socket_.available() == 0)
    boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
  char reply[max_length];
  size_t bytesToRead = socket_.available(); 
  size_t reply_length = boost::asio::read(socket_,
                         boost::asio::buffer(reply, bytesToRead));
     
  mode_ = reply;
  
}


void OTBioLabClient::getChannelConfig() {
  
  vector<short> gain;
  sendCommand("gain"); 
  readData(gain, nGain_); 
  channelConfig_.push_back(gain);
  
  
  vector<short> filtLowPass;
  sendCommand("filt_low_pass"); 
  readData(filtLowPass, nGain_); 
  channelConfig_.push_back(filtLowPass);
  
  
  vector<short> filtHighPass;
  sendCommand("filt_high_pass"); 
  readData(filtHighPass, nGain_); 
  channelConfig_.push_back(filtHighPass);
 
  
}

void OTBioLabClient::start() {
  sendCommand("start"); 
}

void OTBioLabClient::stop()
{
  sendCommand("stop"); 
}

void OTBioLabClient::readChannels(std::vector< short >& newData)
{
  
  // wait for enough bytesToRead
  while ( socket_.available() < blockSizeInBytes_ )
    boost::this_thread::sleep( boost::posix_time::milliseconds(1) );
  
  char block[max_length]; 
  size_t reply_length = boost::asio::read(socket_,
                         boost::asio::buffer(block, blockSizeInBytes_)); 
  
  fromBytesToShort(block, newData, blockSizeInBytes_);  

}


// --- output methods

ostream& operator<<(ostream& out, const OTBioLabClient& client)  
{
    out << "Configuration: \n";
    out << "Sample Rate:  " << client.sampleRate_ << endl;
    out << "EMG Channels: " << client.noEMGchannels_ << endl;
    out << "AUX Channels: " << client.noAUXchannels_ << endl;
    out << "nGain:        " << client.nGain_ << endl;
    out << "Acquisition Mode: " << client.mode_ << endl;
   
    return out;
}
 

void OTBioLabClient::printChannelConfiguration(std::ostream& out)
{
    out << "Ch.\t Gain \t LP \t HP " << endl;
    out << "-------------------------------------------------------\n";
    for (unsigned int i = 0; i < channelConfig_.at(0).size(); ++i) {
      out << i << ": \t" << channelConfig_.at(0).at(i) << "\t "
                        << channelConfig_.at(1).at(i) << "\t" 
                        << channelConfig_.at(2).at(i) << endl;
    }

}
  

   