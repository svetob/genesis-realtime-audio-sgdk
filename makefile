SGDK_PATH := D:\Gamedev\Genesis\SGDK
MAME_PATH := D:\Gamedev\Genesis\Emulators\Mame
BLASTEM_PATH := D:\Gamedev\Genesis\Emulators\blastem-win32-0.6.2

OUT := out

# --- Build commands
clean:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen clean

build: clean
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen

build-sgdk:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makelib.gen

# --- Disassembly tools
asm:
	$(SGDK_PATH)\bin\make.exe -f $(SGDK_PATH)\makefile.gen asm

asm-motorola: asm
	@python3 tools/lst2motorola.py $(OUT)

disasm: 
	$(SGDK_PATH)\bin\objdump.exe -d -S $(OUT)/rom.out > $(OUT)/rom.dis

# --- Emulator: run
run-blastem: build asm-motorola
	$(BLASTEM_PATH)\blastem.exe $(OUT)\rom.bin

run-mame: build asm-motorola
	$(MAME_PATH)\mame.exe genesis -cart $(OUT)\rom.bin

# --- Emulator: debug
debug-blastem: build asm-motorola
	$(BLASTEM_PATH)\blastem.exe $(OUT)\rom.bin

# Add your mame breakpoints here as symbols. TODO: Move to some file under debug/
MAME_BP := pcmstream_sound_raw_playback_body_2pcm
gen-mame-debug-session:
	@python3 tools/gen_mame_debug_session.py $(MAME_BP)


debug-mame: build asm-motorola disasm gen-mame-debug-session
	$(MAME_PATH)\mame.exe genesis -cart $(OUT)\rom.bin -debug -debugscript debug/mame-session



# Format and lint using clang-format
SRC_FILES := $(wildcard src/*.c src/*.h src/*/*.c src/*/*.h src/*/*/*.c src/*/*/*.h)

format:
	clang-format -i $(SRC_FILES)

format-check:
	clang-format --dry-run --Werror $(SRC_FILES)
