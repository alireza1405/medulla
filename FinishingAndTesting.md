# Introduction #




# Finishing the Hardware #

The medullas come from the manufactures almost ready to go; there's just a few finishing touches that need to be done before they'll be ready for an EtherCAT bus.  Below is an example of the raw system after assembly.

Before beginning the process to finish the boards it would be wise to preform a continuity test on each power rail.  Using a multi-meter, check to see if the 3.3v, 5v, or Vin is shorted to ground.  If any of the voltage rails are shorted to ground, label this and set the board aside and find another to work with.  It's also recommended that the continuity between voltage rails be check as well.  Again if any are shorted together, label it and set it aside.

https://lh3.googleusercontent.com/-dfbs_hGJ46k/Tn43BG7QtsI/AAAAAAAABPo/ZMLql6MFIZk/s400/DSCN1316.JPG


The first thing that's needed is a fix to enable the 5 V supply.  Last minute changes produced and error in the design of the board, disconnecting the enable line of **U4** from the input supply.  To fix that we'll have to connect pins 2 and 6 together using a fine wire:

https://lh6.googleusercontent.com/-AjAEyPJpIWQ/Tn429ceCHuI/AAAAAAAABPk/nYdVEpJi2z0/s400/DSCN1321.JPG

https://lh5.googleusercontent.com/-Itd0RC8sf-Y/Tn46KkQgrVI/AAAAAAAABPw/bj5uhz3XOAM/s400/DSCN1324.JPG

Flipping the board over, you'll notice a large patch of empty pads with the labels **PHY0** and **PHY1**.  These are jumpers to set the local physical address for the Ethernet hardware.

https://lh5.googleusercontent.com/-DBiOH6Mmqac/Tn49Xbvx90I/AAAAAAAABP8/lMdUuUhuuwM/s400/DSCN1332.JPG

https://lh4.googleusercontent.com/-ug6TIgPni00/Tn46YAVFm8I/AAAAAAAABP4/uuO7sdZQrSo/s400/DSCN1329.JPG

To get the EtherCAT hardware setup properly we'll need to populate the jumpers with zero ohm resistors (or for the patient, you can bridge the pads with solder).  Below is photo of how the jumpers should be set - all of the jumpers for **PHY0** is set to the zero position and all of the **PHY1** jumpers set to zero except for the first one.

https://lh6.googleusercontent.com/-32u0qSOUtoU/Tn49bVfqYdI/AAAAAAAABQA/oCqkf4d55Zg/s400/DSCN1337.JPG

The final step is to solder in the connectors.  This can be a little tricky because there was a manufacturing defect that caused some of the gold plating to flake off exposing the copper pads.  The exposed copper can be difficult to solder to so don't worry too much about having a nice even solder flow around each pin - just inspect that each pin has a good connection to some part of the pad.

https://lh4.googleusercontent.com/-uPSJJm3SRXs/Tn49l8YeNlI/AAAAAAAABQE/flm9zo673O8/s400/DSCN1341.JPG



# Testing #

The very first test