#!/bin/bash

# First ensure we have a configure script:
pushd ..
./autogen.sh
make distclean
popd

# Note the configure stage under wine** is really slow can easily be over 15 minutes
# make of the icons is also very slow** - can easily over 5 minutes
# compartively the make of the actual src code is not too bad
wine ~/.wine/drive_c/windows/system32/cmd.exe /c configure_and_make.bat

# ** slowness is probably due to lots of forking going on starting many new small processes
