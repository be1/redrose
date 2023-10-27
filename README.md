# Redrose
ABC music notation integrated environment, _ps_ flavour.

## Dependencies
- drumstick-file (library and headers)
- fluidsynth (library and headers)
- libspectre (library and headers)
- abcm2ps (program)
- abc2midi (program, optional)

Note that you must install abcm2ps binary for this _ps_ flavour of Redrose to display a view of the score.

## Screenshot
![Redrose dark theme screenshot](http://brouits.free.fr/redrose/redrose.png)

## Setting up
```
git clone https://github.com/be1/redrose.git
git checkout ps
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
First, this project was based on previously QAbc experimental `fluid` branch.
Then this _ps_ flavour replaces SVG rendering by Postscript rendering.

## Thanks
- Thanks to JF Moine for his free software `abcm2ps`
- Thanks to Morgan Leborgne for his QProgressIndicator free widget. https://github.com/mojocorp/QProgressIndicator

