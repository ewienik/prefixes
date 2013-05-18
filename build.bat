@rem @call mingwenv.bat
@call qt484msvc2010env.bat
cl /Zi /EHsc test.cpp prefixes.cpp /link /debug /out:prefixes.exe
