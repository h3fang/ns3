# Contributor: Yunhui Fu <yhfudev@gmail.com>
# Maintainer: Shuwen Jethro Sun <jethro.sun7+arch@gmail.com>

pkgname=ns3
pkgver=3.27
pkgrel=1
pkgdesc='Discrete-event network simulator for Internet systems'
arch=('x86_64')
url='http://www.nsnam.org/'
license=('GPL')
depends=(
    'gcc' 'python2' 'qt5-base' 
    'sqlite'
    'libxml2' 'gtk2' 'pygtk'
    'gdb' 'valgrind'
    )
makedepends=(
    'fakeroot'
    'findutils'
    'mercurial'
    'git'
    'qt5-tools'
    'cmake'
    )
optdepends=(
    'tcpdump' 'wireshark-gtk' 'openmpi' 'gsl' 'boost' 'boost-libs'
    )
source=(
    "https://www.nsnam.org/release/ns-allinone-${pkgver}.tar.bz2"
    "ns3-openflow-hg::hg+http://code.nsnam.org/openflow"
    "ns3-click-git::git+https://github.com/kohler/click"
    "ns3-brite-hg::hg+http://code.nsnam.org/BRITE"
    )
md5sums=('SKIP'
         'SKIP'
         'SKIP'
         'SKIP')
sha1sums=('SKIP'
          'SKIP'
          'SKIP'
          'SKIP')

prepare() {
    cd $srcdir/ns-allinone-$pkgver

    grep -rl '/usr/bin/env python' . | xargs sed -i 's|/usr/bin/env python|/usr/bin/python2|g'

# compile openflow lib support
    cd $srcdir/ns3-openflow-hg
    grep -rl '/usr/bin/env python' . | xargs sed -i 's|/usr/bin/env python|/usr/bin/python2|g'
    ./waf configure
    ./waf build

# compile click support
    cd $srcdir/ns3-click-git
    ./configure --disable-linuxmodule --enable-nsclick --enable-wifi
    rm -rf bin
    make -j $(cat /proc/cpuinfo | grep processor | wc -l | awk '{print $0 + 1;}')

# BRITE
    cd $srcdir/ns3-brite-hg
    make -j $(cat /proc/cpuinfo | grep processor | wc -l | awk '{print $0 + 1;}')
}

build() {
    cd $srcdir/ns-allinone-$pkgver

#    --enable-examples \
#    --enable-tests \
    ./build.py \
    --qmake-path=/usr/bin/qmake-qt5 \
    --build-options=--progress \
    -- \
    --build-profile=debug \
    --prefix=/usr \
    --disable-python \
    --enable-mpi \
    --enable-sudo \
    --libdir=/usr/lib \
    --with-nsclick=$srcdir/ns3-click-git \
    --with-openflow=$srcdir/ns3-openflow-hg \
    --with-brite=$srcdir/ns3-brite-hg
#    --with-python=/usr/bin/python2 \
}

package() {
    cd $srcdir/ns-allinone-$pkgver/ns-$pkgver
    ./waf install --destdir=$pkgdir/
}
