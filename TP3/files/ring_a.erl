-module(ring_a).
-compile(export_all).

proceso(Siguiente) ->
   receive
      'exit' ->
         io:format("Exit: ~p~n", [self()]),
         Siguiente ! 'exit',
         {ok, 'exit'};
      {msg, N} when N > 0 ->
         io:format("Received: ~p~n", [N]),
         Siguiente ! {msg, N - 1},
         proceso(Siguiente);
      {msg, 0} ->
         Siguiente ! 'exit',
         proceso(Siguiente);
      {change, NewPid} ->
         proceso(NewPid)
    end.

createRings(0) ->
   Pid = spawn(?MODULE, proceso, [self()]),
   {Pid, Pid};
createRings(N) ->
   {Siguiente, LastPid} = createRings((N - 1)),
   Pid = spawn(?MODULE, proceso, [Siguiente]),
   {Pid, LastPid}.

initRings(Nprocs) ->
    {NewPid, Proceso} = createRings(Nprocs - 1),
    Proceso ! {change, NewPid},  
    NewPid.

init() ->
   P1 = initRings(5),      
   P1 ! {msg, 3}.