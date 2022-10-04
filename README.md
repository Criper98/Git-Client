# Git-Client
Un client per Git realizzato interamente in C++.  
Dipendenze: [Git Bash](https://gitforwindows.org/).  
OS: Windows7/10/11 x64

Funzioni generali:
- Creazione di un repo da 0.
- Clonare un repo esistente.
- Aggiungere un repo locale alla lista dei repo salvati.
- Rimuovere un repo dalla lista.
- Impostazioni di Debug e dimensione della console.

Funzioni per i singoli repository:
- Stage dei cambiamenti (equivalente al comando "git add").
- Commit con relativo commento.
- Push al repository remoto, in caso non sia impostato verrà chiesto l'URL.
- Cambiare Branch scegliendo tra quelli esistenti.
- Creare un nuovo Branch.
- Eliminare un Branch.
- Pull dal repository remoto.
- Fetch, il Fetch viene eseguito automaticamente all'apertura del repository.
- Stash, esegue automaticamente Stash e Drop per annullare i cambimenti.

Funzionalità integrate:
- Status sempre aggiornato ad ogni focus della finestra della console.
- Branch attivo riportato in maniera dinamica.
- Sistema di log semplice o verboso in base all'impostazione "Debug"

----
## TODO

- [ ] Check degli Update all'avvio.
- [ ] Integrare il comando Merge.
- [ ] Integrare gitignore.

----

![Immagine 2022-09-02 153026](https://user-images.githubusercontent.com/62108281/188157404-373d81fd-1620-42e3-8d15-ba3d347fa887.png)

![Immagine 2022-09-02 153253](https://user-images.githubusercontent.com/62108281/188157459-c44b395c-a7a4-4c66-a057-24ecbc4732c5.png)
