(define game-cppref)
(define ball-cppref)
(define board-cppref)

(define (initialize-game)
  (let ((vec
	 (construct-game screen-width screen-height background window-title ball-sprite board-sprite)))
    (set! game-cppref (vector-ref vec 0))
    (set! ball-cppref (vector-ref vec 1))
    (set! board-cppref (vector-ref vec 2))))



(define (create-entity-maker prototype-cppref)
  (lambda (x y)
    (push-prototype prototype-cppref x y)))

(define (create-border-entity-maker prototype-cppref)
  (lambda (x y)
    (push-prototype-border prototype-cppref x y)))

(define (set-moving-entity-attribute moving-entity-cppref x y w h vx vy sprite)
  (set-entity-x! moving-entity-cppref x)
  (set-entity-y! moving-entity-cppref y)
  (set-entity-w! moving-entity-cppref w)
  (set-entity-h! moving-entity-cppref h)
  (set-moving-entity-vel-x! moving-entity-cppref vx)
  (set-moving-entity-vel-y! moving-entity-cppref vy)
  (set-entity-sprite! moving-entity-cppref sprite))

(define (set-entity-attribute entity-cppref x y w h sprite)
  (set-entity-x! entity-cppref x)
  (set-entity-y! entity-cppref y)
  (set-entity-w! entity-cppref w)
  (set-entity-h! entity-cppref h)
  (set-entity-sprite! entity-cppref sprite))

