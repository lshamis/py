#!/bin/bash
cd "$(dirname "$0")"

docker build -t alephzero_py_testinstall -f testinstall.Dockerfile ..

docker run --rm -it alephzero_py_testinstall
