;;
;; latex mapping of utilities for writing manuals
;;
(fset 'm-argdesc    "\\AD{\C-y}{}\C-b")
(fset 'm-verb       "\\verb|")
(fset 'm-deffun     "\\FD{\C-y}\C-m\\SY\C-m\\begin{verbatim}\C-m\C-m\\end{verbatim}\C-m\\AR\C-m\\begin{tabbing}\C-mXXXXXXXXX \\= \\kill\C-m\C-m\\end{tabbing}\C-m\\RE\C-m\\SA\C-m\\vspace{2mm}\C-m\C-m")

(define-key tex-mode-map "\C-c|"  'm-verb)
(define-key tex-mode-map "\C-c+"  'm-deffun)
(define-key tex-mode-map "\C-c>"  'm-argdesc)
