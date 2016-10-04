TheRoom
=======

Introduction
------------
An OpenGL Real-Time Rendering Engine written in C++. TheRoom features various advanced rendering techniques
such as Physical Based Rendering, Cascaded Shadow Maps, Deferred Shading and others.

You can read more about the CG part of the engine if you build and read the report in docs directory.

Building
--------
Currently we have two build systems. The first and the one we use in production is a Makefile while the second
(legacy build system) is written in Haskell using Shake.

### Make
Run the commands below. The binaries will be in the `bin/<Deubg|Release>` directory.

```
set TOOLCHAIN=<MSVC|GCC>
set VARIANT=<Debug|Release>
make deps
make
```
Note: You can use the `-j 4` make flag to speed up the process and the `--no-print-directory` flag
to get rid of make's messages when changing directories.

### Shake
Run the commands below. Built binaries will reside in the `bin/<ARCH>/<VARIANT>` directory.

```
runhaskell Shakefile.hs --toolchain=<TOOLCHAIN> --variant=<VARIANT>
```
where <VARIANT> can be either Release|Debug and <TOOLCHAIN> can be either MSVC|GCC|LLVM.

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
* "Vlachakis Dimitris" <dimitrisvlh@gmail.com>

Licensing
---------
Read [LICENSE](LICENSE.md)  

Copyright (C) 2015-2016 Agorgianitis Loukas <agorglouk@gmail.com>, Vlachakis Dimitris <dimitrisvlh@gmail.com>
All rights reserved.

