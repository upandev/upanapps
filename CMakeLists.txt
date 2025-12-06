#	 Upanix - An x86 based Operating System
#	 Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
#
#  I am making my contributions/submissions to this project solely in
#  my personal capacity and am not conveying any rights to any
#  intellectual property of any third parties.
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

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CROSS_TOOL_PATH $ENV{HOME}/opt/cross)

set(UPANLIB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../upanlibs)
set(UPANLIB_OUT_DIR ${UPANLIB_SOURCE_DIR}/out)

set(UPANEXLIB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../upanexlibs)
set(UPANEXLIB_OUT_DIR ${UPANEXLIB_SOURCE_DIR}/out)

set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(CMAKE_CXX_COMPILER ${CROSS_TOOL_PATH}/bin/x86_64-elf-g++)
set(CMAKE_C_COMPILER ${CROSS_TOOL_PATH}/bin/x86_64-elf-gcc)
set(CMAKE_LINKER ${CROSS_TOOL_PATH}/bin/x86_64-elf-ld)
set(CMAKE_AR ${CROSS_TOOL_PATH}/bin/x86_64-elf-ar)

set(ENABLE_CXX_RT ON)
	
set(EXCEPTION_SUP " -fno-exceptions -fno-rtti")
if(ENABLE_CXX_RT)
	set(EXCEPTION_SUP " -fexceptions -frtti")
endif()

set(SO_COMPILE_FLAGS -shared -fPIC)
set(SO_LD_FLAGS " -Wl,-shared -Wl,-fPIC")

set(CMAKE_CXX_FLAGS " -O0 -Wall -ffreestanding -nodefaultlibs -nostdlib -nostartfiles -nostdinc -std=c++11 -nostdinc++ -fno-threadsafe-statics -fpermissive ${EXCEPTION_SUP}")
set(COMMON_COMPILE_FLAGS " -std=c11 -O0 -Wall -ffreestanding -nodefaultlibs -nostdlib -nostartfiles -nostdinc -D__GCC__")
set(CMAKE_C_FLAGS ${COMMON_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS " -T${PROJECT_SOURCE_DIR}/upanapps.ld -O0 -nostdlib")
set(CMAKE_SHARED_LINKER_FLAGS " -O0 -nostdlib ${SO_LD_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${UPANLIB_SOURCE_DIR}/libc/include)
include_directories(${UPANLIB_SOURCE_DIR}/libc/stdio)
include_directories(${UPANLIB_SOURCE_DIR}/libc/sysdeps/mos/common/)
include_directories(${UPANLIB_SOURCE_DIR}/libc/sysdeps/mos/common/bits)

include_directories(${UPANLIB_SOURCE_DIR}/libupthread/include)

include_directories(${UPANLIB_SOURCE_DIR}/libustd/infra)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/ds)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/algo)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/mem)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/mthread)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/stream)
include_directories(${UPANLIB_SOURCE_DIR}/libustd/net)

include_directories(${UPANLIB_SOURCE_DIR}/libm/include)
include_directories(${UPANLIB_SOURCE_DIR}/libm/include/bits)

include_directories(${PROJECT_SOURCE_DIR}/libmcal/include)

include_directories(${PROJECT_SOURCE_DIR}/libmterm/include)

include_directories(${UPANLIB_SOURCE_DIR}/libcxxrt)

include_directories(${UPANLIB_SOURCE_DIR}/libupanui)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/frame)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/image)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uiobject)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uiobject/core)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uiobject/core/layout)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uiobject/canvas)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uicomponent)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uicomponent/button)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uicomponent/menu)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uicomponent/scroller)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/uicomponent/console)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/events)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/default)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/usfn)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/text/textarea)
include_directories(${UPANLIB_SOURCE_DIR}/libupanui/interop/includes)

include_directories(${UPANEXLIB_SOURCE_DIR}/libupanssl)
include_directories(${UPANEXLIB_SOURCE_DIR}/libupanssl/include)
include_directories(${UPANEXLIB_SOURCE_DIR}/libupanssl/providers/implementations/include)
include_directories(${UPANEXLIB_SOURCE_DIR}/libupanssl/providers/common/include)
include_directories(${UPANEXLIB_SOURCE_DIR}/libupanssl/providers/fips/include)
include_directories(${UPANEXLIB_SOURCE_DIR}/libupanssl/upanssl/include)

link_directories(${UPANLIB_OUT_DIR})
link_directories(${UPANEXLIB_OUT_DIR})
link_directories(${OUTPUT_DIR})

#by default CMAKE first finds shared and then static
find_library(LIBUPANUI upanui PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBUSTD ustd PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBGCCUCORE gccucore PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBM m PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBC c PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBUPTHREAD upthread PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBCXXRT cxxrt PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBGCCEH gcceh PATHS "${UPANLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBSSL ssl PATHS "${UPANEXLIB_OUT_DIR}" NO_DEFAULT_PATH)
find_library(LIBCRYPTO crypto PATH "${UPANEXLIB_OUT_DIR}" NO_DEFAULT_PATH)

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
target_link_libraries(mterm "${LIBCXXRT}")
target_compile_options(mterm PRIVATE ${SO_COMPILE_FLAGS})

add_custom_target(crt
  COMMAND make WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/crt
  BYPRODUCTS ${PROJECT_SOURCE_DIR}/crt/crt0.o ${PROJECT_SOURCE_DIR}/crt/crti.o ${PROJECT_SOURCE_DIR}/crt/crtn.o)

execute_process(COMMAND bash -c "${CMAKE_CXX_COMPILER} -print-file-name=crtbegin.o" OUTPUT_VARIABLE CRTBEGIN_OBJ)
string(REPLACE "\n" "" CRTBEGIN_OBJ ${CRTBEGIN_OBJ})
execute_process(COMMAND bash -c "${CMAKE_CXX_COMPILER} -print-file-name=crtend.o" OUTPUT_VARIABLE CRTEND_OBJ)
string(REPLACE "\n" "" CRTEND_OBJ ${CRTEND_OBJ})

set(CRTINIT_OBJ "${PROJECT_SOURCE_DIR}/crt/crt0.o ${PROJECT_SOURCE_DIR}/crt/crti.o ${CRTBEGIN_OBJ}")
set(CRTFINI_OBJ "${CRTEND_OBJ} ${PROJECT_SOURCE_DIR}/crt/crtn.o")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> ${CMAKE_CXX_FLAGS} ${CMAKE_EXE_LINKER_FLAGS} ${CRTINIT_OBJ} <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${CRTFINI_OBJ}")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> ${CMAKE_C_FLAGS} ${CMAKE_EXE_LINKER_FLAGS} ${CRTINIT_OBJ} <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${CRTFINI_OBJ}")

#NASM
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./nasm/nasm_src -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE NASM_SRC)
string(REPLACE "\n" ";" NASM_SRC ${NASM_SRC})
add_executable(nasm "${NASM_SRC}")
target_compile_options(nasm PRIVATE -O2)
target_link_options(nasm PRIVATE -e main)
target_include_directories(nasm PRIVATE ${PROJECT_SOURCE_DIR}/nasm/include)
target_link_libraries(nasm "${LIBC}")
target_link_libraries(nasm "${LIBGCCUCORE}")
target_link_libraries(nasm "${LIBUPTHREAD}")
target_link_libraries(nasm "${LIBUSTD}")
target_link_libraries(nasm "${LIBCXXRT}")
target_link_libraries(nasm "${LIBGCCEH}")

#NDISASM
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./nasm/ndisasm_src -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE NDISASM_SRC)
string(REPLACE "\n" ";" NDISASM_SRC ${NDISASM_SRC})
add_executable(ndisasm "${NDISASM_SRC}")
target_compile_options(ndisasm PRIVATE -O2)
target_link_options(ndisasm PRIVATE -e main)
target_include_directories(ndisasm PRIVATE ${PROJECT_SOURCE_DIR}/nasm/include)
target_link_libraries(ndisasm "${LIBC}")
target_link_libraries(ndisasm "${LIBGCCUCORE}")
target_link_libraries(ndisasm "${LIBUPTHREAD}")
target_link_libraries(ndisasm "${LIBUSTD}")
target_link_libraries(ndisasm "${LIBCXXRT}")
target_link_libraries(ndisasm "${LIBGCCEH}")

##SHELL
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./shell -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE MSH_SRC)
string(REPLACE "\n" ";" MSH_SRC ${MSH_SRC})
add_executable(msh "${MSH_SRC}")
add_dependencies(msh crt)
target_include_directories(msh PRIVATE ${PROJECT_SOURCE_DIR}/shell)
target_compile_options(msh PRIVATE -O2)
target_link_libraries(msh "${LIBC}")
target_link_libraries(msh "${LIBUPANUI}")
target_link_libraries(msh "${LIBGCCUCORE}")
target_link_libraries(msh "${LIBM}")
target_link_libraries(msh mcal)
target_link_libraries(msh "${LIBSSL}")
target_link_libraries(msh "${LIBCRYPTO}")
target_link_libraries(msh "${LIBUPTHREAD}")
target_link_libraries(msh "${LIBUSTD}")
target_link_libraries(msh "${LIBCXXRT}")
target_link_libraries(msh "${LIBGCCEH}")

#EDITOR
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./editor -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE EDITOR_SRC)
string(REPLACE "\n" ";" EDITOR_SRC ${EDITOR_SRC})
add_executable(edit "${EDITOR_SRC}")
add_dependencies(edit crt)
target_include_directories(edit PRIVATE ${PROJECT_SOURCE_DIR}/editor)
target_compile_options(edit PRIVATE -O2)
target_link_libraries(edit "${LIBC}")
target_link_libraries(edit mterm)
target_link_libraries(edit "${LIBUPANUI}")
target_link_libraries(edit "${LIBGCCUCORE}")
target_link_libraries(edit "${LIBM}")
target_link_libraries(edit "${LIBUPTHREAD}")
target_link_libraries(edit "${LIBUSTD}")
target_link_libraries(edit "${LIBCXXRT}")
target_link_libraries(edit "${LIBGCCEH}")

#CLOCK
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./clock -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE CLOCK_SRC)
string(REPLACE "\n" ";" CLOCK_SRC ${CLOCK_SRC})
add_executable(clock "${CLOCK_SRC}")
add_dependencies(clock crt)
target_include_directories(clock PRIVATE ${PROJECT_SOURCE_DIR}/clock)
target_compile_options(clock PRIVATE -O2)
target_link_libraries(clock "${LIBC}")
target_link_libraries(clock "${LIBM}")
target_link_libraries(clock "${LIBUPANUI}")
target_link_libraries(clock "${LIBGCCUCORE}")
target_link_libraries(clock "${LIBUPTHREAD}")
target_link_libraries(clock "${LIBUSTD}")
target_link_libraries(clock "${LIBCXXRT}")
target_link_libraries(clock "${LIBGCCEH}")

#DEMO C APP
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./democapp -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE DEMO_C_APP_SRC)
string(REPLACE "\n" ";" DEMO_C_APP_SRC ${DEMO_C_APP_SRC})
add_executable(democ "${DEMO_C_APP_SRC}")
add_dependencies(democ crt)
target_compile_options(democ PRIVATE -O2)
#target_link_options(democ PRIVATE -e main)
target_link_libraries(democ "${LIBC}")
target_link_libraries(democ "${LIBGCCUCORE}")
target_link_libraries(democ "${LIBUPTHREAD}")
target_link_libraries(democ "${LIBUSTD}")
target_link_libraries(democ "${LIBCXXRT}")
target_link_libraries(democ "${LIBGCCEH}")
