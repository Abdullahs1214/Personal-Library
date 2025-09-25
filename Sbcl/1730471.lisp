;QUESTION 1
;ducmentation
#|the function xcount takes a list L and checks how many elements exist in the list.
An occurance of nil is treated as an empty list and not counted in the overall count
Test cases: 
xcount '(a (a b) ((c) a))) -> 5
xcount nil) -> 0
(xcount '(nil a b ())) -> 2
|#
(defun xcount (L)
    (cond
        ; check if empty nested list has elements after
        ((eq 'nil L) 0)
        ((null (car L) ) (xcount (cdr L)))
        ((or (null L)) 0)
        ; edit this condition to check if more elements exist after nested
        ; if no extra elements
        ((and (null  (atom (car L))) (null (cdr L))) (xcount (car L)))
        ; if extra elements
        ((and (null  (atom (car L))) (not (null (cdr L)))) (xcount (append (car L) (cdr L))))
        (t (+ 1 (xcount (cdr L))))
    )
)

;QUESTION 2
;ducomentatin
#|  the function remove-function takes a list x and outputs a list  with the distinct elements in the list. 
the function calls the helper function contains duplicate to check if the element has appeared already
Test case: (remove-duplicate '(a b c a d b)) -> (C A D B) 
|#
(defun remove-duplicate (x)
  (cond 
    ((null x) nil)
    ((contains-duplicate (car x) (cdr x)) (remove-duplicate (cdr x)))
    (t (cons (car x) (remove-duplicate (cdr x))))
  )
)
#|The helper function contains-duplicate is given the car of the original list as the element were checking for (element), 
and the rest of the list as list. It checks if the car of list is equal to the element were checking and if yes it returns true
else it recursievly calls the cdr of list and checks the rest of the elemenets
|#
(defun contains-duplicate (element list)
  (cond
    ((null list) nil)
    ((equal element (car list)) t)
    (t (contains-duplicate element (cdr list)))
  )
)

;QUESTION 3a
;documentation
;last exmaple with nil dont work
#|The mix functions takes 2 lists, L1 and L2 and mixes the elements one by one of both lists.
it starts with the first element of L2, then L1 and then so on in that order. It does this by calling the mix helper function.
Test cases: (mix '(a b c) '(d e f)) -> (d a e b f c)
(mix '((a) (b c)) '(d e f g h))  -> (d (a) e (b c) f g h)
(mix '(1 2 3) nil) -> (1 2 3)
(mix '(1 2 3) '(nil)) -> (nil 1 2 3)
|#
(defun mix (L1 L2)
    (cond
      (t (mix-helper L1 L2))
    )
)
#|The mix-helper function takes in the 2 lists L1 and L2 as x and y respectively. 
It cakes the car of both lists, first L2 and appends to a third list called mixed which holds the output list.|#
(defun mix-helper (x y &optional mixed)
    (cond
        ((and (not (atom y)) (equal (car y) 'nil)) (mix-helper (cdr x) (cdr y) (append mixed (list 'nil (car x)))))
        ((null (car x)) (append mixed y))
        ((null (car y)) (append mixed x))
        (t (mix-helper (cdr x) (cdr y) (append mixed (list (car y) (car x)))))
    )
)

;my own reverse function
(defun my-reverse (lst &optional acc)
  (if (null lst)
      acc
      (my-reverse (cdr lst) (cons (car lst) acc))))

;QUESTION 3b
;documentation
#|The split function takes a list L and splits the elements into lists, elements that are even and odd in that order
It performs this by calling the split-helper function
Test cases: 
(split '(1 2 3 4 5 6)) -> ((2 4 6) (1 3 5)) 
(split '((a) (b c) (d e f) g h)) -> (((b c) g) ((a) (d e f) h)) 
(split '()) -> (nil nil)
|#
(defun split (L) 
  (split-helper L)
)
#| The split-helper takes the L1 list from split. 
It will then take the 1st and 2nd elements of that list, car L and cadr L and add them to even and odd lists respectively 
the function also utilizes my own reverse function defined at the bottom to reverse the list order 
and present the elements in the manner they appeared
|#
(defun split-helper (L &optional (even '()) (odd '()))
    (cond
        ((null L) (list (my-reverse even) (my-reverse odd)))
        ((null (car L)) (split-helper (cdr L) odd (cons (cadr L) even)))
        ((null (cadr L)) (split-helper (cdr L) even (cons (car L) odd)))
        (t (split-helper (cddr L) (cons (cadr L) even) (cons (car L) odd)))
    )
)

;QUESTION 5
;documentation
#|The substitute-exp function takes 2 elements E1 and E2, and a list of elements L. 
the function will replace every instance of E1 element with E2 in the list L
Test Cases: 
(substitute-exp 'a 'b '(a (a 2) (1 2 a))) -> (b (b 2) (1 2 b)) 
(substitute-exp '(a 2) 'b '(a (a 2) (1 2 a))) -> (a b (1 2 a))
(substitute-exp '(c e) '(1 2) '(a ((c e) 3) (b a) (c e) (a))) -> (a ((1 2) 3) (b a) (1 2) (a))
|#
(defun substitute-exp (E1 E2 L)
    (cond
    ((null L) nil)
    ((equal (car L) E1) (cons E2 (substitute-exp E1 E2 (cdr L))))
    (t (cons (if (not (atom (car L)))
                (substitute-exp E1 E2 (car L))
                (car L))
            (substitute-exp E1 E2 (cdr L))))    
    )
)
;QUESTION 6
;Ducomentation
#|the function my-count takes a list of elements L and returns the number of distinct elements in that list
it does this using the my-count-helper function passing the car L as the element to check for, 
and cdr L as the rest of the list to check
|#
(defun my-count (L)
  (if (null L)
      0
      (+ (if (my-count-helper (car L) (cdr L))
             0
             1)
  (my-count (cdr L)))))
#|The my-count-helper function takes an element and the rest of the list, 
and it recursively checks the list to see if the element is repeated 
|#
(defun my-count-helper (el list)
  (cond
    ((null list) nil)
    ((equal el (car list)) t)
    (t (my-count-helper el (cdr list)))))

