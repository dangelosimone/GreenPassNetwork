#!bin/bash
mkdir eseguibili
gcc centro_vaccinale/centro_vaccinale.c centro_vaccinale/function_centVacc.c wrapper.c -o eseguibili/centro_vaccinale.out
gcc 'clientS_[app_verifica]'/app_verifica.c wrapper.c -o eseguibili/app_verifica.out
gcc 'clientT_[asl]'/asl.c wrapper.c -o eseguibili/asl.out
gcc 'serverG_[server_verifica]'/server_G.c 'serverG_[server_verifica]'/server_verifica.c wrapper.c -o eseguibili/serverG.out
gcc 'serverV_[server_vaccinale]'/server_V.c 'serverV_[server_vaccinale]'/server_Vaccinale.c wrapper.c -o eseguibili/serverV.out
gcc utente/utente.c wrapper.c -o eseguibili/utente.out
cd eseguibili
chmod +x serverG.out serverV.out centro_vaccinale.out utente.out app_verifica.out asl.out
