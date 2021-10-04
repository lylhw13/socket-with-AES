Example for tcp socket with AES encrypt.

Use server and client chat as an example to explian useage.

# Build
```sh
sudo apt install libssl-dev
make
```

# Test

## generate a random file
```sh
head -c 2G < /dev/urandom > randomfile
```

## test socket tranfer
### server side
```sh
./server.out < randomfile
```
### client side
```sh
./client.out > randomfile.out
```

## check wehter the tranfer is ok
```sh
shasum -a 512 randomfile*
```

The program is test on 2G file. It works OK.
