#!/bin/bash

(set -x;
 socat tcp:172.19.21.10:62000 pty,link=/tmp/ttymm0,raw,echo=0 &
 socat tcp:172.19.21.10:62001 pty,link=/tmp/ttymm1,raw,echo=0 &
 socat tcp:172.19.21.10:62002 pty,link=/tmp/ttymm2,raw,echo=0 &
 socat tcp:172.19.21.10:62003 pty,link=/tmp/ttymm3,raw,echo=0 &
)

while [ 1 ]; do sleep 1; done

trap "echo 'Cleaning up'" EXIT
