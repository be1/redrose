# Redrose
ABC music notation integrated environment

## Dependencies
- drumstick-file (library and headers)
- fluidsynth (library and headers)
- libspectre (library and headers)
- abcm2ps (program)
- abc2midi (program, optional)

Note that you *must* install **abcm2ps** binary for Redrose to display a view of the score.

## Screenshot
![Redrose dark theme screenshot](http://brouits.free.fr/redrose/redrose.png)

## Setting up
```
sudo apt install qtbase5-dev
sudo apt install libdrumstick-dev
sudo apt install libspectre-dev
sudo apt install libfluidsynth-dev
sudo apt install abcm2ps
sudo apt install abcmidi
git clone https://github.com/be1/redrose.git
```
## Building
Just a matter of:
```
$ qmake -config release
$ make
# make install (as root)
```

## Starting
Launch `redrose` by clicking on the red rose icon, click on Score->New and type some ABC music in the editor, press `Play` button: you're done!

## Bugs
- sometimes crash on pressing `Stop` button while playing music.
- internal parser is weak, you can use abcmidi instead.

## Thanks
- Thanks to JF Moine for his free software `abcm2ps`
- Thanks to Morgan Leborgne for his QProgressIndicator free widget. https://github.com/mojocorp/QProgressIndicator

