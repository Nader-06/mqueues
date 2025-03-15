```
git add .
git commit -m "YOUR MESSAGE HERE"
git push origin main
```
export LD_LIBRARY_PATH=.
gcc -shared -o libclaves.so claves.c -Wall -Wextra -g -pthread -lrt -fPIC^C
