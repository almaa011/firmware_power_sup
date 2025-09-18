======== UMNSVP CMake Scripts ========

These files are used to help us set up boards (located in src/boards/) with the
libraries (located in src/libraries). There is a lot going on, but don't worry,
once the scar tissue forms everything will start to make sense.


=== Part 1: Types of libraries ===

The first thing you'll notice when scrolling through these files is the mention
of an INTERFACE library. These are a bit tricky to explain, but here's my best
shot:

Normal libraries (fatfs is a good example) don't need to know anything about the
board that we are using - they are purely software constructs. To use them, we
take the source and header files and compile it to a STATIC library (fatfs.a).
The static library contains all of the functions and variables that the source
files have (i.e we compile all the .c files into one "unit"). Then, when we want
to use fatfs in a project, we tell cmake this, and it adds the header files
(describing what fatfs can do) to the "includes" (search path) of the board.
That way in our code we can make references to the functions that fatfs
contains.
When it comes time to compile, we compile fatfs *once* into a fatfs.a, and then
_link_ fatfs to the rest of our code. Linking is how our code knows where the
fatfs code actually is - the headers describe how to use it, the code describes
how to *do* it.
https://stackoverflow.com/questions/3322911/what-do-linkers-do
https://stackoverflow.com/questions/6264249/how-does-the-compilation-linking-process-work


    ┌───────────────────────────────────────────┐
    │                                           │
    │                      ┌────────────┐       │
    │                      │ lib sources│       │
    │                      └────┬───────┘       │
    │                           │               │
    │                           │ compiles to   │
    │  ┌──────────┐             ▼               │
    │  │   app    │ links  ┌──────────┐         │
    │  ├──────────┼───────►│ lib.a    │         │
    │  │app.c     │        └──────────┘         │
    │  │lib_conf.h│      lib.a knows nothing    │
    │  └──────────┘      about app headers      │
    │ lib_conf.h won't configure lib.a          │
    │                                           │
    │                                           │
    └───────────────────────────────────────────┘


The problem comes in when we get to more low-level libraries that actually
interact with the registers of the MCU. CMSIS and HAL are the big examples.
These libraries have a *lot* of features, and we need to be able to configure
them between boards with a header file (stm32f4xx_hal_conf.h for example).
Slight problem: these header files are per-board! Normal libraries have no
access to any of the source/headers of the targets that use them. So instead we
use an INTERFACE library, which *doesn't* compile to a static library (so no
hal.a containing all the hal code). Instead, an INTERFACE library *adds* the
source and headers to the target that links against it, so now they share
the same code base, and the HAL code can access the board-specific configuration
files when building a board.

Most of our libraries are therefore INTERFACE libraries because they depend on
board-specific configuration/header files to work.
- CMSIS: contains MCU specific startup assembly.
- HAL: Needs to know what peripherals we use/clock setup and such.
- UMNSVP: need to know what MCU family (G4, L4) is being used.
- Skylab2: Needs access to the skylab2_io.h header for configuration.

Some libraries don't need to be an interface/board-specific:
- fatfs: pure software - needs a hardware mapping but not part of lib.

Some libraries aren't used, and should be cleaned up.

=== Part 2: Compiler Flags ===

Once you begin to understand how the linker/CMake operate with INTERFACE libraries,
you may be tempted to spin up a new microcontroller, add a library, and get hacking.
There is one critical thing remaining. Each microcontroller has a different ARM Cortex-m
processor in it. These different processors have different features and instruction sets.
It is *extremely* important that the compiler knows what processor is being used, since
it may try to use an instruction that doesn't exist, causing headaches and hard faults.

To resolve this, I wrote stm32_compile_options.cmake, which is a system to allow for
easily registering new microcontrollers and specifying their compiler flags. Refer to
that file for more information.



=== Part 3: CMake, Trauma, Apotheosis ===

As you browse the files here, you may begin to feel an intense sense of nausea,
ringing in the ears, or heart palpitations. Don't be alarmed, that indicates
only that you are still sane. Here are some resources to help understand what's
going on and guide you on the path to insanity^H^H^H^H^H^H^H^H enlightenment.

https://cmake.org/cmake/help/latest/index.html cmake general help doc.
https://cliutils.gitlab.io/modern-cmake/ Modern Cmake (we are modern)
https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1 gist of tips
https://github.com/ObKo/stm32-cmake very complex cmake template for stm32
https://cmake.org/cmake/help/latest/guide/tutorial/index.html cmake tutorial


Here's how the CMake code is structured. We will take a closer look at the internals later.
                        ┌───────────────────────────────────────────────────┐
                        │                                                   │
                        │ add_executable(l5Example)                         │
                        │                                                   │
  target props  ──────► │ setup_board(l5Example STM32L562 VETX)             │
  added here            │                                                   │
                    ┌── │ add_cmsis(l5Example)                              │
                    │   │ add_hal(l5Example)                                │
            library │   │ add_umnsvp(l5Example)                             │
       helper funcs │   │ add_segger(l5Example)                             │
                    └── │ add_skylab2(l5Example)                            │
                        │                                                   │
                        │ # Add the `inc` directory for this board          │
                        │ target_include_directories(l5Example PRIVATE inc) │
                        │                                                   │
                        │ #use the correct linker script                    │
         compilation    │ use_stm32_linker_scripts(l5Example)               │
         setup          │ add_CPU_options(l5Example)                        │
                        │                                                   │
                        │ # Set all the sources for this board              │
                        │ target_sources(l5Example                          │
                        │         PRIVATE                                   │
                        │         src/main.cc                               │
                        │         src/stm32l5xx_it.cc                       │
                        │         src/application.cc                        │
                        │         )                                         │
                        │                                                   │
                        └───────────────────────────────────────────────────┘

(this code is taken from l5example CMakeLists.txt)

In short, we do the following:
1. declare our new board firmware
2. Call our special setup_board function with the microcontroller model + variant
3. add special libraries that need to know more details about our hardware
4. run some helper functions to link + compile the board firmware correctly
5. add our application sources.

To see what the setup_board function does, check out utils.cmake. It basically
adds two properties to the CMake target in the first argument. Target properties
can be later used by the library helper functions like `add_cmsis`, so they know
what folder to read from (g0 vs l5). Care is taken to ensure that libraries are only
generated as-needed, and are reused. This is safe since they are INTERFACE libraries
like mentioned above.

The compilation setup scripts set the linker script to the correct variant (the last 4 letters,
VETX in the example). They also set the CPU options, which is per-family (L5/G4/G0 etc).
finally we add our own sources to the target, which contains things like interrupt handlers
and the `main` function alongside the rest of our application code.