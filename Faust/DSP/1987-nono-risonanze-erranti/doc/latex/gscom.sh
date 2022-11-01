#!/usr/bin/env bash

# use it by terminal typing:
# bash gscom.sh commitname

git status
git add .
git commit -am "$1"
