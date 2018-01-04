## Cinder LiquidFun

Cinder Block of [LiquidFun from Google](https://github.com/google/liquidfun). 
Just checkout out repo into your Cinder Blocks directory.

## Android

#### Building ``liquidfun`` for Android
Starting from the Cinder LiquidFun directory:
```
cd android/jni
<your_ndk_path>/ndk-build
```

#### Building and running ``Testbed`` sample for Android
Use Android Studio to build and run the ``Testbed`` sample.

### Linux

#### Building ``liquidfun`` for Linux
Starting from the Cinder LiquidFun directory:
```
cd linux
./build -b Release
```

#### Building and running ``Testbed`` sample for Linux
Starting from the Cinder LiquidFun directory:
```
cd samples/Testbed/linux
./build -b Release
Release/ogl/Testbed
```

### Mac OS X and iOS

#### Building ``liquidfun`` for Mac OS X and iOS
Use XCode to build the static library.

#### Building and running ``Testbed`` sample for Mac OS X and iOS
Use XCode for build the ``Testbed`` sample.   
  
### Windows

Tested Compatible with Cinder 0.9+

#### Building ``liquidfun`` for Windows
Use Visual Studio 2013/2015 to build the static library.

#### Building and running ``Testbed`` sample for Windows
Use Visual Studio 2013/2015 to build the ``Testbed`` sample. 

  
## Screenshot
![alt tag](https://raw.githubusercontent.com/chaoticbob/CinderLiquidFun/master/screenshots/linux/Sandbox.png)
