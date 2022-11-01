# XeLaTeX article template

This article uses *XeLaTeX* to type natively in _Alegreya_ font, _Andale Mono_
for code blocks and [Libertinus Math](https://github.com/alif-type/libertinus)
for equations.

It uses a makefile typesetting to builds bibliography and counts words and
characters on built pdf.

```bash
brew cask install pdftotext
```


## characteristics

This template provides a two-columns article with a focus on fonts and
typographic issues. It employs strategies to count characters and words in the
produced PDF.

The abstract is an external file (placed into `includes/abstract.txt`) imported
to have text access without formatting rapidly. It is useful to strictly look at
words count during the writing or for the standalone forwarding for the abstract
reviews.

### file structure

```text
.
├── 2020-GS-ARTICLE.tex
├── LICENSE
├── README.md
├── gs2020.tex
├── gscom.sh
├── img
│   ├── image1.png
│   └── image2.jpg
├── includes
│   ├── abstract.txt
│   ├── bibliography.bib
│   ├── char.txt
│   └── words.txt
└── makefile
```

## usage

The main file is named `2020-GS-ARTICLE.tex` it includes a call to `gs2020.tex`
as an external style. You can create different stylesheets to call at necessity.

You can run LaTeX in multiple ways. I prefer to run a makefile into the terminal
to automate some passages. Bibliography printing requires numerous compilations
to connect the references, the same for the strategy adopted to count words and
characters on the final PDF.

Type

```bash
make
```

to generate all the passages to obtain the PDF.

Looking at the makefile there are three strategies:

 - *publish* the one you implicitly invoke with the `make` command (so `make`
   and `make publish` are equivalent). After the building process, it removes
   all the intermediate files. This is the longest-running process, not useful
   for middle writings, but for the publishing phase;
 - *build* the process of complete building without removing the intermediate
   file. This process is useful for the reconnection of references during
   intermediate writings;
 - *step* is the single step advancing mode, useful during writings, fast
   because it does not reconnect references, not launch the word-count strategies.

So

```bash
make step
```

is the command to the unreferenced PDF production.

```bash
make build
```

is the command to produce a full PDF, the intermediate files remain for the next build steps.

```bash
make publish
```

to produce a full PDF and clean the folder from the intermediate files.

### counter strategy

The count of characters and words is processed by pdftotext on the PDF
generating two separate files containing respectively words counts and character
counts. It is necessary then a multiple generation strategy to obtain full PDF
with cross-references, bibliography (BibTeX assumes three compilation processes)
and counters. Both counters are printed at the footer of each page, alongside
the page number and the building date.

## github usages and bash script

Using the `template` mode offered by GitHub you can generate future articles repository
by this starting point.

The following `gscom` bash script is self-explained. It is the _lazy mode on_,
with the single command `bash gscom.sh commitname` (where _commitname_ is the name you
want assign to the commit, so channge it!) you are doing all the three listed `git` commands

```bash
#!/usr/bin/env bash

# use it by terminal typing:
# bash gscom.sh commitname

git status
git add .
git commit -am "$1"
```
