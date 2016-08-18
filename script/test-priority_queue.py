import os
import sys
import subprocess

testCompile=subprocess.Popen(["g++","-std=c++11","-O2","../test/priority_queue/priority_queue-basic.cc", "-o", "../pq_basic.exe"], stderr="../pq_basic.txt");
testCompile.wait()
