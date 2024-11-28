CPPFLAGS = -std=c++11 -Wall -g3 -lncurses
CPPFLAGSCOMPILE = -std=c++11 -Wall -g3
SRCS = game_loop.cpp game_object.cpp game_space.cpp spawn_object.cpp player.cpp timer.cpp util.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
EXE = game

export ASAN_OPTIONS="print_legend=0:log_path=asan"

# Rule to link obj files -> executable
$(EXE): $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS)

-include $(DEPS)
# -MMD -MP creates the .d dependency files
.cpp.o:; g++ $(CPPFLAGSCOMPILE) -MMD -MP -c $<

# Clean rule to remove generated files
clean:;	rm $(EXE) $(OBJS) $(DEPS)
