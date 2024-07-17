# Redrose
ABC music notation integrated environment

## Dependencies
- drumstick-file (library and headers)
- fluidsynth (library and headers)
- libspectre (library and headers)
- abcm2ps (program)
- abc2midi (program, optional)

Note that you *must* install **abcm2ps** binary for Redrose to display a view of the score.
Note that drumstick-file must be compiled against the same QT\_MAJOR\_VERSION number as redrose will be compiled against.

## Screenshot
![Redrose dark theme screenshot](http://brouits.free.fr/redrose/redrose.png)

## Setting up with Qt5 (Ubuntu Jammy, 22.04)
```
$ sudo apt install qtbase5-dev
$ sudo apt install qttools5-dev
$ sudo apt install qttools5-dev-tools
$ sudo apt install libdrumstick-dev
$ sudo apt install libspectre-dev
$ sudo apt install libfluidsynth-dev
```

## Setting up with Qt6 (Ubuntu Noble, 24.04)
```
$ sudo apt install qt6-base-dev
$ sudo apt install qt6-tools-dev
$ sudo apt install qt6-tools-dev-tools
$ sudo apt install libdrumstick-dev
$ sudo apt install libspectre-dev
$ sudo apt install libfluidsynth-dev
```

## Building
```
$ sudo apt install build-essential cmake git pkg-config
$ git clone https://github.com/be1/redrose.git
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ sudo make install
$ sudo apt install abcm2ps
$ sudo apt install abcmidi
```

## Starting
Launch `redrose` by clicking on the red rose icon, click on Score->New and type some ABC music in the editor, press `Play` button: you're done!

## Bugs
Internal ABC parser is weak, you can use abc2midi instead (see player preferences).

## Thanks
- Thanks to JF Moine for his free software `abcm2ps`
- Thanks to Morgan Leborgne for his QProgressIndicator free widget. https://github.com/mojocorp/QProgressIndicator

