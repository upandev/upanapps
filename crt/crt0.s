//	Upanix - An x86 based Operating System
//  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
//
//  I am making my contributions/submissions to this project solely in
//  my personal capacity and am not conveying any rights to any
//  intellectual property of any third parties.
//	                                                                        
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//	                                                                        
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//	                                                                        
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/
/* x86 crt0.s */
.code64
.section .text

.global _start
_start:
	mov %rsp, %rbp

	push %rdi
	push %rsi

    # 1. Relocate cross module references such as TLS variables
    # 2. Call dll _init that initializes global objects defined in the dll
    # 3. Call _init of the main executable that initialized global objects defined in the main executable
    call _process_init_relocate

	# Prepare signals, memory allocation, stdio and such.
	call _initialize_standard_library
	
	pop %rsi
	pop %rdi
	# Run main
	call main

    # global destructos of the executable and all the shared libraries will be invoked as part of exit() - defined in libc

	# Terminate the process with the exit code.
	mov %rax, %rdi
	call exit
.size _start, . - _start
