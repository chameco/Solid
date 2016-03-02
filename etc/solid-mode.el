;;; solid-mode.el --- Major mode for writing Solid source code
;;
;; Copyright 2015 Eric James Michael Ritz
;;
;; Author: Eric James Michael Ritz <ejmr@plutono.com>
;; URL: https://github.com/ejmr/Solid/blob/master/etc/solid-mode.el
;; Version: 1.0.0
;; Package-Requires: ((emacs "24"))
;; Keywords: languages, programming
;;
;;
;;
;;; License:
;;
;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3 of the License,
;; or (at your option) any later version.
;;
;; This file is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this file; if not, write to the Free Software
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
;; 02110-1301, USA.
;;
;;
;;
;;; Commentary:
;;
;; This mode adds basic support to Emacs for writing software using
;; the Solid programming language, available at:
;;
;;     https://github.com/chameco/Solid
;;
;; This package associates Solid Mode with files that have the "*.sol"
;; filename extension.
;;
;;
;;
;;; Code:

(defconst solid-mode-version-number "1.0.0"
  "Solid Mode version number.

This number adheres to Semantic Versioning (`http://semver.org/').")

;;;###autoload
(define-generic-mode 'solid-mode
  ;; Comment Syntax
  '("#")
  ;; Keywords
  '("if" "while" "fn" "do" "end" "return" "$" "ns")
  ;; Operators and Other Syntax
  '(("=" . font-lock-operator)
    ("!!" . font-lock-operator)
    ("~" . font-lock-builtin)
    ("[" . font-lock-builtin)
    ("]" . font-lock-builtin)
    (";" . font-lock-builtin))
  ;; Files
  '("\\.sol$")
  ;; Other Functions
  nil
  ;; Docstring
  "Major mode for the Solid programming language.

`https://github.com/chameco/Solid'")

(provide 'solid-mode)

;;; solid-mode.el ends here
