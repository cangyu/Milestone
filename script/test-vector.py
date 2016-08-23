import os
import sys
import subprocess

selfname="vector"
outputType=".exe"
resultType=".txt"
globalResultsDir="../results"
outputPath=globalResultsDir+"/"+selfname
srcPath="../test/"+selfname+"/"

if(os.path.exists(globalResultsDir)==False):
	os.mkdir(globalResultsDir)

if(os.path.exists(outputPath)==False):
	os.mkdir(outputPath)

srcFileList=os.listdir(srcPath)

for file in srcFileList:
	(fileName, fileType)=os.path.splitext(file)
	print("compiling " + file + " ......")
	testCompile=subprocess.Popen(["g++","-std=c++11","-O2", srcPath+file, "-o", outputPath+"/"+fileName+outputType])
	testCompile.wait()

	print("executing " + fileName+outputType + " ......")
	run_output=open(outputPath+"/"+fileName+resultType,'w')
	testRun=subprocess.Popen([outputPath+"/"+fileName+outputType],stdout=run_output)
	testRun.wait()

	print("done, output in "+outputPath+"/"+fileName+resultType)
