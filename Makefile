CPPFLAGS = -std=c++11 -Wall -g3
LDLIBS = -lncurses
SRCS = game_loop.cpp game_object.cpp game_space.cpp spawn_object.cpp player.cpp timer.cpp util.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
ifeq ($(OS), Windows_NT)
EXE = game.exe
else
EXE = game
endif

# Rule to link obj files -> executable
ifeq ($(OS), Windows_NT)
# echo $(g++ $(CPPFLAGS) -o $@ $(OBJS) -lncurses -DNCURSES_STATIC)
$(EXE): $(OBJS)
	g++ -I/mingw64/include/ncurses $(CPPFLAGS) -o $@ $(OBJS) $(LDLIBS) -DNCURSES_STATIC
else
$(EXE): $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS) $(LDLIBS)
endif

-include $(DEPS)
# -MMD -MP creates the .d dependency files
ifeq ($(OS), Windows_NT)
.cpp.o:; g++ $(CPPFLAGS) -MMD -MP -c $< $(LDLIBS) -DNCURSES_STATIC
else
.cpp.o:; g++ $(CPPFLAGS) -MMD -MP -c $< 
endif

# Clean rule to remove generated files
clean:;	rm $(EXE) $(OBJS) $(DEPS)
