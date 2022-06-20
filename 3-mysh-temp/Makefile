# use GCC to compile 
CC = gcc 

CPPFLAGS = $(INCLUDE)
LDFLAGS = -lm
CFLAGS = 
# CFLAGS = -DDEBUG

# directory where files are stored
TARGETNAME = mysh
TARGDIR = .
SRCDIRS = ./src
OBJDIR = ./build
INCDIRS = ./include 

# files require for compilation
TARGET = ./mysh
SRCS = ./src/main.c ./src/my_buildin_cmd.c ./src/submain.c ./src/pipe.c ./src/redirect.c ./src/my_execve.c ./src/my_ampersand.c ./src/signal_handler.c
OBJS = ./build/main.o ./build/my_buildin_cmd.o ./build/submain.o ./build/pipe.o ./build/redirect.o ./build/my_execve.o ./build/my_ampersand.o ./build/signal_handler.o
INCLUDE = -I ./include
DEPS = $(OBJS:.o=.d)

# for make clean
# RM = rm -rf

# if exist clean file, you can execute clean command
.PHONY: all clean

# for first target 
all: $(TARGETNAME)

# to make bufcache, you need all of .o file
$(TARGETNAME): $(OBJS)
	$(CC) -o $(TARGDIR)/$@ $^ $(LDFLAGS)

# to make .o file, you need .c file
$(OBJDIR)/%.o: $(SRCDIRS)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

clean: 
	$(RM) $(TARGET) $(OBJS) $(DEPS)

# additionally, bufcahce.o depends on headher.h
-include $(DEPS)