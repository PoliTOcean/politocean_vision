# Politocean Vision

## C++

In VSC open the project folder and insert into the `includePath` field of the __c_cpp_properties.json__ (`CTRL+SHIFT+P` and then `C/C++: Edit Configurations (JSON)`)

```json
"includePath": [
    ... ,
    "/usr/local/include/opencv4"
]
```

To build, inside the project folder

```bash
mkdir build
cd build

cmake ..
make
```

To run

```bash
./PolitoceanShapes
```

## Python

Install pip for Python3

```bash
sudo apt update
sudo apt install python3-pip
```

Install virtualenv using pip3

```bash
sudo pip3 install virtualenv
```

Create a Python virtual environment for your OpenCV project and activate it

```bash
cd opencv_project
virtualenv venv
source venv/bin/activate
pip3 install opencv-contrib-python
```

To test that everything goes right

```bash
python3
>>> import cv2
>>> cv2.__version__
'4.1.1'
```

To deactivate the virtual environment run inside the project root

```bash
source venv/bin/deactivate
```
