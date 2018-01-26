#!/bin/sh

sudo apt-get update
sudo apt-get upgrade
sudo apt-get install gcc g++ python python-dev mercurial python-setuptools git qt5-default python-pygraphviz python-kiwi python-pygoocanvas libgoocanvas-dev ipython openmpi-bin openmpi-common openmpi-doc libopenmpi-dev autoconf cvs bzr unrar gdb valgrind uncrustify gsl-bin libgsl2 libgsl-dev flex bison libfl-dev tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev cmake libc6-dev libc6-dev-i386 libclang-dev libgtk2.0-0 libgtk2.0-dev  libboost-signals-dev libboost-filesystem-dev
sudo pip install cxxfilt
cd
mkdir repos
cd repos
hg clone http://code.nsnam.org/ns-3-allinone
./download.py -n ns-3.27
./build.py
./test.py
