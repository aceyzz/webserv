<img src="git_utils/banner.png" alt="logo" style="width: 100%">

<br>

---

<br>

<details>
<summary>FRENCH VERSION</summary>

<br>

# WEBSERV
Ce programme implémente un serveur web capable de servir des fichiers statiques et de gérer des requêtes CGI. Il est capable de gérer plusieurs connexions simultanées et de gérer des fichiers volumineux, de manière asynchrone et non-bloquante.


## Consignes :
> Le projet Webserv de l'école 42 consiste à implémenter un serveur web en C++ au standard C++98. Ce serveur doit être capable de servir des fichiers statiques et de gérer des requêtes CGI. Il doit également pouvoir gérer plusieurs connexions simultanées, des fichiers volumineux de manière asynchrone et non-bloquante. De plus, le serveur doit prendre en charge les méthodes HTTP GET, POST et DELETE, ainsi que les requêtes CGI pour GET et POST. Il doit être capable de gérer les erreurs et de retourner les codes de statut HTTP appropriés. Le serveur doit également pouvoir gérer les requêtes avec des headers et des bodies volumineux, avec une limite définie dans le fichier de configuration. Le comportement de référence pour ce projet est celui de Nginx.


<br>

## Documentations

- Voir ce [document](docs/webserv.md) pour plus d'informations sur le fonctionnement de notre implémentation du programme.
- Voir ce [document](docs/index_http.md) pour plus d'informations sur la liste des retours HTTP implémentés.
- Voir ce [document](docs/Readme.md) pour plus d'informations sur les liens vers des ressources utiles pour le projet.

<br>

## Installation

### Prérequis
- `CMake` (version 3.19.2)
- `Make` (version 4.2.1)
- `GCC` (version 9.3.0)
- `Clang` (version 10.0.0)
- `C++98`

### Installation
```bash
git clone https://github.com/aceyzz/webserv_beta.git
cd webserv_beta
make
```

### Utilisation
```bash
./webserv [config_file]
```

*`config_file` facultatif, si non spécifié, le programme utilisera le fichier de configuration par défaut `config/default.conf` (boring as hell, choose the `config/config.conf` file for a more interesting configuration)*


<br>

## Auteurs

- [cedmulle](https://github.com/aceyzz) - [intra42](https://profile.intra.42.fr/users/cedmulle)
- [cchevalier](https://github.com/ChrstphrChevalier) - [intra42](https://profile.intra.42.fr/users/waziz)

<br>

## License

Ce projet est sous licence MIT - voir le fichier [LICENSE](license.md) pour plus de détails.

</details>

<br>

---

<br>

<details>
<summary>ENGLISH VERSION</summary>

<br>

# WEBSERV
This program implements a web server capable of serving static files and handling CGI requests. It can manage multiple simultaneous connections and handle large files asynchronously and non-blockingly.

## Instructions:
> The Webserv project from 42 school consists of implementing a web server in C++ following the C++98 standard. This server must be able to serve static files and handle CGI requests. It should also manage multiple simultaneous connections, large files asynchronously and non-blockingly. Additionally, the server must support HTTP methods GET, POST, and DELETE, as well as CGI requests for GET and POST. It should handle errors and return appropriate HTTP status codes. The server must also handle requests with large headers and bodies, with a limit defined in the configuration file. The reference behavior for this project is based on Nginx.

<br>

## Documentation

> Most of the documentation is in French

- See this [document](docs/webserv.md) for more information about the implementation of our program.
- See this [document](docs/index_http.md) for more information about the list of implemented HTTP responses.
- See this [document](docs/Readme.md) for more information about useful resources for the project.

<br>

## Installation

### Prerequisites
- `CMake` (version 3.19.2)
- `Make` (version 4.2.1)
- `GCC` (version 9.3.0)
- `Clang` (version 10.0.0)
- `C++98`

### Installation
```bash
git clone https://github.com/aceyzz/webserv_beta.git
cd webserv_beta
make
```

### Usage
```bash
./webserv [config_file]
```

*`config_file` is optional. If not specified, the program will use the default configuration file `config/default.conf` (boring as hell, choose the `config/config.conf` file for a more interesting configuration).*

<br>

## Authors

- [cedmulle](https://github.com/aceyzz) - [intra42](https://profile.intra.42.fr/users/cedmulle)
- [cchevalier](https://github.com/ChrstphrChevalier) - [intra42](https://profile.intra.42.fr/users/waziz)

<br>

## License

This project is licensed under the MIT License - see the [LICENSE](license.md) file for details.

</details>