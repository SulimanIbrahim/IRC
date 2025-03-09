NAME= ircserve

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98

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
	 

OBJ= $(SRC:.cpp=.o)

HEADERS= ./include/Server.hpp ./include/Parser.hpp ./include/Channel.hpp ./include/Client.hpp ./include/Commands.hpp ./include/Auth.hpp ./include/Bot.hpp

ifeq ($(shell uname), Linux)
	KQUEUE= -lkqueue
endif

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(KQUEUE)

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

all: $(NAME)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(KQUEUE)

$(NAME): $(OBJ) $(HEADERS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run nc


	 