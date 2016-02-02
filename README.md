Xarcade2Joystick
================

_Xarcade2Jstick_ exclusively captures the keyboard events of the __Xarcade Tankstick__ and maps these events to corresponding events on two virtual game pad devices.

_Xarcade2Jstick_ was originally written as a supplementary tool for the [RetroPie Project](http://blog.petrockblock.com/retropie/). Using this tool allows the usage of the auto-config capability of [RetroArch](http://themaister.net/retroarch.html), a central component of a RetroPie installation.

## Usage

Your Xarcade will appear as two gamepads and can be used accordingly. There are also some special combinations of buttons that have special meaning:

* P1 start + P2 start = TAB
* P1 select + P2 select = ESC (the front side buttons)
* P1 select + P1 start = 5
* P2 select + P2 start = 6

## Downloading

If you would like to download the current version of _Xarcade2Jstick_ from [its Github repository](https://github.com/petrockblog/Xarcade2Joystick), you can use this command:
```bash
git clone https://github.com/petrockblog/Xarcade2Jstick
```

## Building and Installation

To build Xarcade2Jstick follow these commands:
```bash
cd Xarcade2Jstick
make
```

If everything went fine you can install with the command
```bash
sudo make install
```

## Installation as Service

You can install _Xarcade2Jstick_ as daemon with this command:
```bash
sudo make installservice
```

## Uninstalling the service and/or the binary

You can uninstall the daemon with this command:
```bash
sudo make uninstallservice
```

You can uninstall the binary with this command:
```bash
sudo make uninstall
```

<br><br>
__Have fun!__

-Florian [petrockblock.com](http://blog.petrockblock.com)