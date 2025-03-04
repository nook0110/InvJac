# InvJac Project

![Test Workflow](https://github.com/nook0110/InvJac/actions/workflows/test.yml/badge.svg)

## Overview

The InvJac project tests a hypothesis concerning the sum of the reciprocals of the Jacobian of polynomial maps. It verifies contractions and generates points with a unit Jacobian.

## Dependencies

- **GiNaC**: C++ library for symbolic calculations.
- **Glog**: C++ logging library.
- **Boost**: Program options parsing.
- **MySQL Connector/C++**: MySQL database connector for C++.
- **PHCpack**: Solves polynomial systems by homotopy continuation.
  *Note: Using a forked version with fixes: [PHCpack fork](https://github.com/nook0110/PHCpack/tree/file-fix)*

## Building the Project

Ensure dependencies are installed. Use CMake to configure and build:

1. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```

2. Run CMake:
   ```sh
   cmake ..
   ```

3. Build:
   ```sh
   make
   ```

## Running the Application

### Running Locally

After building, run the application:

```sh
./invjac
```

### Running via Docker

Run the application using Docker:

```sh
docker run --pull always --rm --name invjac -i nook0110/invjac:prog
```

To use local MySQL, run with extra keys:

```sh
docker run --pull always --rm --name invjac -i nook0110/invjac:prog -d --db-host host.docker.internal
```

## Usage

### Command Line Options

- `-h, --help`: Display help.
- `-i, --input-file`: Specify input file.
- `-d, --use-database`: Use MySQL database.
- `--db-host`: Database host.
- `--db-user`: Database user.
- `--db-password`: Database password.
- `--db-name`: Database name.
- `--results-table`: Results table name.
- `--passed-table`: Passed results table name.
- `--failed-table`: Failed results table name.
- `--error-table`: Error results table name.

### MySQL Database Setup

Create necessary tables with this SQL script:

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

### Example

Run the application with an input file and database:

```sh
./invjac -i input.txt -d --db-host localhost --db-user root --db-password root --db-name checked_functions
```

## Documentation

For detailed documentation, visit: [InvJac Documentation](https://nook0110.github.io/InvJac/html/index.html)

## Contributing

Contributions are welcome. Fork the repository and submit pull requests.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
