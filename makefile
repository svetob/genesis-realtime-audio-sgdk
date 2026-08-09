SGDK_PATH := D:\Gamedev\Genesis\SGDK
OUT := out

build:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen

asm:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen asm

# Convert GCC's AT&T syntax to Motorola syntax for 68K Counter extension
asm-motorola: asm
	@python3 tools/lst2motorola.py $(OUT)

build-sgdk:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makelib.gen
