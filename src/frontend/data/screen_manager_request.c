#include "frontend/data/screen_manager_request.h"

ScreenManagerRequest screenManagerRequest_getNone(void)
{
    ScreenManagerRequest request;

    request.type = screenManagerRequestNone;

    return request;
}