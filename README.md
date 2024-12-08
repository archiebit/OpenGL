<div>
  <h1 align="center">Moon Mice - OpenGL as a C++ Class</h1>
  <br>
  <div align="center">
    <a href="https://github.com/archiebit/OpenGL">
      <img src=".github/logo.png" width="256" height="256"/>
    </a>
  </div>
</div>



## About
This program takes the OpenGL API registry in an XML file and encapsulates
a specific version of the API in a C++ class.

This is a small project, for personal study.



### How to run
To run the program, you must pass the path to the registry file in XML format
and the version number.

Example:
```
moogl --in="input.xml" --version=3.3 --profile=core
```

Here are all the flags.

| Option                  | Necessary  | Description                                                                                                                                           |
|:------------------------|:-----------|:------------------------------------------------------------------------------------------------------------------------------------------------------|
| `--version=<MAJ>.<MIN>` | Yes        | Sets the API version.<br/><br/>`MAJ` denotes the major version,<br/>`MIN` denotes the minor version.                                                  |
| `--profile=<NAME>`      | It depends | Sets the OpenGL profile. This flag must be present, if the version is 3.0 or higher.<br/><br/>Possible values for `NAME` are `core` and `compatible`. |
| `--in=<PATH>`           | Yes        | Specifies the path to the registry in XML format.                                                                                                     |
| `--out=<PATH>`          | No         | Specifies the path to the output file. The Default path is `.`.                                                                                       |



### How to build
Clone this repo in some folder.
```
mkdir moogl
cd    moogl
git clone https://github.com/archiebit/OpenGL.git .
```
<br/>
<br/>


Execute `make` (`mingw32-make` if you use MinGW) program with target config.
```
make config=win-x64-dev
```
<br/>
<br/>


Output will be in the __out__ folder.
>[!NOTE]
>Possible configs at the moment are: 'win-x64-dev' and 'win-x64-rel'



## Contacts
If you want message me something interesting or funny, do it via [Telegram](https://t.me/ArchieBIT).