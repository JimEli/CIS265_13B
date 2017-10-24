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
*
* Submitted in partial fulfillment of the requirements of PCC CIS-265.
*************************************************************************
* Change Log:
*   09/10/2017: Initial release. JME
*   10/19/2017: Added inline assembly code to trim function. JME
*   10/20/2017: Added commandline retreval of text file name. JME
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

// Trim all but digits from string.
char *trim(char *s) {
	char *d;

	// Save start of string for function return and set d=s.
#ifdef _MSC_VER
	__asm {
		mov eax, s // Get address of string.
		mov d, eax // d = s.
		push eax   // Save start of string for function return.
	}
#elif __GNUC__
	asm volatile (
		"mov %1, %0 \n" // Set d = s.
		"push %1"       // Save start of string for function return.
		: "=r" (d) : "r" (s)
		);
#endif

	while (*s != '\0') {
		if (isdigit(*s))
			*d++ = *s;
		s++;
	}
	*d = '\0'; // Terminate string.

#ifdef _MSC_VER
	__asm { pop eax } // Set beginning of string as return value.
#elif __GNUC__
	asm(
		"pop %eax \n" // Retrieve beginning of string.
		"leave    \n" // Exit function.
		"ret"
	);
	return NULL;     // Never reach here, but required otherwise GCC squawks of no return value.
#endif
}

// Program starts here.
int main(int argc, char *argv[]) {
	FILE* stream;
	char line[80];
	char fileName[80] = "numbers.txt"; // Default text file.

	// Required to make eclipse console output work properly.
	setvbuf(stdout, NULL, _IONBF, 0);
	fflush(stdout);

	// Check command line arguments 
	if (argc == 2) {
		// Open the commandline file name. 
		strcpy(fileName, argv[1]);
	}

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

