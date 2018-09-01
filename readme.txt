------------------------------------------------------------------------
Mysteria MUD Engine II
------------------------------------------------------------------------
Copyright 2001+ by Julian Forsythe and Jason McCaughey

Base Code: TFE/TFH by Alan Button, Tyrone, Squeed and Heironymus
           Mysteria MUD Engine by Julian Forsythe and Jason Nappi
           MERC 2.1 by Hatchet, Furney, and Kahn
           CircleMUD 3.0 by Jeremy Elson
           DikuMUD by Hans Staerfeldt, Katja Nyboe, Tom Madsen,
                      Michael Seifert, and Sebastian Hammer.
------------------------------------------------------------------------


1. Copyright Stuff

All files included in this MUD distribution are subject to copyright and
may not be used on an Open MUD without appropriate attributation, as per
the above Diku/Circle/Merc/Mysteria/TFE/TFH/Mysteria II credits.

These names/credits must be displayed during the player login sequence as
well as in a helpfile under "credits".  The format may be changed to suit
your game.

You must also comply with any license requests from the parent codebases.

The names "Legacy's Dawning", "Mysteria MUD Engine" and "Mysteria MUD Engine II"
remain the intellectual property of Seltha and may not be used for any purpose
without first attaining written permission from Seltha.

"Open MUD" means a MUD running on a server that is accessible from the
internet or on a private network with more than five users (including players
and immortals).

"Seltha" is Julian Forsythe (email seltha@seltha.net).



2. Getting Started with Mysteria II (on Windows)

Extract the archive to wherever you wish, C:\Mysteria\ is as good place as any.

Running the MUD - go to the 'bin' directory (ie C:\Mysteria\bin) and execute
Mysteria II.exe (note that it will appear not to do anything, but it places an
icon in the system tray next to your clock which shows the MUD is running).

You might need to allow it through the Windows (or your antivirus) firewall.

You might also need to download and configure a sendmail program (I used to
use 'softmail') to allow sending of account confirmation emails.

Logging on - the head immortal is 'mysteria' with password 'mysteria'.
The mud runs on port 4242 (this is not user-definable at the moment).
Connect with your favourite MUD client to localhost, port 4242.

Change your name 'rename mysteria <newname>'
Change your password 'password mysteria <newpasswd>'

Getting your friends on - they can connect to your IP on port 4242 (if
you're behind a firewall then you'll have to set up port forwarding, etc).

To make a new immortal, first trust the player at the level you wish them to be
'trust person <level>'  (92 is an apprentice, 93+ are builders, 98 is a demigod, 99 is god).
Have them quit, log back in, quit, log back in.  Then 'advance person <level>'.

To make a new zone for building type 'area new <continent> <start vnum>'
(and 'area summary' to list all the appropriate zones).



3. Support

Unfortunately, you're on your own, however I have included all the online
helpfiles and tables from Legacy's Dawning so you have a basis to work
from.  Use the 'index' and 'help' commands to get started.

Remember to 'write all' every so often, to save the building that you've
been doing.  There has been one occurance of object file corruption (obj.obj)
so make sure you keep backups of your work.  The 'prev' directory will contain
the previous 'good' version of a file, so that's one step of backup, but I can't
stress enough to keep regular backups.

(I should have listened to my own advice, hey?).



4. Contacting the Developer

You may email seltha@seltha.net if you wish to contact me, but I don't
promise to be quick to respond! :D
