#include <ofConstants.h>

#ifdef TARGET_EMSCRIPTEN
#include "ofURLFileLoader_EM.cpp"
#else
#include "ofURLFileLoader_SYNC.cpp"
#endif

