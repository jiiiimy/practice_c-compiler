# What is this ?

Repo to try https://www.sigbus.info/compilerbook

## Get start

```
$ docker build . -t c-compiler
$ docker run --name c-compiler --interactive --tty --rm --volume $(pwd):/app c-compiler bash
```
