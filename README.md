"Playground" for detection of image modifications by inpainting algorithms.

Can detect various methods such as the one used in Photoshop, but is specifically targeted (and performs better) 
at the patch based methods such as the one [presented in this IPOL article](http://www.ipol.im/pub/art/2015/136/).

Here is [the final report](https://github.com/antoinewdg/inpainting-detection/blob/master/files/report.pdf) for the project, and here is the support for my [oral presentation](https://github.com/antoinewdg/inpainting-detection/blob/master/files/oral_presentation.pdf) (the latter is in French).

## How to build

Building requires OpenCV 3.1 and Boost.

``` bash
git clone git@github.com:antoinewdg/inpainting-detection.git --recursive
cd inpainting-detection/
mkdir build
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## How to run
To `detection` executable produced runs the detection on all files listed in `files/in/index.txt`.
This is not meant to be a flexible CLI tool: the location of the `index.txt` is hardcoded, you should run the executable in a subfolder of the root folder in order for it to work (the `build` folder of the build phase is a good candidate).

## Documentation

This project has currently no documentation because I do not see anyone having an immediate use of it besides myself,
if you feel it could be of any use to you or have any question feel free to contact me.

