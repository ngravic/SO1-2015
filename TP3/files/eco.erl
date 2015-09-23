-module(eco).
-compile(export_all).
                          
server() ->
    {ok, ListenSocket} = gen_tcp:listen(8000, [{active, false}]),
    wait_connect(ListenSocket, 1).

wait_connect(ListenSocket, N) ->
    {ok, Socket} = gen_tcp:accept(ListenSocket),    
    io:format("Connected - Client: ~p ~n", [N]),
    spawn(?MODULE, get_request, [Socket, N]),
	wait_connect(ListenSocket, N + 1).

get_request(Socket, N) ->
	case State = gen_tcp:recv(Socket, 0) of 
		{error, Reason} -> 
			io:format("Disconnected - Client: ~p Reason: ~p~n", [N, Reason]);
		{ok, Reply} ->
			gen_tcp:send(Socket, Reply),
			get_request(Socket, N)
	end.	