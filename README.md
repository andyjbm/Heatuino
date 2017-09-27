# Heatuino
Central heating control on an Arduino

V0.4
1 in 10 cycling allowed in prerun or overrun instead of being ignored or short firing the boiler.
Change so that 1 in 10 cycling uses residual heat from the boiler for the 1st minute (or stays in overrun) 
and doesn't call for heat until the second minute.
Most timing values moved to #defines at the start of the project file.

V0.3
Stat Delay added.
Mainly added a delay to the stat class to be able to wait for DEMAND to be longer than a minute before responding.
This is to circumvent short boiler firing when the RoomStat is cycling heat for 1 minute in 10.
(The problem was actually with the honeywell system cos a HR80 had become detached from its base.)

V0.2
This is the 1st main working version
Refactored code. (Tidy up).
Made the loop a bit prettier.
Added debug comments.
Refactored serial output in the loop to be able to post state changes to emoncms via a EMONCMS: token in the serial line.
Other (debug/info) output will be sent to a log file via oemgateway on the pi.

V0.1
Base build. Kinda worked.
