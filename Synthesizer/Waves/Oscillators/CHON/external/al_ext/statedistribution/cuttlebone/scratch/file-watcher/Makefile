.SUFFIXES:
.PHONY: _

FRAMEWORKS += -framework CoreServices
LINKER += $(FRAMEWORKS)
LINKER += -lc++
CXX11 += -std=c++11
CXX11 += -stdlib=libc++

_: watch

watch: main.o FileWatcherMacos.o
	clang -o $@ $^ $(LINKER)

%.o :: %.mm
	clang -x objective-c++ $(CXX11) -c $< -o $@
# this leads to linker warning
#clang -x objective-c++ $(CXX11) -c $< -o $@ $(FRAMEWORKS)

%.o :: %.cpp
	clang -x c++ $(CXX11) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf watch main.o FileWatcherMacos.o
