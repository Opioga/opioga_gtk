
pkgname="emu8086"
pkgver=1.0.0
pkgrel=1
arch=('x86_64')
pkgdesc="A text editor for GNOME (mingw-w64)"
depends=("${MINGW_PACKAGE_PREFIX}-adwaita-icon-theme"
           "${MINGW_PACKAGE_PREFIX}-gtk3"
 
   
       )
makedepends=("${MINGW_PACKAGE_PREFIX}-gcc"
   
             "pkg-config"
)
 license=("GPL")
url="https://www.gnome.org"
 

 
build() {
    mkdir -p "${srcdir}/build"
    cd "${srcdir}/build"
    ${srcdir}/../configure --prefix=/usr
	make
    
}

package() {
  cd "${srcdir}/build"
  DESTDIR="${pkgdir}" make install

#   install -Dm644 "${srcdir}/${_realname}-${pkgver}/COPYING" "${pkgdir}${MINGW_PREFIX}/share/licenses/${_realname}/COPYING"
}