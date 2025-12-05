# Local IPC Chat – Comunicação Entre Processos com Pipes Nomeados

##  Objetivo
Demonstrar o uso de:
- Processos e threads POSIX,
- Mecanismos de sincronização (mutexes e semáforos),
- Comunicação entre processos (IPC) usando FIFO (pipes nomeados),
- Controle de entrada/saída concorrente.

---

##  Arquitetura do Projeto

projeto_final/

 ├── chat_server.c
 
 ├── chat_client.c
 
 ├── chat_server_fifo
 
 ├── chat_client_fifo
 
 └── Makefile (se quiser)
 


O sistema tem 2 processos:

### **1. Servidor**
- Aguarda mensagens do cliente via FIFO `chat_server_fifo`
- Responde usando `chat_client_fifo`
- Usa threads para:
  - Ler mensagens
  - Enviar mensagens

### **2. Cliente**
- Envia mensagens para o servidor
- Recebe respostas
- Também possui duas threads internas

Ambos utilizam:
- `pthread_create`, `pthread_join`
- `pthread_mutex`
- `sem_t` (opcional)
- `mkfifo`, `open`, `read`, `write`

---

# Local IPC Chat – Comunicação Entre Processos com Pipes Nomeados

## Objetivo
Demonstrar o uso de:
- Processos e threads POSIX,
- Mecanismos de sincronização (mutexes e semáforos),
- Comunicação entre processos (IPC) usando FIFO (pipes nomeados),
- Controle de entrada/saída concorrente.

---

## Arquitetura do Projeto

O sistema tem 2 processos:

### **1. Servidor**
- Aguarda mensagens do cliente via FIFO `chat_server_fifo`
- Responde usando `chat_client_fifo`
- Usa threads para:
  - Ler mensagens
  - Enviar mensagens

### **2. Cliente**
- Envia mensagens para o servidor
- Recebe respostas
- Também possui duas threads internas

Ambos utilizam:
- `pthread_create`, `pthread_join`
- `pthread_mutex`
- `sem_t` (opcional)
- `mkfifo`, `open`, `read`, `write`

---

##  Como Executar

### 1. Compilar
```bash
make

### 2. Executar o programa
### terminal 1
./server
###terminal 2++
./user

chat estabelecido :D
