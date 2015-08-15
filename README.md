TheRoom
=======

Introduction
------------
A sample OpenGL App to learn in depth Graphics development

Building
--------
 1. Clone the project and cd to the directory of the sample you want to build.
 2. Run:  
    ```
    python waf distclean configure --check-c-compiler=<CC> --check-cxx-compiler=<CXX>
    ```
    where CC and CXX combination can either be {msvc,msvc} | {gcc, g++} | {clang, clang++} and after that run:  
    ```
    python waf clean build install --variant=<VARIANT> --arch=<ARCH>
    ```
    where VARIANT can be either Release|Debug and ARCH can be either x86|x64.
 3. Built binaries will reside in the `bin\<ARCH>\<VARIANT>` directory.

ChangeLog
---------
 * TODO: Track major changes

Contributing
------------
 * For bug fixes, any well checked pull requests are welcome

Credits
-------
Written and maintained by: 
* "Agorgianitis Loukas" <agorglouk@gmail.com>

Licensing
---------
Read [LICENSE](LICENSE.md)  

Copyright (C) 2015 Agorgianitis Loukas <agorglouk@gmail.com>  
All rights reserved.

