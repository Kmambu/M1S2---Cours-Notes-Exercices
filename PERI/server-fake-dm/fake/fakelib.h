#ifndef _FAKELIB_H
#define _FAKELIB_H

/*
 * handleRequest : request handling, returns -2 if incorrect request
 * returns the value of called routine otherwise
 * */
int handleRequest(char *buf, char *ret);

/* 
 * writeToLED : writes a value to a LED
 * return -1 if error, 0 otherwise
 * */
int writeToLED(char *buf);

int readFromBP(char *buf, char *ret);
#endif
