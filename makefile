SGDK_PATH := D:\Gamedev\Genesis\SGDK

build:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen

build-sgdk:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makelib.gen
