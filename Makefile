NAME= ircserve

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98

SRC= main.cpp \
	 src/Server.cpp \
	 src/Parser.cpp \
	 src/Channel.cpp \
	 src/Client.cpp \
	 src/Commands.cpp\
	 src/commands/auth.cpp\
	 src/commands/help.cpp\
	 src/commands/nick.cpp\
	 src/commands/pass.cpp\
	 src/commands/user.cpp

OBJ= $(SRC:.cpp=.o)

HEADERS= ./include/Server.hpp ./include/Parser.hpp ./include/Channel.hpp ./include/Client.hpp ./include/Commands.hpp

all: $(NAME)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(NAME): $(OBJ) $(HEADERS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re


	 