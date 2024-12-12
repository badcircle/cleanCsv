# CSV MySQL Optimizer

A lightweight C tool that processes CSV files to make them compatible with MySQL imports. The tool handles various CSV formatting challenges including:
- Empty fields
- Fields containing commas
- Quoted fields
- UTF-8 encoding
- Proper escaping of special characters

## Features

- Preserves exact column positions and structure
- Handles empty cells correctly
- Processes quoted fields containing commas
- Maintains UTF-8 encoding
- Automatically escapes special characters
- Creates an optimized output file with the original filename + '_mysql.csv' suffix
- Displays the first row of processed data for verification

## Building

### Windows
```bash
gcc csv_mysql_optimizer.c -o csv_mysql_optimizer.exe
```

### Linux/MacOS
```bash
gcc csv_mysql_optimizer.c -o csv_mysql_optimizer
```

## Usage

```bash
./csv_mysql_optimizer input.csv
```

The program will:
1. Read the input CSV file
2. Process and optimize the content
3. Create a new file named `input_mysql.csv` (preserves base filename)
4. Display the first row of the processed data for verification

## Input/Output Examples

### Input CSV:
```csv
name,address,age
John,"123 Main St, Apt 4",30
Jane Doe,,25
"Smith, Robert",456 Oak Road,45
```

### Output CSV:
```csv
"name","address","age"
"John","123 Main St, Apt 4","30"
"Jane Doe","","25"
"Smith, Robert","456 Oak Road","45"
```

## Compatibility

- Works with Windows 10 (UTF-8 support)
- Compatible with Linux and MacOS
- Handles both Unix (LF) and Windows (CRLF) line endings

## Technical Details

- Maximum line length: 65,535 characters
- Supports:
  - UTF-8 encoded input files
  - Quoted and unquoted fields
  - Empty fields
  - Fields containing commas
  - Fields containing quotes (properly escaped)

## Common Issues

1. **Unclosed Quotes**: The program will warn about unclosed quotes in the input CSV
2. **File Access**: Ensure you have write permissions in the output directory
3. **Large Files**: Files are processed line by line, so memory usage remains constant

## License

Copyright (C) 2024  Adam Wood

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A  ARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
