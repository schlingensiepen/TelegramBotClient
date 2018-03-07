#pragma once
#ifndef TBDebug_h
#define TBDebug_h

//define TBCVERBOSE

#ifdef TBCVERBOSE
 #define DOUT(x)        { Serial.print (F(x)); \
                          Serial.print (F(" -- ")); \
                          Serial.print (__FILE__); Serial.print (F(" in line ")); \
                          Serial.print (__LINE__);  Serial.print (F(" -- ")); \
                          Serial.println (__FUNCTION__); \
                        }
 #define DOUTKV(k, v)   { Serial.print (F(k)); Serial.print (':'); Serial.print (v); \
                          Serial.print (F(" -- ")); \
                          Serial.print (__FILE__); Serial.print (F(" in line ")); \
                          Serial.print (__LINE__);  Serial.print (F(" -- ")); \
                          Serial.println (__FUNCTION__); \
                        }
#else
 #define DOUT(x)        
 #define DOUTKV(k, v)   
#endif

#endif
