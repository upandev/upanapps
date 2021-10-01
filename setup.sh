#  Upanix - An x86 based Operating System
#	 Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
#
#  I am making my contributions/submissions to this project solely in
#  my personal capacity and am not conveying any rights to any
#  intellectual property of any third parties.
#                                                                            
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#                                                                          
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#                                                                          
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/
export CXX=i686-elf-g++
export CC=i686-elf-gcc

UPANAPPS=`basename $UPANAPPS_HOME`
PWD=`pwd`
COMP=`echo $PWD | sed "s/^.*$UPANAPPS//g" | cut -d"/" -f2`

IS_LIB="no"

LIBC_INCLUDE="-I${UPANAPPS_HOME}/libc/include \
-I${UPANAPPS_HOME}/libc/sysdeps/mos/common/ \
-I${UPANAPPS_HOME}/libc/sysdeps/mos/common/bits"

LIBM_INCLUDE="-I${UPANAPPS_HOME}/libm/include -I${UPANAPPS_HOME}/libm/include/bits"

LIBMCAL_INCLUDE="-I${UPANAPPS_HOME}/libmcal/include"

LIBMTERM_INCLUDE="-I${UPANAPPS_HOME}/libmterm/include"

COMPILE_OPTIMIZE="-O2"

if [ $COMP = "libc" ]
then
  export INCLUDE=" -I./ ${LIBC_INCLUDE}"
#because of inline asm codes in syscall sections
  COMPILE_OPTIMIZE="-O0"
  IS_LIB="yes"
elif [ $COMP = "libm" ]
then
  export INCLUDE=" -I./ ${LIBM_INCLUDE} ${LIBC_INCLUDE}"
  IS_LIB="yes"
elif [ $COMP = "libmcal" ]
then
  export INCLUDE=" -I./ ${LIBMCAL_INCLUDE} ${LIBC_INCLUDE}"
  IS_LIB="yes"
elif [ $COMP = "libmterm" ]
then
  export INCLUDE=" -I./ ${LIBMTERM_INCLUDE} ${LIBC_INCLUDE}"
  IS_LIB="yes"
elif [ $COMP = "libit" ]
then
  export INCLUDE=" -I./ ${LIBC_INCLUDE}"
elif [ $COMP = "editor" ]
then
  export INCLUDE=" -I./ ${LIBC_INCLUDE} ${LIBMTERM_INCLUDE}"
elif [ $COMP = "nasm" ]
then
  export INCLUDE=" -I./ -I${UPANAPPS_HOME}/nasm/include ${LIBC_INCLUDE}"
elif [ $COMP = "shell" ]
then
  export INCLUDE=" -I./ ${LIBC_INCLUDE}"
fi

SO_FLAGS=""
if [ $IS_LIB = "yes" ]
then
  SO_FLAGS="-shared -fPIC"
  SO_LD_FLAGS="-Wl,-shared -Wl,-fPIC"
fi

COMPILE_FLAGS=" -Wall ${COMPILE_OPTIMIZE} -nostdlib -nodefaultlibs -ffreestanding -nostdinc -nostartfiles -D__GCC__"
export CFLAGS="-c $COMPILE_FLAGS $SO_FLAGS"
export LD_FLAGS="$COMPILE_FLAGS $SO_LD_FLAGS"
