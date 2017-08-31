(import (rnrs (6)))
(define screen-width 800)
(define screen-height 600)
(define background "white.png")
(define window-title "Demo")
(define board-sprite "container.jpg")
(define ball-sprite "awesomeface.png")
(initialize-game)
(define ball-speed 7.0)
(define board-speed 5.0)
;;Width and height of board and ball
(set-entity-w! ball-cppref 40)
(set-entity-h! ball-cppref 40)
(set-entity-w! board-cppref 100)
(set-entity-h! board-cppref 20)

;;Distance from the bottom
(define distance_from_bottom 30)

;;Initial position
(define (ball-initial-x) (/ (- screen-width (get-entity-w ball-cppref)) 2))
(define (ball-initial-y) (- screen-height(+ (get-entity-h ball-cppref) distance_from_bottom)))
(define (board-initial-x) (/ (- screen-width (get-entity-w board-cppref)) 2))
(define (board-initial-y) (- screen-height distance_from_bottom))

(define (ball-board-pos-reset)
  (set-entity-x! ball-cppref (ball-initial-x))
  (set-entity-y! ball-cppref (ball-initial-y))
  (set-entity-x! board-cppref (board-initial-x))
  (set-entity-y! board-cppref (board-initial-y)))

(define (velocity-reset)
  (set-moving-entity-vel-x! ball-cppref 0)
  (set-moving-entity-vel-y! ball-cppref 0))

(ball-board-pos-reset)

(define (bounce-moving-entity-off rigid-body entity)
  (let ((ball-x (get-entity-x entity))
	(ball-y (get-entity-y entity))
	(ball-w (get-entity-w entity))
	(ball-h (get-entity-h entity))
	(entity-x (get-entity-x rigid-body))
	(entity-y (get-entity-y rigid-body))
	(entity-w (get-entity-w rigid-body))
	(entity-h (get-entity-h rigid-body)))
    (let((right (cons 'right (abs (- ball-x (+ entity-x entity-w)))))
	 (left (cons 'left (abs (- entity-x (+ ball-x ball-w)))))
	 (top (cons 'top (abs (- entity-y (+ ball-y ball-h)))))
	 (bottom (cons 'bottom (abs (- ball-y (+ entity-y entity-h))))))
      (define side (fold-left (lambda (min_value elem)(if(< (cdr elem) (cdr min_value)) elem min_value))
			 left (list right bottom top)))
      (cond ((eq? side right)
	     (set-entity-x! entity (+ entity-x entity-w))
	     (set-moving-entity-vel-x! entity (- (get-moving-entity-vel-x entity))))
	    ((eq? side left)
	     (set-entity-x! entity (- entity-x ball-w))
	     (set-moving-entity-vel-x! entity (- (get-moving-entity-vel-x entity))))
	    ((eq? side top)
	     (set-entity-y! entity (- entity-y ball-h))
	     (set-moving-entity-vel-y! entity (- (get-moving-entity-vel-y entity))))
	    ((eq? side bottom)
	     (set-entity-y! entity (+ entity-y entity-h))
	     (set-moving-entity-vel-y! entity (- (get-moving-entity-vel-y entity))))))))


(define (board-callback board ball)
  (bounce-moving-entity-off board ball)
  (let ((ball-xmid (+ (get-entity-x ball) (/ (get-entity-w ball) 2)))
	(board-xmid (+ (get-entity-x board) (/ (get-entity-w board) 2))))
    (define offset (/ (- ball-xmid board-xmid) 10))
    (set-moving-entity-vel-x! ball offset)))


(register-ball-callback board-cppref board-callback)


(define (brick-callback entity-ref ball-ref)
  (bounce-moving-entity-off entity-ref ball-ref)
  (remove-brick! entity-ref))


(define brick (create-prototype 50 20 "./container.jpg" brick-callback))

(define (create-a-line proto x y w cnt)
  (if (= cnt 0)
      #f
      (begin
	(push-prototype proto x y)
	(create-a-line proto (+ x w) y w (- cnt 1)))))

(create-a-line brick 40 40 50 15)
(create-a-line brick 40 60 50 15)
(create-a-line brick 40 80 50 15)
(create-a-line brick 40 100 50 15)


(define (bottom-call-back rigid ball)
  (die-game)
  (set! ball-out #f)
  (ball-board-pos-reset)
  (velocity-reset))

(push-entity-bound -30 -30 860 30 "awesomeface.png" bounce-moving-entity-off)
(push-entity-bound -30 -30 30 630 "awesomeface.png" bounce-moving-entity-off)
(push-entity-bound 800 -30 30 630 "awesomeface.png" bounce-moving-entity-off)
(push-entity-bound -30 600 860 30 "awesomeface.png" bottom-call-back)



(define ball-out #f)

(define (space-cb)
  (set! ball-out #t)
  (set-moving-entity-vel-x! ball-cppref 0)
  (set-moving-entity-vel-y! ball-cppref (- ball-speed)))

(define (left-cb)
  (set-moving-entity-vel-x! board-cppref (- board-speed))
  (if (not ball-out)
      (set-moving-entity-vel-x! ball-cppref (- board-speed))))

(define (right-cb)
  (set-moving-entity-vel-x! board-cppref board-speed)
  (if (not ball-out)
      (set-moving-entity-vel-x! ball-cppref board-speed)))



(register-space-callback! space-cb)
(register-left-callback! left-cb)
(register-right-callback! right-cb)

(run-game)

