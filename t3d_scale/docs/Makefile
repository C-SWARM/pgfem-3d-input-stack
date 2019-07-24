LATEX		= latex
LATEX_OPTS	=

DVI_PS		= dvips
DVI_PS_OPTS	=

PS_PDF		= ps2pdf
PS_PDF_OPTS	=

OBJ		= guide

all: $(OBJ).pdf 

$(OBJ).dvi: $(OBJ).tex
	$(LATEX) $(LATEX_OPTS) $<
	$(LATEX) $(LATEX_OPTS) $<

$(OBJ).ps: $(OBJ).dvi
	$(DVI_PS) $(DVI_PS_OPTS) $<

$(OBJ).pdf: $(OBJ).ps
	$(PS_PDF) $(PS_PDF_OPTS) $<

clean:
	rm -rf *.ps *.pdf *.dvi *~ *.aux *.log
