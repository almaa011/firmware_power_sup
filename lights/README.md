# F1.50 Lights # 

## Description ##
This directory is to provide a simple implentation of the F1.50 Lights board.

## Pin Mapping- corresponds to channels on the lights board ##

Mappings can be changed via the Application Constructor in the ```app.cc``` file.

*Front Board:
**DRL Left - Ch0
**DRL Right - Ch1
**Left Turn - Ch2
**Right Turn - Ch3
**Accent Left - Ch4
**Accent Right - Ch5


*Back Board
**Brake Lights - Ch0
**Blinky Light - Ch1
**Left Turn - Ch3
**Right Turn - Ch4
**Camera - Ch5

**Ch4 is unused

## Light Class ##

This class provides wrapper functions for easey use of the light channels on the F1.50 Lights board. It can be initialized as follows:

```Light light_0 = Light(PORT_LIGHT_0, PIN_LIGHT_0);```

Calling ```light_0.init();``` will set up the necessary hardware interfaces for correct interaction. 
