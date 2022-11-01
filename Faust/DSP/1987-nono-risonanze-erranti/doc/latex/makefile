TEX = xelatex
BIB = biber
CMP = gspdfcomp.sh
OPN = open

src = 2022-nono-re.tex
bcf = 2022-nono-re

PDF = 2022-nono-re.pdf
WCN = pdftotext 2022-nono-re.pdf - | wc -c > includes/char.txt
CCN = pdftotext 2022-nono-re.pdf - | wc -w > includes/words.txt

publish :
	$(TEX) $(src) && $(BIB) $(bcf) && $(TEX) $(src) && $(TEX) $(src) && $(WCN) && $(CCN) && $(TEX) $(src) && $(CMP) $(PDF) && rm *.aux *.log *.toc *.run.xml *.bbl *.blg *.bcf *.fdb_latexmk *.fls *.idx *.ilg *.ind *.lof *.lot *.out

build :
	$(TEX) $(src) && $(BIB) $(bcf) && $(TEX) $(src) && $(TEX) $(src) && $(WCN) && $(CCN) && $(TEX) $(src)

step :
	$(TEX) $(src) $(OPN) $(PDF)

.PHONY: clean
clean :
	rm *.aux *.log *.toc *.run.xml *.bbl *.blg *.bcf *.fdb_latexmk *.fls *.idx *.ilg *.ind *.lof *.lot *.pdf *.out
