![RImStone Logo](https://rimstone-lang.com/rimstone.png)

RimStone is memory-safe, high-performance programming language and framework. It's general purpose and web enabled. It's simple yet powerful and feature-rich. 

Build web applications, distributed systems, middleware and command-line applications in minutes. Try [Hello World](https://rimstone-lang.com/article-hello.html) now, both as a command-line program and a service. RimStone applications are highly-optimized native executables without Virtual Machines.

[Example](#example)

[Tutorial](#tutorial)

[Documentation](#documentation)

[Install](#install)

[Blog](#blog)

[Web site](#web-site)

[License](#license)

## Example

Here's what RimStone code looks like. It's just a tiny little snippet, and the highlighting is automatic in Vim editor. This handles the upload of a file, updating the database, and showing a confirmation web page to a web client (it's a part of [web file manager in less than 100 lines of code](https://rimstone-lang.com/article-file-manager.html) example:

![Rimstone Example](https://rimstone-lang.com/rimstone-code-example.png)

## Tutorial

Learn RimStone in no time with a number of step-by-step easy-to-replicate [full examples](https://rimstone-lang.com/examples.html).

## Documentation

RimStone comes with well-written [documentation](https://rimstone-lang.com/documentation.html).

## Install

RimStone installs as source only and it's built on the spot. RimStone does NOT need sudo - it installs in .rimstone folder in your home directory. Here's what you need to do to install RimStone:

```bash
wget "https://github.com/rimstone/rimstone/archive/refs/heads/main.tar.gz" -O main.tar.gz
tar xvfz main.tar.gz
cd rimstone-main
./riminst.sh
. $HOME/.bashrc
```
RimStone uses toolkit libraries for some functionality (such as MariaDB/PostgreSQL/SQLite client, libxml2, CURL, OpenSSL), and to install those you'll need sudo of course (the RimStone installer can install those for you or you can do it yourself).  

You can also install from the github repo, or from a package (such as for apt, dnf, zypper or pacman) - see [all installation methods](https://rimstone-lang.com/install-rimstone.html). All the files (including the above main.tar.gz as well as the packages) are in [RimStone releases](https://github.com/rimstone/rimstone/releases/tag/5.0.0).

## Blog
Bookmark [RimStone blog](https://rimstone-lang.com/blog.html) for the latest releases, articles, examples etc.

## Web site
Visit https://rimstone-lang.com for documentation, examples, blog and more information about RimStone.

## License
RimStone is licensed under Apache 2 License, see https://rimstone-lang.com/rimstone-license.html
