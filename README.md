# Redrose
ABC music notation integrated environment

## Dependencies
Redrose uses third-party software to generate MIDI, AUDIO, SVG and Postscript files.
- abc2midi (program, not mandatory)
- drumstick-file (library and headers)
- fluidsynth (library and headers)

Note that Redrose uses a *modified* version of the open source abcm2ps code, maintained in a specific submodule.
Original `abcm2ps` code is Copyright © 2014-2016 Jean-Francois Moine.

Note that Redrose provides an optionnal internal abc notation parser and MIDI generator.

## Screenshot
[Redrose dark theme screenshot](http://brouits.free.fr/redrose/redrose.png)

## Setting up
```
git clone https://github.com/be1/redrose.git
cd abcm2ps
git submodule init
git submodule update
git checkout lib
cd ..
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
Probably lots of!

## Colophon
This project is based on previously QAbc experimental `fluid` branch.

## Thanks
- abcm2ps original code is from JF Moine, thanks to him for setting this as free software.
- QProgressIndicator is from Morgan Leborgne, thanks to him for this free piece of software. https://github.com/mojocorp/QProgressIndicator

