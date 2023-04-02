# Computer Networking

## Transferência de Conteúdos em Redes Arbóreas

This project, "Transferência de Conteúdos em Redes Arbóreas" (Content Transfer in Tree Networks), aims to implement a distributed system for content sharing among nodes in a tree-structured network. The system allows users to create, search, retrieve, and delete content within the network.


## Project Overview

```plaintext
.
├── Makefile                           # Makefile for building the project
├── README.md                          # Project documentation
├── create_network.sh                  # Shell script to create a network
├── remove_all_from_network.sh         # Shell script to remove all nodes from the network
│
└── `src`                              # Source code directory
    ├── main.c                         # Main function of the project
    ├── arguments_parser.c             # Command line arguments parsing implementation
    ├── arguments_parser.h             # Command line arguments parsing header
    │
    ├── error_handling                 # Directory containing error handling related files
    │   ├── error_checking.c           # Error checking implementation
    │   ├── error_checking.h           # Error checking header
    │   ├── error_messages.c           # Error messages implementation
    │   └── error_messages.h           # Error messages header
    │
    ├── essentials                     # Directory containing essential components of the project
    │   ├── circular_buffer.c          # Circular buffer implementation
    │   ├── circular_buffer.h          # Circular buffer header
    │   ├── host_handling.c            # Host handling implementation
    │   ├── host_handling.h            # Host handling header
    │   ├── new_connections_list.c     # New connections list implementation
    │   ├── new_connections_list.h     # New connections list header
    │   └── struct.h                   # Struct definitions header
    │
    ├── fildes_handling                # Directory containing file descriptor handling related files
    │   ├── core                       # Core networking components (TCP, UDP)
    │   │   ├── core_common.c          # Common functions for core networking
    │   │   ├── core_common.h          # Common header for core networking
    │   │   ├── TCP.c                  # TCP implementation
    │   │   ├── TCP.h                  # TCP header
    │   │   ├── UDP.c                  # UDP implementation
    │   │   └── UDP.h                  # UDP header
    │   │
    │   ├── custom_protocols           # Custom protocols implementation
    │   │   ├── common.c               # Common custom protocols functions
    │   │   ├── common.h               # Common custom protocols header
    │   │   ├── delete_node_module.c   # Delete node module implementation
    │   │   ├── delete_node_module.h   # Delete node module header
    │   │   ├── protocol_commands.h    # Protocol commands header
    │   │   ├── query_module.c         # Query module implementation
    │   │   ├── query_module.h         # Query module header
    │   │   ├── update_backup_module.c # Update backup module implementation
    │   │   ├── update_backup_module.h # Update backup module header
    │   │   ├── utility.h              # Utility header for custom protocols
    │   │   ├── withdraw_module.c      # Withdraw module implementation
    │   │   └── withdraw_module.h      # Withdraw module header
    │   │
    │   ├── descriptor_control         # Directory containing descriptor control related files
    │   │   ├── descriptor_control.c   # Descriptor control implementation
    │   │   ├── descriptor_control.h   # Descriptor control header
    │   │   ├── process_descriptors.c  # Process descriptors implementation
    │   │   └── process_descriptors.h  # Process descriptors header
    │   │
    │   └── user_interface             # User interface components
    │       ├── clear_module.c         # Clear module implementation
    │       ├── clear_module.h         # Clear module header
    │       ├── content_module.c       # Content module implementation
    │       ├── content_module.h       # Content module header
    │       ├── join_module.c          # Join module implementation
    │       ├── join_module.h          # Join module header
    │       ├── leave_module.c         # Leave module implementation
    │       ├── leave_module.h         # Leave module header
    │       ├── show_module.c          # Show module implementation
    │       ├── show_module.h          # Show module header
    │       ├── user_commands.h        # User commands header
    │       └── utility.h              # Utility header for user interface
    │
    └── misc                           # Directory containing miscellaneous files
        ├── handle_terminal.c          # Terminal handling implementation
        ├── handle_terminal.h          # Terminal handling header
        ├── prompts.c                  # Prompts implementation
        └── prompts.h                  # Prompts header

9 directories, 57 files
```
