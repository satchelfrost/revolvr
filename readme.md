![](logo.png)
# *A VR-focused Game Engine*

## Table of Contents
* [Overview](#overview)
* [Target Hardware](#target-hardware)
* [Clone Recursively](#clone-recursively)
* [Run Examples](#run-examples)

## Overview
This is not a production ready game engine. The target audience is the tinkerer, or the person interested in making games from scratch (particularly on the Meta Quest).

## Target Hardware
Meta Quest 1, 2, 3, & Pro.

## Run examples

To run the examples you must have android studio properly installed for your OS. Please refer to Meta's documentation [Android Development Setup](https://developer.oculus.com/documentation/native/android/mobile-studio-setup-android/). Do not skip this step.

### Clone Recursively

```bash
git clone --recurse-submodules https://github.com/satchelfrost/revolvr-sample-project.git
```

or, if you've already cloned without doing that, you can run:

```bash
git submodule update --init --recursive
```

### Option 1 - *Running examples from command line*

Navigate to the examples folder and run the following commands (be sure to replace the sample name with the actual name of the sample)

```bash
./gradlew :sample-projects:<SAMPLE NAME>:assembleDebug
```

Then install the build to the headset (ensure that you have enabled usb debugging and allowed modification of files).

```bash
adb install sample-projects/<SAMPLE NAME>/build/outputs/apk/debug/<SAMPLE NAME>-debug.apk
```

Then run

```bash
adb shell am start com.wamwadstudios.<SAMPLE NAME>/android.app.NativeActivity
```

To quit, either quit from the headset, or run the following command

```bash
adb shell am force-stop com.wamwadstudios.<SAMPLE NAME>
```

### Option 2 - *Running examples from android studio*

Open the project from the examples folder. Allow gradle to initialize. The drop down should have various run configurations. Choose one to deploy to the headset by hitting the play button.