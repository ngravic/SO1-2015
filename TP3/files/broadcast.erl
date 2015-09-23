-module(broadcast).
-compile(export_all).

broadcaster(Subscribers)->
   receive
      {Pid, subscribe} ->
         io:format("~p Suscripto~n", [Pid]),
         broadcaster(Subscribers ++ [Pid]);
      {Pid, unsubscribe} ->
         io:format("~p Desuscripto~n", [Pid]),
         broadcaster(Subscribers -- [Pid]);      
      {send_msg, Msg} ->
         lists:foreach(fun(Proc) -> Proc ! Msg end, Subscribers),
         broadcaster(Subscribers)
   end.
   
client() ->
   receive   
      Data ->
         io:format("recibio de ~p: ~p~n", [self(), Data]),
         client()
   end.
   
init() ->
   Server = spawn(?MODULE, broadcaster, [[]]),
   C1 = spawn(?MODULE, client, []),
   C2 = spawn(?MODULE, client, []),
   C3 = spawn(?MODULE, client, []),
   Server ! {C1, subscribe},
   Server ! {C2, subscribe},
   Server ! {C3, subscribe},   
   Server ! {send_msg, "Broadcast"},
   Server ! {C1, unsubscribe},   
   Server ! {send_msg, "Broadcast"}.