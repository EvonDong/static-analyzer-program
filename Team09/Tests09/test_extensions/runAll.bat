@echo off
title System Test Iteration 3 Queries

set "buildpath=..\..\Code09\build_win\x86-Debug\src\autotester\autotester"

%buildpath% prog1.txt affectsBipProg1.txt 				out-affectsProg1.xml


echo FINISHED
pause
exit