# This Too Shall Pass

This is my entry for Handmade Network's One Thing a Month jam.

It'll, hopefully, be a very small 2D platformer with nice physics and a cute design.


...


...


We'll see.

## Building

Currently only tested on Ubuntu. However, it should work on anything where it runs. SDL2 is the only dependency not packaged with the repository. Make sure you have ran `git submodule update` to get everything else (GLAD and HandmadeMath).

To build, move into the root git repository and:

`bin/build.sh`

and to run:

`bin/run.sh`

Alternatively, if you're going to be doing any development (like me) you should:

`source env.sh`

and then you can build and run with the `m` and `r` commands respectively.
