# Computer Networking

## Project Overview

```plaintext
.
├── Makefile                        # Main makefile for building the project
└── `src`
    ├── common                      # Common utilities and functions used throughout the project
    │   ├── arguments_parser.c      # Functions for parsing command-line arguments
    │   ├── arguments_parser.h
    │   ├── handle_terminal.c       # Functions for handling terminal input and output
    │   ├── handle_terminal.h
    │   ├── prompts.c               # Functions for displaying user prompts
    │   ├── prompts.h
    │   ├── retry.c                 # Functions for retrying operations
    │   ├── retry.h
    │   ├── struct.h                # Struct definitions used throughout the project
    │   ├── utils.c                 # General utility functions
    │   └── utils.h
    ├── error_handling              # Error handling functions and messages
    │   ├── error_checking.c        # Functions for checking for errors
    │   ├── error_checking.h
    │   ├── error_messages.c        # Functions for displaying error messages
    │   └── error_messages.h
    ├── fildes_handling             # File descriptor handling and related modules
    │   ├── descriptor_control.c    # Functions for controlling file descriptors
    │   ├── descriptor_control.h
    │   ├── process_descriptors.c   # Functions for processing file descriptors
    │   ├── process_descriptors.h
    │   ├── socket_protocols_interface
    │   │   ├── common.c            # Common functions for socket protocols
    │   │   ├── common.h
    │   │   ├── delete_node_module.c
    │   │   ├── delete_node_module.h
    │   │   ├── protocol_commands.h
    │   │   ├── query_module.c
    │   │   ├── query_module.h
    │   │   ├── utility.h
    │   │   ├── withdraw_module.c
    │   │   └── withdraw_module.h
    │   └── user_interface          # User interface related functions and modules
    │       ├── content_module.c
    │       ├── content_module.h
    │       ├── join_module.c
    │       ├── join_module.h
    │       ├── leave_module.c
    │       ├── leave_module.h
    │       ├── show_module.c
    │       ├── show_module.h
    │       ├── user_commands.h
    │       └── utility.h
    ├── protocols                   # Protocol implementations (TCP and UDP)
    │   ├── TCP.c
    │   ├── user_commands.h
    │   ├── TCP.h
    │   ├── user_commands.h
    │   ├── UDP.c
    │   ├── user_commands.h
    │   ├── UDP.h
    │   └── user_commands.h
    └── main.c                      # Main entry point for the program
8 directories, 45 files
```

The program structure is organized into a main directory with a Makefile, and a `src` subdirectory containing the source code files. The `src` directory is further divided into several subdirectories, each focusing on a specific aspect of the program:

1. **common** - Contains utility functions, data structures, and other common components used throughout the program:
    * `arguments_parser.c` and `arguments_parser.h`: Argument parsing functionality.
    * `handle_terminal.c` and `handle_terminal.h`: Terminal handling functions.
    * `prompts.c` and `prompts.h`: Functions related to user prompts.
    * `retry.c` and `retry.h`: Functions for retrying operations.
    * `struct.h`: Definitions of common data structures.
    * `utils.c` and `utils.h`: General utility functions.

2. **error_handling** - Contains functions related to error handling and error messages:
    * `error_checking.c` and `error_checking.h`: Functions for checking and validating errors.
    * `error_messages.c` and `error_messages.h`: Functions for displaying error messages.

3. **fildes_handling** - Contains functions related to file descriptor handling and socket protocols:
    * `descriptor_control.c` and `descriptor_control.h`: Functions for controlling file descriptors.
    * `process_descriptors.c` and `process_descriptors.h`: Functions for processing file descriptors.

4. **fildes_handling/socket_protocols_interface** - Contains functions and modules related to socket protocols:
    * `common.c` and `common.h`: Common functions for socket protocols.
    * `delete_node_module.c` and `delete_node_module.h`: Module for deleting a node.
    * `protocol_commands.h`: Definitions of protocol commands.
    * `query_module.c` and `query_module.h`: Module for querying information.
    * `utility.h`: Utility functions for socket protocols.
    * `withdraw_module.c` and `withdraw_module.h`: Module for withdrawing a node.

5. **fildes_handling/user_interface** - Contains functions and modules for user interface components:
    * `content_module.c` and `content_module.h`: Module for managing content.
    * `join_module.c` and `join_module.h`: Module for joining a network.
    * `leave_module.c` and `leave_module.h`: Module for leaving a network.
    * `show_module.c` and `show_module.h`: Module for displaying information.
    * `user_commands.h`: Definitions of user commands.
    * `utility.h`: Utility functions for user interface components.

6. **protocols** - Contains functions related to different communication protocols, like TCP and UDP:
    * `TCP.c` and `TCP.h`: Functions for handling TCP communication.
    * `UDP.c` and `UDP.h`: Functions for handling UDP communication.

7. **main.c**: The main file of the program that contains the entry point.

This structure follows a modular approach, where each directory focuses on a specific aspect of the program.
