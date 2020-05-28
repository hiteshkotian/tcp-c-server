# Concurrent server 

TCP echo server (to begin with).

## Building the server code

Run the following

```make all```

By default the number of threads in the thread pool is set to 5.
To change this run the make command with the `THREAD_POOL_SIZE` parameter as follows.


```make all THREAD_POOL_SIZE=20```

The command above will compile the code with 20 threads


## Building the docs

Docuemntation is generated using doxygen. Make sure doxygen is installed.

```make doc```

### In progress

- [ ] Create callbacks to handle the incoming requests.
- [ ] Fix the latex build
- [ ] Add unit tests
- [ ] Using logging instead of printing to standard out