#!/bin/sh


sudo apt-get update
sudo apt-get upgrade

sudo apt-get install build-essential

# Note: Ubuntu 14.04 LTS release requires upgrade to gcc-4.9 from default gcc-4.8. More recent Ubuntu versions are OK. The instructions at this link: http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-on-ubuntu?answertab=votes#tab-top have been found to work.

#sudo add-apt-repository ppa:ubuntu-toolchain-r/test
#sudo apt-get update
#sudo apt-get remove gcc-4.8 g++-4.8
#sudo apt-get autoremove
#sudo apt-get install gcc-4.9 g++-4.9

sudo apt-get install python python-dev python-pip python-setuptools mercurial git qt5-default openmpi-bin openmpi-common openmpi-doc libopenmpi-dev autoconf cvs bzr unrar gdb valgrind uncrustify gsl-bin flex bison libfl-dev tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev cmake libc6-dev libc6-dev-i386 libclang-dev libgtk2.0-0 libgtk2.0-dev libboost-signals-dev libboost-filesystem-dev

# python API is experimental
#sudo apt-get install python-pygraphviz python-kiwi python-pygoocanvas libgoocanvas-dev ipython

sudo pip install cxxfilt

mkdir -p ~/projects
cd ~/projects

hg clone http://code.nsnam.org/openflow
hg clone http://code.nsnam.org/ns-3-allinone

# build openflow
cd openflow
./waf configure
./waf build

# build ns3
cd ../ns-3-allinone
./download.py -n ns-3.27
# use $HOME or it will fail
./build.py --build-options=--progress -- --build-profile=debug --prefix=/usr --disable-python --with-openflow=$HOME/projects/openflow
