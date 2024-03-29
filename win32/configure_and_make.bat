:: Full build
::
:: Note this assumes the 'configure' script already exists
::  either via part of the source download or via separate generation - e.g. using wine
::
set PATH=%PATH%;%SystemDrive%\Mingw\bin;%SystemDrive%\msys\1.0\bin
cd ..\
sh configure CFLAGS=-DWINDOWS LIBCURL=-lcurldll LIBS=-lzdll --disable-realtime-gps-tracking --disable-scrollkeeper
make
