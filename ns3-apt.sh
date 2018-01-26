#!/bin/sh


sudo apt-get update
sudo apt-get upgrade

# Note: Ubuntu 14.04 LTS release requires upgrade to gcc-4.9 from default gcc-4.8. More recent Ubuntu versions are OK. The instructions at this link: http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-on-ubuntu?answertab=votes#tab-top have been found to work. 
#sudo add-apt-repository ppa:ubuntu-toolchain-r/test
#sudo apt-get update
#sudo apt-get install gcc-4.9 g++-4.9
#sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9

sudo apt-get install gcc g++ python python-dev python-pip mercurial python-setuptools git qt5-default python-pygraphviz python-kiwi python-pygoocanvas libgoocanvas-dev ipython openmpi-bin openmpi-common openmpi-doc libopenmpi-dev autoconf cvs bzr unrar gdb valgrind uncrustify gsl-bin flex bison libfl-dev tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev cmake libc6-dev libc6-dev-i386 libclang-dev libgtk2.0-0 libgtk2.0-dev  libboost-signals-dev libboost-filesystem-dev
sudo pip install cxxfilt
cd
mkdir -p ~/projects
cd ~/projects
hg clone http://code.nsnam.org/ns-3-allinone
cd ns-3-allinone
./download.py -n ns-3.27
./build.py
./test.py
