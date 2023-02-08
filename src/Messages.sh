#!/bin/sh
$EXTRACTRC ui/*.ui >> rc.cpp
$XGETTEXT `find -name \*.cpp -o -name \*.qml` -o $podir/accountwizard.pot
