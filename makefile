SGDK_PATH := D:\Gamedev\Genesis\SGDK
MAME_PATH := D:\Gamedev\Genesis\Emulators\Mame
BLASTEM_PATH := D:\Gamedev\Genesis\Emulators\blastem-win32-0.6.2

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

disasm: 
	$(GDK)/bin/m68k-elf-objdump -d -S $(OUT)/rom.out > $(OUT)/rom.dis

build-sgdk:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makelib.gen

run-blastem: build asm-motorola
	$(BLASTEM_PATH)\blastem.exe $(OUT)\rom.bin

run-mame: build asm-motorola
	$(MAME_PATH)\mame.exe genesis -cart $(OUT)\rom.bin

debug-mame: build asm-motorola
	$(MAME_PATH)\mame.exe genesis -cart $(OUT)\rom.bin -debug



# Format and lint using clang-format
SRC_FILES := $(wildcard src/*.c src/*.h src/*/*.c src/*/*.h src/*/*/*.c src/*/*/*.h)

format:
	clang-format -i $(SRC_FILES)

format-check:
	clang-format --dry-run --Werror $(SRC_FILES)
