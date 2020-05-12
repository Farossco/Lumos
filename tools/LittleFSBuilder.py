# Thanks to StridingDragon and Yuriy (https://community.platformio.org/t/how-to-define-littlefs-partition-build-image-and-flash-it-on-esp32/11333/16)
Import("env")
env.Replace(MKSPIFFSTOOL="tools/mklittlefs.exe")
