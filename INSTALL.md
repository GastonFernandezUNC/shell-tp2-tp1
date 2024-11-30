# INSTALL.md
## Installation process for the **Shell** project

- Requirements
    - microhttpd-dev lib
    ```
    sudo apt install libmicrohttpd-dev
    ```
---
- Steps to build the Shell Project
1. Clone the repository
    ```
    git@github.com:ICOMP-UNC/so-i-24-chp2-GastonFernandezUNC.git
    ```

2. Switch to the dev branch 
    ```
    git switch dev
    ```

3. Download **metrics-tp1-so1** submodule 
    ```
    cd metrics-tp1-so1
    git submodule update --init --recursive
    ```

4. Set the python virtual environment with conan
    ```
    python3 -m venv .venv
    source .venv/bin/activate
    pip install conan
    ```

5. Set conan by stablishing a profile 
    ```
    conan profile detect
    ```

6. Run the `setup.sh` script, and choose option `0` (build as release), it will run the conan installation and cmake
    ```
    sh setup.sh
    0
    ```

7. This will create the `build` directory, go into it and build the project.
    ```
    cd build
    make
    ```

8.  The project must have been build, and its name is Shell. \
It has some built-in commands, like:
    
    - `cd -` It changes directories to the previous one
    - `clr` It essentially is the same as `clear`
    - It can handle the execution of a list of commands contained within a file if its path it's passed an argument `./Shell commands.txt`
    - `start_monitor` Starts the `metrics` process, exposing data into the `localhost:8000/metrics`
    - `stop_monitor` Kills/interrupts the `metrics` process
    - `status_monitor` Prints some data in the shell
    - `config_monitor` Configures what data gets printed into the shell, and can be used to modify the refresh frequency of the metrics. Configurations won't take effect until `status_monitor` is ran
    
9. The config.json file can be manually modified, but it will be considered **ONLY BEFORE** the `make` command is run, since it's copied into another directory, and then it has to be modified using the `config_monitor` command from the shell