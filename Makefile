CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -g 

csim: csim_main.o cache.o 
	$(CC) -o csim csim_main.o cache.o 

csim_main.o: csim_main.cpp cache.h 
	$(CC) $(CFLAGS) -c csim_main.cpp 

cache.o: cache.cpp cache.h 
	$(CC) $(CFLAGS) -c cache.cpp 

clean :
	rm -f csim depend.mak solution.zip *.o

# Use this target to create a zipfile that you can submit to Gradescope
.PHONY: solution.zip
solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile README.txt *.h *.cpp

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak









