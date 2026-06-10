# Building and Running the StoneAge Servers

The suite has two daemons, built and verified on Ubuntu 24.04 / gcc 13 (x86-64):

| binary | role | listens on |
|---|---|---|
| `saac/saac` | account/auth server (MariaDB-backed) | 9300 (game servers), 9400 (workers) |
| `gmsv/gmsv` | game world server | 1234 (game clients) |
| `saac/wk/acwk` | account housekeeping worker | - |

## Prerequisites

```sh
apt-get install build-essential libmariadb-dev libmariadb-dev-compat \
                mariadb-server mariadb-client
```

## Build

```sh
make            # builds both servers (or make -C gmsv / make -C saac)
```

The makefiles promote the 64-bit-critical diagnostics to errors
(implicit declarations, pointer/int casts, return types) and compile
with `-fno-strict-aliasing -fwrapv`, which this 1990s codebase assumes.

## Database setup (once)

Credentials and column names live in `saac/acserv.cf`. The matching
schema:

```sql
CREATE DATABASE IF NOT EXISTS sa CHARACTER SET utf8mb4;
CREATE USER IF NOT EXISTS 'stoneage'@'localhost' IDENTIFIED BY '123456789';
CREATE USER IF NOT EXISTS 'stoneage'@'127.0.0.1' IDENTIFIED BY '123456789';
GRANT ALL PRIVILEGES ON sa.* TO 'stoneage'@'localhost';
GRANT ALL PRIVILEGES ON sa.* TO 'stoneage'@'127.0.0.1';
USE sa;
CREATE TABLE IF NOT EXISTS logindata (
  name      VARCHAR(32)  NOT NULL PRIMARY KEY,
  password  VARCHAR(64)  NOT NULL,
  regtime   DATETIME     NULL,
  logintime DATETIME     NULL,
  path      VARCHAR(255) NULL,
  online    VARCHAR(64)  NULL
) CHARACTER SET utf8mb4;
```

`AutoReg 1` in acserv.cf means unknown accounts are auto-registered on
first login.

## Run

```sh
service mariadb start
(cd saac && ./saac)     # wait for "端口:9300"
(cd gmsv && ./gmsv)     # world boot takes 2-4 minutes
```

Boot is verified when gmsv prints the heartbeat line
(`玩家=0 宠物=0.0% ...`) and saac logs `服务器密码正确 公益石器`
(gmsv authenticated). Both servers shut down cleanly on SIGTERM
(gmsv saves running data first).

`gmsv/setup.cf` is the world config: ports, the saac address/password
(`acserv`/`acservport`/`acpasswd`, must match `pass` in acserv.cf),
data paths, and the memory pool. The pool
(`usememoryunit` x `usememoryunitnum`) is sized 40M units (~5.1 GB)
here: pointer-heavy structures roughly double on 64-bit, so the
historical 15M-unit (1.9 GB) setting exhausts mid-boot.

## Known caveats

- ~1.4k of the ~10.7k entries in `data/itemset6.txt` (mostly 合成/compose
  gear) fail the 2005 parser's field validators and are skipped at boot
  with `[ITEM data Error]` messages. The mismatch is identical with the
  original GB-encoded data - the data pack is from a newer patch level
  than this server source. Cosmetic at boot; those items just don't exist.
- `data/raceman.txt` was missing from the recovered data; a minimal valid
  stub is committed (pet-race NPCs need real entries to be meaningful).
- All sources and text data are UTF-8 (converted from the original
  EUC-JP/GBK mix). A 2005-era Big5/GB client would display garbage; any
  client work needs a matching encoding decision on the wire.
- Remaining compiler warnings are cosmetic classes (unused variables,
  intentional truncation); the dangerous classes were fixed.
