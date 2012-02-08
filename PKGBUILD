# Author: mosra <mosra@centrum.cz>
pkgname=magnum-plugins
pkgver=dev
pkgrel=1
pkgdesc="Plugins for Magnum OpenGL 3 graphics engine"
arch=('i686' 'x86_64')
url="http://mosra.cz/blog/"
license=('LGPLv3')
depends=('magnum' 'qt')
makedepends=('cmake' 'mesa' 'glew')
options=(!strip)

build() {
    mkdir -p "$startdir/build"
    cd "$startdir/build/"

    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTS=TRUE
    make
}

package() {
  cd "$startdir/build"
  make DESTDIR="$pkgdir/" install
}
