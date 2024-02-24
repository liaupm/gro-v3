cd Utility
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