/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "app.h"
#include <logger.h>

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
    Log::SetLevel(Log::Level::Verbose);
    auto game = std::make_unique<rvr::App>();
    try {
        game->Run(app);
    }
    catch (const std::exception& e) {
        rvr::PrintError(e.what());
    }
    catch (...) {
        rvr::PrintError("Unknown error");
    }
}
