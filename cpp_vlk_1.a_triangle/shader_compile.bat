@echo off

REM Look for any .vert .geom or .frag file and compile them to SPIR-V
for /R "./shaders" %%f in (*.vert) do (
    echo  - %%~nf.vert
    call glslc.exe %%f -o "./shaders/compiled/%%~nf.vert.spv"
)
for /R "./shaders" %%f in (*.geom) do (
    echo  - %%~nf.geom
    call glslc.exe %%f -o "./shaders/compiled/%%~nf.geom.spv"
)
for /R "./shaders" %%f in (*.frag) do (
    echo  - %%~nf.frag
    call glslc.exe %%f -o "./shaders/compiled/%%~nf.frag.spv"
)