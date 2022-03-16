################################################################################
# Makefile
# Descrição:
# forma simples e intuitiva de compilar o código, de fácil edição.
################################################################################
# Nomes de variaveis usadas ao longo do Makefile 
################################################################################

# variaveis do compilador
CC = gcc
MODE = -g
CFLAGS = -Wall -pedantic -Wextra

# Variaveis para limpeza de ficheiros inerentes a geracao do executavel 
C_TRASH = *.o *.so
OBJS_CTRASH = $(addprefix , $(C_TRASH))
SO_CTRASH = $(addprefix , $(C_TRASH))

# Nomes para os executaveis 
RING_NAME = ring.proj

PROG_NAME = $(RING_NAME) 
# Nomes para as fontes e objetos 

RING_OBJS = ring.o
RING_SOURCES = ring.c 

################################################################################
################################################################################
.PHONY: default  
default: $(PROG_NAME)


################################################################################
# Compilacao e geracao de executaveis                               
################################################################################ 

$(RING_NAME):  $(RING_OBJS)
	$(CC) $(MODE) -L. $(CFLAGS) -o $@ $(RING_OBJS) 


ring.o: $(RING_SOURCES)
	$(CC) $(MODE) $(CFLAGS) -c $< -o $@ 

################################################################################
# limpeza de ficheiros inerentes a geracao do executavel  
################################################################################
.PHONY: clean cleanall
 
cleanall: clean cleanExe  

clean:
	rm -rf $(C_TRASH)  

cleanExe:
	rm -rf $(PROG_NAME)

