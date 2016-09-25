# Mental Blossoming Lamp

Macro-Toolbox Mental Blossoming Lamp is a mental controlable lamp.
You can use it to train your attention/concentration and meditation skills.
Demo video :
https://youtu.be/caP1dLanq6c

This is my own version of the blossoming lamp of Emmett and the bloom of Puzzlebox.
Design was very slighty modified, but hardware and software has been significantly optimized (no more need to use a phone as an interface between the EEG headset and the lamp).
Overview of modifications are listed bellow and a full Design, Hardware, Software and Assembly guide is available at the link :
http://www.macro-toolbox.com/toolbox/mental-blossoming-lamp/

This lamp is :

3d printable with any 3d printer having a printable volume of 130x130x130.
filled with simple electronic hardwares (as arduino board, bluetooth module, led, servo motor,…).
fully open source (design, hardware and software and you can adapt it to your needs.
controlable with a EEG headset (currently a neurosky mindwave mobile headset) with bluetooth communication.
Design was :
Initiated by emmett and his Blossoming Lamp :
http://www.thingiverse.com/thing:37926
Adapted by Puzzlebox to allow the inclusion of hardware and the motorization :
http://www.thingiverse.com/thing:618490
And finaly I modded the bulb to make it nicer and allow its printing on my 3d printer.

Hardware was :
Initiated by Puzzlebox and,
Modded by macro-toolbox to add the following ameliorations :

Headset can communicate directly with the lamp.
contrary to puzzlebox version, you don't need a ble bluetooth board and a ble bluetooth device (phone or tablet) as an interface between the headset and the lamp.
added a power board to drive the servo and to support the bluetooth module.

Software was :
Based on Neurosky Arduino example found at neurosky.com for the mindwave protocol and,
Fully written by macro-toolbox to control the lamp features (light and motorization), at the exception of a bit of puzzlebox code for the rgb loop crossfade.
