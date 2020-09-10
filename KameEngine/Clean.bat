echo deleting file

del "*.sln" /q /s
del "*.vcxproj " /q /s
del "*.user" /q /s

RMDIR ".vs" /q /s
RMDIR "bin" /q /s
RMDIR "bin-int" /q /s

echo Done!