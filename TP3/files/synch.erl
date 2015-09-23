-module(synch).
-compile(export_all).

lock (L) ->
	L ! {lock, self()},
	receive
		{signal, trylock} ->
			lock(L);
		{lock, true} ->
			ok
	end.

unlock (L) ->
	L ! {unlock, self()},
	receive
		{reply, Msg} ->
		Msg
	end.

createLock () ->
	spawn(?MODULE, lockfunction, [{state, [], 0}]).

destroyLock (L) ->
	L ! destroy,
	{ok, L, destroyed}.

createSem (N) ->
	spawn(?MODULE, semfunction, [{state, [], N}]).

destroySem (S) ->
	S ! destroy,
	{ok, S, destroyed}.

semP (S) ->
	S ! {p, self()},
	receive
		{signal, trylock} ->
			semP(S);
		{sem, true} ->
			ok
	end.

semV (S) ->
	S ! v.

lockfunction(State) ->
	receive
		{lock, Pid} ->
			case {state, Pidlist, Owner} = State of
				{state, _Pidlist, 0} ->
					Pid ! {lock, true},
					lockfunction({state, Pidlist, Pid});
				_otherwise ->
					lockfunction({state, [Pid] ++ Pidlist, Owner})
			end;

		{unlock, Pid} ->
			case {state, Pidlist, Owner} = State  of
				_case when Pid == Owner ->
					Pid ! {reply, ok},
					spawn(?MODULE, broadcast, [Pidlist]),
					lockfunction({state, [], 0});
				_otherwise ->
					Pid ! {reply, error},
					lockfunction({state, Pidlist, Owner})
			end;

		destroy ->
			{ok, destroyed}
	end.

broadcast([]) -> ok;
broadcast(Pidlist) ->
	[Pid | Pl] = Pidlist,
	Pid ! {signal, trylock},
	broadcast(Pl).

semfunction(State) ->
	receive
		{p, Pid} ->
			case {state, Pidlist, N} = State of
				_N when N > 0 ->
					Pid ! {sem, true},
					semfunction({state, Pidlist, N - 1});
				_otherwise ->
					semfunction({state, [Pid] ++ Pidlist, N})
			end;

		v ->
			case {state, Pidlist, N} = State of
				_Pidlist when length(Pidlist) /= 0 ->
					spawn(?MODULE, broadcast, [Pidlist]),
					semfunction({state, [], N + 1});
				_otherwise ->
					semfunction({state, [], N + 1})
			end;

		destroy -> {ok, destroyed}

	 end.

f (L,W) -> lock(L),	
	io:format("uno ~p~n",[self()]),
	io:format("dos ~p~n",[self()]),
	io:format("tre ~p~n",[self()]),
	io:format("cua ~p~n",[self()]),
	unlock(L),
	W!finished.

waiter (L,0)  -> destroyLock(L), io:format("test complete~n");
waiter (L,N)  -> receive finished -> waiter(L,N-1) end.

waiter_sem (S,0)  -> destroySem(S), io:format("test complete~n");
waiter_sem (S,N)  -> receive finished -> waiter_sem(S,N-1) end.

testLock () -> L = createLock(),
	W=spawn(?MODULE,waiter,[L,3]),
	spawn(?MODULE,f,[L,W]),
	spawn(?MODULE,f,[L,W]),
	spawn(?MODULE,f,[L,W]),
	ok.

sem (S,W) -> 
	semP(S),	
	io:format("uno ~p~n",[self()]),
	io:format("dos ~p~n",[self()]),
	semV(S),
	W!finished.

testSem () -> S = createSem(2),
	W=spawn(?MODULE,waiter_sem,[S,5]),
	spawn(?MODULE,sem,[S,W]),
	spawn(?MODULE,sem,[S,W]),
	spawn(?MODULE,sem,[S,W]),
	spawn(?MODULE,sem,[S,W]),
	spawn(?MODULE,sem,[S,W]).