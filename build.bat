@rem @call mingwenv.bat
@call qt484msvc2010env.bat
@rem cl /Zi /EHsc test.cpp prefixes.cpp /link /debug /out:prefixes.exe
cl /EHsc /O2 test.cpp prefixes.cpp /link /out:prefixes.exe
