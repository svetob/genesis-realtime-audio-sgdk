SGDK_PATH := D:\Gamedev\Genesis\SGDK
MAME_PATH := D:\Gamedev\Genesis\Emulators\Mame

OUT := out

clean:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen clean

build: clean
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen

asm:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen asm

# To debug generated ASM, run `make clean build asm-motorola`
asm-motorola: asm
	@python3 tools/lst2motorola.py $(OUT)

build-sgdk:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makelib.gen

run-mame: build
	$(MAME_PATH)\mame.exe genesis -cart $(OUT)\rom.bin

debug-mame: build
	$(MAME_PATH)\mame.exe genesis -cart $(OUT)\rom.bin -debug

# Format and lint using clang-format
SRC_FILES := $(wildcard src/*.c src/*.h src/*/*.c src/*/*.h src/*/*/*.c src/*/*/*.h)

format:
	clang-format -i $(SRC_FILES)

format-check:
	clang-format --dry-run --Werror $(SRC_FILES)
