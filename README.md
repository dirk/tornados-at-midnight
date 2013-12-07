## Building

For building on Mac you'll need a copy of gcc-4.8. This can be installed via Homebrew by doing `brew tap homebrew/versions` followed by `brew install gcc48`. Then configure ns-3 with the GCC 4.8 version of g++:

    CXX="g++-4.8" ./waf configure

## Using the Samples

Running the Wifi mesh sample:

    ./run.sh --sample=wifi-mesh

Listing all possible samples:

    ./run.sh --sample=list

### License

Released under the MIT license. See LICENSE for details.
