RED := '\033[0;31m'
YELLOW := '\033[0;33m'
NC := '\033[0m' #no color

files := out/main.o out/parts.o out/load_save.o

compileArgs = -O3 

#default
default:
	@echo -e no option specified

#recompile everything
recompile: refresh program

refresh:
	@touch src/*

clear:
	@clear

#linking
program: clear ${files}
	@echo -e ${RED}linking program${NC}
	@g++ -Wall -Wextra -pedantic -o drawer ${files} -lncursesw

#compiling
out/main.o: src/main.cpp
	@echo -e ${YELLOW}compiling main.cpp${NC}
	@g++ -Wall -Wextra -pedantic ${compileArgs} -c src/main.cpp
	@mv main.o out/

out/parts.o: src/parts.cpp
	@echo -e ${YELLOW}compiling parts.cpp${NC}
	@g++ -Wall -Wextra -pedantic ${compileArgs} -c src/parts.cpp
	@mv parts.o out/

out/load_save.o: src/load_save.cpp
	@echo -e ${YELLOW}compiling load_save.cpp${NC}
	@g++ -Wall -Wextra -pedantic ${compileArgs} -c src/load_save.cpp
	@mv load_save.o out/

#updating files
src/main.cpp:
	@touch src/main.cpp

src/parts.cpp:
	@touch src/parts.cpp

src/load_save.cpp:
	@touch src/load_save.cpp
