#!/bin/sh
$XGETTEXT `find -name \*.cpp -o -name \*.qml | grep -v '/legacy/'` -o $podir/accountwizard.pot
