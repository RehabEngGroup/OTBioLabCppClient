This is a simple implementation of a client that connect to OTBioLab software and get the data from the socket. 
Nothing too fancy... :) 

## Download ##
If you are reading this file you know that there is a GitHub repository.  Just clone the repository in your machine:
```
$ git clone https://github.com/RehabEngGroup/OTBioLabCppClient.git
```
Then check that you are in the right branch. Use the git command status to check you are in the branch develop.
```
$ git status
On branch develop
Your branch is up-to-date with 'origin/develop'.

nothing to commit, working directory clean
```

## Install ##
Well I have to work on the CMakeList.txt because it is not update (it looks for something I do not need, and doesn't look for something that you actually need). Basically install Boost (all of them if you haven't already) and you should be ok. For the Socket connection I used the Boost.Asio

Than go in the usual way 
```
$ mkdir build
$ cd build
$ cmake .
$ make 
```
If everything went fine in the previous step you have two executable in build/src directory.

## Running ##

Open OTBioLab, run Visualization.

On your shell, if you are on the same computer write:
```
$ ./OTBioLabClient localhost
```
or, if you are in a remote computer write:
```
$ ./OTBioLabClient IPaddress
```

CTRL-C to stop the program.

### Output ###

The program will generate two files:

* Config.dat : configuration as it was received by he OT BioLab
* Sample.dat : the data read from the EMG + AUX channels

If you do not want the output, comment #define LOG inside src/connect.cpp


### Dummy Server ###
The second executable is a dummy server, i.e. reads a Config.dat and Sample.dat file and send the data  , somehow simulating the behavior of the OT.

You need a Config.dat a a Sample.dat from a previous acquisition. Then open two shells.
In the first one, run the dummy server. 
```
$ ./fakedServer <inputDirectory> <freq>
```
where: inputDirectory is the directory where Config.dat e Sample.dat are stored and freq is the frequency of the data.

In the second one, run the client:
```
$ ./OTBioLabClient localhost
```

Watch out: Do not run the OTBioLabClient from the inputDirectory. It will overwrite the .dat files used by the fakedServer

This fakedServer does not behave exactly as the OTBioLab. Indeed, the OT Bioelettronica software sends data only in burst. I.e. you are not going to receive a single reading but severals packed together.













