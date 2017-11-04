Release Process
===============

* Update translations

* * *

### Update (commit) version in sources

	doc/README*
	trollocoin-qt.pro
	share/setup.nsi
	src/version.h

### Tag version in git

	git tag -s v(new version, e.g. 0.8.0)

### Write release notes. git shortlog helps a lot, for example:

	git shortlog --no-merges v(current version, e.g. 0.7.2)..v(new version, e.g. 0.8.0)

* * *

### Perform gitian builds

From a directory containing the trollocoin source, gitian-builder and gitian.sigs

	export SIGNER=(your gitian key, ie bluematt, sipa, etc)
	export VERSION=(new version, e.g. 0.8.0)
	cd ~/gitian-builder

### Fetch and build inputs:
	mkdir -p inputs; cd inputs/
	wget 'http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.9.tar.gz' -O miniupnpc-1.9.tar.gz
	wget 'https://www.openssl.org/source/openssl-1.0.1h.tar.gz'
	wget 'http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz'
	wget 'http://zlib.net/zlib-1.2.8.tar.gz'
	wget 'ftp://ftp.simplesystems.org/pub/png/src/history/libpng16/libpng-1.6.8.tar.gz'
	wget 'http://fukuchi.org/works/qrencode/qrencode-3.4.3.tar.bz2'
	wget 'http://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.bz2'
	wget 'https://svn.boost.org/trac/boost/raw-attachment/ticket/7262/boost-mingw.patch' -O boost-mingw-gas-cross-compile-2013-03-03.patch
	wget 'https://download.qt-project.org/archive/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.tar.gz'
	cd ..
	./bin/gbuild ../trollocoin/contrib/gitian-descriptors/boost-linux.yml
	cp build/out/boost-linux*-1.55.0-gitian-r1.zip inputs/
	./bin/gbuild ../trollocoin/contrib/gitian-descriptors/deps-linux.yml
	cp build/out/trollocoin-deps-linux*-gitian-r5.zip inputs/
	./bin/gbuild ../trollocoin/contrib/gitian-descriptors/boost-win.yml
	cp build/out/boost-win*-1.55.0-gitian-r6.zip inputs/
	./bin/gbuild ../trollocoin/contrib/gitian-descriptors/qt-win.yml
	cp build/out/qt-win*-4.8.5-gitian-r3.zip inputs/
	./bin/gbuild ../trollocoin/contrib/gitian-descriptors/deps-win.yml
	cp build/out/trollocoin-deps-win*-gitian-r12.zip inputs/

### Build Bitcoin Core for Linux and Windows:
**Build for Linux32 and Linux64:**

    ./bin/gbuild --commit trollocoin=v${VERSION} ../trollocoin/contrib/gitian-descriptors/gitian-linux.yml
	./bin/gsign --signer $SIGNER --release ${VERSION} --destination ../gitian.sigs/ ../trollocoin/contrib/gitian-descriptors/gitian-linux.yml
	pushd build/out
	zip -r trollocoin-${VERSION}-linux-gitian.zip *
	mv trollocoin-${VERSION}-linux-gitian.zip ../../
	popd

**Build for Win32 and Win64:**

	./bin/gbuild --commit trollocoin=v${VERSION} ../trollocoin/contrib/gitian-descriptors/gitian-win.yml
	./bin/gsign --signer $SIGNER --release ${VERSION}-win --destination ../gitian.sigs/ ../trollocoin/contrib/gitian-descriptors/gitian-win.yml
	pushd build/out
	zip -r trollocoin-${VERSION}-win-gitian.zip *
	mv trollocoin-${VERSION}-win-gitian.zip ../../
	popd
Build output expected:

1. linux 32-bit and 64-bit binaries + source (trollocoin-${VERSION}-linux-gitian.zip)
2. windows 32-bit and 64-bit binaries, installers + source (trollocoin-${VERSION}-win-gitian.zip)
3. Gitian signatures (in gitian.sigs/${VERSION}[-win]/(your gitian key)/

Repackage gitian builds for release as stand-alone zip/tar/installer exe

**Linux .tar.gz:**

	unzip trollocoin-${VERSION}-linux-gitian.zip -d trollocoin-${VERSION}-linux
	tar czvf trollocoin-${VERSION}-linux.tar.gz trollocoin-${VERSION}-linux
	rm -rf trollocoin-${VERSION}-linux

**Windows .zip and setup.exe:**

	unzip trollocoin-${VERSION}-win32-gitian.zip -d trollocoin-${VERSION}-win32
	mv trollocoin-${VERSION}-win32/trollocoin-*-setup.exe .
	zip -r trollocoin-${VERSION}-win32.zip trollocoin-${VERSION}-win32
	rm -rf trollocoin-${VERSION}-win32

**Perform Mac build**

See this blog post for how Gavin set up his build environment to build the OSX
release; note that a patched version of macdeployqt is not needed anymore, as
the required functionality and fixes are implemented directly in macdeployqtplus:

http://gavintech.blogspot.com/2011/11/deploying-bitcoin-qt-on-osx.html

Gavin also had trouble with the macports py27-appscript package; he
ended up installing a version that worked with: /usr/bin/easy_install-2.7 appscript

	qmake RELEASE=1 USE_UPNP=1 USE_QRCODE=1 bitcoin-qt.pro
	make
	export QTDIR=/opt/local/share/qt4  # needed to find translations/qt_*.qm files
	T=$(contrib/qt_translations.py $QTDIR/translations src/qt/locale)
	python2.7 contrib/macdeploy/macdeployqtplus Bitcoin-Qt.app -add-qt-tr $T -dmg -fancy contrib/macdeploy/fancy.plist

Build output expected: Trollocoin-Qt.dmg

### Next steps:

* Upload builds to SourceForge

* Create SHA256SUMS for builds, and PGP-sign it

* Update trollocoin.com version

* Update forum version

* Update wiki download links

* Update wiki changelog

* Commit your signature to gitian.sigs:

    pushd gitian.sigs
	git add ${VERSION}/${SIGNER}
	git add ${VERSION}-win/${SIGNER}
	git commit -a
	git push  # Assuming you can push to the gitian.sigs tree
	popd

* * *

### After 3 or more people have gitian-built, repackage gitian-signed zips:
From a directory containing trollocoin source, gitian.sigs and gitian zips

	export VERSION=0.5.1
	mkdir trollocoin-${VERSION}-linux-gitian
	pushd trollocoin-${VERSION}-linux-gitian
	unzip ../trollocoin-${VERSION}-linux-gitian.zip
	mkdir gitian
	cp ../trollocoin/contrib/gitian-downloader/*.pgp ./gitian/
	for signer in $(ls ../gitian.sigs/${VERSION}/); do
     cp ../gitian.sigs/${VERSION}/${signer}/trollocoin-build.assert ./gitian/${signer}-build.assert
     cp ../gitian.sigs/${VERSION}/${signer}/trollocoin-build.assert.sig ./gitian/${signer}-build.assert.sig
	done
	zip -r trollocoin-${VERSION}-linux-gitian.zip *
	cp trollocoin-${VERSION}-linux-gitian.zip ../
	popd
	mkdir trollocoin-${VERSION}-win-gitian
	pushd trollocoin-${VERSION}-win-gitian
	unzip ../trollocoin-${VERSION}-win-gitian.zip
	mkdir gitian
	cp ../trollocoin/contrib/gitian-downloader/*.pgp ./gitian/
	for signer in $(ls ../gitian.sigs/${VERSION}-win/); do
     cp ../gitian.sigs/${VERSION}-win/${signer}/trollocoin-build.assert ./gitian/${signer}-build.assert
     cp ../gitian.sigs/${VERSION}-win/${signer}/trollocoin-build.assert.sig ./gitian/${signer}-build.assert.sig
	done
	zip -r trollocoin-${VERSION}-win-gitian.zip *
	cp trollocoin-${VERSION}-win-gitian.zip ../
	popd

* Upload gitian zips to SourceForge
