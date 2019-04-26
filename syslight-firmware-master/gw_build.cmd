@echo off

cd .\zephyr

call zephyr-env.cmd

cd ..

if exist "build_gw" (
	cd build_gw
	cmake -G"Eclipse CDT4 - Ninja" -DCMAKE_ECLIPSE_VERSION=4.9 ..\syslight-gw
)

cmd 