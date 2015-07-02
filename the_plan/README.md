#the_plan<br>
###Software design goals<br>
Ease-of-use and accessibility<br>
No matter which tool is used to write and program the module, the software should<br>
&nbsp;&nbsp;Compile without errors when using the latest Arduino IDE<br>
&nbsp;&nbsp;Not require external libraries to be installed on the user's local machine beyond the repo itself<br>
Each project should contain a .ino file as it's entry point, even if it invokes a .cpp or .c file that contains the actual program<br>
**Why?** Arduino is a common and popular framework and has many useful libraries, as well as support across multiple OSes along with the programmer it utilizes, avrdude<br>
###Hardware design goals<br>
Low-cost: right now the ATmega328p is the most common microcontroller target<br>
Compact: surface-mount components and low chip-count solutions<br>
Distributed and modular: large number of smaller units which each have very specific tasks networked together<br>
###Timeline<br>
####July<br>
Finish I2C inter-module and serial debugging includes<br>
Remove obsolete software<br>
Standardize module communication, switch to one or two well-commented files to include with each module<br>
Write 'make a module' tutorial<br>
####August<br>
Publish debugging and inter-module communication API<br>
Write guide to more advanced programming, including command line, Atmel Studio, and ICSP<br>
Update module to ROS communication **or** write wrapper for debugging API for ROS use<br>
