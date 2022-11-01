PROG=tests
OBJDIR=.obj
CC=g++

CFLAGS = -Wall --std=c++1z -I ..

$(shell mkdir -p $(OBJDIR)) 

OBJS = $(OBJDIR)/tests.o $(OBJDIR)/Generator.o

$(PROG) : $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(PROG)

-include $(OBJS:.o=.d)

Generator.cpp: Generator.h
	wget https://raw.githubusercontent.com/jachappell/Random/master/src/Generator.cpp

Generator.h:
	wget https://raw.githubusercontent.com/jachappell/Random/master/include/Generator.h

Random.h: Generator.h
	wget https://raw.githubusercontent.com/jachappell/Random/master/include/Random.h

tests.cpp: Random.h

$(OBJDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $*.cpp -o $(OBJDIR)/$*.o
	$(CC) -MM $(CFLAGS) $*.cpp > $(OBJDIR)/$*.d
	@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp

clean:
	rm -rf $(PROG) $(OBJDIR) Generator.h Random.h Generator.cpp

