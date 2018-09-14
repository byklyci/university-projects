#lang scheme
; compiling: yes
; complete: yes
; 2014400123


; (card-color one-card) -> symbol?
; one-card : pair?
;
; Returns the color of one-card.
; Hearts (H) and Diamonds (D) are red. Spades (S) and Clubs (C) are black.
;
; Examples:
; > ( card-color '(H . A) )
; => 'red
; > ( card-color '(S . 10) )
; => 'black
(define (card-color one-card)
  (case (car one-card)
    ((H D) 'red)
    ((S C) 'black)))

; (card-rank one-card) -> number?
; one-card : pair?
;
; Returns the rank of one-card.
; 11 for ace (A), 10 for king (K) and queen (Q) and jack (J)
; 10 to 2 for 10 to 2
;
; Examples:
; > ( card-rank '(H . A) )
; => 11
; > ( card-rank '(S . 10) )
; => 10
(define (card-rank one-card)
  (case (cdr one-card)
    ((A) 11)
    ((K Q J) 10)
    (else (cdr one-card)))) ; 10 to 2

; (all-same-color list-of-cards) -> boolean?
; list-of-cards : list?
;
; Returns #t if all the cards in list-of-cards have same color, #f otherwise.
;
; Examples:
; > ( all-same-color '((S . 3) (S . 2) (S . A) (C . A) (C . Q) (C . J)) )
; => #t
; > ( all-same-color '((H . 3) (H . 2) (H . A) (D . A) (D . Q) (C . J)) )
; => #f
(define (all-same-color list-of-cards)
  (cond ((equal?
          list-of-cards
          (filter  ; returns a list of cards with red color
           (lambda (one-card)
             (eqv? 'red (card-color one-card)))
           list-of-cards))
         #t)
        ((equal?
          list-of-cards
          (filter ; returns a list of cards with black color
           (lambda (one-card)
             (eqv? 'black (card-color one-card)))
           list-of-cards)) 
         #t)
        (else #f)))

; (fdraw list-of-cards held-cards) -> list?
; list-of-cards : list?
; held-cards : list?
;
; Returns a new list of held-cards after the action draw is taken.
;
; Examples:
; > ( fdraw '((H . 3) (H . 2) (H . A) (D . A) (D . Q) (D . J)) '())
; => '((H . 3))
; > ( fdraw '((H . 3) (H . 2) (H . A) (D . A) (D . Q) (D . J)) '((S . 3) (S . 2) (S . A)))
; => '((S . 3) (S . 2) (S . A) (H . 3))
(define (fdraw list-of-cards held-cards)
  (append held-cards (list (car list-of-cards))))

; (fdiscard list-of-cards list-of-moves goal held-cards) -> list?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; held-cards : list?
;
; Returns a new list of held-cards after the action discard is taken.
;
; Examples:
; > ( fdiscard '((C . 3) (C . 2) (C . A) (S . J) (S . Q) (H . J)) '(draw draw draw discard) 66 '((H . 3) (H . 2) (H . A) (D . A) (D . Q) (D . J)))
; => ((H . 3) (H . A) (D . A) (D . Q) (D . J))
(define (fdiscard list-of-cards list-of-moves goal held-cards)
  (remove (card-discard list-of-cards list-of-moves goal held-cards) held-cards))

; (card-discard list-of-cards list-of-moves goal held-cards) -> pair?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; held-cards : list?
;
; Returns the card that will be discarded.
;
; Examples:
; > (card-discard '((C . 3) (C . 2) (C . A) (S . J) (S . Q) (H . J)) '(draw draw) 40 '((H . 3) (H . 2) (D . Q) (D . J)))
; => (H . 2)
(define (card-discard list-of-cards list-of-moves goal held-cards)
  ;if  100.000 < (num of heldcards)^(num of discard)
  ;precaution for large input
  (if (< 100000 (expt (length held-cards) (count (lambda (x) (eq? x 'discard)) list-of-moves)))
      (car held-cards)  ; discard the first card
      ; else find the card that will be discarded bruteforce
      (list-ref held-cards  ;return the card that result in minimum score
                (min-position   ;position of minimum score for discards
                 (map (lambda (discarded-held-cards) ; for each possible discard calculate score 
                        (calc-score (find-held-cards-2 (find-steps-h list-of-cards list-of-moves goal discarded-held-cards)
                                                       discarded-held-cards)
                                    goal))
                      (all-possible-discards held-cards))))))

; (min-position l) -> number?
; l : list?
;
; Returns the index of minimum number in the list.
; Precondition: list contains only numbers
; Index 0 is the first element.
;
; Examples:
; > (min-position '(6 9 6 8 5))
; => 4
; > (min-position '(81 7 57 71))
; => 1
(define (min-position l)
  (define (min-position2 count pos l)
    (cond ((null? l) #f)
          ((= 1 (length l)) pos)
          ((< (car l) (cadr l))
           (min-position2 (+ count 1) pos (cons (car l) (cddr l))))
          (else (min-position2 0 (+ count pos 1) (cons (cadr l) (cddr l))))))
  (min-position2 0 0 l))

; (all-possible-discards held-cards) -> list?
; held-cards : list?
;
; Returns a list which contains each card in heldcards discarded once.
;
; eg: heldcards is (A B C)
; function returns ((B C) (A C) (A B))
; for the first list in returned list A is discarded
; for the second list in returned list B is discarded
; for the last one C is discarded
;
; Examples:
; > (all-possible-discards '((H . 3) (H . 2) (H . A)))
; => (((H . 2) (H . A)) ((H . 3) (H . A)) ((H . 3) (H . 2)))
(define (all-possible-discards held-cards)
  (map (lambda (element)
         (remove element held-cards))
       held-cards))

; (find-steps list-of-cards list-of-moves goal) -> list?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
;
; Wrapper function for find-steps-h
; Returns a list of steps that is a list of list of moves and
; corresponding cards along the game.
; The game has 4 endings:
; 1: If the list of moves is empty, the game ends.
; 2: If the current move is draw and the card-list is empty, the game ends.
; 3: If the current move is discard and the held-cards is empty, the game ends.
; 4: At any time if the sum of the values in the held-cards is greater than the
;    goal after the last move, the game ends.
;
; Examples:
; > ( find-steps '((H . 3) (H . 2) (H . A) (D . J) (D . Q) (C . J)) '(draw draw draw discard) 16 )
; => '((draw (H . 3)) (draw (H . 2)) (draw (H . A)) (discard (H . 3)))
; (Note: (H . 3) is discarded for discard move. My fdiscard function may discard a different card.)
(define (find-steps list-of-cards list-of-moves goal)
  (find-steps-h list-of-cards list-of-moves goal '() ))

; (find-steps-h list-of-cards list-of-moves goal held-cards) -> list?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; held-cards : list?
;
; Same as find-steps. Only difference is that this function stores held-cards.
;
; Examples:
; > ( find-steps-h '((H . 3) (H . 2) (H . A) (D . J) (D . Q) (C . J)) '(draw draw draw discard) 16 '() )
; => '((draw (H . 3)) (draw (H . 2)) (draw (H . A)) (discard (H . 3)))
; (Note: (H . 3) is discarded for discard move. My fdiscard function may discard a different card.)
(define (find-steps-h list-of-cards list-of-moves goal held-cards)
  (if (> (calc-playerpoint held-cards) goal)
      '() ; End 4
      (if (null? list-of-moves)
          '() ; End 1
          (if  (eqv? 'draw (car list-of-moves)) ; draw
               (if (null? list-of-cards)
                   '() ; End 2
                   (cons
                    (list 'draw (car list-of-cards))
                    (find-steps-h (cdr list-of-cards) (cdr list-of-moves) goal (fdraw list-of-cards held-cards))) )
               ;else   discard
               (if (null? held-cards)
                   '() ; End 3
                   (let ((discarded-card (card-discard list-of-cards (cdr list-of-moves) goal held-cards) ))
                     (cons
                      (list 'discard discarded-card)
                      (find-steps-h list-of-cards (cdr list-of-moves) goal (remove discarded-card held-cards)) )))))))


; (find-held-cards list-of-steps) -> list?
; list-of-steps : list?
;
; Returns the list of held-cards after the list-of-steps is applied.
; The list of held-cards is initially empty.
;
; Examples:
; > ( find-held-cards '((draw (H . 3)) (draw (H . 2)) (draw (H . A)) (discard (H . 3))) )
; => '((H . 2) (H . A))
(define (find-held-cards list-of-steps)
  (foldl ; for each step in list of steps
   (lambda (step held-cards)
     (if (eqv? 'draw (car step)) ; if step is draw
         (append held-cards (cdr step)) ; add card to held-cards
         (remove (car (cdr step)) held-cards) )) ; remove card from held-cards
   '()
   list-of-steps) )

; (find-held-cards-2 list-of-steps held-cards) -> list?
; list-of-steps : list?
; held-cards : list?
;
; Returns the list of held-cards after the list-of-steps is applied.
; The list of held-cards is initially not empty.
;
; Examples:
; > ( find-held-cards-2 '((draw (H . 3)) (draw (H . 2)) (draw (H . A)) (discard (H . 3))) '((H . 4)) )
; => '((H . 4) (H . 2) (H . A))
(define (find-held-cards-2 list-of-steps held-cards)
  (foldl ; for each step in list of steps
   (lambda (step held-cards)
     (if (eqv? 'draw (car step)) ; if step is draw
         (append held-cards (cdr step)) ; add card to held-cards
         (remove (car (cdr step)) held-cards) )) ; remove card from held-cards
   held-cards
   list-of-steps) )

; (calc-playerpoint list-of-cards) -> number?
; list-of-cards : list?
;
; Calculates and returns the corresponding playerpoint for list-of-cards.
; Playerpoint is the sum of the values of held-cards.
;
; Examples:
; > ( calc-playerpoint '((H . A) (H . 3) (H . 2) (D . Q) (D . J) (C . J)) )
; => 46
(define (calc-playerpoint list-of-cards)
  (foldl + 0 (map card-rank list-of-cards)))

; (calc-score list-of-cards goal) -> number?
; list-of-cards : list?
; goal : number?
;
; Calculates and returns finalscore using inputs list-of-cards and goal.
; First of all, prescore for the player is calculated. The finalscore is
; calculated with prescore according to held-cards. The finalscore is
; the prescore, if there are different colored cards in held-cards list.
; If all the held-cards are same color, the finalscore is the half of
; the prescore (rounded down with integer division).
;
; Examples:
; > ( calc-score '((H . 3) (H . 2) (H . A) (D . J) (D . Q) (C . J)) 50 )
; => 4
; > ( calc-score '((H . 3) (H . 2) (H . A) (D . J) (D . Q) (C . J)) 16 )
; => 150
(define (calc-score list-of-cards goal)
  (if (all-same-color list-of-cards)
      (quotient (calc-prescore list-of-cards goal) 2)
      (calc-prescore list-of-cards goal) ) )

; (calc-prescore list-of-cards goal) -> number?
; list-of-cards : list?
; goal : number?
;
; Calculates and returns prescore. The prescore is 5 times
; ( playerpoint – goal ), if the playerpoint is greater than goal.
; Otherwise, the prescore is ( goal – playerpoint ).
;
; Examples:
; > ( calc-prescore '((H . 3) (H . 2)) 3 )
; => 10
; > ( calc-prescore '((H . 3) (H . 2)) 6 )
; => 1
(define (calc-prescore list-of-cards goal)
  (let ((playerpoint (calc-playerpoint list-of-cards)))
    (if ( > playerpoint goal)
        (* 5 (- playerpoint goal))
        (- goal playerpoint) )))

; (play list-of-cards list-of-moves goal) -> number?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
;
; Returns finalscore at the end of the game after processing (some or all of)
; the moves in the move list in order.
;
; Examples:
; > ( play '((H . 3) (H . 2) (H . A) (D . J) (D . Q) (C . J)) '(draw draw draw discard) 16 )
; => 1
; (Note: (H . 3) is discarded for discard move. My fdiscard function may discard a different card. )
(define (play list-of-cards list-of-moves goal)
  (calc-score (find-held-cards (find-steps list-of-cards list-of-moves goal)) goal))

