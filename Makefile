.PHONY: emulator
emulator: ./bin/nes.o ./bin/cpu.o ./bin/ppu.o ./bin/gui.o ./bin/memory.o ./bin/mapper000.o ./bin/ppuMemory.o ./bin/romLoader.o
	gcc -o ./bin/emu $^ -L ./lib/SDL/lib -lmingw32 -lSDL2main -lSDL2 -g

./bin/mapper000.o: ./src/mappers/mapper000.c
	gcc -c $< -o $@ -Wall -Ofast -g

./bin/%.o: ./src/%.c
	gcc -c $< -o $@ -Wall -Ofast -g

.PHONY: clean
clean:
	rm -f ./bin/*.o ./bin/emu