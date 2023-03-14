#pragma once

#include <afxwin.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include "rapidjson/document.h"

// Initialize locale so that std::wcout can work.
void initLocale();

// Setup DBSDK environment.
bool setupEnv();

// Unsetup DBSDK environment.
bool unsetupEnv();

// Initialize configuration.
extern rapidjson::Document gConfig;
bool initConfig();