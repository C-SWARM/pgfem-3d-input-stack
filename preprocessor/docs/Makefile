base = con3d++_guide


all: fast

fast:
	latex $(base).tex

refs:
	bibtex $(base)

full: fast # refs
#	latex $(base).tex
#	latex $(base).tex

convert:
	dvips $(base).dvi
	ps2pdf $(base).ps

clean:
	rm -rf $(base).{dvi,ps,log,aux,pdf,out,bbl,blg,cpt}
