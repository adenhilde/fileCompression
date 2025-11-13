/*-  Libraries  -*/
#include <stdio.h>
#include <stdlib.h>

/*-  Constants  -*/
#define WINDOW_SIZE 4096                                                /* Maximum size of the sliding window */
#define LOOKAHEAD 8                                                     /* Maximum number of bytes that will be matched */ 

/* LZ77 uses a 2 window technique, using a "past" window, and a "future" window. However I will be referring to the "future" window as the "present" window */

/*-  Functions  -*/
int findMatch(unsigned char window[], int windowStart, unsigned char *look, int lookSize, int *outLen);     /* This searches for repeated sequences */
void updateWindow(unsigned char window[], int *windowStart, int offset, int len, unsigned char next);       /* Updates the "window" to move it forward by 1 */
void compressLZ77(FILE *in, FILE *out);                                                                     /* The compressor */

/* Looks through the entire sliding window and finds the longest substring that matches the beginning of the lookahead buffer (present window) */
int findMatch(unsigned char window[], int windowStart, unsigned char *look, int lookSize, int *outLen) { 

    /* Stores the longest match found so far in the program */
    int bestLength = 0;  
    int bestOffset = 0;

    int maxLen;
    if(lookSize > 0) {
        maxLen = lookSize - 1;                                          /* This stores the next character (as is needed in LZ77) */
    } else {
        maxLen = 0;                                                     /* Incase it's the first character */
    }
    if(maxLen <= 0) {
        *outLen = 0;
        return 0;
    }

    int curr = windowStart % WINDOW_SIZE;                               /* Locates where in the "sliding window" we can write the next byte by computing the current write position inside the circular buffer (window[]) */

    int i = 0;                  
    int L = 0;  
    unsigned char w;                                
    for(i = 0; i < WINDOW_SIZE; i++) { 
        L = 0;
        while(L < maxLen) {                                             /* Compares window[i + L] with look[L], checking each window position trying to match it to look[L] comparing one byte at a time */
            w = window[(i+L) % WINDOW_SIZE];
            if(w != look[L]) {                                          /* Ends while loop once it finds a match */
                break;
            }
            L++;
        }
        if(L > bestLength) {                                            /* Tracks the "best match" found within the window */
            bestLength = L;                     
            bestOffset = (curr - i + WINDOW_SIZE) % WINDOW_SIZE;
            if(bestLength == maxLen) {                                  /* There's a cap for the size of a match to help with performance, this will end early if they reached the max length of a match */
                break;  
            }
        }
    }
    
    *outLen = bestLength;                                               /* Updates outLen (length) */
    return bestOffset;                                                  /* Returns the best Offset found at position in window */
}       

/* Writing into the past window */ 
void updateWindow(unsigned char window[], int *windowStart, int offset, int len, unsigned char next) {

    int writePosition = *windowStart;                                   /* Saves the next byte position in the "past" window */
    
    int i = 0;
    int src = 0;
    unsigned char c;
    for(i = 0; i < len; i++) {                                          /* src is checking the byte in the past window that we're trying to copy */ 
        src = (writePosition - offset + WINDOW_SIZE) % WINDOW_SIZE; 
        c = window[src];
        window[writePosition % WINDOW_SIZE] = c;                        /* Writes the byte at the current position */
        writePosition++;
    }

    window[writePosition % WINDOW_SIZE] = next;                         /* Sets current position to "next" then moves forward by another position in the next line*/
    writePosition++;

    *windowStart = writePosition % WINDOW_SIZE;                         /* Moves the sliding window forward by len(length) + 1 bytes */
}


void compressLZ77(FILE *in, FILE *out) {
    unsigned char window[WINDOW_SIZE] = {0};                            /* This array is the past window */ 
    unsigned char look[LOOKAHEAD + 1];                                  /* This arrow is the present window */
    int windowStart = 0;                                                /* Where the next byte goes in window[] (past window) */ 
    int lookSize = 0;                                                   /* How many bytes are in look[] (present window) */
    int len;                                                            /* Tracks how many characters are matched */
    int offset;                                                         /* How far back the match starts */
    unsigned char next;                                                 /* The next character after a match has been found */
    int ch;
    int adv;
    int i = 0;
    
    while(lookSize < (LOOKAHEAD + 1) && (ch = fgetc(in)) != EOF) {      /* Fills look with the first few byte */
        look[lookSize] = (unsigned char)ch;                             /* Casting is necessary to set the variable equal, while still keeping ch as an integer so the while loop check still works */
        lookSize++;
    }

    while(lookSize > 0) {                                               /* Main compression loop */
        len = 0;                                                        /* Resets length to 0 after each loop */
        offset = findMatch(window, windowStart, look, lookSize, &len);  /* Calls the findMatch function, which finds the longest match between the past and present windows */
        
        next = look[len];                                               /* Finds the next byte after the match so it can return to that byte once the previous sequence has been mentioned */

        /* Writing the LZ77Tuple */                                     /* Low byte and High Byte is necessary for fputc as fputc can only handle up to 255 without the use of high & low bytes */
        fputc((offset >> 8) & 255, out);                                /* The High Byte - Shifts it to the right by 8 bits */
        fputc(offset & 255, out);                                       /* The Low Byte */ 
        fputc(len & 255, out);                                          /* Copies the length to the out file */
        fputc(next, out);                                               /* Copies the literal "next" to the out file */
        
        updateWindow(window, &windowStart, offset, len, next);          /* Calls the updateWindow function to update the past window so it can be referred to by the present window*/ 

        /* In simple terms this just removes the encoded byte from the lookahead buffer so it doesn't create an infinite loop */
        adv = len+1;
        if(adv > lookSize) { 
            adv = lookSize;
        }
        for(i = 0; i + adv < lookSize; i++) {
            look[i] = look[i+adv];
        }
        lookSize -= adv;
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        while(lookSize < (LOOKAHEAD +1) && (ch = fgetc(in)) != EOF) {   /* Fills up the present window so it doesn't remain empty as we emptied the window in the previous step */
            look[lookSize++] = (unsigned char)ch;
        }
    }
}

void decompressLZ77(FILE *in, FILE *out) {
    unsigned char window[WINDOW_SIZE] = {0};
    int writePosition = 0;
    int i = 0;
    int b0;
    int b1;
    int len;
    int next;
    
    while ((b0 = fgetc(in)) != EOF) {
        b1   = fgetc(in);                                           
        if (b1 == EOF) {
            break;
        }

        len  = fgetc(in); 
        if (len == EOF) {
            break;
        }

        next = fgetc(in); 
        if (next == EOF) {
            break;
        }

        int offset = ((b0 & 255) << 8) | (b1 & 255);
        int src = 0;
        unsigned char c;

        for (i = 0; i < len; i++) {
            src = (writePosition - offset + WINDOW_SIZE) % WINDOW_SIZE;
            c = window[src];
            fputc(c, out);
            window[writePosition % WINDOW_SIZE] = c;
            writePosition++;
        }

        fputc(next, out);
        window[writePosition % WINDOW_SIZE] = (unsigned char)next;  
        writePosition++;
    }
}
