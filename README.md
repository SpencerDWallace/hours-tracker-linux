# hours-tracker-linux
bash and c++ tool to track hours worked and output the desired month/year and pay period

################ PLEASE READ BELOW COMMENTS FOR SETUP ################
#### will most likely need to update PATH in the clockin and clockout scripts
#### to give the location of doit.cpp - uses home directory by default
#### ex: if doit.cpp is in path ~/some/path/ - PATH should be "some/path"
#### PATH variable in doit.cpp may also need tinkering if you would like
#### to store the data files in a nested folder
#### ex: store data in ~/some/path/hours - PATH should be "./hours"
######################################################################

How to use:    
run clockin to add a clockin time to the data file
ex: (if currently in directory of the scripts) $ . clockin

run clockout to add a clockout time to the data file
ex: (if currently in directory of the scripts) $ . clockout

compile doit.cpp
ex: g++ doit.cpp -o foo

run doit.cpp
ex: ./foo

Some additional options:
When running program, a few arguments can be passed to skip user input
- month and year can be passed as MM/YY
- pay cycle can be passed as 1 (1-15) 2 (16-end) or 3 (entire month)
ex: ./foo 02/23 1 | ./foo 02/23 2 | ./foo 02/23 3

Example output:

![alt text](https://imgur.com/a/11jOBKb)