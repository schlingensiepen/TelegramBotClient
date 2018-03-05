#pragma once
#ifndef TBDebug_h
#define TBDebug_h

//define TBCVERBOSE

#ifdef TBCVERBOSE
 #define DOUT(x)        { Serial.print (__FILE__); Serial.print (F(" in line ")); \
                          Serial.print (__LINE__);  Serial.print (F(" -- ")); \
                          Serial.print(__FUNCTION__); Serial.print (F(":")); \                                                    
                          Serial.println (F(x)); }
 #define DOUTKV(k, v)   { Serial.print (__FILE__); Serial.print (F(" in line ")); \
                          Serial.print (__LINE__);  Serial.print (F(" -- ")); \
                          Serial.print(__FUNCTION__); Serial.print (F(":")); \                                                    
                          Serial.print (F(k)); Serial.print (':'); Serial.println (v); }
#else
 #define DOUT(x)        
 #define DOUTKV(k, v)   
#endif

#endif
