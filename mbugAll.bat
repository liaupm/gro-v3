cd Utility
rmdir /s /q build\win\debug
md build\win\debug
make MODE=debug -B
cd ..

cd CellEngine
rmdir /s /q build\win\debug
md build\win\debug
make MODE=debug -B
cd ..

cd cellsignals
rmdir /s /q build\win\debug
md build\win\debug
make MODE=debug -B
cd ..

cd CellPro
rmdir /s /q build\win\debug
md build\win\debug
make MODE=debug -B
cd ..

Echo 