# ft_ping

## Introduction
`ft_ping` is a C-based project that aims to recreate the functionality of the traditional `ping` command, which is used to test the reachability of a host on an IP network and measure the round-trip time for messages sent from the originating host to a destination computer.

## Table of Contents
- [Installation](#installation)
- [Features](#features)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Installation

To install `ft_ping`, you will need a C compiler and make. Here's how you can compile the project:

```bash
git clone https://github.com/Apyre83/ft_ping.git
cd ft_ping
make
```

This will compile the source files and produce an executable named `ft_ping`.

## Features

`ft_ping` replicates the basic features of the traditional `ping` command, including:
- Sending ICMP echo requests to a host to measure the network latency.
- Displaying the round-trip time for each received packet.

## Usage

To use `ft_ping`, run the executable followed by the hostname or IP address of the target host:

```bash
sudo ./ft_ping google.com
```

Note: `ft_ping` may require administrative privileges to create ICMP packets, depending on your system's configuration.

## Contributing

If you'd like to contribute to `ft_ping`, please fork the repository and use a feature branch. Pull requests are warmly welcome.

## License

This project is freely distributed under the MIT License. See `LICENSE` for more details.