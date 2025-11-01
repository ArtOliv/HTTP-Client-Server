# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Executables
S_EXEC = Server/meuservidor
C_EXEC = Client/meunavegador

# Source files
S_SRC = Server/meuservidor.c
C_SRC = Client/meunavegador.c

# Object files
S_OBJ = Server/meuservidor.o
C_OBJ = Client/meunavegador.o

all: $(S_EXEC) $(C_EXEC)

# Compile Server
$(S_EXEC): $(S_OBJ)
	$(CC) $(S_OBJ) -o $(S_EXEC) $(CFLAGS)

$(S_OBJ): $(S_SRC)
	$(CC) $(CFLAGS) -c $(S_SRC) -o $(S_OBJ)

# Compile Client
$(C_EXEC): $(C_OBJ)
	$(CC) $(C_OBJ) -o $(C_EXEC) $(CFLAGS)

$(C_OBJ): $(C_SRC)
	$(CC) $(CFLAGS) -c $(C_SRC) -o $(C_OBJ)

# Remove all
clean:
	rm -f Server/*.o Server/meuservidor Client/*.o Client/meunavegador

# Remove all and recompile
rebuild: clean all