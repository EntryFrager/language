CC = g++

CFLAGS = -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline					\
	-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default				\
	-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy            \
	-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers \
	-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing   \
	-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

INCLUDES = ../tree/tree.h ../Include_file/error/error.h ../Include_file/utilities/utils.h inc/input.h inc/syntax_error.h

SOURCES = main.cpp input.cpp lexical_analysis.cpp parsing.cpp ../tree/tree.cpp ../tree/dump.cpp ../Include_file/error/error.cpp ../Include_file/utilities/utils.cpp

OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = frontend.exe

VPATH = ./src/

all: $(SOURCES) $(INCLUDES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o: $(INCLUDES) $(SOURCES)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf *.o frontend
	rm -rf ../tree/*.o
	rm -rf ../Include_file/error/*.o
	rm -rf ../Include_file/utilities/*.o
	rm -rf frontend.exe

clean_debug:
	rm -rf ../debug/
