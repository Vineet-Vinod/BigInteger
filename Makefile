CPPFLAGS = -std=c++23 -O3 -Wall --pedantic -Wshadow -Wvla -Werror -Wunreachable-code
GMPFLAGS = -lgmp -lgmpxx
CPPFILES = main.cpp bigint.cpp
HEADERS = bigint.h
APP = bigint
GPP = g++

$(APP): $(CPPFILES)
	$(GPP) $(CPPFLAGS) $(CPPFILES) $(GMPFLAGS) -o $(APP)

clean:
	/bin/rm -rf $(APP)

test: $(APP)
	./bigint $(ND1) $(ND2) $(OP) $(ITER)