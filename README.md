# open_robotics_embedded
Drive motor and turntable controller for OpenRobotics.ca<br>
Seriously any questions get at me max@theprogrammingclub.com<br>
###Things you need to be effective here:<br>
git:<br>
&nbsp;&nbsp;<a href="https://windows.github.com/">Windows install</a><br>
&nbsp;&nbsp;<a href="http://git-scm.com/download/linux">Linux/Unix install</a><br>
Arduino IDE (more advanced users use Atmel studio as below):<br>
&nbsp;&nbsp;http://www.arduino.cc/en/Main/Software<br>
Our Trello page for task-tracking:<br>
&nbsp;&nbsp;https://trello.com/openrobotics<br>
Slack is helpful for communication:<br>
&nbsp;&nbsp;https://opbots.slack.com/<br>
###About the repo<br>
We program in a mix of Arduino and CPP over USB and using ICSP, usually using avrdude.exe to program<br>
http://www.atmel.com/microsite/atmel_studio6 is used as a dev environment with the Visual Micro extension<br>
<br>
For serial programming in a non-Arduino project:<br>
&nbsp;&nbsp;Select Tools -> External tools<br>
&nbsp;&nbsp;&nbsp;&nbsp;Choose Add<br>
&nbsp;&nbsp;&nbsp;&nbsp;Set Title to 'Serial Program'<br>
&nbsp;&nbsp;&nbsp;&nbsp;Change the command to your Arduino directory that includes avrdude<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Example: <code>C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avrdude.exe</code><br>
&nbsp;&nbsp;&nbsp;&nbsp;Change the Arguments to match your situation, replacing COM16 with the COM port you are using
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Example: <code>-C"C:\Program Files (x86)\Arduino\hardware\tools\avr\etc\avrdude.conf" -patmega328p -carduino -P\\.\COM16 -b57600 -U flash:w:"$(ProjectDir)Debug\$(ItemFileName).hex":i -v</code><br>
&nbsp;&nbsp;&nbsp;&nbsp;Click ok<br>
&nbsp;&nbsp;&nbsp;&nbsp;Now to program, select Tools -> Serial Program<br>
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
###Helpful links for programming over serial<br>
<a href="http://playground.arduino.cc/Code/MegaISP">Arduino MegaISP</a>
