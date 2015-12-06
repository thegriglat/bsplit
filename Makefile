ROOTLIBS=-lCore -lRIO -lTree

bsplit: bsplit.cpp
	g++ -O3 -o bsplit -L$$(root-config --libdir) $$(root-config --cflags  --ldflags) $(ROOTLIBS) bsplit.cpp
