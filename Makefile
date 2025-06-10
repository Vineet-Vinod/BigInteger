CPPFLAGS = -std=c++23 -O3 -Wall --pedantic -Wshadow -Wvla -Werror -Wunreachable-code
GMPFLAGS = -lgmp -lgmpxx
CPPFILES = main.cpp bigint.cpp
HEADERS = bigint.h
APP = bigint
GPP = g++

$(APP): $(CPPFILES)
	$(GPP) $(CPPFLAGS) $(CPPFILES) $(GMPFLAGS) -o $(APP)

clean:
	rm -rf $(APP)

time: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -DTIMER $(CPPFILES) $(GMPFLAGS) -o $(APP)

print: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -DPRINT $(CPPFILES) $(GMPFLAGS) -o $(APP)

print_time: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -DPRINT -DTIMER $(CPPFILES) $(GMPFLAGS) -o $(APP)

test: $(APP)
	./bigint $(ND1) $(ND2) $(OP) $(ITER)

memory: $(APP)
	valgrind --leak-check=full -s ./bigint $(ND1) $(ND2) $(OP) $(ITER)
