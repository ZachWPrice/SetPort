Setport
=======

Author: Zachary Price  
Email: 10607300@my.uvu.edu

####Installation:
  Prior to using the software you will want to run: changeToSetport.sh
You may need root permissions to run this, as it will add the software to your PATH. 
You can then run the software as 'sudo setport' - to remove the sudo, change the 
owner of /bin/setport to the owner that you're using. 

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
  Currently English and Spanish are the only supported languages. The software will look at a few enfironment variables to find which language to use

####Purpose: 
  This is a school project at UVU under instructor Wagstaff. Who approved of this project being put into a Git repository.
