;;
;; latex mapping of utilities for writing manuals
;;
(fset 'm-argline    "\\\\ \C-m\C-a \\> \\tt \M-f\M-f\C-f\C-f") 
(fset 'm-argdesc    "\\verb|\C-y| \\> \\parbox[t]{8.5cm}{  \\vspace{1.5mm}} \\\\ \M-b\M-b\M-b\C-b\C-b\C-b")
(fset 'm-verb       "\\verb|")
(fset 'm-deffun     "\\begin{flushleft}\C-m\\large \\bf \
%%%%%%%%%%%%%%%%%%%%%%%%%%%%% \C-m\C-y \
\C-m\\rule{120mm}{0.5mm}\C-m\\end{flushleft}\C-m\\begin{flushleft}\C-m\\bf Synopsis\C-m\\end{flushleft}\C-m\\begin{verbatim}\C-m\\end{verbatim}\C-m\
\\begin{flushleft}\C-m\\bf Arguments\C-m\\end{flushleft}\C-m\\begin{tabbing}\C-mXXXXXXXXX \\= \\kill\C-m\\end{tabbing}\C-m\
\\begin{flushleft}\C-m\\bf Returns\C-m\\end{flushleft}\C-m\\begin{flushleft}\C-m\\bf See also\C-m\\end{flushleft}\C-m\\vspace{2mm}\C-m\C-m")

(define-key tex-mode-map "\C-c|"  'm-verb)
(define-key tex-mode-map "\C-c+"  'm-deffun)
(define-key tex-mode-map "\C-c\\" 'm-argline)
(define-key tex-mode-map "\C-c>"  'm-argdesc)
