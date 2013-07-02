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


#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/concept_check.hpp>

#include <fstream>
using std::ifstream;
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;


using boost::asio::ip::tcp;

const int max_length = 1024;
const int OTBIOLAB_PORT = 31000;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

/*!
 Convert a vector of short in an array of bytes
 
 */
void fromShortsToBytes(const vector<short>& from, char* to){
  for (unsigned int i = 0; i < from.size(); ++i) {
    to[i*2+1] = (char) (from.at(i) & 0xFF);
    to[i*2] = (char) ((from.at(i) >> 8) & 0xFF);
  }
}

/*!
 Send a sequence of chars through the socket sock
 */
void sendString(socket_ptr sock,const char* command) { 
  boost::asio::write(*sock, boost::asio::buffer(command, strlen(command))); 
}

/*!
 Send a sequence of short through the socekt sock
 */
void sendData(socket_ptr sock, const vector< short >& data ) { 
  char dataToSend[data.size()*2];
  fromShortsToBytes(data, dataToSend);
  boost::asio::write(*sock, boost::asio::buffer(dataToSend, data.size()*2)); 
}

/*!
 Receive a command from the client through the socket sock
 */
void readCommand(socket_ptr sock, string& command) {
  char data[max_length];
  boost::system::error_code error;
  size_t length = sock->read_some(boost::asio::buffer(data), error);
  if (error)
     throw boost::system::system_error(error); // Some other error.
      
  data[length] = '\0';
  command = data;
}



/**  
  Store the configuration data that are read from the Config.dat file
  Refer to the OTBioLab manual for the meaning of the fields
*/
typedef struct {
  unsigned short sampleRate; 
  unsigned short noEMGchannels;
  unsigned short noAUXchannels;
  unsigned short nGain; 
  char mode[max_length];
  std::vector< short > gains;
  std::vector< short > LPs;
  std::vector< short > HPs; 
} Configuration;

/*!
 Read the configuration from the Config.dat file and store the information
 in the structure otBioLabConfiguration
 */
void readConfiguration(const string& inputDirectory, Configuration& otBioLabConfiguration){
  
  string configFilename = inputDirectory + "/Config.dat";
  ifstream configFile(configFilename.c_str());
  
  if (!configFile.is_open()) {
    cout << "ERROR: " << configFilename << " could not be open\n";
    exit(EXIT_FAILURE);
  }
  
  string trash;
  configFile >> trash >> trash >> trash;  
  configFile >> otBioLabConfiguration.sampleRate;
  
  configFile >> trash >> trash;  
  configFile >> otBioLabConfiguration.noEMGchannels;
  
  configFile >> trash >> trash;  
  configFile >> otBioLabConfiguration.noAUXchannels;
  
  configFile >> trash;  
  configFile >> otBioLabConfiguration.nGain;
  
  configFile >> trash >> trash; 
  configFile.getline(otBioLabConfiguration.mode, max_length-1); 
  
  char trashChars[max_length];
  configFile.getline(trashChars, max_length-1);
  configFile.getline(trashChars, max_length-1);
  configFile.getline(trashChars, max_length-1);
  
   
  for (unsigned int i = 0; i < otBioLabConfiguration.nGain; ++i) {
    configFile >> trash; 
    short gain; 
    configFile >> gain;
    otBioLabConfiguration.gains.push_back(gain);
    short LP; 
    configFile >> LP;
    otBioLabConfiguration.LPs.push_back(LP);
    short HP;
    configFile >> HP;
    otBioLabConfiguration.HPs.push_back(HP);
  }
   
  configFile.close();  
}


void readChannelValues(const string& inputDirectory, const Configuration& otBioLabConfiguration, vector< vector< short> >& channelValues) {
  
  unsigned short noChannels = otBioLabConfiguration.noAUXchannels + otBioLabConfiguration.noEMGchannels; 
  string sampleFilename = inputDirectory + "/Sample.dat";
  ifstream sampleFile(sampleFilename.c_str());
  
  if (!sampleFile.is_open()) {
    cout << "ERROR: " << sampleFilename << " could not be open\n";
    exit(EXIT_FAILURE);
  }
  
  short next;
  int i = 0;
  channelValues.resize(noChannels);
  while (sampleFile >> next) {
    channelValues.at(i).push_back(next);
    i = ( i+1 ) % (noChannels);
  }
   
  sampleFile.close();  
}


/*
 Function executed by the thread deadling with client request
 
 Please refer to the OTBioLabClient documentation for information
 about the possible client requests.
 */
void session(socket_ptr sock, const string& inputDirectory)
{
  try
  {
    Configuration otBioLabConfiguration;
    readConfiguration(inputDirectory, otBioLabConfiguration);

    // start communication - OTBioLab send "OTBioLab" string
    sendString(sock, "OTBioLab");
     
    string command;
    while(true) {
      readCommand(sock, command);
      cout << "New command: ";
      cout << command << endl;
        
      if ( command == "config" )  { 
        vector<short> configValues;
        configValues.push_back(otBioLabConfiguration.sampleRate);
        configValues.push_back(otBioLabConfiguration.noEMGchannels);
        configValues.push_back(otBioLabConfiguration.noAUXchannels);
        configValues.push_back(otBioLabConfiguration.nGain);

        sendData(sock, configValues);
        
      } 
      else if (command == "mode") 
        sendString(sock, otBioLabConfiguration.mode); 
      else if (command == "gain") 
        sendData(sock, otBioLabConfiguration.gains); 
      else if (command == "filt_low_pass")  
        sendData(sock, otBioLabConfiguration.LPs);
      else if (command == "filt_high_pass")
        sendData(sock, otBioLabConfiguration.HPs);
      else if (command == "start") {
        vector< vector< short > > channelValues;
        readChannelValues(inputDirectory, otBioLabConfiguration, channelValues);
        
        for (unsigned int i = 0; i < channelValues.at(0).size() ; ++i) {
          vector< short > nextEMGs;
          for (unsigned int j = 0; j < (otBioLabConfiguration.noAUXchannels + otBioLabConfiguration.noEMGchannels); ++j)
            nextEMGs.push_back(channelValues.at(j).at(i));
          sendData(sock, nextEMGs);
          boost::this_thread::sleep( boost::posix_time::microseconds(1./otBioLabConfiguration.sampleRate*pow10(6) ) ); 
        } 
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
}

/*!
 Accept a new request and open a new socket. A new thread execute session function 
 on this socket. The socket is socketstream, i.e. a new connection (socket) is 
 setup for each new client.
 */
void server(boost::asio::io_service& io_service, short port, const string& inputDirectory)
{
  tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
  for (;;)
  {
    socket_ptr sock(new tcp::socket(io_service));
    a.accept(*sock);
    boost::thread t(boost::bind(session, sock, inputDirectory));
  }
}


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: fakedServer <inputDirectory> <freq>\n";
      exit(EXIT_FAILURE);
    }

    boost::asio::io_service io_service;
    server(io_service, OTBIOLAB_PORT, argv[1]);

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  exit(EXIT_SUCCESS);
}