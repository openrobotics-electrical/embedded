#open_robotics_embedded<br>
Seriously any questions get at me max@theprogrammingclub.com<br>
###Organization of this repo<br>
&nbsp;&nbsp;<a href="../../tree/master/doc/">**doc/**</a> has IC pinouts, device information<br>
&nbsp;&nbsp;<a href="../../tree/master/libraries/">**libraries/**</a> holds usefuls source code that is not project-specific and device symbols/footprints<br>
&nbsp;&nbsp;<a href="../../tree/master/projects/">**projects/**</a> contains subfolders, each one dedicated to a single project<br>
&nbsp;&nbsp;&nbsp;&nbsp;each project subfolder is meant to correspond 1-to-1 with a physical module<br>
&nbsp;&nbsp;&nbsp;&nbsp;module-specific schematics, PCB files, project source code, and other info go in these<br>
###How to program a module<br>
####Using USB-serial with Arduino IDE<br>
&nbsp;&nbsp;Retrieve the latest version of this repository<br>
&nbsp;&nbsp;Find the folder of the module you want to program<br>
&nbsp;&nbsp;Open the .ino file in Arduino<br>
&nbsp;&nbsp;Select the appropriate board and serial port from the **Tools** dropdown<br>
&nbsp;&nbsp;Hit upload
####Setting up Atmel Studio for programming over USB-serial<br>
&nbsp;&nbsp;Select Tools -> External tools<br>
&nbsp;&nbsp;&nbsp;&nbsp;Choose Add<br>
&nbsp;&nbsp;&nbsp;&nbsp;Set Title to 'Serial Program'<br>
&nbsp;&nbsp;&nbsp;&nbsp;Change the command to your Arduino directory that includes avrdude<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Example: <code>C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avrdude.exe</code><br>
&nbsp;&nbsp;&nbsp;&nbsp;Change the Arguments to match your situation, replacing COM16 with the COM port you are using
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Example: <code>-C"C:\Program Files (x86)\Arduino\hardware\tools\avr\etc\avrdude.conf" -patmega328p -carduino -P\\.\COM16 -b57600 -U flash:w:"$(ProjectDir)Debug\$(ItemFileName).hex":i -v</code><br>
&nbsp;&nbsp;&nbsp;&nbsp;Click ok<br>
&nbsp;&nbsp;&nbsp;&nbsp;Now to program, select Tools -> Serial Program<br>
####COMING SOMETIME IN 2015<br>
&nbsp;&nbsp;A more in-depth guide to programming using AVRdude and ICSP, see some links at the end of this README for now<br>
<br>
###Tools<br>
**git**<br>
&nbsp;&nbsp;<a href="https://windows.github.com/">Windows install</a><br>
&nbsp;&nbsp;<a href="http://git-scm.com/download/linux">Linux/Unix install</a><br>
**Arduino IDE**<br>
&nbsp;&nbsp;http://www.arduino.cc/en/Main/Software<br>
**Atmel Studio** (Windows or Linux/UNIX with a VM)<br>
&nbsp;&nbsp;http://www.atmel.com/microsite/atmel_studio6<br>
&nbsp;&nbsp;You will want to install the Visual Micro extension<br>
<br>
Email me! max@theprogrammingclub.com<br>
I want to make this documentation effective<br>
###Git workflow: <br>
<code>  git pull origin master</code><br>
<code>  [do your stuff]</code><br>
<code>  git add [new files]</code><br>
<code>  git pull origin master</code><br>
<code>  git commit -am "[commit message]"</code><br>
<code>  git push origin master</code><br>
<br>
In case of Git conflicts: <br>
<code>  git checkout -b [some name that indicates this branch is screwed up]</code><br>
&nbsp;&nbsp;Initiate a pull request on github.com<br>
<br>
###Helpful links/tips for ICSP(In-Circuit Serial Programming)<br>
&nbsp;&nbsp;<a href="http://playground.arduino.cc/Code/MegaISP">Arduino MegaISP</a><br>
&nbsp;&nbsp;<a href="https://learn.sparkfun.com/tutorials/installing-an-arduino-bootloader">Installing a bootloader</a><br>
&nbsp;&nbsp;<a href="/usr/local/CrossPack-AVR-20131216/manual/gettingstarted.html">Programming with mac OS X</a><br>
<br>
&nbsp;&nbsp;Example Makefile settings (OS X):<br>
<code>    PROGRAMMER = -c avrisp -b 19200 -U flash:w:main.hex -P /dev/tty.usbserial-A600bTaM -v</code><br>
<br>
&nbsp;&nbsp;If you are receiving stk500 errors when attempting to program via ICSP try holding down the RESET button of the programmer (or Arduino as ISP) right before you program<br>
<br>

 
