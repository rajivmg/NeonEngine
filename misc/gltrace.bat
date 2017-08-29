@echo off
pushd neon\data
apitrace trace -v ../build/neon_sdl.exe --output neon_sdl.trace
qapitrace neon_sdl.trace
del neon_sdl.trace
popd
