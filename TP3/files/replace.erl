-module(replace).
-compile(export_all).

hello() ->
   {A1, A2, A3} = now(),
   random:seed(A1, A2, A3),
   ?MODULE:helloloop().

helloloop() ->
   receive
      after 1000 -> ok
   end,
   io:format("Hello ~p~n",
      [case random:uniform(10) of 10 -> 1/uno; _ -> self() end]),
   ?MODULE:helloloop().

supervisor() ->
   process_flag(trap_exit, true),
   spawn_link(?MODULE, hello, []),
   receive
      stop -> ok;
      {'EXIT', _From, _Reason} ->
         ?MODULE:supervisor()
   end.

init() ->
   spawn(?MODULE, supervisor, []).