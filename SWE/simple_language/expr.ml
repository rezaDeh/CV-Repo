(* Hwk 05.  Extend the construts below as specified.
	 *)

type value
  = Int of int
	| Bool of bool
	| Closure of string * expr * value_environment
  | Ref of expr
	| PairV of value * value
	| ListV of value list
	| NilV

and value_environment = (string * value) list

and expr
	= Val of value

	| Add of expr * expr
	| Sub of expr * expr
	| Mul of expr * expr
	| Div of expr * expr

	| Lt  of expr * expr
	| Eq  of expr * expr
	| And of expr * expr
	| Not of expr

	| Let of string * typ * expr * expr
	| Id  of string

	| App of expr * expr
	| Lam of string * typ * expr

	| LetRec of string * typ * expr * expr
	| If of expr * expr * expr
	| Pair of expr * expr
	| Fst of expr
	| Snd of expr

	| Nil
	| Cons of expr * expr
	| Head of expr
	| Tail of expr

and typ = Int_type
	| Bool_type
  | Func_type of typ * typ
	| Pair_type of typ * typ
	| List_type of typ
	| Nil_type

type type_environment = (string * typ option) list

let rec lookup (env: value_environment) (n: string) : value =
  match env with
  | [] -> failwith ("The name " ^ n ^ " is not bound by a let.")
  | (n',v)::rest when n' = n -> v
  | (_,_)::rest -> lookup rest n


(* Part 1. Complete unparse----------------------------------------------------- *)
let rec string_Let (str: string) (t: typ): string =
	match t with
	| Int_type -> "int"
	| Bool_type -> "bool"
	| Func_type (t1,t2)
		-> str ^ ": (" ^ string_Let str t1 ^ " -> " ^ string_Let str t2 ^ ") = "
	| Pair_type (t1,t2)
		-> "(" ^ str ^ ": (" ^ string_Let str t1 ^ " * " ^ string_Let str t2 ^ ")) = "
	| List_type t -> "(" ^ string_Let str t ^ " list)"
	| Nil_type -> "[]"

let rec string_Lam (s: string) (t: typ): string =
	match t with
	| Int_type -> "int"
	| Bool_type -> "bool"
	| Func_type (t1,t2)
		-> "fun (" ^ s ^ " : " ^ string_Lam  s t1 ^ " -> " ^ string_Lam  s t2 ^ ") -> "
	| Pair_type (t1, t2)
	  -> "fun (" ^ s ^ " : (" ^ string_Lam s t1 ^ " * " ^ string_Lam s t2 ^ ")) -> "
	| List_type t -> "fun (" ^ s ^ " : " ^ string_Lam s t ^ " list" ^ ") -> "
	| Nil_type -> "[]"



let rec unparse (e: expr) : string =
  match e with
	| Val (Int i) -> string_of_int i
	| Val (Bool b) -> string_of_bool b

  | Add (e1,e2) -> "(" ^ unparse e1 ^ " + " ^ unparse e2 ^ ")"
  | Sub (e1,e2) -> "(" ^ unparse e1 ^ " - " ^ unparse e2 ^ ")"
  | Mul (e1,e2) -> "(" ^ unparse e1 ^ " * " ^ unparse e2 ^ ")"
  | Div (e1,e2) -> "(" ^ unparse e1 ^ " / " ^ unparse e2 ^ ")"

  | Lt (e1,e2) -> "(" ^ unparse e1 ^ " < " ^ unparse e2 ^ ")"
  | Eq (e1,e2) -> "(" ^ unparse e1 ^ " = " ^ unparse e2 ^ ")"
  | And (e1,e2) -> "(" ^ unparse e1 ^ " && " ^ unparse e2 ^ ")"
  | Not e -> "(not" ^ unparse e ^ ")"

	| Let (str,typ,e1,e2) ->
		( match typ with
			| Int_type
				-> "(let (" ^ str ^ ": int) = " ^ unparse e1 ^ " in " ^ unparse e2 ^ ")"
			| Bool_type
				-> "let ((" ^ str ^ ": bool) = " ^ unparse e1 ^ " in " ^ unparse e2 ^ ")"
			| List_type t -> "let " ^ (string_Let str typ) ^   " = "
			| _ -> "(let " ^ (string_Let str typ) ^ unparse e1 ^ " in " ^ unparse e2 ^ ")"
		)
  | Id str -> str ^ " "
  | App (e1,e2) -> "((" ^ unparse e1 ^ ") " ^ unparse e2 ^ ")"
  | Lam (str,typ,e) ->
		( match typ with
			| Int_type -> "(fun (" ^ str ^ ": int) -> " ^ unparse e ^ ")"
			| Bool_type -> "(fun (" ^ str ^ ": bool) -> " ^ unparse e ^ ")"
			| _ -> "(" ^ (string_Lam str typ) ^ unparse e ^ ")"
		)
  | LetRec (str,typ,e1,e2) ->
		( match typ with
			| Int_type
				-> "let rec (" ^ str ^ ": int) = " ^ unparse e1 ^ " in " ^ unparse e2
			| Bool_type
				-> "let rec (" ^ str ^ ": bool) = " ^ unparse e1 ^ " in " ^ unparse e2
			| List_type t -> "(let rec " ^ (string_Let str typ) ^   ") = "
			| _ -> "(let rec " ^ (string_Let str typ) ^ unparse e1 ^ " in " ^ unparse e2 ^ ")"
		)
  | If (e1,e2,e3)
    -> "(if " ^ unparse e1 ^ " then " ^ unparse e2 ^ " else " ^unparse e3 ^ ")"
	| Pair (e1, e2) -> "(" ^ unparse e1 ^ ", " ^ unparse e2 ^ ")"
	| Fst e -> "(fst " ^ unparse e ^ ")"
	| Snd e -> "(snd " ^ unparse e ^ ")"
	| Nil -> "[]"
	| Cons (e1,e2) -> "(" ^ unparse e1 ^ "::" ^ unparse e2 ^ ")"
	| Head e -> "(List.hd " ^ unparse e ^ ")"
	| Tail e -> "(List.tl " ^ unparse e ^ ")"


(* Part 2. Complete freevars---------------------------------------------------- *)
let rec freevars (e: expr) : string list =
  match e with
	| Val _ -> []
  | Add (e1,e2) | Sub (e1,e2) | Mul (e1,e2) | Div (e1,e2)
	| Lt (e1, e2) | Eq (e1, e2) | And (e1,e2) | App (e1,e2)
	| Pair (e1,e2) | Cons (e1 ,e2) -> freevars e1 @ freevars e2
  | Not e | Fst e | Snd e | Head e | Tail e-> freevars e
  | If (e1,e2,e3) -> freevars e1 @ freevars e2 @ freevars e3
  | Id str -> [str]
	| Nil -> []
  | Lam (str,typ,e) -> List.filter (fun elem -> str <> elem) (freevars e)
  | Let (str,typ,e1,e2) | LetRec (str,typ,e1,e2)
    -> freevars e1 @ List.filter (fun elem -> str <> elem) (freevars e2)


(* Part 3. Type Checking-------------------------------------------------------- *)
type result = OK of typ
            | Errs of (expr * string) list

let expect_Int (r: result) (e: expr) : (expr * string) list =
  match r with
  | OK Int_type -> []
  | OK Bool_type ->  [(e, "expected Int, got Bool") ]
  | Errs errs -> errs

let expect_Bool (r: result) (e: expr) : (expr * string) list =
  match r with
  | OK Bool_type -> []
  | OK Int_type ->  [(e, "expected Bool, got Int") ]
  | Errs errs -> errs

type typ_env = (string * typ option) list

let rec lookup_typ (x: string)  (env: typ_env) : result =
  match env with
  | (y, Some ty)::ys -> if x = y then OK ty else lookup_typ x ys
	| _ -> Errs ( [(Id x, "identifier not bound")] )


let rec type_check (e:expr) (env: type_environment) : result =
  match e with
  | Val (Int _) -> OK Int_type
  | Val (Bool _) -> OK Bool_type
	| Add (e1,e2) | Sub (e1,e2) | Mul (e1,e2) ->
	 	( match type_check e1 env, type_check e2 env with
			| OK Int_type, OK Int_type -> OK Int_type
			| r1, r2 -> Errs (expect_Int r1 e1 @ expect_Int r2 e2)
		)
	| Div (e1,e2) ->
		( match type_check e1 env, type_check e2 env with
			| OK Int_type, OK Int_type ->
				( match unparse e2 with
					| "0" -> Errs [e2, "Division by zero"]
					| _ -> OK Int_type
				)
			| r1, r2 -> Errs (expect_Int r1 e1 @ expect_Int r2 e2)
		)
	| Lt (e1, e2) ->
 		( match type_check e1 env, type_check e2 env with
	 		| OK Int_type, OK Int_type -> OK Bool_type
	 		| r1, r2 -> Errs (expect_Int r1 e1 @ expect_Int r2 e2)
 		)
	| Eq (e1,e2) ->
		( match type_check e1 env, type_check e2 env with
			| OK Int_type, OK Int_type | OK Bool_type, OK Bool_type
			| OK Nil_type, OK Nil_type -> OK Bool_type
			| OK List_type t, OK Nil_type | OK Nil_type, OK List_type t-> OK Bool_type
			| OK List_type t1, OK List_type t2 ->
			 	if t1 = t2 then OK Bool_type
				else Errs [Eq(e1,e2), "type mismatch in Eq"]
			| OK Pair_type (t1,t2), OK Pair_type (t3,t4)
			| OK Func_type (t1,t2), OK Func_type (t3,t4) ->
				if t1 = t3 && t2 = t4 then OK Bool_type
				else Errs [Eq(e1,e2), "type mismatch in Eq"]
			| _ -> Errs [Eq (e1,e2), "type error in Eq"]
		)

	| And (e1,e2) ->
		( match type_check e1 env, type_check e2 env with
			| OK Bool_type, OK Bool_type -> OK Bool_type
			| r1, r2 -> Errs (expect_Bool r1 e1 @ expect_Bool r2 e2)
		)
	| Not e ->
		( match type_check e env with
			| OK Bool_type -> OK Bool_type
			| r -> Errs (expect_Bool r e)
		)
	| Let (str,typ,e1,e2) ->
		( match typ with
			|	Func_type (t1,t2) ->
			if t1 = t2 then OK t1
			else if t1 = List_type t2 then OK t2
			else if t2 = List_type t1 then OK t1
			else if t2 = Nil_type then OK t1
			else if t1 = Nil_type then OK t2
			else Errs [Let (str,typ,e1,e2), "type mismatch in Let"]
			| _ ->
			(match (type_check e1 env) with
				| OK t -> type_check e2 ((str,(Some t))::env)
				|	Errs er -> Errs er
			)
		)
	| Id str ->	lookup_typ str env
	| App (e1,e2) ->
		( match type_check e1 env, type_check e2 env with
			| OK Func_type (t1,t2), OK t when OK t = OK t2 -> OK t2
			| _ -> Errs [App(e1,e2), "type mismatch"]
		)
	| Lam (str,e1,e2) -> type_check e2 ((str,Some e1)::env)
	| LetRec (str,typ,e1,e2) ->
		( match typ with
			|	Func_type (t1,t2) ->
				if t1 = t2 then OK t1
				else if t1 = List_type t2 then OK t2
				else if t2 = List_type t1 then OK t1
				else if t2 = Nil_type then OK t1
				else if t1 = Nil_type then OK t2
				else Errs [Let (str,typ,e1,e2), "type mismatch in Let rec"]
			| _ ->
			( match type_check e1 env with
				| OK t -> type_check e2 ((str,(Some t))::env)
				|	Errs er -> Errs er
			)
		)
	| If (e1,e2,e3) ->
		( match type_check e1 env with
			| OK Bool_type ->
				( match type_check e2 env, type_check e3 env with
					| OK Int_type, OK Int_type -> OK Int_type
					| OK Bool_type, OK Bool_type -> OK Bool_type
					| _ -> Errs [If (e1,e2,e3), "type mismatch in if clause"]
				)
			| _ -> Errs [e1, "expected type Bool in if clause"]
		)
	| Pair (e1,e2) ->
		( match type_check e1 env, type_check e2 env with
			| Errs er, _ | _, Errs er -> Errs er
			| Errs er1, Errs er2 -> Errs (er1@er2)
			| OK t1, OK t2 -> OK (Pair_type (t1, t2))
		)
	| Fst e ->
		( match type_check e env with
			| OK (Pair_type (t1,t2)) -> OK (Pair_type (t1,t2))
			| _ -> Errs [Fst e, "expected Pair type"]
		)
	| Snd e ->
		( match type_check e env with
			| OK (Pair_type (t1,t2)) -> OK (Pair_type (t1,t2))
			| _ -> Errs [Snd e, "expected Pair type"]
		)
	| Nil -> OK Nil_type
	| Cons (e1,e2) ->
		( match type_check e1 env, type_check e2 env with
			| OK t1, OK t2 ->
				if t1 = t2 then OK (List_type t1)
				else if t2 = Nil_type then OK (List_type t1)
				else if t2 = (List_type t1) then OK (List_type t1)
				else Errs [Cons (e1,e2), "type mismatch for Cons"]
		)
	| Head e ->
		( match type_check e env with
			| OK (List_type t) -> OK t
			| _ -> Errs [Head e, "type List expected"]
		)
	| Tail e ->
	( match type_check e env with
		| OK (List_type t) -> OK (List_type t)
		| _ -> Errs [Tail e, "type List expected"]
	)

let check e = type_check e []


(* Part 4. Evaluation----------------------------------------------------------- *)
let rec eval (env: value_environment) (e:expr) : value =
	match e with
	| Val v -> v
	| Add (e1,e2) ->
	   ( match eval env e1, eval env e2 with
	     | Int v1, Int v2 -> Int (v1 + v2)
	     | _ -> raise (Failure "incompatible types, Add")
	   )
  | Sub (e1,e2) ->
    ( match eval env e1, eval env e2 with
      | Int v1, Int v2 -> Int (v1 - v2)
      | _ -> raise (Failure "incompatible types, Sub")
    )
  | Mul (e1, e2) ->
    ( match eval env e1, eval env e2 with
      | Int v1, Int v2 -> Int (v1 * v2)
      | _ -> raise (Failure "incompatible types, Mul")
    )
  | Div (e1, e2) ->
    ( match eval env e1, eval env e2 with
      | Int v1, Int v2 -> Int (v1 / v2)
      | _ -> raise (Failure "incompatible types, Div")
    )
  | Lt (e1,e2) ->
    ( match eval env e1, eval env e2 with
      | Int v1, Int v2 -> Bool (v1 < v2)
      | _ -> raise (Failure "incompatible types, Lt")
    )
  | Eq (e1,e2) ->
    ( match eval env e1, eval env e2 with
      | Int v1, Int v2 -> Bool (v1 = v2)
      | Bool v1, Bool v2 -> Bool (v1 = v2)
			| NilV ,NilV -> Bool (Nil = Nil)
			| ListV v1, ListV v2 -> Bool (v1 = v2)
			| PairV (v1,v2), PairV (v3,v4) -> Bool ((v1 = v2) && (v3 = v4))
      | _ -> raise (Failure "incompatible types, Eq")
    )
  | And (e1,e2) ->
    ( match eval env e1, eval env e2 with
      | Bool v1, Bool v2 -> Bool (v1 && v2)
      | _ -> raise (Failure "incompatible types, And")
    )
  | Not e ->
    ( match eval env e with
      | Bool true -> Bool false
      | Bool false -> Bool true
      | _ -> raise (Failure "incompatible types, Not")
    )
	| Let (n,t,dexpr,body) ->
	    let v = eval env dexpr in
	    eval ( (n,v)::env ) body
  | Id str -> lookup env str
  | App (e1,e2) ->
    ( match eval env e1 with
      | Closure (str,e,envi) -> eval ((str,eval env e2)::envi) e
      | Ref ref -> eval env (App (ref,e2))
      | _ -> raise (Failure "incompatible types, App")
    )
  | Lam (str,typ,e) -> Closure (str,e,env)
  | LetRec (str,typ,e1,e2) -> eval ((str,eval((str,Ref e1)::env)e1)::env) e2
	| If (e1,e2,e3) ->
    ( match eval env e1 with
      | Bool true -> eval env e2
      | Bool false -> eval env e3
      | _ -> raise (Failure "incompatible types, If")
    )
	| Pair (e1,e2) ->
		( match eval env e1, eval env e2 with
		  | v1, v2 ->  PairV (v1,v2)
		)
	| Fst e ->
		( match eval env e with
			| PairV (v1,v2) -> fst (v1,v2)
			| _ -> raise (Failure "incompatible types, Fst")
		)
	| Snd e ->
	( match eval env e with
		| PairV (v1,v2) -> snd (v1,v2)
		| _ -> raise (Failure "incompatible types, Fst")
	)
	| Nil -> ListV []
	| Cons (e1, e2) ->
		( match eval env e1, eval env e2 with
			| v1, v2 ->
				(	match v2 with
					| v1 -> ListV [v1;v2]
					| NilV -> ListV [v1]
					| ListV v3 ->  ListV (v1::v3)
					| _ -> raise (Failure "incompatible types, Cons")
				)
		)
	| Head e ->
		( match eval env e with
			| ListV v -> List.hd v
			| _ -> raise (Failure "incompatible types, Head")
		)
	| Tail e ->
		( match eval env e with
			| ListV v -> ListV (List.tl v)
			| _ -> raise (Failure "incompatible types, Head")
		)

let evaluate e = eval [] e


(* some sample expressions------------------------------------------------------ *)
let e1 = Add (Val (Int 3), Val (Int 5))
let e2 = Add (Val (Int 3), Val (Bool true))
let e3 = Mul (Val (Bool true), Val (Int 5))
let e4 = Add (e2, e3)

let e5 = Let ("x", Int_type, Add (Val (Int 3), Val (Int 4)),
	            Add (Id "x", Val (Int 5)))

let e6 = Let ("x", Int_type, Add (Val (Int 3), Val (Int 4)),
	            Lt (Id "x", Val (Int 5)))

(* ``let x = 3 < 5 in x && let x = 1 + 2 in x = 3 *)
let e7 = Let ("x", Bool_type,
	       Lt (Val (Int 3), Val (Int 5)),
	       And (Id "x",
	           Let ("x", Int_type,
	           Add (Val (Int 1), Val (Int 2)),
	               Eq (Id "x", Val (Int 3)))))

(* ``let x = 3 < 5 in y && let x = 1 + 2 in x = 3 *)
let e8 = Let ("x", Bool_type,
        Lt (Val (Int 3), Val (Int 5)),And (Id "y",
          Let ("x", Int_type,
          Add (Val (Int 1), Val (Int 2)),
          Eq (Id "x", Val (Int 3)))))

let err_1 = Let ("x", Int_type, Add (Val (Int 3), Val (Int 4)),
                And (Id "x", Val (Bool true)))

let err_2 = Let ("x", Int_type, Add (Id "x", Val (Int 4)),
                 And (Id "y", Val (Bool true)))

let inc_use = Let ("inc", Func_type (Int_type, Int_type),
                 Lam ("n", Int_type, Add (Id "n", Val (Int 1))),
                 App (Id "inc", Val (Int 3)))

let sumToN : expr = LetRec ("sumToN", Func_type (Int_type, Int_type),
                 	  Lam ("n", Int_type, If (Eq (Id "n", Val (Int 0)),
                 	  Val (Int 0),Add (Id "n",App (Id "sumToN",
                 	  Sub (Id "n", Val (Int 1)))))),Id "sumToN")

let sumTo3 = App (sumToN, Val (Int 4))
