# Redrose
ABC music notation integrated environment

## Dependencies
- drumstick-file version 1 or 2 (library and headers)
- fluidsynth version < 2.3 or >= 2.3 (library and headers)
- libspectre (library and headers)
- abcm2ps (program)
- abc2midi (program, optional)

Note that you *must* install **abcm2ps** binary for Redrose to display a view of the score.
Note that **drumstick-file** must be compiled against the same QT\_MAJOR\_VERSION number as redrose will be compiled against.

## Screenshot
![Redrose dark theme screenshot](http://brouits.free.fr/redrose/redrose.png)

## Setting up with Qt5 (Ubuntu Jammy, 22.04 or Ubuntu Focal, 20.04)
```
sudo apt install qtbase5-dev qttools5-dev qttools5-dev-tools libdrumstick-dev libspectre-dev libfluidsynth-dev
```

## Setting up with Qt6 (Ubuntu Noble, 24.04)
```
sudo apt install qt6-base-dev qt6-tools-dev qt6-tools-dev-tools libdrumstick-dev libspectre-dev libfluidsynth-dev
```

## Building
```
sudo apt install build-essential cmake git pkg-config
git clone https://github.com/be1/redrose.git
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
sudo apt install abcm2ps abcmidi
```

## Starting
Launch `redrose` by clicking on the red rose icon, click on Score->New and type some ABC music in the editor, press `Play` button: you're done!

## Bugs
For MIDI generation, the internal ABC parser is weak and does not generate "Guitar chords". You can use abc2midi for that if you prefer (see player preferences).

## TODO
- Jack MIDI output (in addition to current Jack audio output)

## Thanks
- Thanks to JF Moine for his free software `abcm2ps`
- Thanks to Seymour Shlien for his free software `abc2midi`
- Thanks to Morgan Leborgne for his [QProgressIndicator free widget](https://github.com/mojocorp/QProgressIndicator).

