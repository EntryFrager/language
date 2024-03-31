/// @file ass.h

#ifndef ASS_H
#define ASS_H

#include "..\include\func_ass.h"                                                                    ///< Connects a file that reads text from a file.

int input_text (SPU *spu);                                                                          ///< A function that reads text from a file into one buffer.

void split_commands (SPU *spu);                                                                     ///< Function that splits a buffer into lines.

void clean_comment (SPU *spu);                                                                      ///< Function that removes comments.

int code_compilation (SPU *spu);                                                                    ///< Compiling the code and outputting it to a binary file.

int pars_command (SPU *spu);                                                                        ///< A function that compiles user code into machine code and outputs it to a file.

int write_buf (COMMANDS *cmd, int *buf, size_t counter);                                            ///< A function that writes the converted command to the buffer.

int process_param (SPU *spu, size_t ip, size_t cmd_len);                                            ///< A function that checks whether a command has a parameter.

int get_param (SPU *spu, size_t ip, size_t cmd_len, size_t len);                                    ///< Function that finds a parameter for a command.

void print_listing (SPU *spu);                                                                      ///< Function for outputting general information about commands to a file.

#endif //ASS_H