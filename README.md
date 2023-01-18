# SO_Escalonador

Compilando execprocd com ```cc execprocd.c list.c semaphore.c -o execprocd```  
Compilando execproc com ```cc execproc.c semaphore.c -o execproc```  
Compilando cancela_proc com ```cc cancela_proc.c semaphore.c -o cancela_proc```  
Compilando termina_execprocessod com ```cc termina_execprocessod.c semaphore.c -o termina_execprocessod```

Utilizamos para teste o testproc.c, compilando com ```cc testproc.c -o testproc```

Instruções de uso:
    1 - executa execprocd
    2 - executa execproc como "./execproc <procName> <priority> <arg1> <arg2>...\n"
    3 - cancela um processo adicionado com ./cancela_proc e o PID do processo a ser cancelado
    4 - termina execução do execprocd executando termina_execprocessod
