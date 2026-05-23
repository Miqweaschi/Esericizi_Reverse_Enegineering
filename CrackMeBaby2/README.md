# CrackMeBaby2

##  Informazioni sul Binario
* **File:** `CanYouCrackMe2`
* **Architettura:** x86-64 Windows 
* **Livello:** Intermedio
* **Protezioni/Ottimizzazioni:** Stripped (`-s`), Anti-Debugging, Falso bersaglio, Cifratura dei Dati
* **Tool Utilizzato:** Ghidra
* **Autore:** igr0t7
* **link:** https://crackmes.one/crackme/6a0bd0592b3df128c1df5c16

##  Descrizione della Sfida
Trovare la combinazione giusta di stringhe per vincere la sfida. ( "You Win.. (;" ) 

##  Analisi Statica (Ghidra)

### 1. Trovare il main

Trovare il main è stato abbastanza semplice, da ghidra è bastato partire dalla Function *entry* che conteneva 2 funzioni ( FUN_140003f90() e FUN_1400036e4() ) 
la prima è l'inizializzazione del compilatore, mentre l'altra è quella che alla fine chiama il codice di inizializzazione del runtime C/C++ (CRT) generato automaticamente dal compilatore Microsoft Visual Studio 
dove alla fine trovo la vera chiamata al main
```c++
iVar3 = main(uVar6,uVar4,uVar8);  //funzione rinominata da me per una questione di ordine.
```

### 2. Anti-Debugger

Una volta nel main la prima cosa che ho fatto è stato cercare le stringhe scritte dall'autore per identificare una sorta di percorso da seguire a ritroso per poter trovare la combinazione giusta,
e ho notato la riga di codice:
```c++
MessageBoxA((HWND)0x0,"Debugger detected! Bye (:","Anti-Debug",0x10); 
```
Quindi, è presente un Anti-Debug, uilizzando Ghidra non è un problema per me.


### 3. Trappola

Vedendo le righe successive, noto diverse inizializzazioni di array allocati dinamicamente, tra cui il primo:
```c++
_Buf2 = (undefined4 *)operator_new(0x20);
  *_Buf2 = 0x67616c66;  
  _Buf2[1] = 0x6e61637b;
  _Buf2[2] = 0x756f792d;
  _Buf2[3] = 0x6172632d;
  _Buf2[4] = 0x7d3f6b63;
  *(undefined *)(_Buf2 + 5) = 0;
```
Tradotto da Esadecimale a Decimale e confrontiamo il valore con la tabella ASCII ( ovviamente al contrario Questo perché i processori con architettura x86/x64 (Intel e AMD) utilizzano una rappresentazione della memoria definita Little-Endian ) otteniamo la frase.
*flag{can-you-crack?}* 
che se inserita da Access Denied, quindi una falsa strada.

### 3. La PRIMA Frase

Come ho detto c'erano altri array allocati dinamicamente e se controlliamo il successivo di quello che si è rivelato un vicolo cieco:
```c++
  _Buf2_00 = (undefined4 *)operator_new(0x20);
  *_Buf2_00 = 0x6d6b666c;
  _Buf2_00[1] = 0x786d6330;
  _Buf2_00[2] = 0x7e277e3a;
  _Buf2_00[3] = 0x63616f62;
  *(undefined2 *)(_Buf2_00 + 4) = 0x6d64;
  *(undefined *)((longlong)_Buf2_00 + 0x12) = 0;
```
Stesso procedimento di prima, otteniamo la frase lfkm0cmx:~'~boacdm, la prima cosa che ho pensato è stata "è una frase cifrata", andando in basso nel codice di Ghidra ho notato questo ciclo fondamentale:
```c++
    uVar11 = local_e0;
    if (local_e0 != 0) {
    do {
      puVar8 = &local_f0;
      if (0xf < local_d8) {
        puVar8 = (undefined *)CONCAT71(uStack_ef,local_f0);
      }
      puVar8[uVar12] = puVar8[uVar12] ^ 10;
      uVar12 = uVar12 + 1;
      uVar11 = local_e0;
    } while (uVar12 < local_e0);
  }
```
Questo ciclo prende l'input dell'utente (puVar8) e applica l'operatore ^ (che in C/C++ rappresenta lo XOR bitwise) tra ogni singolo carattere e la costante decimale 10.
Poiché l'operazione di XOR è simmetrica (invertibile), per scoprire la stringa di partenza che il programma si aspetta, mi è bastato applicare nuovamente lo XOR con il valore 10 a tutti i caratteri della stringa cifrata.
otteniamo la Frase *flag:igr0t-theking*
se inserita:
```bash
  You are good , but...
  Say my name...
```

### 3. La SECONDA Frase

La ricerca continua e qui le strade si dividono in un modo divertente:

**La prima strada:** la frase che mi è uscita dopo che ho inserito la flag giusta dice esplicitamente *Say my name...* (grande citazione a Breaking Bad) quindi inserendo il nome dell'autore, Non funziona.
MA rivedendo la flag, noto che l'autore si firma anche solo igr0t, senza il 7 finale, quindi provo e... Funziona.
```bash
  You Win.. (;
```
**La seconda strada:** Cercando effettivamente il nome dell'autore all'interno del codice cerco sotto la parte di codice che mi porta alla stringa finale you win e trovo questo blocco di codice:
```c++
       FUN_140002610((longlong *)cin_exref,(void **)local_90,uVar11);
      uVar11 = local_78;
      puVar13 = (undefined *)local_90._0_8_;
      puVar10 = local_90;
      if (0xf < local_78) {
        puVar10 = (undefined *)local_90._0_8_;
      }
      if ((local_80 == 5) && (iVar5 = memcmp(puVar10,local_130,5), iVar5 == 0)) {
        plVar9 = FUN_140002430((longlong *)cout_exref,"You Win.. (;");
```
Il codice inzia chiedendo all'utente effettivamente il nome e viene inserito nella variabile local_90, successivamente viene salvato il valore anche nella variabile puVar10 e viene fatto un confronto tra puVar10
e un altra variabile: *local_130. Andando a vedere cosa contiene questa variabile trovo:
```c++
      local_130._0_6_ = 0x7430726769;
```
se tradotto otteniamo: *igr0t*.
```bash
  You Win.. (;
```
