Setport
=======

Author: Zachary Price  
Email: 10607300@my.uvu.edu

####Installation:
  Prior to using the software you will want to run: changeToSetport.sh
You may need root permissions to run this, as it will add the software to your PATH. 
You can then run the software as 'sudo setport' - to remove the sudo, change the 
owner of /bin/setport to the owner that you're using. 

  At that point, you can follow the usage as stated below. 
  
  Finally, to run the test cases, please navigate to the 'Testing' folder, and run ./test_setport.cpp.o
This should run through all of the tests found in 'Testing/TestCmds/mainTestCmds.txt' in English and Spanish. 
Tests can be added to this file as needed and they will run along with the rest though you will not be able to
see so in the output of the tests (only if the test fails will mention of it be found).

####Usage: 
```
Portsetter --option <port>
    Options:
        -?, -h, --help  Display this help page [Must not have any other arguments]
        -a, --about, -! Display information about this software
        -v, --version   Display the version
        -p, --port      Follwing argument is 'port' [Required for non-help execution]
        -a, --about     Prints information about the software
        -e ['ENVVAR']   Used along -p or --port, used to specify an environment variable to use as the port, the default is 'PORT'
```
####Localization: 
  Currently English and Spanish are the only supported languages supported by default. The software will look at a few enfironment variables to find which language to use.
  If you would like to add any languages, please create a folder with the cooresponding two-character language code. with the same files found in the provided language folders. 
  All information in those folder should follow the same structure, and all files must be included. Otherwise the software will exhibit undefined behavior. 

####Purpose: 
  This is a school project at UVU under instructor Wagstaff. Who approved of this project being put into a Git repository.
