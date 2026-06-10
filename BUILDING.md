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

## Encoding policy

- **Source code is UTF-8.** Comments are Simplified Chinese (the original
  Japanese comments were machine-restored from fossilized mojibake and
  then translated); a handful of undecipherable fossil fragments and
  katakana monster names remain as-is.
- **Game data stays GB18030** (`gmsv/data`, exact bytes of the original
  pack). This is deliberate: the 2005 parsers read fields into fixed
  64-byte buffers sized for 2-byte CJK - a UTF-8 conversion of the data
  (3-byte CJK) overflowed them and silently rejected ~95% of itemset6
  entries and broke 619 NPC enemy-spawn params. Era clients also expect
  GB bytes on the wire. Boot must show zero `[ITEM data Error]` lines;
  if you re-encode the data you will get ~10k of them.
- Code string literals (server messages) are UTF-8; an unmodified 2005
  client renders them as mojibake. If you pair this server with an
  original client, either convert literals at the protocol boundary
  (lssproto send/recv) or accept garbled server notices.

## Known caveats

- `data/raceman.txt` was missing from the recovered data; a minimal valid
  GB18030 stub is committed (pet-race NPCs need real entries to be
  meaningful).
- Remaining compiler warnings are cosmetic classes (unused variables,
  intentional truncation); the dangerous classes were fixed.
