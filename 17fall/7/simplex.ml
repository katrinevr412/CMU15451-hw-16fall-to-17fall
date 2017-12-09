(*  
    This code was adapted from the UBC CODEARCHIVE 2014.
    It is in the public domain.  ---Danny Sleator Nov. 2017
*)

let rec fold i j f init = if i>j then init else fold (i+1) j f (f i init)
let maxf i j f = fold (i+1) j (fun k a -> max (f k) a) (f i)
let minf i j f = fold (i+1) j (fun k a -> min (f k) a) (f i)  

type lp_answer = Infeasible | Unbounded | Feasible of float * float array
    
let simplex m n a b c =
  (*
    input:
      m = #constraints, n =#variables
      max c dot x s.t. a x <= b
      where a = mxn, b = m vector, c = n vector
    output:
      Infeasible, or Unbounded, or a pair Feasible (z,soln) where z is
      the maximum objective function value, and soln is an n-vector of
      variable values.
    caveats:
      Cycling is possible.  Nothing is done to mitigate loss of
      precision when the number of iterations is large.
  *)

  let a0 = a in
  let a = Array.make_matrix (m+1) (n+1) 0.0 in

  (* the variables are numbered 0,...,n+m-1 *)
  let basic = Array.init m (fun j -> n+j) in   (* indices of basic variables *)  
  let non_basic = Array.init n (fun i -> i) in (* indices of non-basic variables *)

  let inf = 1e100 in
  let eps = 1e-9 in

  let pivot r c =
    (* first swap variables non_basic.(c) and basic.(r) *)
    let (nc,br) = (non_basic.(c),basic.(r)) in
    non_basic.(c) <- br;
    basic.(r) <- nc;
      
    a.(r).(c) <- 1.0 /. a.(r).(c);
    for j=0 to n do if j <> c then a.(r).(j) <- a.(r).(j) *. a.(r).(c) done;
    for i=0 to m do if i <> r then (
      for j=0 to n do 
	if j<>c then a.(i).(j) <- a.(i).(j) -. a.(i).(c) *. a.(r).(j)
      done;
      a.(i).(c) <- -. a.(i).(c) *. a.(r).(c)
    ) done
  in

  let feasible () =
    let rec loop () =
      let (p,row) = minf 0 (m-1) (fun i -> (a.(i).(n), i)) in
      if p > -.eps then true else (
	let (p,col) = minf 0 (n-1) (fun j -> (a.(row).(j), j)) in
	if p > -.eps then false else (
	  let (p,row) = minf row (m-1) (fun i ->
	    if i=row || a.(i).(col) > eps then (a.(i).(n) /. a.(i).(col),i)
	    else (inf, i))
	  in
	  pivot row col;
	  loop ()
	)
      )
    in
    loop ()
  in

  for i=0 to m-1 do
    a.(i).(n) <- b.(i);
    for j=0 to n-1 do
      a.(i).(j) <- a0.(i).(j)
    done
  done;

  for j=0 to n-1 do
    a.(m).(j) <- c.(j)
  done;

  if not (feasible ()) then Infeasible else (
    let rec loop () =
    (* Printf.printf "  z = %f\n%!" (-. a.(m).(n)); *)
      let (p,col) = maxf 0 (n-1) (fun i -> (a.(m).(i), i)) in
      if p < eps then (
	let soln = Array.make n 0.0 in
	for j=0 to m-1 do if basic.(j) < n then soln.(basic.(j)) <- a.(j).(n) done;
        Feasible (-.a.(m).(n), soln)
      ) else (
	let (p,row) = minf 0 (m-1) (fun i ->
	  if a.(i).(col) > eps then (a.(i).(n) /. a.(i).(col), i) else (inf,i)
	) in
	if p = inf then Unbounded else (
	  pivot row col;
	  loop ()
	)
      )
    in
    loop ()
  )

open Printf

let () =
  let a = [| [|1.0; 3.0; 1.0|];
	     [|-1.0; 0.0; 3.0|];
	     [|2.0; -1.0; 2.0|];
	     [|2.0; 3.0; -1.0|]
	  |] in

  let b = [|3.0; 2.0; 4.0; 2.0|] in

  let c = [|5.0; 5.0; 3.0|] in

  let (m,n) = (4,3) in

  match simplex m n a b c with
    | Infeasible -> printf "Infeasible\n"
    | Unbounded -> printf "Unbounded\n"
    | Feasible (z, x) ->
      printf "the maximum objective function value is %f\n" z;
      for i=0 to n-1 do
        printf "x%d = %f\n" i x.(i);
      done;
