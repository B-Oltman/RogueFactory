//Custom library for all sorts of rogue pirate string manipulation.

#ifndef RP_STRINGS_RPH
#define RP_STRINGS_RPH

/********String Length Constants*******/
#define RP_MAX_LABEL_LENGTH 50
#define RP_MAX_MENU_NAME_LENGTH 100
#define RP_MAX_FILE_NAME_LENGTH 100

// Compare strings and return boolean if they are equal or not
int RpStrCompare(char *string1, char *string2);

// Copy source string into destination, destination must be large enough to hold both
void RpStrCopy(char *destination, char *source);

// Return length of string, not including null terminator
int RpStrLength(char *source);

// Appends source to the end of destination, destination must be large enough to hold both
char *RpStrAppend(char *destination, char *source);

// Convert an integer to a string version of that integer
void RpIntToString(int number, char *destinationBuf);

#endif

