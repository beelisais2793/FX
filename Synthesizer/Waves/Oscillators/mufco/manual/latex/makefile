TARGET=mufco_manual
# make pdf by default
all: ${TARGET}.pdf

# it doesn't really need the .dvi, but this way all the refs are right
%.pdf : %.dvi
	latex ${TARGET}.tex
	latex ${TARGET}.tex
	latex ${TARGET}.tex
	dvipdfmx ${TARGET}.dvi

${TARGET}.dvi: ${TARGET}.tex
	@latex ${TARGET}.tex

# shortcut, so we can say "make ps"
ps: ${TARGET}.ps

${TARGET}.ps: ${TARGET}.dvi
	@dvips -t a4 ${TARGET}.dvi

clean:
	rm -f ${TARGET}.log
	rm -f ${TARGET}.aux
	rm -f ${TARGET}.ps
	rm -f ${TARGET}.dvi
	rm -f ${TARGET}.bbl
	rm -f ${TARGET}.blg
	rm -f ${TARGET}.toc
	rm -f ${TARGET}.out
	rm -f ${TARGET}.pdf

PHONY : ps all clean reallyclean
