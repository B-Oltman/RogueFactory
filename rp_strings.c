#include "rp_strings.h"

// Returns 0 if strings are not the same, or 1 if they are.
int RpStrCompare(char *string1, char *string2)
{
    
    while(*string1 != '\0' || *string2 != '\0')
    {
        if(*string1 != *string2)
        {
            return 0;
        }
        string1++;
        string2++;
    }
    
    return 1;
}

// Copy source to destination. Destination must be large enough to hold source
void RpStrCopy(char *destination, char *source)
{
    while(*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    
    // Add null terminator after last character of new string
    *destination = '\0';
}

// Return length of string, excludes null terminator
int RpStrLength(char *source)
{
    int count = 0;
    while(*source++ != '\0')
    {
        ++count;
    }
    
    return count;
}

// Appends source onto end of destination. Destination must be long enough to hold the entire concatenated string.
char *RpStrAppend(char *destination, char *source)
{
    int destLength = RpStrLength(destination);
    
    for(int i = destLength, j = 0; source[j] != '\0'; ++i, ++j)
    {
        destination[i] = source[j];
    }
    
    return destination;
}

// Converts integer to string
void RpIntToString(int number, char *destinationBuf)
{
    int offsetForNeg = 0;
    
    if(number < 0)
    {
        // If number is negative offset buffer by 1 and put a '-' in the first
        //index position. This way we can display negative integers.
        number *= -1;
        offsetForNeg = 1;
    }
    // Calculate length of number
    int length = 1;
    int val = number;
    while(val > 9)
    {
        length++;
        val /= 10;
    }
    
    // Calculate highest exponent
    int exp = 1;
    for(int i = 1; i < length; ++i)
    {
        exp *= 10;
    }
    
    char digit = 0;
    char strBuf[length];
    // Break number into digits and convert to string
    for(int i = 0; i < length; ++i)
    {
        digit = number / exp;
        number %= exp;
        exp /= 10;
        
        strBuf[i + offsetForNeg] = ('0' + digit);
    }
    
    // If number is negative, set first character to '-'
    if(offsetForNeg)
    {
        strBuf[0] = '-';
    }
    
    // Set last character to '/0'
    strBuf[length + offsetForNeg] = '\0';
    
    RpStrCopy(destinationBuf, strBuf);
}

