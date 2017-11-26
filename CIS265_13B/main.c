/*************************************************************************
* Title: (CIS265_13B) Phone Number Formatter
* File: main.c
* Author: James Eli
* Date: 9/10/2017
*
* Write a program that reads a series of phone numbers from a file and
* displays them in a standard format. Each line of the file will contain
* a single phone number, but the numbers may be in a variety of formats.
* You may assume that each line contains 10 digits, possibly mixed with
* other characters (which should be ignored). For example, suppose that
* the file contains the following lines:
*   404.817.6900
*   (215) 686-1776
*   312-746-6000
*   877 275 5273
*   6173434200
* The output of the program should have the following appearance:
*   (404) 817-6900
*   (215) 686-1776
*   (312) 746-6000
*   (877) 275-5273
*   (617) 343-4200
* Have the program obtain the file name from the command line.
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141), using C
*      language options.
*  (2) Conditional compilation information from here:
*      http://nadeausoftware.com/articles/2012/02/
*      c_c_tip_how_detect_processor_type_using_compiler_predefined_macros
*
* Submitted in partial fulfillment of the requirements of PCC CIS-265.
*************************************************************************
* Change Log:
*   09/10/2017: Initial release. JME
*   10/19/2017: Added inline assembly code to trim function. JME
*   10/20/2017: Added commadline retreval of text file name. JME
*   11/19/2017: Correct conditional compile macros. JME
*************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
// C/C++ Preprocessor Definitions: _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#endif

// Three separate versions of same function differ due to x86 inline assembly syntax.
#if defined (_MSC_VER) && (defined(__i386) || defined(_M_IX86))

char *trim(char *s) {
	__asm {
		push  ebx       // Preserve register.
		mov   eax, s    // eax = source.
		mov   ebx, eax  // ebx = destination.
		push  eax       // Save start of string for function return.

	start_:
		movzx cl, byte ptr[eax]
		test  ecx, ecx  // End of source?
		je    exit_
		// Source character a digit?
		cmp   ecx, 0x30 // '0'
		jl    notDigit_
		cmp   ecx, 0x39 // '9'
		jg    notDigit_
		// Copy source digit to destination.
		mov   byte ptr[ebx], cl
		add   ebx, 1    // Increment destination pointer.

	notDigit_: 
		add   eax, 1    // Increment source pointer.
		jmp   start_

	exit_:
		mov   byte ptr[ebx], 0 // add trailing zero.
		pop   eax       // Return start of string.
		pop   ebx       // Reset register.
	}
}

#elif __GNUC__ && defined(_X86_)
/*
__asm__ (
	"_trim:                 \n"
		"push %ebp          \n" // Set up stack frame.
		"mov  %esp, %ebp    \n"

		"mov  8(%ebp), %eax \n" // eax = source.
		"mov  %eax, %ebx    \n" // ebx = destination.
		"push %eax          \n" // save for return.

	"loop:                  \n"
		"movzx (%eax), %ecx \n" // get source character.
		"test %ecx, %ecx    \n" // test for NULL terminator.
		"je   exit          \n" // end found.

		"cmp  $'0', %ecx    \n" // >= '0'
		"jl   notDigit      \n"
		"cmp  $'9', %ecx    \n" // <= '9'
		"jg   notDigit      \n"

		"movb  %cl, (%ebx)  \n"
		"add  $1, %ebx       \n" // increment destination pointer.

	"notDigit:              \n"
		"add  $1, %eax       \n" // increment source pointer.
		"jmp  loop          \n"

	"exit:                  \n"
		"movb $0, (%ebx)  \n"

		"pop  %eax          \n" // return start of string.
		"pop  %ebp          \n" // clean up and return.
		"ret                \n"
);

char *trim();
*/

void _trim(char *s) {
	char *d = NULL;

	__asm__ __volatile__ (
			"movl  %1, %0         \n" // Set destination == source.
			"push  %1             \n" // Save start of string for return.

		"loop:                    \n"
			"movzx (%1), %%ecx    \n" // Get next source character.
			"test  %%ecx, %%ecx   \n" // Test for string end.
			"je    exit           \n"

			"cmp   $'0', %%ecx    \n" // >= '0'
			"jl    notDigit       \n"
			"cmp   $'9', %%ecx    \n" // <= '9'
			"jg    notDigit       \n"

			"movb  %%cl, (%0)     \n" // Save character to destination.
			"add   $1, %0         \n" // Increment destination pointer.

		"notDigit:                \n"
			"add   $1, %1         \n" // Increment source pointer.
			"jmp   loop           \n"

		"exit:                    \n"
			"movb  $0, (%0)       \n" // Terminate destination string.
			"pop   %%eax          \n" // Return start of string.
			:  : "r" (d), "r" (s) : "memory"
	);
}

// Define weak alias to prevent gcc from squawking about no return value.
char *trim(char *) __attribute__((weak, alias("_trim")));

#else

// Default version of trim().
char *trim(char *s) {
	char *d = s, *_s = s;

	while (*s != '\0') {
		if (isdigit(*s))
			*d++ = *s;
		s++;
	}
	*d = '\0'; // Terminate string.
	return _s;
}

#endif

// Program starts here.
int main(int argc, char *argv[]) {
	FILE* stream;
	char line[80];
	char fileName[80] = "numbers.txt"; // Default text file.

	// Required to make eclipse console output work properly.
	setvbuf(stdout, NULL, _IONBF, 0);
	fflush(stdout);

	// Check command line arguments
	if (argc == 2)
		// Open the commandline file name.
		strcpy(fileName, argv[1]);

	// Attempt to open default filename.
	if ((stream = fopen(fileName, "r")) == NULL)
		fprintf(stderr, "File: \"%s\" could not be opened.\n", fileName);
	else {
		while (fgets(line, sizeof line, stream) != NULL)
			if (strlen(trim(line)) == 10)
				fprintf(stdout, "(%.3s) %.3s-%.4s\n", line, line + 3, line + 6);
			else
				fputs(line, stdout);
		fclose(stream);
	}
}
