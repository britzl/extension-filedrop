#pragma once

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX)

typedef void (*OnEventCallback)(const char* event, const char* filename, const uint8_t* data, int data_length);

extern "C" {
    void FileDrop_PlatformSetEventListener(OnEventCallback callback);
    void FileDrop_Finalize();
}

#endif
