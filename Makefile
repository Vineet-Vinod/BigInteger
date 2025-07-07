CPPFLAGS = -std=c++23 -Wall --pedantic -Wshadow -Wvla -Werror -Wunreachable-code
GMPFLAGS = -lgmp -lgmpxx
CPPFILES = main.cpp bigint.cpp
HEADERS = bigint.h
APP = bigint
GPP = g++

$(APP): $(CPPFILES)
	$(GPP) $(CPPFLAGS) -O3 $(CPPFILES) $(GMPFLAGS) -o $(APP)

debug: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -g -O0 $(CPPFILES) $(GMPFLAGS) -o $(APP)
	gdb --args ./bigint $(ND1) $(ND2) $(OP) $(ITER)

clean:
	rm -rf $(APP)

time: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -DTIMER -O3 $(CPPFILES) $(GMPFLAGS) -o $(APP)

print: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -DPRINT -O3 $(CPPFILES) $(GMPFLAGS) -o $(APP)

print_time: $(CPPFILES)
	$(GPP) $(CPPFLAGS) -DPRINT -DTIMER -O3 $(CPPFILES) $(GMPFLAGS) -o $(APP)

test: $(APP)
	./bigint $(ND1) $(ND2) $(OP) $(ITER)

memory: $(APP)
	valgrind --track-origins=yes --leak-check=full --show-reachable=yes -s ./bigint $(ND1) $(ND2) $(OP) $(ITER)
