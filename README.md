# InvJac Project

## Overview

The InvJac project is a sophisticated mathematical software application developed to test a hypothesis concerning the sum of the reciprocals of the Jacobian of polynomial maps. The application is capable of verifying contractions and generating points that possess a unit Jacobian.

## Dependencies

- **GiNaC**: A C++ library for symbolic mathematical calculations.
- **Glog**: A C++ logging library.
- **Boost**: Used for program options parsing.
- **MySQL Connector/C++**: A MySQL database connector for C++.
- **PHCpack**: A software package for solving polynomial systems by homotopy continuation.
  *Note: The PHCpack library required some fixes, so I am using a forked version. You can find the forked repository here: [https://github.com/nook0110/PHCpack/tree/file-fix](https://github.com/nook0110/PHCpack/tree/file-fix)*

## Building the Project

To build the project, ensure you have the necessary dependencies installed. Then, use CMake to configure and build the project. Here are the steps:

1. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```

2. Run CMake to configure the project:
   ```sh
   cmake ..
   ```

3. Build the project:
   ```sh
   make
   ```

## Running the Application

### Running Locally

After building the project, you can run the application using:

```sh
./invjac
```

The application will prompt you for input to generate and check polynomial maps.

### Running via Docker

You can also run the application using Docker. The Docker image is hosted on Docker Hub and will be pulled automatically. Use the following command:

```sh
docker run --pull always --rm --name invjac -i nook0110/invjac:prog
```
If you want to use a locally hosted MySQL database, you should add the following commands at the end: `-d --db-host host.docker.internal`

```sh
docker run --pull always --rm --name invjac -i nook0110/invjac:prog -d --db-host host.docker.internal
```

## Usage

### Command Line Options

The application supports several command line options:

- `-h, --help`: Display the help message.
- `-i, --input-file`: Specify an input file for the map data.
- `-d, --use-database`: Use a MySQL database to store results.
- `--db-host`: Specify the database host.
- `--db-user`: Specify the database user.
- `--db-password`: Specify the database password.
- `--db-name`: Specify the database name.
- `--results-table`: Specify the results table name.
- `--passed-table`: Specify the passed results table name.
- `--failed-table`: Specify the failed results table name.
- `--error-table`: Specify the error results table name.

### MySQL Database Setup

To use the MySQL database, you need to create the necessary tables. Here is the SQL script to create the required tables:

```sql
-- Main table for all test results
CREATE TABLE test_results (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    result_type ENUM('Passed', 'Failed', 'Error', 'Contraction') NOT NULL,
    run_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- Table for Passed results
CREATE TABLE passed_results (
    result_id INT PRIMARY KEY,
    amount_of_tests INT UNSIGNED NOT NULL,
    FOREIGN KEY (result_id) REFERENCES test_results(id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Table for Failed results
CREATE TABLE failed_results (
    result_id INT PRIMARY KEY,
    test TEXT NOT NULL,
    result TEXT NOT NULL,
    FOREIGN KEY (result_id) REFERENCES test_results(id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Table for Error results
CREATE TABLE error_results (
    result_id INT PRIMARY KEY,
    error TEXT NOT NULL,
    FOREIGN KEY (result_id) REFERENCES test_results(id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Table for Contraction results
CREATE TABLE contraction_results (
    result_id INT PRIMARY KEY,
    contraction_point TEXT NOT NULL,
    FOREIGN KEY (result_id) REFERENCES test_results(id) ON DELETE CASCADE
) ENGINE=InnoDB;
```

You can execute this script in your MySQL database to create the necessary tables.

### Example

To run the application with an input file and use a database, you might use:

```sh
./invjac -i input.txt -d --db-host localhost --db-user root --db-password root --db-name checked_functions
```

## Documentation

For detailed documentation, please visit: [InvJac Documentation](https://nook0110.github.io/InvJac/html/index.html)

## Contributing

Contributions to the project are welcome. Please fork the repository and submit pull requests with your changes.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
