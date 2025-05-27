set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
:: call vcvarsall.bat
call %VCVARS% x64

echo Generating Debug lib...
lib /def:libTiledGameTVM_d.def /out:libTiledGameTVM_d.lib /machine:x64