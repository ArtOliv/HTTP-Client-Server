# Projeto Cliente-Servidor (HTTP)

**Languages:** [Portuguese](README.md) | [English](README.en.md)

---

## Descrição Geral

Este projeto implementa um **Cliente-Servidor em C**, utilizando **sockets TCP** e o protocolo **HTTP/1.1**, capaz de responder requisições de navegadores (como Chrome, Firefox e Edge) e de um cliente próprio desenvolvido também em C. O servidor lista arquivos, envia páginas HTML e responde com os códigos HTTP adequados (200, 404, etc). O cliente realiza requisições HTTP de arquivos e salva eles na pasta local.

---

## Estrutura do Projeto

```
.
├── Client/
│ ├── meunavegador.c
├── Server/
│ ├── meuservidor.c
├── README.md
└── Makefile
```

- `Server/` - Código fonte do servidor HTTP.  
- `Client/` - Código fonte do cliente que realiza requisições.  
- `Makefile` - Arquivo para compilação.  
- `README.md` - Documentação do projeto.

---

## Tecnologias Utilizadas
- **Linguagem C**
- **BSD Sockets (TCP/IP)**
- **Protocolo HTTP/1.1**
- **Linux (Ubuntu)**

---

## Como Compilar

> **Pré-requisito**  
> Certifique-se de ter o compilador GCC instalado
> ```bash
> sudo apt install build-essential
> ```

Na própria pasta do projeto execute no terminal:

```bash
make
```

Os arquivos objeto e os executáveis serão criados nas pastas dos códigos fonte, como no exemplo:

```
.
├── Client/
│ ├── meunavegador.c
| ├── meunavegador.o
| ├── meunavegador
├── Server/
│ ├── meuservidor.c
│ ├── meuservidor.o
│ ├── meuservidor
```

Para excluir os arquivos objeto e executáveis, execute:

```bash
make clean
```

Para rebuildar, execute:

```bash
make rebuild
```

---

## Como Executar

### Servidor

Entre na pasta `Server` em um terminal e execute:

```bash
./meuservidor /home/usr/caminho/para/seu/diretorio
```

Para rodar o servidor e servir o diretório de arquivos. O servidor tem suporte para servir vários tipos de arquivos sendo **.html, .gif, .pdf, .jpeg, .png**, alguns deles.

Caso o arquivo da requisição não exista o erro **404 Not Found** é retornado como resposta.

Além disso possui um timeout de dois minutos caso o cliente fique inativo, fechando a conexão e voltando a esperar por outra.

### Cliente

Entre na pasta `Client` em outro terminal e execute:

```bash
./meunavegador http://host:porta/index.html
```

Essa é a forma de fazer a requisição. Caso não especifique a porta o cliente tentará conexão na porta **80 (padrão HTTP)**.

Caso não requisite nenhum arquivo ou não exista um index.html o cliente recebe como resposta (em HTML) a lista de arquivos presente no diretório do servidor.

Para acessar pelo navegador, na barra de pesquisa digite o host, a porta e o arquivo.

---

## Exemplo de Requisição

Requisição:

```
GET /index.html HTTP/1.1
Host: 127.0.0.1
Connection: close
```

Resposta:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Lenght: 48
Connection: close

<html>
  <body>
    <h1>Hello World!</h1>
  </body>
</html>
```
---

## Funcionalidades

- ✅ **Suporte a múltiplas conexões (multi-clientes)**
- ✅ **Cabeçalhos HTTP completos**
- ✅ **Compatível com navegadores reais**
- ✅ **Tratamento de erros (404, 400)**
- ✅ **Listagem automática de diretórios**
- ✅ **Logs de conexão no terminal**
- ✅ **Timeout por inatividade do cliente**

---

## Autor

`Arthur Carvalho Rodrigues Oliveira`

Projeto desenvolvido para estudo de Programação em Rede.

---

## Licença

Este projeto está licenciado sob a MIT License — Sinta-se à vontade para usar meu projeto da forma que quiser
