-module(ring_b).
-compile(export_all).

proceso({Siguiente, MsgList}) ->
   receive
      'exit' ->
         io:format("Exit: ~p~n", [self()]),
         Siguiente ! 'exit',
         {ok, 'exit'};
      {msg, Msg} ->
         case lists:member(Msg, MsgList) of
         false -> 
            io:format("Received: ~p~n", [Msg]),
            Siguiente ! {msg, Msg},
            proceso({Siguiente, [Msg] ++ MsgList});
         true ->
            proceso({Siguiente, MsgList -- [Msg]})
         end;            
      {request, Pid, next} ->
         Pid ! Siguiente,
         proceso({Siguiente, MsgList});
      {change, {NewPid, MsgList}} ->
         proceso({NewPid, MsgList})
    end.

createRings(0) ->
   Pid = spawn(?MODULE, proceso, [{self(), []}]),
   {Pid, Pid};
createRings(N) ->
   {Siguiente, LastPid} = createRings((N - 1)),
   Pid = spawn(?MODULE, proceso, [{Siguiente, []}]),
   {Pid, LastPid}.

initRings(Nprocs) ->
    {NewPid, Proceso} = createRings(Nprocs - 1),
    Proceso ! {change, {NewPid, []}},  
    NewPid.

init() ->
   P1 = initRings(5),      
   P1 ! {msg, 3}.