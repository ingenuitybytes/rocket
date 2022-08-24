# Rocket
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/tutu-inc/rocket?color=GREEN&label=Github%20Code%20Size&logo=Github)](https://github.com/Tutu-Inc/Rocket.git)
[![GitHub repo size](https://img.shields.io/github/repo-size/tutu-inc/rocket?label=Docker%20Image%20Size&logo=Docker&logoColor=FFF)](https://github.com/Tutu-Inc/Rocket/pkgs/container/rocket)
[![Discord](https://img.shields.io/discord/488677373835870208?color=5865f2&label=Discord&logo=Discord&logoColor=FFF)](https://discord.com/invite/bQzf79Y)
[![GitHub license](https://img.shields.io/github/license/tutu-inc/rocket?label=License)](https://github.com/Tutu-Inc/Rocket/blob/master/LICENSE)
[![GitHub top language](https://img.shields.io/github/languages/top/Tutu-Inc/Rocket?color=A07&label=Top%20Language%20&logo=C%2B%2B)](https://github.com/Tutu-Inc/Rocket)

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#versioning">Versioning</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About the Project
This repository is built for the Raspberry Pi Pico.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started
A [Docker Package](https://github.com/Tutu-Inc/Rocket/pkgs/container/rocket) is provided via Github's Container Registry to build and upload the code to a Raspberry Pi Pico. 

### Prerequisites
To use this repository you have to install [Docker Desktop](https://www.docker.com/products/docker-desktop/) on your PC/MAC. If you are using a Raspberry Pi through SSH to upload the code to your Raspberry Pi Pico, you have to install Docker Engine following this [guide].(https://docs.docker.com/engine/install/ubuntu/)

### Installation
To install and setup the package follow the steps bellow:
1. Step

Use CLI or CMD to install the package. You may have to enable privilege user.
```bash
docker pull ghcr.io/tutu-inc/rocket:latest
```

_It could be helpful to setup [Portainer](https://docs.portainer.io/v/ce-2.9/start/install/server/docker/linux), if you are not comfortable with CLI_

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->
## Usage
examples, screenshots, code examples, demos

_For more examples, please refer to the [Documentation](https://github.com/Tutu-Inc/Rocket/wiki)_

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ROADMAP -->
## Roadmap

### Rocket
- [ ] Basic module communication
- [ ] Schematic of FC
- [ ] Processing sensor data
- [ ] Testflight with FC and peripherals
- [ ] Processing sensor data (Setup Kalmanfilter)
- [ ] Design rocket
    - [ ] Ejection mech
    - [ ] TVC mech
- [ ] Simulate rocket trajectory

### Ground station
- [ ] Design launch pad
    - [ ] Clamps
    - [ ] Pad
- [ ] Communication between user and GC
- [ ] Program interface

_See the [open issues](https://github.com/Tutu-Inc/Rocket/issues) for a full list of proposed features (and known issues)_

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTRIBUTING -->
## Contributing
Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

_Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us_

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- VERSIONING -->
## Versioning

We use [Semantic Versioning](http://semver.org/) for versioning. For the versions
available, see the [tags on this
repository](https://github.com/tutu-inc/rocket/tags).

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

  - **Alois Zimmermann** - *Provided README Template* -
    [AttractiveToad](https://github.com/AttractiveToad)

_See also the list of
[contributors](https://github.com/tutu-inc/rocket/contributors)
who participated in this project_

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments
Inspiration, code snippets, librarys, etc.
* [--](https://github.com)
* [--](https://gist.github.com)

<p align="right">(<a href="#top">back to top</a>)</p>
