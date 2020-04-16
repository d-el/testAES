###Test AES

#Description
This project is demonstration AES encrypt in build time and decrypt in runtime./
This strategy use in crypto bootloader.

#build:
```
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make
```

#use:
```
./testAES.elf ../text.enc
or
./testAES.elf ../text.enc > text.dec
```
