CC = g++

CFLAGS = -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline					\
	-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default				\
	-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy            \
	-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers \
	-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing   \
	-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
	
INCLUDES = tree.h error.h utils.h input.h

SOURCES = main.cpp error.cpp tree.cpp utils.cpp input.cpp lexical_analysis.cpp parsing.cpp dump.cpp

OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = frontend

VPATH = ../include/

all: $(SOURCES) $(INCLUDES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ 

.cpp.o: $(INCLUDES) $(SOURCES)
	$(CC) $(CFLAGS) $< -o $@ 

.PHONY: clean

clean:
	rm -rf obj/* frontend.exe
	rm -rf *.o frontend
	rm -rf frontend.exe
	rm -rf frontend.exe.log
	rm -rf frontend.exe.log.dmp