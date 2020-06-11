//#define DEBUG
#ifdef DEBUG
  #define INIT_DBG() Serial.begin(9600)
  #define DBG(x) Serial.println(x)
#else
  #define INIT_DBG()
  #define DBG(x)
#endif
