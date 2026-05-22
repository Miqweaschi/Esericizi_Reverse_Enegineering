# Esercizio 1

##  Informazioni sul Binario
* **File:** `Esercizio1`
* **Architettura:** x86-64 Linux ELF
* **Livello:** Intermedio
* **Protezioni/Ottimizzazioni:** Stripped (`-s`), Anti-Debugging (`ptrace`).

##  Descrizione della Sfida
L'obiettivo della sfida è bypassare i controlli di sicurezza di un binario ottimizzato per ottenere l'accesso di root. Il software implementa una protezione multi-stage in cui la chiave 
di licenza valida non è statica, ma dipende esplicitamente dall'algoritmo di hashing applicato all'username inserito.

---

##  Analisi Statica (Ghidra)

### 1. Protezione Anti-Debugging
All'inizio dell'esecuzione, il binario invoca una subroutine di sicurezza (`FUN_001011a9`) che sfrutta la chiamata di sistema `ptrace`:

```c
lVar1 = ptrace(PTRACE_TRACEME, 0, 1, 0);
if (lVar1 < 0) {
    puts(" Errore di sistema...");
    exit(1);
}
```
Il flag PTRACE_TRACEME richiede al kernel di tracciare il processo attuale. Se un debugger (es. GDB) è già agganciato, la chiamata fallisce ritornando -1, forzando l'uscita immediata dell'applicazione.

### 2. Hashing con DJB2

```c
// Valore iniziale (Seed): 0x1337 (4919)
// Moltiplicatore costante ad ogni iterazione: 0x21 (33)
local_1c = (int)param_1[local_20] + local_1c * 0x21;
```

### 3. I Caratteri segreti

In questo caso, i caratteri segreti che corrispondono alla license_key dell'user non risiedono in memeoria ma sono inseriti come operandi direttamente nella funzione

```c
local_12 = 0xefbeadde;
local_e = 0x42;
```
Considerando l'architettura Little Endian, la lettura sequenziale dei byte mappa la costante segreta: 0xDE, 0xAD, 0xBE, 0xEF, 0x42.

### 4. Risoluzione Matematica dello XOR

Il decompiler mostra il controllo finale strutturato come uno XOR bit-a-bit tra l'input dell'utente e il byte dell'hash corrispondente:

```c
Input[i] ^ Hash_Byte[i] == Expected_Byte[i]
```

Sfruttando la proprietà associativa e l'involutività dell'operazione logica XOR (per cui $A \oplus B = C \iff A \oplus C = B$), è possibile isolare l'input sconosciuto dell'utente. 

L'equazione viene quindi invertita matematicamente per ricavare direttamente i singoli caratteri della licenza valida:

$$Input[i] = Expected\_Byte[i] \oplus Hash\_Byte[i]$$

Questa formula stabilisce che la chiave di licenza non è un valore statico hardcoded, ma una stringa dinamica che muta in base all'username inserito, poiché generata dall'interazione bit-a-bit tra le costanti fisse dello stack e i byte dell'hash estratti a runtime.

---

###  Applicazione Pratica (Caso di studio: `admin`)

Per convalidare la formula, viene presa come riferimento la stringa `admin`, il cui hash DJB2 calcolato e troncato a 32-bit risulta essere `0xD739DF00`. Due fattori hardware condizionano il calcolo:
1. **Rappresentazione Little Endian:** I byte dell'hash vengono estratti a passi di 8 bit partendo dall'estrema destra (`0x00`, `0xDF`, `0x39`, `0xD7`, `0x00`).
2. **Set di caratteri esteso (ISO-8859-1 / Windows-1252):** Le operazioni XOR generano valori numerici superiori a 127, mappati nel terminale come caratteri speciali estesi.

I singoli passaggi algebrici per i 5 cicli di verifica sono così determinati:

*   **Giro 0 ($i = 0$):**  
    $$0xDE \oplus 0x00 = 0xDE \implies \mathbf{\text{Þ}}$$ (Valore decimale 222)
*   **Giro 1 ($i = 1$):**  
    $$0xAD \oplus 0xDF = 0x72 \implies \mathbf{\text{r}}$$ (Valore decimale 114)
*   **Giro 2 ($i = 2$):**  
    $$0xBE \oplus 0x39 = 0x87 \implies \mathbf{\text{‡}}$$ (Valore decimale 135)
*   **Giro 3 ($i = 3$):**  
    $$0xEF \oplus 0xD7 = 0x38 \implies \mathbf{\text{8}}$$ (Valore decimale 56)
*   **Giro 4 ($i = 4$):**  
    $$0x42 \oplus 0x00 = 0x42 \implies \mathbf{\text{B}}$$ (Valore decimale 66)

Unendo i risultati vettoriali, la stringa di licenza esatta per l'utente `admin` è: **`Þr‡8B`**.


