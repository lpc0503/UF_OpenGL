#!/bin/bash

"H:\Program Files\NVIDIA Corporation\Nsight Graphics 2022.5.0\host\windows-desktop-nomad-x64\ngfx.exe" --exe "`pwd`/cmake-build-debug/project2/project02.exe" --dir "`pwd`/project2" --activity "Frame Debugger" --launch-detached #--wait-hotkey