#!/bin/bash
xterm -title "serverG" -e bash -c "./eseguibili/serverG.out;exec bash" &
sleep 0.2
xterm -title "serverV" -e bash -c "./eseguibili/serverV.out;exec bash" &
sleep 0.2
xterm -title "centro_vaccinale" -e bash -c "./eseguibili/centro_vaccinale.out;exec bash" &
sleep 0.2
xterm -title "app_verifica" -e bash -c "./eseguibili/app_verifica.out;exec bash" &
sleep 0.2
xterm -title "asl" -e bash -c "./eseguibili/asl.out;exec bash" &
sleep 0.2
xterm -title "utente" -e bash -c "./eseguibili/utente.out;exec bash"
