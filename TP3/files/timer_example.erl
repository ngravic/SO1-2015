-module(timer_example).
-export([wait/1, cronometro/3, init/0]).

wait(P1) ->
	receive
	after P1 -> 
		ok
	end.

cronometro(Fun, To, Tick) ->
	if  To > 0 ->
			spawn(Fun), wait(Tick), cronometro(Fun, To - Tick, Tick);
		true -> 
			ok
	end.

init() ->
	wait(100),
	cronometro(fun () -> io:format("Tick~n") end,10000,1000).