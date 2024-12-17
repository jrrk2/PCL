Building LibRaw for PCL Development
-----------------------------------

Since version 1.8.8 of PixInsight, LibRaw no longer forms part of the PCL distribution. This is because the authors of LibRaw have made changes to their code distribution that prevent its inclusion as a standard PCL static library.

To build LibRaw for PCL development, follow these steps:

1. Download LibRaw's source code. We recommend cloning the [https://github.com/LibRaw/LibRaw](official LibRaw GitHub repository) to a directory on your local file system. Let's represent this directory as $LIBRAWDIR.

2. Copy the required Makefile.pcl.xxx file for your platform (where xxx is one of linux, macosx or windows) to $LIBRAWDIR. These files are available on [https://gitlab.com/pixinsight/PCL/tree/master/src/3rdparty/libraw]($PCLSRCDIR/3rdparty/libraw).

3. Use the appropriate Makefile.pcl.xxx file to build LibRaw. For example:

$ cd $LIBRAWDIR
$ make -j -f Makefile.pcl.linux

4. You'll find the LibRaw static library file on the $LIBRAWDIR/lib subdirectory:

On Linux and macOS: liblibraw_r-pxi.a
On Windows: libraw_r-pxi.lib

Copy this file to your $PCLLIBDIR directory.

5. Copy the $LIBRAWDIR/libraw subdirectory to your $PCLINCDIR directory.

Now you can build the RAW module on your platform.

******
###### Copyright (C) 2003-2024 Pleiades Astrophoto
