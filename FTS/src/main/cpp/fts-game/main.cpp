//
// Created by cris on 7/24/22.
//

#include "include/rvr_app.h"

class FTSApp : public RVRApp {
protected:
    void OnInitialize() override {
        RVRApp::OnInitialize();
    }

    void OnShutdown() override {
        RVRApp::OnShutdown();
    }

    void OnUpdate() override {
        RVRApp::OnUpdate();
    }

    void OnInput() override {
        RVRApp::OnInput();
    }

    void OnRender() override {
        RVRApp::OnRender();
    }
private:

};

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
    auto rvrApp = std::make_unique<FTSApp>();
    rvrApp->Run(app);
}