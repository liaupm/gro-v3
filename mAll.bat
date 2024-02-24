cd Utility
rmdir /s /q build\win\release
md build\win\release
make MODE=release -B
cd ..

cd CellEngine
rmdir /s /q build\win\release
md build\win\release
make MODE=release -B
cd ..

cd cellsignals
rmdir /s /q build\win\release
md build\win\release
make MODE=release -B
cd ..

cd CellPro
rmdir /s /q build\win\release
md build\win\release
make MODE=release -B
cd ..

Echo 