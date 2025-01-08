 UI-Virtual-Fence-Repo
## Generating Code Documentation

This repository utilizes doxygen for code documentation. The version it was built around is 1.9.1. Generating documentation is straightforward and can be done with the following steps:

- with doxygen install on your computer navigate to the git repository in your terminal.
- run: ```doxygen Doxyfile```
- navigate to the newly created html folder in your file explorer
- open index.html
- enjoy

## Building

This repository uses platform IO as the build method. Typically Visual Studio Code is used along side the platform io plugin, which will then allow you to choose exactly what you would like to build.

To install on a linux PC:
- `sudo apt install python3`
- `sudo apt install python3-venv`
- install visual studio
- install the platform io plug in inside visual studio

From there you'll be able to navigate to the platform io build menu, then in the options select the one that corresponds to your microcontroller, desired functionality, and the beacon or tag. You can determin what microcontroller you have based on the version listed on the board:

**328p based boards:**
- Tag versions 1-3

**2580 based boards:**
- Beacon versions 3 and 4

**Optional Functions:**
- 0D: A basic function that sets a boundry based on a distance, and aims to keep animals out of that distance.
