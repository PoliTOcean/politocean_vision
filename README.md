# Politocean Vision

In VSC open the project folder and insert into the `includePath` field of the __c_cpp_properties.json__ (`CTRL+SHIFT+P` and then `C/C++: Edit Configurations (JSON)`)

```json
"includePath": [
    ... ,
    "/usr/local/include/opencv4"
]
```

To build, inside the project folder

```bat
mkdir build
cd build

cmake ..
make
```

To run

```bat
./PolitoceanShapes
```

## If OpenCV is not system installed

Suppose **$OPENCV_INSTALL_DIR** is the OpenCV installation directory.

Add

`set(OpenCV_DIR $OPENCV_INSTALL_DIR/lib/cmake/opencv4)`

inside the __CMakeLists.txt__ soon after line 4.

On VSC replace

`"/usr/local/include/opencv4"`

with

`"$OPENCV_INSTALL_DIR/include/opencv4"`

inside the __c_cpp_properties.json__ `includePath` field.
