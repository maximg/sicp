#lang racket
(require racket/stream)
(require racket/promise)

;; Useful: https://github.com/masapir/StreamsInRacket/blob/master/streams.rkt

;; Define a few aliases to match SICP vocabulary.
;; stream-cons is a special form and cannot be aliased through `define`.
(define stream-car stream-first)
(define stream-cdr stream-rest)

;; Stream building blocks.

(define (integers-starting-from n)
  (stream-cons n (integers-starting-from (+ n 1))))

(define integers (integers-starting-from 1))

(define (stream-map2 op s1 s2)
  (stream-cons
   (op (stream-car s1) (stream-car s2))
   (stream-map2 op (stream-cdr s1) (stream-cdr s2))))

(define (add-streams s1 s2) (stream-map2 + s1 s2))

(define (scale-stream s n) (stream-map (lambda (x) (* n x)) s))

(define odds (stream-filter (lambda (x) (= 1 (remainder x 2))) integers))
(define evens (stream-filter (lambda (x) (= 0 (remainder x 2))) integers))

(define (p-n n s)
  (define (print-one x) (display x)(display #\space))
  (stream-for-each print-one (stream-take s n)))
(define (p5 s) (p-n 5 s)) 
(define (p10 s) (p-n 10 s)) 

;; 3.53 - add-streams

(define (ex3-53)
  (define s (stream-cons 1 (add-streams s s)))
  (p5 s))

;; 3.54 - factorials

(define (mul-streams s1 s2) (stream-map2 * s1 s2))

(define (ex3-54)
  (define factorials
    (stream-cons 1 (mul-streams factorials integers)))
  (p5 factorials))

;; 3.55 - partial-sums

(define (ex3-55)
  (define (partial-sums s)
    (stream-cons
     (stream-car s)
     (add-streams (stream-cdr s) (partial-sums s))))
  (p5 (partial-sums integers)))

;; 3.56 - Hamming 2-3-5

(define (ex3-56)
  (define (merge s1 s2)
    (cond ((stream-empty? s1) s2)
          ((stream-empty? s2) s1)
          (else
           (let ((s1car (stream-car s1))
                 (s2car (stream-car s2)))
             (cond ((< s1car s2car)
                    (stream-cons s1car (merge (stream-cdr s1) s2)))
                   ((< s2car s1car)
                    (stream-cons s2car (merge s1 (stream-cdr s2))))
                   (else
                    (stream-cons s1car (merge (stream-cdr s1)(stream-cdr s2)))))))))
  (define S
    (stream-cons
     1
     (merge (scale-stream S 2)
            (merge (scale-stream S 3) (scale-stream S 5)))))
  (p10 S))

;; 3.59 - integrate-series

(define (integrate-series s)
  (mul-streams s
               (stream-map (lambda (x) (/ 1 x)) integers)))

(define (ex3-59a)
  (define exp-series
    (stream-cons 1 (integrate-series exp-series)))
  (p5 (integrate-series evens)))


(ex3-59a)
