// StegoProject.c : This file contains the 'main' function. Program execution begins and ends there.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// check if using windows
// lines 9-15 found here: https://stackoverflow.com/questions/37437082/which-is-a-good-practice-to-use-access-function
#ifdef _WIN32
inline int access(const char* pathname, int mode) {
	return _access(pathname, mode);
}
#else
#include <unistd.h>
#endif
// read, write, and file_exists which works natively for linux, but not windows
#define R_OK 4
#define W_OK 2
#define F_OK 0
// Error codes for programs
/*
* Error 1: incorrect number of parameters
*
*/
#define DEBUGGING 0
// constants for program
#define HIDE "-hide" // error 5
#define EXTRACT "-extract" // error 6
#define MSG_FLAG "-m" // error 7
#define OPTIONAL_FLAG "-o" // error 8
#define COVER_FLAG "-c" // error 9
#define STEGO_FLAG "-s" // error 10
#define BITS "-b" // bits to hide, or bits to extract
#define READ_FILE 0
#define WRITE_FILE 1
#define SUCCESSFUL 0
#define GENERAL_ERROR 1
#define INCORRECT_NUM_PARAMETERS 2
#define PARAMETERS_PROVIDED_INCORRECT_ERROR 3
#define HIDE_ERROR 5
#define EXTRACT_ERROR 6
#define MSG_ERROR 7
#define OPTIONAL_ERROR 8
#define COVER_ERROR 9
#define STEGO_ERROR 10
#define FILE_ACCESS_ERROR 11
#define ACCESS_DENIED 12
#define DEFAULT_HIDE_OUTPUT_FILE "output_stego"
#define DEFAULT_EXTRACT_OUTPUT_FILE "output_message"

//functions needed:
int checkParams(const int arguments, const char* list[], int* selection, int* optional);
// readOrWrite is 0 for read and 1 for write
// READ for extraction, and WRITE for hiding
int fileAccessCheck(char * filename, FILE* fp, int readOrWrite);
int hideData();
int extractData();
// for testing only

#if DEBUGGING == 1
	char* mf = "messagefile.txt";
	char* cf = "coverfile.txt";
	char* sf = "stegofile.txt";
	char* of = "outputfile.txt";
#endif

int main(int argc, char *argv[])
{
	// 0 for -hide and 1 for -extract
	int selection = 0;
	// files needed for manipulation
	FILE* inputFile = NULL;
	FILE* coverFile = NULL;
	FILE* stegoFile = NULL;
	FILE* outputFile = NULL;
	
	char* mf = NULL; // "messagefile";
	char* cf = NULL; // "coverfile";
	char* sf = NULL; // "stegofile";
	char* of = NULL; // "outputfile";

	int optional = 0;
	// making sure command line arguments are correct
	// will tell us if we are doing hiding or extraction as well as if there is an optional cmd line
	argument
	int result = checkParams(argc, argv, &selection, &optional);
	if (result)
	{
		fprintf(stderr, "Closing program. [Error %d]", result);
		return result;
	}

	// will only use the ones necessary for the HIDE/EXTRACT
	//int iresult = fileAccessCheck(mf, inputFile, READ_FILE);
	int iresult = 0;
	//int cresult = fileAccessCheck(cf, coverFile, READ_FILE);
	int cresult = 0;
	//int sresult = fileAccessCheck(sf, stegoFile, READ_FILE);
	int sresult = 0;
	//int oresult = fileAccessCheck(of, outputFile, WRITE_FILE);
	int oresult = 0;
	//// 0 means the
	//fprintf(stdout, "success=0 - input: %d - cover: %d - stego: %d - output: %d\n", iresult, cresult,
	sresult, oresult);

	if (result)
	{
		fprintf(stderr, "One or more of the files are not accessible. [Error %d]", result);
		return result;
	}
	// we can set values to command line parameters because we've done the necessary checks already
	// then we'll check to make sure the necessary files exist/can be created
	// use selection to know if we are hiding data or extracting data
	if (!selection)
	{
		mf = argv[3];
		// option to hide
		iresult = fileAccessCheck(mf, inputFile, READ_FILE);
		if (iresult)
		{
			return iresult;
		}
		
		cf = argv[5];
		cresult = fileAccessCheck(cf, coverFile, READ_FILE);
		
		if (cresult)
		{
			return cresult;
		}
		
		if (!optional)
		{
			of = DEFAULT_HIDE_OUTPUT_FILE;
		}
		else
		{
			of = argv[7];
		}
		
		oresult = fileAccessCheck(of, outputFile, WRITE_FILE);
		
		if (oresult)
		{
			return oresult;
		}
		
		if (DEBUGGING)
			fprintf(stdout, "result: i: %s - c: %s - o: %s\n", mf, cf, of);
	}
	else
	{
		sf = argv[3];
		// option to extract
		sresult = fileAccessCheck(sf, stegoFile, READ_FILE);
		if (sresult)
		{
			return sresult;
		}
		if (!optional)
		{
			of = DEFAULT_EXTRACT_OUTPUT_FILE;
		}
		else
		{
			of = argv[5];
		}

		oresult = fileAccessCheck(of, outputFile, WRITE_FILE);
	
		if (oresult)
		{
			return oresult;
		}
		if (DEBUGGING)
			fprintf(stdout, "result: i: %s - o: %s\n", sf, of);
	}
		
	return 0;
}

int checkParams(const int arguments, const char* list[], int* selection, int* optional)
{
	// test command line parameters, exit smoothly if not correct
	//
	// test parameters, make sure correct amount and correct values (error checking)
	/*
	* Will check values in the command line parameters
	* HIDE
	* required flags:
	* -m message_file
	* -c cover_file
	* optional flag:
	* -o output_file
	* this will run successfully
	* EX: user runs program as follows:
	* stego.exe -hide -m messageFile -c coverFile -o resultingFile
	* successful
	* EX: user runs program as follows:
	* stego.exe -hide -m messageFile -c coverFile
	* successful, use default file path for outputting data
	* EXTRACT
	* required flag:
	* -s stego_file
	* optional -o message_file
	* EX: user runs program as follows:
	* stego.exe -extract -s stegoFile -o outputfile
	* successful
	* EX: user runs program as follows:
	* stego.exe -extract -s stegofile
	* successful
	*/
	// variable to check which have been checked
	// confirm the argument number is correct respective to being a hider or extractor
	if (arguments != 4 && arguments != 6 && arguments != 8)
	{
	// incorrect number of parameters
	return INCORRECT_NUM_PARAMETERS;
	}
	
	if ((arguments == 6 || arguments == 8) && strncmp(list[1], HIDE, strlen(HIDE)) == 0)
	{
		*selection = 0;
		// test required arguments
		if (strncmp(list[2], MSG_FLAG, strlen(MSG_FLAG)) != 0)
		{
			// missing message flag, must fail program
			return MSG_ERROR;
		}
		
		if (strncmp(list[4], COVER_FLAG, strlen(COVER_FLAG)) != 0)
		{
			// missing cover file flag, must fail program
			return COVER_ERROR;
		}
		
		// test optional arguments
		if (arguments == 8)
		{
			if (strncmp(list[6], OPTIONAL_FLAG, strlen(OPTIONAL_FLAG)) != 0)
			{
				// optional flag is provided incorrectly, must fail program
				return OPTIONAL_ERROR;
			}
			// let us know we did have an optional parameter successfully entered
			*optional = 1;
		}
		// return 0 as successful
		return SUCCESSFUL;
	}
	else if ((arguments == 4 || arguments == 6) && strncmp(list[1], EXTRACT, strlen(EXTRACT)) == 0)
	{
		*selection = 1;
	
		// test required arguments
		if (strncmp(list[2], STEGO_FLAG, strlen(STEGO_FLAG)) != 0)
		{
		// missing stego flag, must fail program
		return STEGO_ERROR;
		}
	
		// test optional arguments
		if (arguments == 6)
		{
			if (strncmp(list[4], OPTIONAL_FLAG, strlen(OPTIONAL_FLAG)) != 0)
			{
				// optional flag is provided incorrectly, must fail program
				return OPTIONAL_ERROR;
			}
	
		// let us know we did have an optional parameter successfully entered
		*optional = 1;
		}
	
		// return 0 as successful
		return SUCCESSFUL;
	}
	else {
		// parameters not provided correctly
		return PARAMETERS_PROVIDED_INCORRECT_ERROR;
	}
	return GENERAL_ERROR;
}

// returns 0 if successful, otherwise returns error number
int fileAccessCheck(char* filename, FILE* fp, int readOrWrite)
{
	char response[256];
	// only want to ask if we are writing to file so we do not overwrite it if undesired, if we are
	reading then no need to ask
	if (access(filename, F_OK) == 0 && readOrWrite)
	{
		fprintf(stdout, "File already exists. Would you like to proceed? (y/n) ");
		if (fgets(response, sizeof(response), stdin))
		{
			if (strncmp(response, "y", strlen("y")) != 0)
			{
				fprintf(stdout, "You did not enter \"y\" therefore \"n\" is assumed for safety and we will need to exit the program.\n");
				return ACCESS_DENIED;
			}
		}
	}
	
	// 0 for read, 1 for write
	// fopen_s for safety features
	readOrWrite ? (fopen_s(&fp, filename, "wb")) : (fopen_s(&fp, filename, "rb"));
	
	if (fp == NULL)
	{
		// File could not be opened, return error to main
		fprintf(stdout, "File [%s] could not be opened.\nProgram will exit.\n", filename);
		return FILE_ACCESS_ERROR;
	}
	else
	{
		fprintf(stdout, "read (0) or write (1) for file %s: %d\n\n", filename, readOrWrite);
	}
	return 0;
}

int hideData()
{
	/*
	Confirm files are of proper 24-bit bitmap format before beginning to hide data
	Validate file header, check first 2 bytes for “BM”, check size, check data start
	Save file size information
	Jump to data as per the data offset field
	Run through data portion byte per byte and input into LSB (3 bits per entry, each
	entry has Red byte, Green byte, Blue byte)
	Run through file and input byte-by-byte and encode data into LSB until EOF
	NOTE: we will only input bits until we run out of room, unless otherwise specified
	Once completed, the original file will be unmodified and the new file will have the
	same header and palette entries, but the data section modified modified with LSB
	steganography, only if given permission to overwrite the file if it already existed
	*/
	return 0;
}
int extractData()
{
	/*
	Extract:
	Confirm file(s) are of proper format
	As of now, we are only encoding with run length encoding for LSB, 1 bit, so we
	will default to extracting the message in the same manner
	Reading header information
	Save file size information
	Jump to data as per the data offset field
	Read through file byte-per-byte and save the LSB into the provided message file
	(or default message file if none was provided)
	Once we reach the end of the file, close out of both files and output success to
	user
	Once completed, the original file will be unmodified and a new message will will
	have been created (or overwritten if given permission to overwrite an existing file) with
	the extracted message
	*/
	return 0;
}