PROGRAM=testRhythmRational
MOREFLAGS=-Wall -g -Weffc++ -Wextra -Wpedantic -std=c++23 -D_GLIBCXX_ASSERTIONS 
${PROGRAM} : ${PROGRAM}.o ../RhythmRational.o
	$(CXX) $(MOREFLAGS) -std=c++20 -g -o $@ $<  ../RhythmRational.o 
${PROGRAM}.o : ${PROGRAM}.cc ../RhythmRational.h
	$(CXX) $(MOREFLAGS) -I.. -std=c++20 -Wall -Wextra -Weffc++ -g -c -o $@ $< 
../RhythmRational.o : ../RhythmRational.cc ../RhythmRational.h
	$(CXX) $(MOREFLAGS) -I.. -std=c++20 -Wall -Wextra -Weffc++ -g -c -o $@ $< 
