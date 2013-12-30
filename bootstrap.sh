#!/bin/sh
rm -f configure
aclocal
autoconf
touch AUTHORS NEWS ChangeLog
ln -s README.md README
automake --add-missing

