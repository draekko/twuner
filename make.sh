#!/bin/bash
rm -fr build
mkdir build
cd build
cmake .. -DWITH_GIR=Off  -DWITH_GTK3=Off -DWITH_QT5=On -DCMAKE_INSTALL_PREFIX=/opt/Apps/twuner/
make
sudo make install
