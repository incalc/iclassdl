# Iclassdl

> Inha Univ. I-Class Course Video Downloader

[![GitHub Release](https://img.shields.io/github/v/release/incalc/iclassdl?logo=github&logoColor=white&style=for-the-badge)](https://github.com/incalc/iclassdl/releases/)
![C](https://img.shields.io/badge/c-11-A8B9CC?logo=c&logoColor=white&style=for-the-badge)
![C++](https://img.shields.io/badge/c++-17-00599C?logo=c%2B%2B&logoColor=white&style=for-the-badge)
[![CMake](https://img.shields.io/badge/cmake->=3.15-064F8C?logo=cmake&logoColor=white&style=for-the-badge)](https://cmake.org/)
[![GitHub Workflow](https://img.shields.io/github/workflow/status/incalc/iclassdl/C++?logo=github&logoColor=white&style=for-the-badge)](https://github.com/incalc/iclassdl/actions)
[![License](https://img.shields.io/github/license/incalc/iclassdl?style=for-the-badge)](./LICENSE)

## ChangeLog

See [CHANGELOG](./CHANGELOG.md)

## Build

### Requirement

- C++ 17
- CMake(>=3.15)
- [Boost](https://www.boost.org/)(>=1.66)
  - OpenSSL
- [FFmpeg](https://ffmpeg.org/)

### Guide

1. Clone the repository:

    ```bash
    $ git clone https://github.com/incalc/iclassdl.git
    ```

2. Build the source:

    ```bash
    $ cd iclassdl
    $ cmake -S . -B build
    $ cmake --build build
    ```

## Usage

#### CLI

> ./iclassdl [Student ID] [Iclass Password] [Video URL] [Filename]

example:

```text
$ ./iclassdl 12191765 P@ssw0rd! https://learn.inha.ac.kr/mod/vod/view.php?id=12345 video.mp4
```

## License

```text
Iclassdl
Copyright (C) 2020  Seungjae Park

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```

Iclassdl is licensed under the [GPL 3.0](./LICENSE).
