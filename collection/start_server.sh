#!/bin/bash

if timeout 120 python pow.py ask 10388606; then
	CGID="$(( ( RANDOM % 4 )  + 1 ))"
	cgexec -g "cpu,memory,pids:$CHAL_NAME/$CGID" timeout 30 python -u server.py
fi
