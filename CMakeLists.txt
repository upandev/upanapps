#	 Upanix - An x86 based Operating System
#	 Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
#																			 
#	 This program is free software: you can redistribute it and/or modify
#	 it under the terms of the GNU General Public License as published by
#	 the Free Software Foundation, either version 3 of the License, or
#	 (at your option) any later version.
#																			 
#	 This program is distributed in the hope that it will be useful,
#	 but WITHOUT ANY WARRANTY; without even the implied warranty of
#	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	 GNU General Public License for more details.
#																			 
#	 You should have received a copy of the GNU General Public License
#	 along with this program.  If not, see <http://www.gnu.org/licenses/
cmake_minimum_required(VERSION 3.15)
project(UpanixApps)

set(CROSS_TOOL_PATH $ENV{HOME}/opt/cross)

set(UPANLIB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../upanlibs)
set(UPANLIB_OUT_DIR ${UPANLIB_SOURCE_DIR}/out)

set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(CMAKE_CXX_COMPILER ${CROSS_TOOL_PATH}/bin/i686-elf-g++)
set(CMAKE_C_COMPILER ${CROSS_TOOL_PATH}/bin/i686-elf-gcc)
set(CMAKE_LINKER ${CROSS_TOOL_PATH}/i686-elf/bin/ld)
set(CMAKE_AR ${CROSS_TOOL_PATH}/i686-elf/bin/ar)

set(ENABLE_CXX_RT ON)
	
set(EXCEPTION_SUP " -fno-exceptions -fno-rtti")
if(ENABLE_CXX_RT)
	set(EXCEPTION_SUP " -fexceptions -frtti")
endif()

set(SO_COMPILE_FLAGS -shared -fPIC)
set(SO_LD_FLAGS " -Wl,-shared -Wl,-fPIC")

set(CMAKE_CXX_FLAGS " -O0 -Wall -ffreestanding -nodefaultlibs -nostdlib -nostartfiles -nostdinc -std=c++11 -nostdinc++ -fno-threadsafe-statics -fpermissive ${EXCEPTION_SUP}")
set(COMMON_COMPILE_FLAGS " -O0 -Wall -std=c11 -nostdlib -nodefaultlibs -ffreestanding -nostartfiles -D__GCC__")
set(CMAKE_C_FLAGS ${COMMON_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${COMMON_COMPILE_FLAGS} ${SO_LD_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${UPANLIB_SOURCE_DIR}/libc/include)
include_directories(${UPANLIB_SOURCE_DIR}/libc/stdio)
include_directories(${UPANLIB_SOURCE_DIR}/libc/sysdeps/mos/common/)
include_directories(${UPANLIB_SOURCE_DIR}/libc/sysdeps/mos/common/bits)

include_directories(${UPANLIB_SOURCE_DIR}/libustd/infra)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/ds)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/algo)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/mem)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/mthread)

include_directories(${UPANLIB_SOURCE_DIR}/libm/include)
include_directories(${UPANLIB_SOURCE_DIR}/libm/include/bits)

include_directories(${PROJECT_SOURCE_DIR}/libmcal/include)

include_directories(${PROJECT_SOURCE_DIR}/libmterm/include)

include_directories(${UPANLIB_SOURCE_DIR}/libcxxrt)

include_directories(${UPANLIB_SOURCE_DIR}/libupanui)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/canvas)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/default)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/usfn)

link_directories(${UPANLIB_OUT_DIR})
link_directories(${OUTPUT_DIR})

#LIBIT
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libit -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE LIBIT_SRC)
string(REPLACE "\n" ";" LIBIT_SRC ${LIBIT_SRC})
add_library(it STATIC "${LIBIT_SRC}")

#LIBMCAL
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libmcal -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE LIBMCAL_SRC)
string(REPLACE "\n" ";" LIBMCAL_SRC ${LIBMCAL_SRC})
add_library(mcal SHARED "${LIBMCAL_SRC}")
target_compile_options(mcal PRIVATE ${SO_COMPILE_FLAGS})

#LIBMTERM
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libmterm -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE LIBMTERM_SRC)
string(REPLACE "\n" ";" LIBMTERM_SRC ${LIBMTERM_SRC})
add_library(mterm SHARED "${LIBMTERM_SRC}")
target_compile_options(mterm PRIVATE ${SO_COMPILE_FLAGS})

add_custom_target(crt
  COMMAND make WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/crt
  BYPRODUCTS ./crt/crt0.o ./crt/crti.o ./crt/crtn.o)

execute_process(COMMAND bash -c "${CMAKE_CXX_COMPILER} -print-file-name=crtbegin.o" OUTPUT_VARIABLE CRTBEGIN_OBJ)
string(REPLACE "\n" "" CRTBEGIN_OBJ ${CRTBEGIN_OBJ})
execute_process(COMMAND bash -c "${CMAKE_CXX_COMPILER} -print-file-name=crtend.o" OUTPUT_VARIABLE CRTEND_OBJ)
string(REPLACE "\n" "" CRTEND_OBJ ${CRTEND_OBJ})

set(CRTINIT_OBJ "./crt/crt0.o ./crt/crti.o ${CRTBEGIN_OBJ}")
set(CRTFINI_OBJ "${CRTEND_OBJ} ./crt/crtn.o")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> ${CMAKE_CXX_FLAGS} ${CMAKE_EXE_LINKER_FLAGS} ${CRTINIT_OBJ} <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${CRTFINI_OBJ}")

#NASM
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./nasm/nasm_src -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE NASM_SRC)
string(REPLACE "\n" ";" NASM_SRC ${NASM_SRC})
add_executable(nasm "${NASM_SRC}")
target_compile_options(nasm PRIVATE -O2)
target_link_options(nasm PRIVATE -e main)
target_include_directories(nasm PRIVATE ${PROJECT_SOURCE_DIR}/nasm/include)
target_link_libraries(nasm ${UPANLIB_OUT_DIR}/libc.so)

#NDISASM
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./nasm/ndisasm_src -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE NDISASM_SRC)
string(REPLACE "\n" ";" NDISASM_SRC ${NDISASM_SRC})
add_executable(ndisasm "${NDISASM_SRC}")
target_compile_options(ndisasm PRIVATE -O2)
target_link_options(ndisasm PRIVATE -e main)
target_include_directories(ndisasm PRIVATE ${PROJECT_SOURCE_DIR}/nasm/include)
target_link_libraries(ndisasm ${UPANLIB_OUT_DIR}/libc.so)

##SHELL
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./shell -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE MSH_SRC)
string(REPLACE "\n" ";" MSH_SRC ${MSH_SRC})
add_executable(msh "${MSH_SRC}")
add_dependencies(msh crt)
target_include_directories(msh PRIVATE ${PROJECT_SOURCE_DIR}/shell)
target_compile_options(msh PRIVATE -O2)
target_link_libraries(msh cxxrt)
target_link_libraries(msh c)
target_link_libraries(msh upanui)
target_link_libraries(msh ustd)
target_link_libraries(msh gcceh)

#EDITOR
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./editor -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE EDITOR_SRC)
string(REPLACE "\n" ";" EDITOR_SRC ${EDITOR_SRC})
add_executable(edit "${EDITOR_SRC}")
add_dependencies(edit crt)
target_include_directories(edit PRIVATE ${PROJECT_SOURCE_DIR}/editor)
target_compile_options(edit PRIVATE -O2)
target_link_libraries(edit cxxrt)
target_link_libraries(edit c)
target_link_libraries(edit mterm)
target_link_libraries(edit upanui)
target_link_libraries(edit ustd)
target_link_libraries(edit gcceh)
