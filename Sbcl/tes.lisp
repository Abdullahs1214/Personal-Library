;factorial
(defun fact (N)
    (if (or (eq N 0) (eq N 1) (eq N -1))
        1
        (* N (fact (- N 1)))
    )
)
;(write (fact -1))
;(terpri)

;xcount
(defun xcount (a)
    (if (null a)
        0
        (+ 1 (xcount (cdr a)))
    )
)
(write (xcount '(1 2 3 4)))

; cond
;(write (car'((2 3))))

(defun xequal (S1 S2)
     (cond
           ((and (atom S1) (atom S2)) (eq S1 S2))
           ((and (atom S1) (not (atom S2))) NIL)
           ((and (atom S2) (not (atom S1))) NIL)
           (t (and (xequal (car S1) (car S2))
                   (xequal (cdr S1) (cdr S2))))))
;(write (xequal '(1) '(4)))


;(defun xcount (L)
;    (write (null (caar L)))
;)
;(xcount '(1 2 3 4))
(terpri)