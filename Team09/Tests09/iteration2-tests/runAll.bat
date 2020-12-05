@echo off
title System Test Iteration 2 Queries

set "buildpath=..\..\Code09\build_win\x86-Debug\src\autotester\autotester"

%buildpath% prog3.txt callsProg3.txt out-Calls.xml
%buildpath% prog3.txt callsStarProg3.txt out-CallsStar.xml

%buildpath% prog3.txt parentProg3.txt out-Parent.xml
%buildpath% prog3.txt parentStarProg3.txt out-ParentStar.xml

%buildpath% prog3.txt followsProg3.txt out-Follows.xml
%buildpath% prog3.txt followsStarProg3.txt out-FollowsStar.xml

%buildpath% prog3.txt modifiesProg3.txt out-Modifies.xml
%buildpath% prog3.txt usesProg3.txt out-Uses.xml

%buildpath% prog3.txt nextProg3.txt out-Next.xml
%buildpath% prog3.txt nextStarProg3.txt out-NextStar.xml

%buildpath% prog3.txt noclausesProg3.txt out-NoClause.xml

%buildpath% prog3.txt patternProg3.txt out-Pattern.xml

%buildpath% prog3.txt withProg3.txt out-WithClause.xml

%buildpath% prog3.txt withSuchThatProg3.txt out-WithSuchThatCombi.xml

%buildpath% prog1.txt queries1.txt out-1.xml
%buildpath% prog2.txt queries2.txt out-2.xml

%buildpath% prog1invalidcyclic1.txt queries1.txt out-1invalidcyclic1.xml
%buildpath% prog1invalidcyclic2.txt queries1.txt out-1invalidcyclic2.xml
%buildpath% prog1invalidrepeatedproc.txt queries1.txt out-1invalidrepeatedproc.xml
%buildpath% prog1invalidnosuchproc.txt queries1.txt out-1invalidnosuchproc.xml

%buildpath% prog2invalidcyclic.txt queries2.txt out-2invalidcyclic.xml
%buildpath% prog2invalidrepeatedproc.txt queries2.txt out-2invalidrepeatedproc.xml
%buildpath% prog2invalidnosuchproc.txt queries2.txt out-2invalidnosuchproc.xml

echo FINISHED
pause
exit