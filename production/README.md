
So here's how this works

Every time the Rasberry Pi boots, this is what happens:

1. The script shutdown.py is started which listens for the shutdown button.  The busy wait delay is set pretty high so you may have to hold the button down for more than a second.  

2.  The script query.py gets oceanographic data from LOBO and stores it in the file query_results.txt in the form of arrays declared in the wiring language.

3.  query.txt is copied into the file src/phytos.ino. 

4.  phytos.ino.master is concatenated to the end of src/phytos.ino.  

5.  http://inotool.org/ turns phytos.ino into hex, which is then uploaded to the raspberry pi, provided there is internet connection

All of this behavior is defined in the shell script build, located in this directory, and should run on startup.  

If things are breaking and you're not sure how to interpret the error messages, here are the most common problems:

query.py crashes - there's no internet (this usually means that the build process will still happen, the build just won't have current data.)

inotool crashes (can't find interface) - make sure the pi is plugged into the arduino.

inotool crashes (during build process) - make sure query_results.txt exists


The script build_wo_query constructs, builds, and uploads phytos.ino without actually activating query.py.  This is mostly for debugging purposes since the queries take a while, but it can be used in production in a pinch since the data for query.py only changes daily, though, at that point, the arduino should have the same code as the version you're feeding it so it's kind of moot.  

the command ino (from inotool) must be run in this directory and it's going to look for the src and lib directories, so I wouldn't change the directory structure too much unless you know what you're doing.  