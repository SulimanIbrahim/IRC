NAME= ircserve

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98

# Detect OS for kqueue handling
OS := $(shell uname -s)
ifeq ($(OS), Linux)
	# Check if libkqueue is installed
	IFKQUEUE := $(shell if [ -f /usr/local/include/kqueue/sys/event.h ]; then echo "found"; else echo "missing"; fi)
	CXXFLAGS += -I/usr/local/include
	LDFLAGS = -L/usr/local/lib -lkqueue
endif

SRC= main.cpp \
	src/Server.cpp \
	src/Parser.cpp \
	src/Channel.cpp \
	src/Client.cpp \
	src/Commands.cpp\
	src/Auth.cpp\
	src/helpCommands/authMsg.cpp\
	src/helpCommands/help.cpp\
	src/Bot.cpp \
	src/BotCommand.cpp \
	 

OBJ= $(SRC:.cpp=.o)

HEADERS= ./include/Server.hpp ./include/Parser.hpp ./include/Channel.hpp ./include/Client.hpp ./include/Commands.hpp ./include/Auth.hpp ./include/Bot.hpp

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(NAME) 6667 123

nc:
	nc 127.0.0.1 6667 

irssi1:
	docker run -it --name irssi-sender -e TERM -u $(shell id -u):$(shell id -g) \
	--log-driver=none \
	-v ${HOME}/.irssi-sender:/home/user/.irssi \
	irssi

irssi2:
	docker run -it --name irssi-receiver -e TERM -u $(shell id -u):$(shell id -g) \
	--log-driver=none \
	-v ${HOME}/.irssi-receiver:/home/user/.irssi \
	irssi

rmirssi1:
	docker rm -f irssi-sender 2>/dev/null

rmirssi2:
	docker rm -f irssi-receiver 2>/dev/null


$(NAME): $(OBJ) $(HEADERS)

clean:
	rm -f $(OBJ)
	rm -f IRC_tester

fclean: clean
	rm -f $(NAME)
	- make rmirssi1
	- make rmirssi2

re: fclean all

test:
	c++ -o IRC_tester IRC_tester.cpp
	./IRC_tester
# Install libkqueue if needed (Linux only)
install-deps:
ifeq ($(OS), Linux)
ifeq ($(IFKQUEUE), missing)
	@echo "\033[1;33mInstalling libkqueue from GitHub...\033[0m"
	@apt update && apt install -y g++ gcc make cmake git 
	@if [ ! -d libkqueue ]; then git clone https://github.com/mheily/libkqueue.git; fi
	@cd libkqueue && mkdir -p build && cd build && cmake .. && make
	@echo "\033[1;33mInstalling libkqueue to system...\033[0m"
	@cd libkqueue/build && make install
	@ldconfig
	@echo "\033[1;32mlibkqueue installed successfully\033[0m"
else
	@echo "\033[1;32mlibkqueue is already installed\033[0m"
endif
endif

# Make all depend on install-deps
all: install-deps $(NAME)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

.PHONY: all clean fclean re run nc install-deps test