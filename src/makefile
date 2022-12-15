#compilation du code Cheerios

# TODO peut etre essayer de linker les libraries staticquement pour faire tel que les gens peuv run le code sans avoir la librarie GSL

# Options de compilation																													  v optimisation flag -Ofast desconseille de utiliser dans les codes paralele et les codes pas tres robust/teste
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wcast-align  -Wcast-qual -Wunreachable-code -Wfloat-equal -Wunsafe-loop-optimizations -O2 #-O 1 2 3 fast g s# -Wconversion  -fsanitize=address -Wstrict-prototypes#-fsanitize=thread -fsanitize=undefined -fstack-protector
#CFLAGS = -g																													# 			  ^  recomende de le laisser on -O2 car apres on voix pas de difference

# nom des librairies a lier     v le emplacement de la librarie gsl (default location) # optionel comme dest dans lib et C regarde dans lib au tout les cas cest mis au cas ou ya des gens qui ont change lemplacement et pour les faciliter
LNAME = -lm -lgsl -lgslcblas -L/usr/local/lib

# nom de l'executable
EXEC=cheerio.exe

#liste des fichiers sources
#SRC=  main.c lectureDonnee.c calculs.c
SRC= $(wildcard *.c) # $(wildcard */*.cpp) #si on peux regarde dans les dossiers 
#liste des fichiers objets
OBJ= $(SRC:.c=.o)

# action declenchee par defaut
all: $(EXEC)

# compilation des fichiers sources en fichiers objets  
%.o: %.c
	$(CC) $(CFLAGS) -c $^

# compilation de l exectubale a partir des fichier sources
$(EXEC) : $(OBJ)
	$(CC)  -o $@ $^ $(LNAME)
	chmod +x $(EXEC)
	@echo compilation terminee
	@echo "faites \"./cheerio.exe && make visualise\" pour visualiser le programme en temps reel et pour le sauvgarder en gif"
# les @ sont ici pour silencer les appels. Comme ca on voix que les resultats de ces fonctions mais pas lappel avel le echo 

clean:
	rm -rf *.o
	rm -rf donnees.txt
	ls

vis: 
	python3 visualisation.py

gif:
	python3 visualisationMakeGIF.py 

visualise: 
	python3 visualisationMakeGIF.py & python3 visualisation.py && fg

make present:
	make clean && make && time ./cheerio.exe && make vis
#   le premier cest pour visualise tout suite car le sauvgarder en gif ca prends pas mal de temps
#https://stackoverflow.com/questions/3004811/how-do-you-run-multiple-programs-in-parallel-from-a-bash-script
# prog1 & prog2 && fg
# This will:
# Start prog1.
# Send it to background, but keep printing its output.
# Start prog2, and keep it in foreground, so you can close it with ctrl-c.
# When you close prog2, you'll return to prog1's foreground, so you can also close it with ctrl-c.


# Peux etre utile pour voir les differente version de os et de faire des changements
# # pour voir quel system on a 
# ifeq ($(OS),Windows_NT)
#     CCFLAGS += -D WIN32
#     ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
#         CCFLAGS += -D AMD64
#     else
#         ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
#             CCFLAGS += -D AMD64
#         endif
#         ifeq ($(PROCESSOR_ARCHITECTURE),x86)
#             CCFLAGS += -D IA32
#         endif
#     endif
# else
#     UNAME_S := $(shell uname -s)
#     ifeq ($(UNAME_S),Linux)
#         CCFLAGS += -D LINUX
#     endif
#     ifeq ($(UNAME_S),Darwin)
#         CCFLAGS += -D OSX
#     endif
#     UNAME_P := $(shell uname -p)
#     ifeq ($(UNAME_P),x86_64)
#         CCFLAGS += -D AMD64
#     endif
#     ifneq ($(filter %86,$(UNAME_P)),)
#         CCFLAGS += -D IA32
#     endif
#     ifneq ($(filter arm%,$(UNAME_P)),)
#         CCFLAGS += -D ARM
#     endif
# endif