Linux
====

The Linux build CMake as the ESP IDF development environment does but
its CMakeFiles.txt are designed to be entirely independent of the IDF
build. To use the Linux build, your source tree needs to be set to the
'linux' branch.

## 1. Create a 'build' directory in the project root and *cd* into it

This is the basic first step of most CMake based projects.

## 2. Launch CMake to configure the build directory

The simplest way to configure the build directory is with CMake is the following:

> cmake ../CMakeLists.txt -DTARGET_TYPE=linux_glib

The TARGET_TYPE variables triggers CMake to configure the Linux
build. Missing that variable, the build will be configured for the
ESP-IDF.

The output on my system is the following:

```
-- The C compiler identification is GNU 11.3.0
-- The CXX compiler identification is GNU 11.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
Configuring: POSIX build
-- Found PkgConfig: /usr/bin/pkg-config (found version "0.29.2") 
-- Checking for module 'glib-2.0'
--   Found glib-2.0, version 2.72.1
-- Configuring done
-- Generating done
-- Build files have been written to: /home/neumann/W/Tria/rinasense/build
```

The only dependency required for the build, beside the obvious (ie:
gcc & co), is `libglib2.0-dev` on Debian.

## 3. Build

The CMakeLists.txt is pretty extensive but for now only the set of
test targets that actually build and run completely will build. Just
run `make` from the **build** directory.

## 4. Test

There are several test programs generated as part of the build. The
can be run altogether with `make test`

```
Running tests...
/usr/bin/ctest --force-new-ctest-process
Test project /home/neumann/W/Tria/rinasense/build
    Start 1: test_rina_name
1/4 Test #1: test_rina_name ...................   Passed    0.00 sec
    Start 2: test_rina_gpha
2/4 Test #2: test_rina_gpha ...................   Passed    0.00 sec
    Start 3: test_buffer_management
3/4 Test #3: test_buffer_management ...........   Passed    0.00 sec
    Start 4: test_arp826
4/4 Test #4: test_arp826 ......................   Passed    0.00 sec
```

100% tests passed, 0 tests failed out of 4

You can also run test program invidually in GDB or Valgrind.

FreeRTOS RINA library for microcontroller platforms
====

## 1. Overview
This piece of software is a RINA library implementation for FreeRTOS, 
so that RINA communication capabilities are supported by devices that 
use this RTOS (for information about RINA please visit 
http://pouzinsociety.org). Initially the RINA implementation can be used over the 
ESP32 hardware platform, using RINA over WiFi and RINA over Bluetooth.
This RINA library will be interoperable with IRATI, a RINA implementation
for Linux Operating Systems (https://github.com/IRATI/stack).

## 2. Development workflow
This project has the following branches:

* **master**: contains the most up to date stable code, may not contain features 
that have been developed and are merged in *dev* but are yet not stable enough.

* **dev**: contains all finished and tested features. When the dev branch is 
considered enough, it can be merged into *master*.

* **feature dev branches**: each feature is developed in a dedicated branch, and 
merged into *dev* when they are ready.

To develop a new feature or a fix for a bug, the workflow is the following (as a 
prerequisite you must fork the main rinasense repo):

1. Create an issue summarising the work to be done (https://github.com/Fundacio-i2CAT/rinasense/issues/new/choose). 
Assign it to someone and add one or more labels.

2. Create a new branch from the *dev* branch. The branch name must follow this 
conventions: <issue number>-<one_or_more_descriptive_words>

3. Start developing the feature. Pull from *dev* as required to avoid merge 
conflicts later. Once the feature is developed, create a pull request to merge
the branch into *dev*.

4. Once the pull request has been accepted and merged into *dev*, remove the 
specific development branch.

## 3. Naming Conventions
This project follows the FREERTOS naming conventions:
* **variables**:
    - Variables of type uint32_t are prefixed ul, where the 'u' denotes 'unsigned' and the 'l' denotes 'long'.
    - Variables of type uint16_t are prefixed us, where the 'u' denotes 'unsigned' and the 's' denotes 'short'.
    - Variables of type uint8_t are prefixed uc, where the 'u' denotes 'unsigned' and the 'c' denotes 'char'.
    - Variables of non stdint types are prefixed x. Examples include BaseType_t and TickType_t, which are portable layer defined typedefs for the type that is the natural or most efficient type for the architecture, and the type used to hold the RTOS tick count, respectively.
    - Variables of type size_t are prefixed ux.
    - Enumerated variables are prefixed e
    - Pointers have an additional p prefixed, for example, a pointer to a uint16_t will have prefix pus.
    - variables of type char * are only permitted to hold pointers to ASCII strings and are prefixed pc.
* **Functions**:
    - File scope static (private) functions are prefixed with prv. 
    - API functions are prefixed with their return type, as per the convention defined for variables with the addition of the prefix v for void.
    - API function names start with the name of the file in which they are defined. For example vTaskDelete is defined in tasks.c, and has a void return type.
* **Macros**:
    - Macros are prefixed with the file in which they are defined. The pre-fix is lower case. For example, configUSE_PREEMPTION is defined in FreeRTOSConfig.h.
    - Other than the prefix, macros are written in all upper case, and use an underscore to separate words.

