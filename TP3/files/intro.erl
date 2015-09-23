-module(intro).
-export([init/0]).

match_test () -> 
  {A,B} = {5,4},
  {C,C} = {5,4},
  {B,A} = {4,5},
  {D,D} = {5,5}.

string_test () -> 
  io:write(helloworld == 'helloworld'),
  io:write("helloworld" < 'helloworld'),
  io:write(helloworld == "helloworld"),
  io:write([$h,$e,$l,$l,$o,$w,$o,$r,$l,$d] == "helloworld"),
  io:write([104,101,108,108,111,119,111,114,108,100] < {104,101,108,108,111,119,111,114,108,100}),
  io:write([104,101,108,108,111,119,111,114,108,100] > 1),
  io:write([104,101,108,108,111,119,111,114,108,100] == "helloworld").

tuple_test (P1,P2) ->
  io:format("El nombre de P1 es ~p y el apellido de P2 es ~p~n",[nombre(P1),apellido(P2)]).

apellido (P) -> 
  {persona, _, {apellido, PP}} = P, PP.
  
nombre (P) -> 
  {persona, {nombre, PP}, _} = P, PP.
  

filtrar_por_apellido(Personas,Apellido) -> 
  [X || X <- Personas, apellido(X) == Apellido ].

init () -> 
  P1 = {persona,{nombre,"Juan"},{apellido, "Gomez"}},
  P2 = {persona,{nombre,"Carlos"},{apellido, "Garcia"}},
  P3 = {persona,{nombre,"Javier"},{apellido, "Garcia"}},
  P4 = {persona,{nombre,"Rolando"},{apellido, "Garcia"}},
  match_test(),
  tuple_test(P1,P2),
  string_test(),
  Garcias = filtrar_por_apellido([P4,P3,P2,P1],"Garcia").