

# Requirements #

## Tools ##
Listed below are the tools that we use and recommend you install to set up your environment.
### CMake ###
We use CMake to help configure our projects and generate our Makefiles. You can find more information about CMake [here](https://cmake.org/). The minimum
version required is `3.20` which is also asserted in the file `src/CMakeLists.txt`.

### Make ###
Make is used to specify source file dependencies through files called Makefiles. You can find more information about Make [here](https://www.gnu.org/software/make/manual/make.html#Overview).

### OpenOCD ###
This is the tool we use to debug our boards. Open On-Chip-Debugger is versatile and open source. You can read about it [here](https://openocd.org/).

### Compiler ###
We use the `arm-none-eabi-gcc` (C) and `arm-none-eabi-g++` (C++) compilers for our ARM based STM32 microcontrollers.

## Installations ##
### Linux ###
For arch-linux users, you can install the tools above using this command:
```
pacman -S arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib
pacman -S base-devel ninja openocd
```

For Debian users, you can install the tools above using this command:
```
sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi
apt install build-essential ninja-build openocd
```
Lastly, to install the required python modules, run this command:

`pip install -r src/cangen/requirements.txt`.

### Mac ###
We recommend you install Homebrew on your computer. If you don't know what Homebrew is, it's a package manager for MacOS that allows you
to install packages from the command line. Check out this [link](https://brew.sh/) to figure out how you can get that set up.

Now that you have Homebrew set up, the next step is to install the packages. You can use the following commands to install the tools listed
above:
```
brew install cmake make ninja open-ocd
brew install --cask gcc-arm-embedded
```

Lastly, to install the required python modules, run this command:

`pip install -r src/cangen/requirements.txt`.

### Windows ###
There is a plethora of options available on Windows and we'll use Scoop as an example, but feel free to search out
the package command for the package manager you are using. 

To set up Scoop, you can follow this [page](https://scoop.sh/). 

Next, you're going to want to install the tool packages with this command:
```
scoop install cmake make ninja openocd
scoop bucket add extras
scoop install extras/gcc-arm-none-eabi
```

Lastly, to install the required python modules, run this command:

`pip install -r src/cangen/requirements.txt`.

## Python
You need a relatively recent version of Python. 3.9 and above should be fine. 

### Windows
Download the latest and greatest at [python.org](https://www.python.org/downloads/). You want the "x64 Installer" variant. Run it and follow the prompts. Make sure to add it to your PATH
### Mac
You'll want to use Homebrew to get a recent version, as the versions included are ancient. You may need to use `pip3` and `python3`.

### Linux
Ubuntu and Debian come with python included. It may be ancient. If things don't work, look at updating your Ubuntu/Debian release to get a newer version, or using `pyenv`.
You may wish to run `sudo apt install python-is-python3` so that `python` defaults to using Python 3.

Rolling release distros likely have the latest version of Python available in their repositories.

### Post-installation instructions
We use a few libraries to generate some of our code. They can be installed with `pip install -r src/cangen/requirements.txt`.

<br /><br />

# Command Line Building #
See [Visual Studio Code Extensions](#Extensions) section for a seamless in-IDE configure/build approach.

## Configuring the Project ##
`cmake -S src -B build`
- The `-S` option points cmake towards the source directory, and the `-B` option the build directory. 


## Build Command ##
`cmake --build build/ --parallel`
The `--build` option directs cmake towards the build directory (`build/` in this case). The `--parallel` option lets you specify the number of threads to be used when building. Not specifying a number will default to the number
of cores you have on your machine, which is generally a good idea.

<br /><br />

# IDEs #
If you have any IDEs and other setups you would like to list here, just add it here and put up a PR (make sure to make some high level Electrical members as reviewers).

## Visual Studio Code ##
We chose Visual Studio Code as an IDE listed here because it is very flexible: to different OSs, build systems, languages, user preferences, and more!

### Installation ###
Please just install the latest version of `gcc-arm-none-eabi` and refer to the [top level CMakeLists file](src/CMakeLists.txt)

#### Windows ####
Go to [VSCode's download page](https://code.visualstudio.com/download) and get the latest version by clicking on the Windows download button.

#### Linux ####
##### Debian, Ubuntu, Red Hat, Fedora, or SUSE #####
Go to [VSCode's download page](https://code.visualstudio.com/download) and get the latest version by clicking on the appropriate Linux download button.

##### Other Linux Distros #####
If you have a different Linux distro than is listed in the previous heading, we assuming that you can likely figure out how to install VSCode yourself. However, if you have any questions, please let us know.

#### Mac ####
Go to [VSCode's download page](https://code.visualstudio.com/download) and get the latest version by clicking on the appropriate Mac download button.

### Extensions ###
In order to make VS Code function smoothly with CMake one will have to install some extensions. If you don't know how to install extensions, look at [VSCode's quick tutorial](https://code.visualstudio.com/learn/get-started/extensions).
* C/C++ (ms-vscode.cpptools)
* CMake Tools (ms-vscode.cmake-tools)
* Cortex-Debug (marus25.cortex-debug)
* YAML (redhat.vscode-yaml)
  * For CAN definition writing, so is optional if just building.

Once the extensions are installed these settings will allow VSCode to call the proper sequence in order to configure, then build the project.

To add these settings:

* Make sure you have this repo's folder open in VSCode (File>Open Folder...). 
* Then, create a folder under `new_member_firmware` called `.vscode`.
* Next, create a new file in that folder called `settings.json`. That file should pop up blank on the screen. 
* Copy and paste what is below to set up CMake correctly.

#### `.vscode/settings.json` for Firmware Repo ####
Once the extensions are installed these settings will allow VS Code to call the proper sequence in order to configure, then build the project:
```
{
  // Set the root CMake location
  "cmake.sourceDirectory": "${workspaceFolder}/src",
  "cmake.configureSettings": {
    // Set the toolchain file, this is relative to the CMake root
    "CMAKE_TOOLCHAIN_FILE": "${workspaceFolder}/src/cmake/stm32_gcc.cmake"
  },
  // Set up clang-format as VSCode's formatter.
  "C_Cpp.formatting": "clangFormat",
  "editor.formatOnSave": true,
  "editor.defaultFormatter": "ms-vscode.cpptools",
}
```
Copy this text into the directory and file listed above.

### General Workflow Example ###
* Configure the project once when starting
  - Ctrl-Shift-P
  - CMake: Configure
* Build a target/project
  - Shift-F7
  - Select Desired Project or all
* Edit files and rebuild as necessary
* Adding files
  - Add source files.
  - Add names of source files to respective `CMakeLists.txt`.
  - CMake will detect that `CMakeLists.txt` changed and reconfigure that part of the project on the next build.

### Debugging ###
After installing the [_Cortex-Debug_](#Extensions) extension, go to the debugging pane in VS Code (left side, looks like a play symbol with a bug next to it) and make a new launch configuration (click "create a launch.json file" then "Cortex Debug").
In the `launch.json` that is created paste these commands into the file (what is there can be deleted):

```
{
  // Use IntelliSense to learn about possible attributes.
  // Hover to view descriptions of existing attributes.
  // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
  "version": "0.2.0",
  "configurations": [
    {
      "cwd": "${workspaceRoot}",
      "executable": "${command:cmake.launchTargetPath}",
      "name": "Debug with OpenOCD",
      "request": "launch",
      "type": "cortex-debug",
      "servertype": "openocd",
      "configFiles": [""],
      "searchDir": [],
      "runToEntryPoint": "main",
      "showDevDebugOutput": "none"
    },
    {
      "name": "ST-LINK V2 STM32::L4",
      "cwd": "${workspaceRoot}",
      // Select the executable using CMake
      "executable": "${command:cmake.launchTargetPath}",
      "request": "launch",
      "type": "cortex-debug",
      // Use openocd to connect to the microchip
      "servertype": "openocd",
      "configFiles": [
        // Set the type of JTAG programmer (ST-LINK/V2, STLINKV2.1, etc)
        "interface/stlink.cfg",
        // Set the MCU type
        "target/stm32l4x.cfg"
      ]
    },
    {
      "name": "ST-LINK V2 STM32::F4",
      "cwd": "${workspaceRoot}",
      // Select the executable using CMake
      "executable": "${command:cmake.launchTargetPath}",
      "request": "launch",
      "type": "cortex-debug",
      // Use openocd to connect to the microchip
      "servertype": "openocd",
      "configFiles": [
        // Set the type of JTAG programmer (ST-LINK/V2, STLINKV2.1, etc)
        "interface/stlink.cfg",
        // Set the MCU type
        "target/stm32f4x.cfg"
      ]
    },
    {
      "name": "ST-LINK V2 STM32::G4",
      "cwd": "${workspaceRoot}",
      // Select the executable using CMake
      "executable": "${command:cmake.launchTargetPath}",
      "request": "launch",
      "type": "cortex-debug",
      // Use openocd to connect to the microchip
      "servertype": "openocd",
      "configFiles": [
        // Set the type of JTAG programmer (ST-LINK/V2, STLINKV2.1, etc)
        "interface/stlink.cfg",
        // Set the MCU type
        "target/stm32g4x.cfg"
      ]
    },
    {
      "name": "ST-LINK V2 STM32::G0",
      "cwd": "${workspaceRoot}",
      // Select the executable using CMake
      "executable": "${command:cmake.launchTargetPath}",
      "request": "launch",
      "type": "cortex-debug",
      // Use openocd to connect to the microchip
      "servertype": "openocd",
      "configFiles": [
        // Set the type of JTAG programmer (ST-LINK/V2, STLINKV2.1, etc)
        "interface/stlink.cfg",
        // Set the MCU type
        "target/stm32g0x.cfg"
      ]
    },
    {
      "name": "ST-LINK V2 STM32::L5",
      "cwd": "${workspaceRoot}",
      // Select the executable using CMake
      "executable": "${command:cmake.launchTargetPath}",
      "request": "launch",
      "type": "cortex-debug",
      "servertype": "openocd",
      "svdFile": "stm32l5_other.svd",
      "configFiles": [
        // Set the type of JTAG programmer (ST-LINK/V2, STLINKV2.1, etc)
        "interface/stlink.cfg",
        // Set the MCU type
        "target/stm32l5x.cfg"
      ]
    }
  ]
}
```
The `launch.json` file above contains the configurations for debugging our microcontrollers with an ST-LINK V2 debugger. Most of our boards use the STM32G4 microcontroller. However other boards or legacy boards use differnt MCU's, so they are listed here as well. 

In order to start the debugger, go to the _Run and Debug_ tab (Ctrl-Shift-D).
Select the appropriate ST-LINK device from the dropdown near the top (by the little green play button) and appropriate microcontroller. Then, click the green arrow and debugging should begin!

### Formatting ###
The [VSCode Settings](#`.vscode/settings.json`-for-CMake) section has some settings that enables `clang-format` as the formatter for this repo. Any SVP source files that are not formatted will cause the Jenkins build to fail on any Pull Requests. The `.clang-format` file at the root of the directory specifies the format configuration for this repo.

### More Recommended Extensions ###
- Doxygen Documentation Generator (cschlosser.doxdocgen): This helps with generating `doxygen` comments in all of our code.
- Remote - SSH (ms-vscode-remote.remote-ssh): Helps make developing through SSH connections seemless.
