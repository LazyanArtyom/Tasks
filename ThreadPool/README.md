# A thread pool that uses work stealing

## Building and run

```bash
> ./build.sh
> ./threadpool
```

## Some benchmarks

custom `paralelAccumlate` func vs standard `std::accumlate` func

used `std::vector` with `100 000 000` data.

## std::accumlate

duration: 1sec

## paralelAccumlate

`with 6 threads` duration: 13sec
