# Heatuino
Central heating control on an Arduino

V0.4
Change so that 1 in 10 cycling uses residual heat from the boiler for the 1st minute (or stays in overrun) 
and doesn't call for heat until the second minute.
Most timing values moved to #defines at the start of the project file.

V0.3
Mainly added a delay to the stat class to be able to wait for DEMAND to be longer than a minute before responding.
This is to circumvent short boiler firing when the RoomStat is cycling heat for 1 minute in 10.
(The problem was actually with the honeywell system cos a HR80 had become detached from its base.)

V0.2
Refactored code. (Tidy up).
Made the loop a bit prettier - added debug comments.
This is the 1st main working version

V0.1
Base build. Kinda worked.
