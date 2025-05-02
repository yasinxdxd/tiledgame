set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
:: call vcvarsall.bat
call %VCVARS% x64

lib /def:libTiledGameTVM.def /out:libTiledGameTVM.lib /machine:x64