rmdir /s /q build\win\release
md build\win\release
make MODE=release -B

Echo 
