#
#	Installation script for the GEM Address Example Application
#	
#	Do not use this as an example. This is just quick and dirty.
#	If you write a real application, you have to use the V.4 package
#	software. See the V.4 documentation.
#
if test ! -d /usr/lib/AtariExample
then
	mkdir /usr/lib/AtariExample
	mkdir /usr/lib/AtariExample/gls
	mkdir /usr/lib/AtariExample/Fm
	mkdir /usr/lib/AtariExample/bitmaps
fi
cp addr.fm /usr/lib/AtariExample/Fm
cp addr.cat addr.cat.m /usr/lib/AtariExample/gls

