#!/bin/bash
cd ..
tar -cvzf wxchaos_1.1.orig.tar.gz wxchaos-1.1-0
cd wxchaos-1.1-0
dpkg-buildpackage -S -sa -rfakeroot
