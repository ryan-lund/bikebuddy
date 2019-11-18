VM: Ubuntu 18.04 LTS (using a nice distribution called PopOS)
Password: 149

NRF SDK is found in Documents; all the supporting stuff (gcc, nrfutils) are all installed, as well as VSCode

Development / Make Instructions:

1) clone bikebuddy into root NRF SDK (~/Documents/nRF5_SDK)
2) open bikebuddy root folder in VSCode
	-important that you open the root folder, as the .vscode config is in root folder
	-the c/cpp properties file is configured with the location of all the includes
	-should be able to use VSCode's "go to definition" to navigate includes (right click the include --> Go to Definition)
3) frontapp and backapp are the app directories. Develop here (or elsewhere, and move files into here)
	-boilerplate code was created by modifying example code with the makefile and linker modifications needed for the nrf52840_mini
4) to make, navigate to "frontapp (or backapp) / nrf52840_mini/s140/armgcc/", and then run "make"


Instructions to Flash:
1) Shut down VM. Plug in the mini.
2) Put the mini into DFU mode (double press the reset button, or press reset button while holding down the other pin 13 button)
3) On host machine, find the serial port of the mini.
	-On Linux: open terminal and type "dmesg | grep tty". It should be the last one (ex: /dev/ttyACM0)
	-On Windows: Device manager should give the port
	-On Mac: I'm too poor to own one. no idea sorry
4) Open Virtualbox, open bikebuddy_dev settings. Go to Serial Ports. Configure Port 1 as follows:
	-Enable Serial Port
	-Port Number (choose something greater than 1): COM2
	-Port Mode: Host Device
	-Connect to existing pipe/socket (should be auto checked)
	-Path/Address: (the serial port found in the previous step) ex: /dev/ttyACM0
5) Boot back into the VM
6) navigate to "frontapp (or backapp) / nrf52840_mini/s140/armgcc/"; run "make"
7) run "dmesg | grep tty" (on the VM this time), and the output that also lists baud rate and stuff is the port on the guest. ex: /dev/ttyS3
8) to flash: make bootload SERIAL_PORT=<port from step 7>
	- ex: make bootload SERIAL_PORT=/dev/ttyS3


Notes:
1) If you get Permission Denied errors relating to the port when trying to flash:
	-sudo usermod -a -G dialout $USER
	-logout and log back in
2) Every time you plug out the mini (or even put into DFU mode), the serial port on the host machine will probably change. You have to shut down the VM, and follow the "Instructions to Flash" from start.
3) If the VM wont run after trying to forward the serial port when trying to flash, that serial port is not available anymore (the port probably changed). Update accordingly in the VM settings (either fix the port name, or disable the serial port)


