
(defun mix (L1 L2)
    (cond
        ((or (null L1) (null L2)) nil)
        (t (mix-helper L1 L2))
    )
)
(defun mix-helper (x y &optional mixed)
    (cond