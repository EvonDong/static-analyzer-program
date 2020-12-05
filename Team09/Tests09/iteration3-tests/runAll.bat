@echo off
title System Test Iteration 3 Queries

set "buildpath=..\..\Code09\build_win\x86-Debug\src\autotester\autotester"

%buildpath% prog1.txt affectsProg1.txt 				out-affectsProg1.xml
%buildpath% prog1.txt clausesProg1.txt 				out-clausesProg1.xml
%buildpath% prog1.txt nextProg1.txt 				out-nextProg1.xml
%buildpath% prog1.txt followsProg1.txt				out-followsProg1.xml
%buildpath% prog1.txt followsStarProg1.txt			out-followsStarProg1.xml
%buildpath% prog1.txt modifiesAndUsesProg1.txt			out-modifiesAndUsesProg1.xml

%buildpath% prog1.txt invalidModifiesAndUsesProg1.txt		out-invalidModifiesAndUsesProg1.xml
%buildpath% prog1.txt invalidProg1.txt				out-invalidProg1.xml

%buildpath% prog2.txt parentProg2.txt 				out-parentProg2.xml
%buildpath% prog2.txt callsProg2.txt 				out-callsProg2.xml
%buildpath% prog2.txt clausesProg2.txt				out-clausesProg2.xml
%buildpath% prog2.txt followsProg2.txt				out-followsProg2.xml
%buildpath% prog2.txt nextProg2.txt				out-nextProg2.xml

%buildpath% prog2.txt invalidBasicCallsProg2.txt 		out-invalidBasicCallsProg2.xml

%buildpath% prog3.txt multiple-basicProg3Iter3.txt 		out-multiple-basicProg3Iter3.xml
%buildpath% prog3.txt noclausesIter3Prog3.txt 			out-noclausesIter3Prog3.xml

echo FINISHED
pause
exit