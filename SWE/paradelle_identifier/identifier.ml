(* This file contains a few helper functions and type declarations
   that are to be used in Homework 2. *)

(* Place part 1 functions 'take', 'drop', 'length', 'rev',
   'is_elem_by', 'is_elem', 'dedup', and 'split_by' here. *)


(*for testing purposes*)
let explode s =
    let l = String.length s
    in
    let rec f i =
      if i = l then [] else s.[i]::f(i+1)
    in f 0


(* length: 'a list -> int *)
let length lst = List.fold_left (fun b a -> b+1) 0 lst


let rec take n l = match l with
  | [] -> []
  | x::xs -> if n > 0 then x::take (n-1) xs else []


let rec drop n l = match l with
  | [] -> []
  | x::xs -> if n > 0 then drop (n-1) xs else l


(* rev: 'a list -> 'a list *)
let rev lst = List.fold_left (fun b a -> a::b) [] lst


(* is_elem_by: ('a -> 'a-> bool) -> 'b -> 'a list -> bool *)
let is_elem_by func elem lst =
  let if_elem b a =
    if func a elem then true
    else b
  in
  List.fold_left if_elem false lst


(* is_elem: 'a -> 'a list -> bool*)
let is_elem elem lst = is_elem_by (=) elem lst


(* dedup: 'a list -> 'a list *)
let dedup lst =
  let if_dup x xs =
    if is_elem x xs then xs
    else x::xs
  in
  List.fold_right if_dup lst []


(*split_by: ('a -> 'b -> bool) -> 'b list -> 'a list -> 'b list list*)
(*
(1) creates a function check_sepors which gets an element x and a
tuple (rest,group).it passes x to is_elem_by with split_lists
first argument as the sub-function and the list of separators to
see if x is in the separators, if it is then it adds list group to
list of list rest and sets group to [] otherwise it adds x to group
(2) use fold_right to apply check_sepors on all elements of orig_lst
to get a tuple (rest,group) rest congaing the possible separated lists
and group contains the last set of possible elements that haven't been added
rest.
(3)in (rest,group) if group is empty then return rest else add group to rest
then return.
*)
let split_by (func: 'a -> 'b -> bool)
              (orig_lst: 'b list) (sepors_lst: 'a list): 'b list list =
  let check_sepors x (rest, group) =
    if is_elem_by func x sepors_lst
    then (group::rest, [])
    else (rest,x::group)
  in
  match List.fold_right check_sepors orig_lst ([], []) with
  |(rest,[]) -> []::rest
  |(rest,group) -> group::rest



(* Some functions for reading files. *)
let read_file (filename:string) : char list option =
  let rec read_chars channel sofar =
    try
      let ch = input_char channel
      in read_chars channel (ch :: sofar)
    with
    | _ -> sofar
  in
  try
    let channel = open_in filename
    in
    let chars_in_reverse = read_chars channel []
    in Some (rev chars_in_reverse)
  with
    _ -> None

type word = char list
type line = word list

let remove_empty lst =
  let if_empty x no_empty =
    match x with
    | [] -> no_empty
    | _ ->  x::no_empty
  in
  List.fold_right if_empty lst []

(* convert_to_non_blank_lines_of_words: char list -> line list *)
let convert_to_non_blank_lines_of_words
        (char_lst: char list): line list =
  let break_char_lst char_lst =
    split_by (=) char_lst ['\n']
  in
  let lines = break_char_lst char_lst
  in
  let rec break_lines line_lst =
    let sepors = [' '; '.'; '!'; ','; ';'; ':'; '-']
    in
    match line_lst with
    | [] -> []
    | a_line::rest ->
          split_by (=) a_line sepors :: break_lines rest
  in
  remove_empty (List.map remove_empty (break_lines lines))


type result = OK
	    | FileNotFound of string
	    | IncorrectNumLines of int
	    | IncorrectLines of (int * int) list
	    | IncorrectLastStanza


(*paradelle: *)

let lower_chars char_lst = List.map Char.lowercase_ascii char_lst

let check_stnz_12_34 lns stnz_num =
  let ln1 = List.map lower_chars (List.concat(take 1 lns))
  in let ln2 = List.map lower_chars (List.concat(take 1 (drop 1 lns)))
  in let ln3 = List.map lower_chars (List.concat(take 1 (drop 1 (drop 1 lns))))
  in let ln4 = List.map lower_chars (List.concat(take 1 (drop 1 (drop 1 (drop 1 lns)))))
  in
  let check_lns (l1,l2) (l1_num,l2_num) =
    if l1 = l2 then []
    else (l1_num,l2_num)::[]
  in
  (check_lns (ln1,ln2) (1 + 6*stnz_num,2 + 6*stnz_num)) @ (check_lns (ln3,ln4) (3 + 6*stnz_num,4 + 6*stnz_num))


let check_stnz_56 (lns: line list) stnz_num =
  let ln5n6 = (List.map lower_chars (List.concat(take 1 (drop 1 (rev lns)))))
              @ (List.map lower_chars (List.concat(take 1 (rev lns))))
  in let ln1n3 = (List.map lower_chars (List.concat(take 1 lns)))
              @ (List.map lower_chars (List.concat(take 1 (drop 1 (drop 1 lns)))))
  in

  if List.sort compare ln5n6 = List.sort compare ln1n3 then []
    else (5 + 6*stnz_num, 6 + 6*stnz_num)::[]


let check_last_stnz [stnz1; stnz2; stnz3; stnz4] =
  let s1_ln1n3 = (List.concat(take 1 stnz1))
                @ (List.concat(take 1 (drop 1 (drop 1 stnz1))))
  in let s2_ln1n3 = (List.concat(take 1 stnz2))
                @ (List.concat(take 1 (drop 1 (drop 1 stnz2))))
  in let s3_ln1n3 = (List.concat(take 1 stnz3))
                @ (List.concat(take 1 (drop 1 (drop 1 stnz3))))
  in let s123_ln1n3 = List.map lower_chars (s1_ln1n3 @ s2_ln1n3 @ s3_ln1n3)
  in let s4_ln1 = List.concat (take 1 stnz4)
  in let s4_ln2 = List.concat (take 1 (drop 1 stnz4))
  in let s4_ln3 = List.concat (take 1 (drop 1 (drop 1 stnz4)))
  in let s4_ln4 = List.concat (take 1 (drop 1 (drop 1 (drop 1 stnz4))))
  in let s4_ln5 = List.concat (take 1 (drop 1 (rev stnz4)))
  in let s4_ln6 = List.concat (take 1 (rev stnz4))
  in let stnz4_lined = List.map lower_chars (s4_ln1 @ s4_ln2 @ s4_ln3 @ s4_ln4 @ s4_ln5 @ s4_ln6)
  in
  List.sort compare (dedup stnz4_lined) = List.sort compare (dedup s123_ln1n3)

let check_stanza stnz stnz_num =
  let poss_inc_lines12_34 = check_stnz_12_34 stnz stnz_num
  in if poss_inc_lines12_34 != [] then poss_inc_lines12_34
  else
  let poss_inc_lines56 = check_stnz_56 stnz stnz_num
  in if poss_inc_lines56 != [] then poss_inc_lines56
  else []


let paradelle file_name =
  let char_lst = read_file file_name
  in
  match char_lst with
  | None -> FileNotFound file_name
  | Some ch_ls -> let lines = convert_to_non_blank_lines_of_words (ch_ls)
    in
    if length lines != 24 then IncorrectNumLines (length lines)
    else
      let stnz1 = take 6 lines
      in let ln_rest1 = drop 6 lines
      in let stnz2 = take 6 ln_rest1
      in let ln_rest2 = drop 6 ln_rest1
      in let stnz3 = take 6 ln_rest2
      in let stnz4 = drop 6 ln_rest2
      in
      let poss_inc_lines = (check_stanza stnz1 0) @
                    (check_stanza stnz2 1) @ (check_stanza stnz3 2)
      in
      if poss_inc_lines != [] then IncorrectLines poss_inc_lines
      else if check_last_stnz [stnz1; stnz2; stnz3; stnz4] then OK
      else IncorrectLastStanza
