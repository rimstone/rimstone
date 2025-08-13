" SPDX-License-Identifier: Apache-2.0
" Copyright 2018-2025 Gliim LLC.
" Licensed under Apache License v2. See LICENSE file.
" On the web http://golf-lang.com/ - this file is part of Golf framework.
" Language: Golf
" Vim indent file
" Maintainer: Gliim LLC
so $VIMRUNTIME/syntax/c.vim
syntax sync minlines=10000
hi def link golfConstruct Statement
hi def link golfClause Identifier
hi def link golfClauseOutput Type
hi def link gg_h_other  Constant
hi def link Comment  SpecialKey
syn region gg_r_inline_dot start="<<[[:space:]]*[\.]" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_dot '<<[[:space:]]*[\.]' contained containedin=gg_r_inline_dot
    syn match gg_h_print_inline_dot '>>' contained containedin=gg_r_inline_dot
    hi def link gg_h_print_inline_dot    golfConstruct
syn region gg_r_excl start="^[[:space:]]*[!]" skip="\\[[:space:]]*$" end="$"  keepend
syn match gg_h_excl_begin '^[[:space:]]*[!]' contained containedin=gg_r_excl
hi def link gg_h_excl_begin    golfConstruct
syn match gg_h_dot '^[[:space:]]*\.'
syn region gg_r_at start="^[[:space:]]*[@]" skip="\\[[:space:]]*$" end="$"  keepend
syn match gg_h_at_begin '^[[:space:]]*[@]' contained containedin=gg_r_at
hi def link gg_h_at_begin    golfConstruct
hi def link gg_h_dot    golfConstruct
syn region gg_r_construct_end_read_line start="^[[:space:]]*end-read-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_read_line,gg_r_inline_end_read_line,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_read_line,gg_r_inline_end_read_line,gg_r_at
    syn match gg_h_construct_end_read_line "^[[:space:]]*end-read-line" contained containedin=gg_r_construct_end_read_line
    hi def link gg_h_clause_end_read_line    golfClause
    hi def link gg_h_clause_output_end_read_line    golfClauseOutput
    hi def link gg_h_construct_end_read_line    golfConstruct
    hi def link gg_h_print_inline_end_read_line    golfConstruct
syn region gg_r_construct_read_line start="^[[:space:]]*read-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_line,gg_r_inline_read_line,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_line,gg_r_inline_read_line,gg_r_at
    syn match gg_h_construct_read_line "^[[:space:]]*read-line" contained containedin=gg_r_construct_read_line
    syn match gg_h_clause_read_line " delimiter \@=" contained containedin=gg_r_construct_read_line
    syn match gg_h_clause_output_read_line " status \@=" contained containedin=gg_r_construct_read_line
    syn match gg_h_clause_output_read_line " to \@=" contained containedin=gg_r_construct_read_line
    hi def link gg_h_clause_read_line    golfClause
    hi def link gg_h_clause_output_read_line    golfClauseOutput
    hi def link gg_h_construct_read_line    golfConstruct
    hi def link gg_h_print_inline_read_line    golfConstruct
syn region gg_r_construct_double_left_par start="^[[:space:]]*((" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_double_left_par,gg_r_inline_double_left_par,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_double_left_par,gg_r_inline_double_left_par,gg_r_at
    syn match gg_h_construct_double_left_par "^[[:space:]]*((" contained containedin=gg_r_construct_double_left_par
    hi def link gg_h_clause_double_left_par    golfClause
    hi def link gg_h_clause_output_double_left_par    golfClauseOutput
    hi def link gg_h_construct_double_left_par    golfConstruct
    hi def link gg_h_print_inline_double_left_par    golfConstruct
syn region gg_r_construct_write_string start="^[[:space:]]*write-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_string,gg_r_inline_write_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_string,gg_r_inline_write_string,gg_r_at
    syn match gg_h_construct_write_string "^[[:space:]]*write-string" contained containedin=gg_r_construct_write_string
    hi def link gg_h_clause_write_string    golfClause
    hi def link gg_h_clause_output_write_string    golfClauseOutput
    hi def link gg_h_construct_write_string    golfConstruct
    hi def link gg_h_print_inline_write_string    golfConstruct
syn region gg_r_construct_double_right_par start="^[[:space:]]*))" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_double_right_par,gg_r_inline_double_right_par,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_double_right_par,gg_r_inline_double_right_par,gg_r_at
    syn match gg_h_construct_double_right_par "^[[:space:]]*))" contained containedin=gg_r_construct_double_right_par
    syn match gg_h_clause_double_right_par " notrim \@=" contained containedin=gg_r_construct_double_right_par
    syn match gg_h_clause_double_right_par " notrim,\@=" contained containedin=gg_r_construct_double_right_par
    syn match gg_h_clause_double_right_par " notrim$" contained containedin=gg_r_construct_double_right_par
    hi def link gg_h_clause_double_right_par    golfClause
    hi def link gg_h_clause_output_double_right_par    golfClauseOutput
    hi def link gg_h_construct_double_right_par    golfConstruct
    hi def link gg_h_print_inline_double_right_par    golfConstruct
syn region gg_r_construct_end_write_string start="^[[:space:]]*end-write-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_write_string,gg_r_inline_end_write_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_write_string,gg_r_inline_end_write_string,gg_r_at
    syn match gg_h_construct_end_write_string "^[[:space:]]*end-write-string" contained containedin=gg_r_construct_end_write_string
    syn match gg_h_clause_end_write_string " notrim \@=" contained containedin=gg_r_construct_end_write_string
    syn match gg_h_clause_end_write_string " notrim,\@=" contained containedin=gg_r_construct_end_write_string
    syn match gg_h_clause_end_write_string " notrim$" contained containedin=gg_r_construct_end_write_string
    hi def link gg_h_clause_end_write_string    golfClause
    hi def link gg_h_clause_output_end_write_string    golfClauseOutput
    hi def link gg_h_construct_end_write_string    golfConstruct
    hi def link gg_h_print_inline_end_write_string    golfConstruct
syn region gg_r_construct_open_file start="^[[:space:]]*open-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_open_file,gg_r_inline_open_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_open_file,gg_r_inline_open_file,gg_r_at
    syn match gg_h_construct_open_file "^[[:space:]]*open-file" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_output_open_file " file-id \@=" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_open_file " new-truncate \@=" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_open_file " new-truncate,\@=" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_open_file " new-truncate$" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_output_open_file " status \@=" contained containedin=gg_r_construct_open_file
    hi def link gg_h_clause_open_file    golfClause
    hi def link gg_h_clause_output_open_file    golfClauseOutput
    hi def link gg_h_construct_open_file    golfConstruct
    hi def link gg_h_print_inline_open_file    golfConstruct
syn region gg_r_construct_close_file start="^[[:space:]]*close-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_close_file,gg_r_inline_close_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_close_file,gg_r_inline_close_file,gg_r_at
    syn match gg_h_construct_close_file "^[[:space:]]*close-file" contained containedin=gg_r_construct_close_file
    syn match gg_h_clause_close_file " file-id \@=" contained containedin=gg_r_construct_close_file
    syn match gg_h_clause_output_close_file " status \@=" contained containedin=gg_r_construct_close_file
    hi def link gg_h_clause_close_file    golfClause
    hi def link gg_h_clause_output_close_file    golfClauseOutput
    hi def link gg_h_construct_close_file    golfConstruct
    hi def link gg_h_print_inline_close_file    golfConstruct
syn region gg_r_construct_file_position start="^[[:space:]]*file-position" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_file_position,gg_r_inline_file_position,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_file_position,gg_r_inline_file_position,gg_r_at
    syn match gg_h_construct_file_position "^[[:space:]]*file-position" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_file_position " file-id \@=" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_output_file_position " get \@=" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_file_position " set \@=" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_output_file_position " status \@=" contained containedin=gg_r_construct_file_position
    hi def link gg_h_clause_file_position    golfClause
    hi def link gg_h_clause_output_file_position    golfClauseOutput
    hi def link gg_h_construct_file_position    golfConstruct
    hi def link gg_h_print_inline_file_position    golfConstruct
syn region gg_r_construct_exit_handler start="^[[:space:]]*exit-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_exit_handler,gg_r_inline_exit_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_exit_handler,gg_r_inline_exit_handler,gg_r_at
    syn match gg_h_construct_exit_handler "^[[:space:]]*exit-handler" contained containedin=gg_r_construct_exit_handler
    hi def link gg_h_clause_exit_handler    golfClause
    hi def link gg_h_clause_output_exit_handler    golfClauseOutput
    hi def link gg_h_construct_exit_handler    golfConstruct
    hi def link gg_h_print_inline_exit_handler    golfConstruct
syn region gg_r_construct_return_handler start="^[[:space:]]*return-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_return_handler,gg_r_inline_return_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_return_handler,gg_r_inline_return_handler,gg_r_at
    syn match gg_h_construct_return_handler "^[[:space:]]*return-handler" contained containedin=gg_r_construct_return_handler
    hi def link gg_h_clause_return_handler    golfClause
    hi def link gg_h_clause_output_return_handler    golfClauseOutput
    hi def link gg_h_construct_return_handler    golfConstruct
    hi def link gg_h_print_inline_return_handler    golfConstruct
syn region gg_r_construct_quit_process start="^[[:space:]]*quit-process" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_quit_process,gg_r_inline_quit_process,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_quit_process,gg_r_inline_quit_process,gg_r_at
    syn match gg_h_construct_quit_process "^[[:space:]]*quit-process" contained containedin=gg_r_construct_quit_process
    hi def link gg_h_clause_quit_process    golfClause
    hi def link gg_h_clause_output_quit_process    golfClauseOutput
    hi def link gg_h_construct_quit_process    golfConstruct
    hi def link gg_h_print_inline_quit_process    golfConstruct
syn region gg_r_construct_finish_output start="^[[:space:]]*finish-output" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_finish_output,gg_r_inline_finish_output,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_finish_output,gg_r_inline_finish_output,gg_r_at
    syn match gg_h_construct_finish_output "^[[:space:]]*finish-output" contained containedin=gg_r_construct_finish_output
    hi def link gg_h_clause_finish_output    golfClause
    hi def link gg_h_clause_output_finish_output    golfClauseOutput
    hi def link gg_h_construct_finish_output    golfConstruct
    hi def link gg_h_print_inline_finish_output    golfConstruct
syn region gg_r_construct_copy_file start="^[[:space:]]*copy-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_copy_file,gg_r_inline_copy_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_copy_file,gg_r_inline_copy_file,gg_r_at
    syn match gg_h_construct_copy_file "^[[:space:]]*copy-file" contained containedin=gg_r_construct_copy_file
    syn match gg_h_clause_output_copy_file " status \@=" contained containedin=gg_r_construct_copy_file
    syn match gg_h_clause_copy_file " to \@=" contained containedin=gg_r_construct_copy_file
    hi def link gg_h_clause_copy_file    golfClause
    hi def link gg_h_clause_output_copy_file    golfClauseOutput
    hi def link gg_h_construct_copy_file    golfConstruct
    hi def link gg_h_print_inline_copy_file    golfConstruct
syn region gg_r_construct_end_do_once start="^[[:space:]]*end-do-once" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_do_once,gg_r_inline_end_do_once,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_do_once,gg_r_inline_end_do_once,gg_r_at
    syn match gg_h_construct_end_do_once "^[[:space:]]*end-do-once" contained containedin=gg_r_construct_end_do_once
    hi def link gg_h_clause_end_do_once    golfClause
    hi def link gg_h_clause_output_end_do_once    golfClauseOutput
    hi def link gg_h_construct_end_do_once    golfConstruct
    hi def link gg_h_print_inline_end_do_once    golfConstruct
syn region gg_r_construct_do_once start="^[[:space:]]*do-once" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_do_once,gg_r_inline_do_once,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_do_once,gg_r_inline_do_once,gg_r_at
    syn match gg_h_construct_do_once "^[[:space:]]*do-once" contained containedin=gg_r_construct_do_once
    hi def link gg_h_clause_do_once    golfClause
    hi def link gg_h_clause_output_do_once    golfClauseOutput
    hi def link gg_h_construct_do_once    golfConstruct
    hi def link gg_h_print_inline_do_once    golfConstruct
syn region gg_r_construct_use_cursor start="^[[:space:]]*use-cursor" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_use_cursor,gg_r_inline_use_cursor,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_use_cursor,gg_r_inline_use_cursor,gg_r_at
    syn match gg_h_construct_use_cursor "^[[:space:]]*use-cursor" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " current \@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " current,\@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " current$" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " get-greater \@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " get-greater,\@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " get-greater$" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " get-lesser \@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " get-lesser,\@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " get-lesser$" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_output_use_cursor " key \@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_output_use_cursor " status \@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_use_cursor " update-value \@=" contained containedin=gg_r_construct_use_cursor
    syn match gg_h_clause_output_use_cursor " value \@=" contained containedin=gg_r_construct_use_cursor
    hi def link gg_h_clause_use_cursor    golfClause
    hi def link gg_h_clause_output_use_cursor    golfClauseOutput
    hi def link gg_h_construct_use_cursor    golfConstruct
    hi def link gg_h_print_inline_use_cursor    golfConstruct
syn region gg_r_construct_delete_tree start="^[[:space:]]*delete-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_tree,gg_r_inline_delete_tree,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_tree,gg_r_inline_delete_tree,gg_r_at
    syn match gg_h_construct_delete_tree "^[[:space:]]*delete-tree" contained containedin=gg_r_construct_delete_tree
    syn match gg_h_clause_delete_tree " key \@=" contained containedin=gg_r_construct_delete_tree
    syn match gg_h_clause_output_delete_tree " status \@=" contained containedin=gg_r_construct_delete_tree
    syn match gg_h_clause_output_delete_tree " value \@=" contained containedin=gg_r_construct_delete_tree
    hi def link gg_h_clause_delete_tree    golfClause
    hi def link gg_h_clause_output_delete_tree    golfClauseOutput
    hi def link gg_h_construct_delete_tree    golfConstruct
    hi def link gg_h_print_inline_delete_tree    golfConstruct
syn region gg_r_construct_read_tree start="^[[:space:]]*read-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_tree,gg_r_inline_read_tree,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_tree,gg_r_inline_read_tree,gg_r_at
    syn match gg_h_construct_read_tree "^[[:space:]]*read-tree" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " equal \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " greater \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " greater-equal \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_output_read_tree " key \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " lesser \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " lesser-equal \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " max-key \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " max-key,\@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " max-key$" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " min-key \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " min-key,\@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " min-key$" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_output_read_tree " new-cursor \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_output_read_tree " status \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_read_tree " update-value \@=" contained containedin=gg_r_construct_read_tree
    syn match gg_h_clause_output_read_tree " value \@=" contained containedin=gg_r_construct_read_tree
    hi def link gg_h_clause_read_tree    golfClause
    hi def link gg_h_clause_output_read_tree    golfClauseOutput
    hi def link gg_h_construct_read_tree    golfConstruct
    hi def link gg_h_print_inline_read_tree    golfConstruct
syn region gg_r_construct_write_tree start="^[[:space:]]*write-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_tree,gg_r_inline_write_tree,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_tree,gg_r_inline_write_tree,gg_r_at
    syn match gg_h_construct_write_tree "^[[:space:]]*write-tree" contained containedin=gg_r_construct_write_tree
    syn match gg_h_clause_write_tree " key \@=" contained containedin=gg_r_construct_write_tree
    syn match gg_h_clause_output_write_tree " new-cursor \@=" contained containedin=gg_r_construct_write_tree
    syn match gg_h_clause_output_write_tree " status \@=" contained containedin=gg_r_construct_write_tree
    syn match gg_h_clause_write_tree " value \@=" contained containedin=gg_r_construct_write_tree
    hi def link gg_h_clause_write_tree    golfClause
    hi def link gg_h_clause_output_write_tree    golfClauseOutput
    hi def link gg_h_construct_write_tree    golfConstruct
    hi def link gg_h_print_inline_write_tree    golfConstruct
syn region gg_r_construct_purge_tree start="^[[:space:]]*purge-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_tree,gg_r_inline_purge_tree,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_tree,gg_r_inline_purge_tree,gg_r_at
    syn match gg_h_construct_purge_tree "^[[:space:]]*purge-tree" contained containedin=gg_r_construct_purge_tree
    hi def link gg_h_clause_purge_tree    golfClause
    hi def link gg_h_clause_output_purge_tree    golfClauseOutput
    hi def link gg_h_construct_purge_tree    golfConstruct
    hi def link gg_h_print_inline_purge_tree    golfConstruct
syn region gg_r_construct_get_tree start="^[[:space:]]*get-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_tree,gg_r_inline_get_tree,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_tree,gg_r_inline_get_tree,gg_r_at
    syn match gg_h_construct_get_tree "^[[:space:]]*get-tree" contained containedin=gg_r_construct_get_tree
    syn match gg_h_clause_output_get_tree " count \@=" contained containedin=gg_r_construct_get_tree
    syn match gg_h_clause_output_get_tree " hops \@=" contained containedin=gg_r_construct_get_tree
    hi def link gg_h_clause_get_tree    golfClause
    hi def link gg_h_clause_output_get_tree    golfClauseOutput
    hi def link gg_h_construct_get_tree    golfConstruct
    hi def link gg_h_print_inline_get_tree    golfConstruct
syn region gg_r_construct_get_list start="^[[:space:]]*get-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_list,gg_r_inline_get_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_list,gg_r_inline_get_list,gg_r_at
    syn match gg_h_construct_get_list "^[[:space:]]*get-list" contained containedin=gg_r_construct_get_list
    syn match gg_h_clause_output_get_list " count \@=" contained containedin=gg_r_construct_get_list
    hi def link gg_h_clause_get_list    golfClause
    hi def link gg_h_clause_output_get_list    golfClauseOutput
    hi def link gg_h_construct_get_list    golfConstruct
    hi def link gg_h_print_inline_get_list    golfConstruct
syn region gg_r_construct_get_lifo start="^[[:space:]]*get-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_lifo,gg_r_inline_get_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_lifo,gg_r_inline_get_lifo,gg_r_at
    syn match gg_h_construct_get_lifo "^[[:space:]]*get-lifo" contained containedin=gg_r_construct_get_lifo
    syn match gg_h_clause_output_get_lifo " count \@=" contained containedin=gg_r_construct_get_lifo
    hi def link gg_h_clause_get_lifo    golfClause
    hi def link gg_h_clause_output_get_lifo    golfClauseOutput
    hi def link gg_h_construct_get_lifo    golfConstruct
    hi def link gg_h_print_inline_get_lifo    golfConstruct
syn region gg_r_construct_get_upload start="^[[:space:]]*get-upload" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_upload,gg_r_inline_get_upload,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_upload,gg_r_inline_get_upload,gg_r_at
    syn match gg_h_construct_get_upload "^[[:space:]]*get-upload" contained containedin=gg_r_construct_get_upload
    syn match gg_h_clause_output_get_upload " client-file \@=" contained containedin=gg_r_construct_get_upload
    syn match gg_h_clause_output_get_upload " extension \@=" contained containedin=gg_r_construct_get_upload
    syn match gg_h_clause_output_get_upload " local-file \@=" contained containedin=gg_r_construct_get_upload
    syn match gg_h_clause_output_get_upload " size \@=" contained containedin=gg_r_construct_get_upload
    syn match gg_h_clause_get_upload " status \@=" contained containedin=gg_r_construct_get_upload
    hi def link gg_h_clause_get_upload    golfClause
    hi def link gg_h_clause_output_get_upload    golfClauseOutput
    hi def link gg_h_construct_get_upload    golfConstruct
    hi def link gg_h_print_inline_get_upload    golfConstruct
syn region gg_r_construct_new_tree start="^[[:space:]]*new-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_tree,gg_r_inline_new_tree,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_tree,gg_r_inline_new_tree,gg_r_at
    syn match gg_h_construct_new_tree "^[[:space:]]*new-tree" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " key-as \@=" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " process-scope \@=" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " process-scope,\@=" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " process-scope$" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " unsorted \@=" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " unsorted,\@=" contained containedin=gg_r_construct_new_tree
    syn match gg_h_clause_new_tree " unsorted$" contained containedin=gg_r_construct_new_tree
    hi def link gg_h_clause_new_tree    golfClause
    hi def link gg_h_clause_output_new_tree    golfClauseOutput
    hi def link gg_h_construct_new_tree    golfConstruct
    hi def link gg_h_print_inline_new_tree    golfConstruct
syn region gg_r_construct_xml_doc start="^[[:space:]]*xml-doc" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_xml_doc,gg_r_inline_xml_doc,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_xml_doc,gg_r_inline_xml_doc,gg_r_at
    syn match gg_h_construct_xml_doc "^[[:space:]]*xml-doc" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_xml_doc " delete \@=" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_output_xml_doc " error-char \@=" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_output_xml_doc " error-line \@=" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_output_xml_doc " error-text \@=" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_xml_doc " length \@=" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_output_xml_doc " status \@=" contained containedin=gg_r_construct_xml_doc
    syn match gg_h_clause_output_xml_doc " to \@=" contained containedin=gg_r_construct_xml_doc
    hi def link gg_h_clause_xml_doc    golfClause
    hi def link gg_h_clause_output_xml_doc    golfClauseOutput
    hi def link gg_h_construct_xml_doc    golfConstruct
    hi def link gg_h_print_inline_xml_doc    golfConstruct
syn region gg_r_construct_json_doc start="^[[:space:]]*json-doc" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_json_doc,gg_r_inline_json_doc,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_json_doc,gg_r_inline_json_doc,gg_r_at
    syn match gg_h_construct_json_doc "^[[:space:]]*json-doc" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " delete \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " error-char \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " error-line \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " error-position \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " error-text \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " length \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " noencode \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " noencode,\@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " noencode$" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " no-enum \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " no-enum,\@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " no-enum$" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " status \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " to \@=" contained containedin=gg_r_construct_json_doc
    hi def link gg_h_clause_json_doc    golfClause
    hi def link gg_h_clause_output_json_doc    golfClauseOutput
    hi def link gg_h_construct_json_doc    golfConstruct
    hi def link gg_h_print_inline_json_doc    golfConstruct
syn region gg_r_construct_read_xml start="^[[:space:]]*read-xml" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_xml,gg_r_inline_read_xml,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_xml,gg_r_inline_read_xml,gg_r_at
    syn match gg_h_construct_read_xml "^[[:space:]]*read-xml" contained containedin=gg_r_construct_read_xml
    syn match gg_h_clause_output_read_xml " key \@=" contained containedin=gg_r_construct_read_xml
    syn match gg_h_clause_read_xml " next \@=" contained containedin=gg_r_construct_read_xml
    syn match gg_h_clause_read_xml " next,\@=" contained containedin=gg_r_construct_read_xml
    syn match gg_h_clause_read_xml " next$" contained containedin=gg_r_construct_read_xml
    syn match gg_h_clause_output_read_xml " status \@=" contained containedin=gg_r_construct_read_xml
    syn match gg_h_clause_output_read_xml " value \@=" contained containedin=gg_r_construct_read_xml
    hi def link gg_h_clause_read_xml    golfClause
    hi def link gg_h_clause_output_read_xml    golfClauseOutput
    hi def link gg_h_construct_read_xml    golfConstruct
    hi def link gg_h_print_inline_read_xml    golfConstruct
syn region gg_r_construct_read_json start="^[[:space:]]*read-json" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_json,gg_r_inline_read_json,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_json,gg_r_inline_read_json,gg_r_at
    syn match gg_h_construct_read_json "^[[:space:]]*read-json" contained containedin=gg_r_construct_read_json
    syn match gg_h_clause_output_read_json " key \@=" contained containedin=gg_r_construct_read_json
    syn match gg_h_clause_read_json " next \@=" contained containedin=gg_r_construct_read_json
    syn match gg_h_clause_read_json " next,\@=" contained containedin=gg_r_construct_read_json
    syn match gg_h_clause_read_json " next$" contained containedin=gg_r_construct_read_json
    syn match gg_h_clause_output_read_json " type \@=" contained containedin=gg_r_construct_read_json
    syn match gg_h_clause_output_read_json " value \@=" contained containedin=gg_r_construct_read_json
    hi def link gg_h_clause_read_json    golfClause
    hi def link gg_h_clause_output_read_json    golfClauseOutput
    hi def link gg_h_construct_read_json    golfConstruct
    hi def link gg_h_print_inline_read_json    golfConstruct
syn region gg_r_construct_read_hash start="^[[:space:]]*read-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_hash,gg_r_inline_read_hash,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_hash,gg_r_inline_read_hash,gg_r_at
    syn match gg_h_construct_read_hash "^[[:space:]]*read-hash" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " begin \@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " begin,\@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " begin$" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " delete \@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " delete,\@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " delete$" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_output_read_hash " key \@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_output_read_hash " status \@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " traverse \@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " traverse,\@=" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_read_hash " traverse$" contained containedin=gg_r_construct_read_hash
    syn match gg_h_clause_output_read_hash " value \@=" contained containedin=gg_r_construct_read_hash
    hi def link gg_h_clause_read_hash    golfClause
    hi def link gg_h_clause_output_read_hash    golfClauseOutput
    hi def link gg_h_construct_read_hash    golfConstruct
    hi def link gg_h_print_inline_read_hash    golfConstruct
syn region gg_r_construct_write_hash start="^[[:space:]]*write-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_hash,gg_r_inline_write_hash,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_hash,gg_r_inline_write_hash,gg_r_at
    syn match gg_h_construct_write_hash "^[[:space:]]*write-hash" contained containedin=gg_r_construct_write_hash
    syn match gg_h_clause_write_hash " key \@=" contained containedin=gg_r_construct_write_hash
    syn match gg_h_clause_output_write_hash " old-value \@=" contained containedin=gg_r_construct_write_hash
    syn match gg_h_clause_output_write_hash " status \@=" contained containedin=gg_r_construct_write_hash
    syn match gg_h_clause_write_hash " value \@=" contained containedin=gg_r_construct_write_hash
    hi def link gg_h_clause_write_hash    golfClause
    hi def link gg_h_clause_output_write_hash    golfClauseOutput
    hi def link gg_h_construct_write_hash    golfConstruct
    hi def link gg_h_print_inline_write_hash    golfConstruct
syn region gg_r_construct_read_array start="^[[:space:]]*read-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_array,gg_r_inline_read_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_array,gg_r_inline_read_array,gg_r_at
    syn match gg_h_construct_read_array "^[[:space:]]*read-array" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " delete \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " delete,\@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " delete$" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " key \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_output_read_array " status \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_output_read_array " value \@=" contained containedin=gg_r_construct_read_array
    hi def link gg_h_clause_read_array    golfClause
    hi def link gg_h_clause_output_read_array    golfClauseOutput
    hi def link gg_h_construct_read_array    golfConstruct
    hi def link gg_h_print_inline_read_array    golfConstruct
syn region gg_r_construct_purge_array start="^[[:space:]]*purge-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_array,gg_r_inline_purge_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_array,gg_r_inline_purge_array,gg_r_at
    syn match gg_h_construct_purge_array "^[[:space:]]*purge-array" contained containedin=gg_r_construct_purge_array
    hi def link gg_h_clause_purge_array    golfClause
    hi def link gg_h_clause_output_purge_array    golfClauseOutput
    hi def link gg_h_construct_purge_array    golfConstruct
    hi def link gg_h_print_inline_purge_array    golfConstruct
syn region gg_r_construct_write_array start="^[[:space:]]*write-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_array,gg_r_inline_write_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_array,gg_r_inline_write_array,gg_r_at
    syn match gg_h_construct_write_array "^[[:space:]]*write-array" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_write_array " key \@=" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_output_write_array " old-value \@=" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_write_array " value \@=" contained containedin=gg_r_construct_write_array
    hi def link gg_h_clause_write_array    golfClause
    hi def link gg_h_clause_output_write_array    golfClauseOutput
    hi def link gg_h_construct_write_array    golfConstruct
    hi def link gg_h_print_inline_write_array    golfConstruct
syn region gg_r_construct_new_hash start="^[[:space:]]*new-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_hash,gg_r_inline_new_hash,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_hash,gg_r_inline_new_hash,gg_r_at
    syn match gg_h_construct_new_hash "^[[:space:]]*new-hash" contained containedin=gg_r_construct_new_hash
    syn match gg_h_clause_new_hash " hash-size \@=" contained containedin=gg_r_construct_new_hash
    syn match gg_h_clause_new_hash " process-scope \@=" contained containedin=gg_r_construct_new_hash
    syn match gg_h_clause_new_hash " process-scope,\@=" contained containedin=gg_r_construct_new_hash
    syn match gg_h_clause_new_hash " process-scope$" contained containedin=gg_r_construct_new_hash
    hi def link gg_h_clause_new_hash    golfClause
    hi def link gg_h_clause_output_new_hash    golfClauseOutput
    hi def link gg_h_construct_new_hash    golfConstruct
    hi def link gg_h_print_inline_new_hash    golfConstruct
syn region gg_r_construct_new_array start="^[[:space:]]*new-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_array,gg_r_inline_new_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_array,gg_r_inline_new_array,gg_r_at
    syn match gg_h_construct_new_array "^[[:space:]]*new-array" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " max-size \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " process-scope \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " process-scope,\@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " process-scope$" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " bool \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " bool,\@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " bool$" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " number \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " number,\@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " number$" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " string \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " string,\@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " string$" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " type \@=" contained containedin=gg_r_construct_new_array
    hi def link gg_h_clause_new_array    golfClause
    hi def link gg_h_clause_output_new_array    golfClauseOutput
    hi def link gg_h_construct_new_array    golfConstruct
    hi def link gg_h_print_inline_new_array    golfConstruct
syn region gg_r_construct_resize_hash start="^[[:space:]]*resize-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_resize_hash,gg_r_inline_resize_hash,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_resize_hash,gg_r_inline_resize_hash,gg_r_at
    syn match gg_h_construct_resize_hash "^[[:space:]]*resize-hash" contained containedin=gg_r_construct_resize_hash
    syn match gg_h_clause_resize_hash " hash-size \@=" contained containedin=gg_r_construct_resize_hash
    hi def link gg_h_clause_resize_hash    golfClause
    hi def link gg_h_clause_output_resize_hash    golfClauseOutput
    hi def link gg_h_construct_resize_hash    golfConstruct
    hi def link gg_h_print_inline_resize_hash    golfConstruct
syn region gg_r_construct_purge_hash start="^[[:space:]]*purge-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_hash,gg_r_inline_purge_hash,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_hash,gg_r_inline_purge_hash,gg_r_at
    syn match gg_h_construct_purge_hash "^[[:space:]]*purge-hash" contained containedin=gg_r_construct_purge_hash
    hi def link gg_h_clause_purge_hash    golfClause
    hi def link gg_h_clause_output_purge_hash    golfClauseOutput
    hi def link gg_h_construct_purge_hash    golfConstruct
    hi def link gg_h_print_inline_purge_hash    golfConstruct
syn region gg_r_construct_get_hash start="^[[:space:]]*get-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_hash,gg_r_inline_get_hash,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_hash,gg_r_inline_get_hash,gg_r_at
    syn match gg_h_construct_get_hash "^[[:space:]]*get-hash" contained containedin=gg_r_construct_get_hash
    syn match gg_h_clause_output_get_hash " average-reads \@=" contained containedin=gg_r_construct_get_hash
    syn match gg_h_clause_output_get_hash " hash-size \@=" contained containedin=gg_r_construct_get_hash
    syn match gg_h_clause_output_get_hash " length \@=" contained containedin=gg_r_construct_get_hash
    hi def link gg_h_clause_get_hash    golfClause
    hi def link gg_h_clause_output_get_hash    golfClauseOutput
    hi def link gg_h_construct_get_hash    golfConstruct
    hi def link gg_h_print_inline_get_hash    golfConstruct
syn region gg_r_construct_read_file start="^[[:space:]]*read-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_file,gg_r_inline_read_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_file,gg_r_inline_read_file,gg_r_at
    syn match gg_h_construct_read_file "^[[:space:]]*read-file" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_output_read_file " end-of-file \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_read_file " file-id \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_read_file " length \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_read_file " position \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_output_read_file " status \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_output_read_file " to \@=" contained containedin=gg_r_construct_read_file
    hi def link gg_h_clause_read_file    golfClause
    hi def link gg_h_clause_output_read_file    golfClauseOutput
    hi def link gg_h_construct_read_file    golfConstruct
    hi def link gg_h_print_inline_read_file    golfConstruct
syn region gg_r_construct_write_file start="^[[:space:]]*write-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_file,gg_r_inline_write_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_file,gg_r_inline_write_file,gg_r_at
    syn match gg_h_construct_write_file "^[[:space:]]*write-file" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " append \@=" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " append,\@=" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " append$" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " file-id \@=" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " from \@=" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " length \@=" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_write_file " position \@=" contained containedin=gg_r_construct_write_file
    syn match gg_h_clause_output_write_file " status \@=" contained containedin=gg_r_construct_write_file
    hi def link gg_h_clause_write_file    golfClause
    hi def link gg_h_clause_output_write_file    golfClauseOutput
    hi def link gg_h_construct_write_file    golfConstruct
    hi def link gg_h_print_inline_write_file    golfConstruct
syn region gg_r_construct_read_lifo start="^[[:space:]]*read-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_lifo,gg_r_inline_read_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_lifo,gg_r_inline_read_lifo,gg_r_at
    syn match gg_h_construct_read_lifo "^[[:space:]]*read-lifo" contained containedin=gg_r_construct_read_lifo
    syn match gg_h_clause_output_read_lifo " key \@=" contained containedin=gg_r_construct_read_lifo
    syn match gg_h_clause_output_read_lifo " status \@=" contained containedin=gg_r_construct_read_lifo
    syn match gg_h_clause_output_read_lifo " value \@=" contained containedin=gg_r_construct_read_lifo
    hi def link gg_h_clause_read_lifo    golfClause
    hi def link gg_h_clause_output_read_lifo    golfClauseOutput
    hi def link gg_h_construct_read_lifo    golfConstruct
    hi def link gg_h_print_inline_read_lifo    golfConstruct
syn region gg_r_construct_read_fifo start="^[[:space:]]*read-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_fifo,gg_r_inline_read_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_fifo,gg_r_inline_read_fifo,gg_r_at
    syn match gg_h_construct_read_fifo "^[[:space:]]*read-fifo" contained containedin=gg_r_construct_read_fifo
    syn match gg_h_clause_output_read_fifo " key \@=" contained containedin=gg_r_construct_read_fifo
    syn match gg_h_clause_output_read_fifo " status \@=" contained containedin=gg_r_construct_read_fifo
    syn match gg_h_clause_output_read_fifo " value \@=" contained containedin=gg_r_construct_read_fifo
    hi def link gg_h_clause_read_fifo    golfClause
    hi def link gg_h_clause_output_read_fifo    golfClauseOutput
    hi def link gg_h_construct_read_fifo    golfConstruct
    hi def link gg_h_print_inline_read_fifo    golfConstruct
syn region gg_r_construct_purge_lifo start="^[[:space:]]*purge-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_lifo,gg_r_inline_purge_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_lifo,gg_r_inline_purge_lifo,gg_r_at
    syn match gg_h_construct_purge_lifo "^[[:space:]]*purge-lifo" contained containedin=gg_r_construct_purge_lifo
    hi def link gg_h_clause_purge_lifo    golfClause
    hi def link gg_h_clause_output_purge_lifo    golfClauseOutput
    hi def link gg_h_construct_purge_lifo    golfConstruct
    hi def link gg_h_print_inline_purge_lifo    golfConstruct
syn region gg_r_construct_purge_fifo start="^[[:space:]]*purge-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_fifo,gg_r_inline_purge_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_fifo,gg_r_inline_purge_fifo,gg_r_at
    syn match gg_h_construct_purge_fifo "^[[:space:]]*purge-fifo" contained containedin=gg_r_construct_purge_fifo
    hi def link gg_h_clause_purge_fifo    golfClause
    hi def link gg_h_clause_output_purge_fifo    golfClauseOutput
    hi def link gg_h_construct_purge_fifo    golfConstruct
    hi def link gg_h_print_inline_purge_fifo    golfConstruct
syn region gg_r_construct_delete_lifo start="^[[:space:]]*delete-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_lifo,gg_r_inline_delete_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_lifo,gg_r_inline_delete_lifo,gg_r_at
    syn match gg_h_construct_delete_lifo "^[[:space:]]*delete-lifo" contained containedin=gg_r_construct_delete_lifo
    hi def link gg_h_clause_delete_lifo    golfClause
    hi def link gg_h_clause_output_delete_lifo    golfClauseOutput
    hi def link gg_h_construct_delete_lifo    golfConstruct
    hi def link gg_h_print_inline_delete_lifo    golfConstruct
syn region gg_r_construct_delete_fifo start="^[[:space:]]*delete-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_fifo,gg_r_inline_delete_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_fifo,gg_r_inline_delete_fifo,gg_r_at
    syn match gg_h_construct_delete_fifo "^[[:space:]]*delete-fifo" contained containedin=gg_r_construct_delete_fifo
    hi def link gg_h_clause_delete_fifo    golfClause
    hi def link gg_h_clause_output_delete_fifo    golfClauseOutput
    hi def link gg_h_construct_delete_fifo    golfConstruct
    hi def link gg_h_print_inline_delete_fifo    golfConstruct
syn region gg_r_construct_rewind_lifo start="^[[:space:]]*rewind-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rewind_lifo,gg_r_inline_rewind_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rewind_lifo,gg_r_inline_rewind_lifo,gg_r_at
    syn match gg_h_construct_rewind_lifo "^[[:space:]]*rewind-lifo" contained containedin=gg_r_construct_rewind_lifo
    hi def link gg_h_clause_rewind_lifo    golfClause
    hi def link gg_h_clause_output_rewind_lifo    golfClauseOutput
    hi def link gg_h_construct_rewind_lifo    golfConstruct
    hi def link gg_h_print_inline_rewind_lifo    golfConstruct
syn region gg_r_construct_rewind_fifo start="^[[:space:]]*rewind-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rewind_fifo,gg_r_inline_rewind_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rewind_fifo,gg_r_inline_rewind_fifo,gg_r_at
    syn match gg_h_construct_rewind_fifo "^[[:space:]]*rewind-fifo" contained containedin=gg_r_construct_rewind_fifo
    hi def link gg_h_clause_rewind_fifo    golfClause
    hi def link gg_h_clause_output_rewind_fifo    golfClauseOutput
    hi def link gg_h_construct_rewind_fifo    golfConstruct
    hi def link gg_h_print_inline_rewind_fifo    golfConstruct
syn region gg_r_construct_write_lifo start="^[[:space:]]*write-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_lifo,gg_r_inline_write_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_lifo,gg_r_inline_write_lifo,gg_r_at
    syn match gg_h_construct_write_lifo "^[[:space:]]*write-lifo" contained containedin=gg_r_construct_write_lifo
    syn match gg_h_clause_write_lifo " key \@=" contained containedin=gg_r_construct_write_lifo
    syn match gg_h_clause_write_lifo " value \@=" contained containedin=gg_r_construct_write_lifo
    hi def link gg_h_clause_write_lifo    golfClause
    hi def link gg_h_clause_output_write_lifo    golfClauseOutput
    hi def link gg_h_construct_write_lifo    golfConstruct
    hi def link gg_h_print_inline_write_lifo    golfConstruct
syn region gg_r_construct_write_fifo start="^[[:space:]]*write-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_fifo,gg_r_inline_write_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_fifo,gg_r_inline_write_fifo,gg_r_at
    syn match gg_h_construct_write_fifo "^[[:space:]]*write-fifo" contained containedin=gg_r_construct_write_fifo
    syn match gg_h_clause_write_fifo " key \@=" contained containedin=gg_r_construct_write_fifo
    syn match gg_h_clause_write_fifo " value \@=" contained containedin=gg_r_construct_write_fifo
    hi def link gg_h_clause_write_fifo    golfClause
    hi def link gg_h_clause_output_write_fifo    golfClauseOutput
    hi def link gg_h_construct_write_fifo    golfConstruct
    hi def link gg_h_print_inline_write_fifo    golfConstruct
syn region gg_r_construct_new_lifo start="^[[:space:]]*new-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_lifo,gg_r_inline_new_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_lifo,gg_r_inline_new_lifo,gg_r_at
    syn match gg_h_construct_new_lifo "^[[:space:]]*new-lifo" contained containedin=gg_r_construct_new_lifo
    hi def link gg_h_clause_new_lifo    golfClause
    hi def link gg_h_clause_output_new_lifo    golfClauseOutput
    hi def link gg_h_construct_new_lifo    golfConstruct
    hi def link gg_h_print_inline_new_lifo    golfConstruct
syn region gg_r_construct_new_fifo start="^[[:space:]]*new-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_fifo,gg_r_inline_new_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_fifo,gg_r_inline_new_fifo,gg_r_at
    syn match gg_h_construct_new_fifo "^[[:space:]]*new-fifo" contained containedin=gg_r_construct_new_fifo
    hi def link gg_h_clause_new_fifo    golfClause
    hi def link gg_h_clause_output_new_fifo    golfClauseOutput
    hi def link gg_h_construct_new_fifo    golfConstruct
    hi def link gg_h_print_inline_new_fifo    golfConstruct
syn region gg_r_construct_delete_list start="^[[:space:]]*delete-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_list,gg_r_inline_delete_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_list,gg_r_inline_delete_list,gg_r_at
    syn match gg_h_construct_delete_list "^[[:space:]]*delete-list" contained containedin=gg_r_construct_delete_list
    syn match gg_h_clause_output_delete_list " status \@=" contained containedin=gg_r_construct_delete_list
    hi def link gg_h_clause_delete_list    golfClause
    hi def link gg_h_clause_output_delete_list    golfClauseOutput
    hi def link gg_h_construct_delete_list    golfConstruct
    hi def link gg_h_print_inline_delete_list    golfConstruct
syn region gg_r_construct_purge_list start="^[[:space:]]*purge-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_list,gg_r_inline_purge_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_list,gg_r_inline_purge_list,gg_r_at
    syn match gg_h_construct_purge_list "^[[:space:]]*purge-list" contained containedin=gg_r_construct_purge_list
    hi def link gg_h_clause_purge_list    golfClause
    hi def link gg_h_clause_output_purge_list    golfClauseOutput
    hi def link gg_h_construct_purge_list    golfConstruct
    hi def link gg_h_print_inline_purge_list    golfConstruct
syn region gg_r_construct_position_list start="^[[:space:]]*position-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_position_list,gg_r_inline_position_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_position_list,gg_r_inline_position_list,gg_r_at
    syn match gg_h_construct_position_list "^[[:space:]]*position-list" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " end \@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " end,\@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " end$" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " first \@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " first,\@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " first$" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " last \@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " last,\@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " last$" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " next \@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " next,\@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " next$" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " previous \@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " previous,\@=" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_position_list " previous$" contained containedin=gg_r_construct_position_list
    syn match gg_h_clause_output_position_list " status \@=" contained containedin=gg_r_construct_position_list
    hi def link gg_h_clause_position_list    golfClause
    hi def link gg_h_clause_output_position_list    golfClauseOutput
    hi def link gg_h_construct_position_list    golfConstruct
    hi def link gg_h_print_inline_position_list    golfConstruct
syn region gg_r_construct_read_list start="^[[:space:]]*read-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_list,gg_r_inline_read_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_list,gg_r_inline_read_list,gg_r_at
    syn match gg_h_construct_read_list "^[[:space:]]*read-list" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_output_read_list " key \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_output_read_list " status \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_read_list " update-key \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_read_list " update-value \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_output_read_list " value \@=" contained containedin=gg_r_construct_read_list
    hi def link gg_h_clause_read_list    golfClause
    hi def link gg_h_clause_output_read_list    golfClauseOutput
    hi def link gg_h_construct_read_list    golfConstruct
    hi def link gg_h_print_inline_read_list    golfConstruct
syn region gg_r_construct_write_list start="^[[:space:]]*write-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_list,gg_r_inline_write_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_list,gg_r_inline_write_list,gg_r_at
    syn match gg_h_construct_write_list "^[[:space:]]*write-list" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " append \@=" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " append,\@=" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " append$" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " key \@=" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " value \@=" contained containedin=gg_r_construct_write_list
    hi def link gg_h_clause_write_list    golfClause
    hi def link gg_h_clause_output_write_list    golfClauseOutput
    hi def link gg_h_construct_write_list    golfConstruct
    hi def link gg_h_print_inline_write_list    golfConstruct
syn region gg_r_construct_new_list start="^[[:space:]]*new-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_list,gg_r_inline_new_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_list,gg_r_inline_new_list,gg_r_at
    syn match gg_h_construct_new_list "^[[:space:]]*new-list" contained containedin=gg_r_construct_new_list
    syn match gg_h_clause_new_list " process-scope \@=" contained containedin=gg_r_construct_new_list
    syn match gg_h_clause_new_list " process-scope,\@=" contained containedin=gg_r_construct_new_list
    syn match gg_h_clause_new_list " process-scope$" contained containedin=gg_r_construct_new_list
    hi def link gg_h_clause_new_list    golfClause
    hi def link gg_h_clause_output_new_list    golfClauseOutput
    hi def link gg_h_construct_new_list    golfConstruct
    hi def link gg_h_print_inline_new_list    golfConstruct
syn region gg_r_construct_unused_var start="^[[:space:]]*unused-var" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_unused_var,gg_r_inline_unused_var,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_unused_var,gg_r_inline_unused_var,gg_r_at
    syn match gg_h_construct_unused_var "^[[:space:]]*unused-var" contained containedin=gg_r_construct_unused_var
    syn match gg_h_clause_unused_var " , \@=" contained containedin=gg_r_construct_unused_var
    syn match gg_h_clause_unused_var " ,,\@=" contained containedin=gg_r_construct_unused_var
    syn match gg_h_clause_unused_var " ,$" contained containedin=gg_r_construct_unused_var
    hi def link gg_h_clause_unused_var    golfClause
    hi def link gg_h_clause_output_unused_var    golfClauseOutput
    hi def link gg_h_construct_unused_var    golfConstruct
    hi def link gg_h_print_inline_unused_var    golfConstruct
syn region gg_r_construct_split_string start="^[[:space:]]*split-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_split_string,gg_r_inline_split_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_split_string,gg_r_inline_split_string,gg_r_at
    syn match gg_h_construct_split_string "^[[:space:]]*split-string" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_output_split_string " count \@=" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_split_string " delete \@=" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_output_split_string " to \@=" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_split_string " with \@=" contained containedin=gg_r_construct_split_string
    hi def link gg_h_clause_split_string    golfClause
    hi def link gg_h_clause_output_split_string    golfClauseOutput
    hi def link gg_h_construct_split_string    golfConstruct
    hi def link gg_h_print_inline_split_string    golfConstruct
syn region gg_r_construct_read_split start="^[[:space:]]*read-split" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_split,gg_r_inline_read_split,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_split,gg_r_inline_read_split,gg_r_at
    syn match gg_h_construct_read_split "^[[:space:]]*read-split" contained containedin=gg_r_construct_read_split
    syn match gg_h_clause_read_split " from \@=" contained containedin=gg_r_construct_read_split
    syn match gg_h_clause_output_read_split " status \@=" contained containedin=gg_r_construct_read_split
    syn match gg_h_clause_output_read_split " to \@=" contained containedin=gg_r_construct_read_split
    hi def link gg_h_clause_read_split    golfClause
    hi def link gg_h_clause_output_read_split    golfClauseOutput
    hi def link gg_h_construct_read_split    golfConstruct
    hi def link gg_h_print_inline_read_split    golfConstruct
syn region gg_r_construct_new_message start="^[[:space:]]*new-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_message,gg_r_inline_new_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_message,gg_r_inline_new_message,gg_r_at
    syn match gg_h_construct_new_message "^[[:space:]]*new-message" contained containedin=gg_r_construct_new_message
    syn match gg_h_clause_new_message " from \@=" contained containedin=gg_r_construct_new_message
    hi def link gg_h_clause_new_message    golfClause
    hi def link gg_h_clause_output_new_message    golfClauseOutput
    hi def link gg_h_construct_new_message    golfConstruct
    hi def link gg_h_print_inline_new_message    golfConstruct
syn region gg_r_construct_get_message start="^[[:space:]]*get-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_message,gg_r_inline_get_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_message,gg_r_inline_get_message,gg_r_at
    syn match gg_h_construct_get_message "^[[:space:]]*get-message" contained containedin=gg_r_construct_get_message
    syn match gg_h_clause_output_get_message " to \@=" contained containedin=gg_r_construct_get_message
    hi def link gg_h_clause_get_message    golfClause
    hi def link gg_h_clause_output_get_message    golfClauseOutput
    hi def link gg_h_construct_get_message    golfConstruct
    hi def link gg_h_print_inline_get_message    golfConstruct
syn region gg_r_construct_read_message start="^[[:space:]]*read-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_message,gg_r_inline_read_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_message,gg_r_inline_read_message,gg_r_at
    syn match gg_h_construct_read_message "^[[:space:]]*read-message" contained containedin=gg_r_construct_read_message
    syn match gg_h_clause_output_read_message " key \@=" contained containedin=gg_r_construct_read_message
    syn match gg_h_clause_output_read_message " status \@=" contained containedin=gg_r_construct_read_message
    syn match gg_h_clause_output_read_message " value \@=" contained containedin=gg_r_construct_read_message
    hi def link gg_h_clause_read_message    golfClause
    hi def link gg_h_clause_output_read_message    golfClauseOutput
    hi def link gg_h_construct_read_message    golfConstruct
    hi def link gg_h_print_inline_read_message    golfConstruct
syn region gg_r_construct_write_message start="^[[:space:]]*write-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_message,gg_r_inline_write_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_message,gg_r_inline_write_message,gg_r_at
    syn match gg_h_construct_write_message "^[[:space:]]*write-message" contained containedin=gg_r_construct_write_message
    syn match gg_h_clause_write_message " key \@=" contained containedin=gg_r_construct_write_message
    syn match gg_h_clause_write_message " value \@=" contained containedin=gg_r_construct_write_message
    hi def link gg_h_clause_write_message    golfClause
    hi def link gg_h_clause_output_write_message    golfClauseOutput
    hi def link gg_h_construct_write_message    golfConstruct
    hi def link gg_h_print_inline_write_message    golfConstruct
syn region gg_r_construct_delete_string start="^[[:space:]]*delete-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_string,gg_r_inline_delete_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_string,gg_r_inline_delete_string,gg_r_at
    syn match gg_h_construct_delete_string "^[[:space:]]*delete-string" contained containedin=gg_r_construct_delete_string
    hi def link gg_h_clause_delete_string    golfClause
    hi def link gg_h_clause_output_delete_string    golfClauseOutput
    hi def link gg_h_construct_delete_string    golfConstruct
    hi def link gg_h_print_inline_delete_string    golfConstruct
syn region gg_r_construct_end_if start="^[[:space:]]*end-if" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_if,gg_r_inline_end_if,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_if,gg_r_inline_end_if,gg_r_at
    syn match gg_h_construct_end_if "^[[:space:]]*end-if" contained containedin=gg_r_construct_end_if
    hi def link gg_h_clause_end_if    golfClause
    hi def link gg_h_clause_output_end_if    golfClauseOutput
    hi def link gg_h_construct_end_if    golfConstruct
    hi def link gg_h_print_inline_end_if    golfConstruct
syn region gg_r_construct_else_if start="^[[:space:]]*else-if" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_else_if,gg_r_inline_else_if,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_else_if,gg_r_inline_else_if,gg_r_at
    syn match gg_h_construct_else_if "^[[:space:]]*else-if" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " and \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " case-insensitive \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " case-insensitive,\@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " case-insensitive$" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " contain \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " equal \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " every \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " greater-equal \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " greater-than \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " length \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " lesser-equal \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " lesser-than \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " not-contain \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " not-equal \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " not-every \@=" contained containedin=gg_r_construct_else_if
    syn match gg_h_clause_else_if " or \@=" contained containedin=gg_r_construct_else_if
    hi def link gg_h_clause_else_if    golfClause
    hi def link gg_h_clause_output_else_if    golfClauseOutput
    hi def link gg_h_construct_else_if    golfConstruct
    hi def link gg_h_print_inline_else_if    golfConstruct
syn region gg_r_construct_if_true start="^[[:space:]]*if-true" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_if_true,gg_r_inline_if_true,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_if_true,gg_r_inline_if_true,gg_r_at
    syn match gg_h_construct_if_true "^[[:space:]]*if-true" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " and \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " case-insensitive \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " case-insensitive,\@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " case-insensitive$" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " contain \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " equal \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " every \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " greater-equal \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " greater-than \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " length \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " lesser-equal \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " lesser-than \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " not-contain \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " not-equal \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " not-every \@=" contained containedin=gg_r_construct_if_true
    syn match gg_h_clause_if_true " or \@=" contained containedin=gg_r_construct_if_true
    hi def link gg_h_clause_if_true    golfClause
    hi def link gg_h_clause_output_if_true    golfClauseOutput
    hi def link gg_h_construct_if_true    golfConstruct
    hi def link gg_h_print_inline_if_true    golfConstruct
syn region gg_r_construct_set_string start="^[[:space:]]*set-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_string,gg_r_inline_set_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_string,gg_r_inline_set_string,gg_r_at
    syn match gg_h_construct_set_string "^[[:space:]]*set-string" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " length \@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " process-scope \@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " process-scope,\@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " process-scope$" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " unquoted \@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " unquoted,\@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " unquoted$" contained containedin=gg_r_construct_set_string
    hi def link gg_h_clause_set_string    golfClause
    hi def link gg_h_clause_output_set_string    golfClauseOutput
    hi def link gg_h_construct_set_string    golfConstruct
    hi def link gg_h_print_inline_set_string    golfConstruct
syn region gg_r_construct_new_string start="^[[:space:]]*new-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_string,gg_r_inline_new_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_string,gg_r_inline_new_string,gg_r_at
    syn match gg_h_construct_new_string "^[[:space:]]*new-string" contained containedin=gg_r_construct_new_string
    syn match gg_h_clause_new_string " length \@=" contained containedin=gg_r_construct_new_string
    hi def link gg_h_clause_new_string    golfClause
    hi def link gg_h_clause_output_new_string    golfClauseOutput
    hi def link gg_h_construct_new_string    golfConstruct
    hi def link gg_h_print_inline_new_string    golfConstruct
syn region gg_r_construct_set_number start="^[[:space:]]*set-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_number,gg_r_inline_set_number,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_number,gg_r_inline_set_number,gg_r_at
    syn match gg_h_construct_set_number "^[[:space:]]*set-number" contained containedin=gg_r_construct_set_number
    syn match gg_h_clause_set_number " process-scope \@=" contained containedin=gg_r_construct_set_number
    syn match gg_h_clause_set_number " process-scope,\@=" contained containedin=gg_r_construct_set_number
    syn match gg_h_clause_set_number " process-scope$" contained containedin=gg_r_construct_set_number
    hi def link gg_h_clause_set_number    golfClause
    hi def link gg_h_clause_output_set_number    golfClauseOutput
    hi def link gg_h_construct_set_number    golfConstruct
    hi def link gg_h_print_inline_set_number    golfConstruct
syn region gg_r_construct_set_bool start="^[[:space:]]*set-bool" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_bool,gg_r_inline_set_bool,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_bool,gg_r_inline_set_bool,gg_r_at
    syn match gg_h_construct_set_bool "^[[:space:]]*set-bool" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " and \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " && \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " &&,\@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " &&$" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " case-insensitive \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " case-insensitive,\@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " case-insensitive$" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " contain \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " equal \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " every \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " greater-equal \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " greater-than \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " length \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " lesser-equal \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " lesser-than \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " ! \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " !,\@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " !$" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " not-contain \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " not-equal \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " not-every \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " or \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " || \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " ||,\@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " ||$" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " process-scope \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " process-scope,\@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " process-scope$" contained containedin=gg_r_construct_set_bool
    hi def link gg_h_clause_set_bool    golfClause
    hi def link gg_h_clause_output_set_bool    golfClauseOutput
    hi def link gg_h_construct_set_bool    golfConstruct
    hi def link gg_h_print_inline_set_bool    golfConstruct
syn region gg_r_construct___ start="^[[:space:]]*%%" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct___,gg_r_inline___,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct___,gg_r_inline___,gg_r_at
    syn match gg_h_construct___ "^[[:space:]]*%%" contained containedin=gg_r_construct___
    syn match gg_h_clause___ " private \@=" contained containedin=gg_r_construct___
    syn match gg_h_clause___ " private,\@=" contained containedin=gg_r_construct___
    syn match gg_h_clause___ " private$" contained containedin=gg_r_construct___
    syn match gg_h_clause___ " public \@=" contained containedin=gg_r_construct___
    syn match gg_h_clause___ " public,\@=" contained containedin=gg_r_construct___
    syn match gg_h_clause___ " public$" contained containedin=gg_r_construct___
    hi def link gg_h_clause___    golfClause
    hi def link gg_h_clause_output___    golfClauseOutput
    hi def link gg_h_construct___    golfConstruct
    hi def link gg_h_print_inline___    golfConstruct
syn region gg_r_construct_begin_handler start="^[[:space:]]*begin-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_begin_handler,gg_r_inline_begin_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_begin_handler,gg_r_inline_begin_handler,gg_r_at
    syn match gg_h_construct_begin_handler "^[[:space:]]*begin-handler" contained containedin=gg_r_construct_begin_handler
    syn match gg_h_clause_begin_handler " private \@=" contained containedin=gg_r_construct_begin_handler
    syn match gg_h_clause_begin_handler " private,\@=" contained containedin=gg_r_construct_begin_handler
    syn match gg_h_clause_begin_handler " private$" contained containedin=gg_r_construct_begin_handler
    syn match gg_h_clause_begin_handler " public \@=" contained containedin=gg_r_construct_begin_handler
    syn match gg_h_clause_begin_handler " public,\@=" contained containedin=gg_r_construct_begin_handler
    syn match gg_h_clause_begin_handler " public$" contained containedin=gg_r_construct_begin_handler
    hi def link gg_h_clause_begin_handler    golfClause
    hi def link gg_h_clause_output_begin_handler    golfClauseOutput
    hi def link gg_h_construct_begin_handler    golfConstruct
    hi def link gg_h_print_inline_begin_handler    golfConstruct
syn region gg_r_construct_end_before_handler start="^[[:space:]]*end-before-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_before_handler,gg_r_inline_end_before_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_before_handler,gg_r_inline_end_before_handler,gg_r_at
    syn match gg_h_construct_end_before_handler "^[[:space:]]*end-before-handler" contained containedin=gg_r_construct_end_before_handler
    hi def link gg_h_clause_end_before_handler    golfClause
    hi def link gg_h_clause_output_end_before_handler    golfClauseOutput
    hi def link gg_h_construct_end_before_handler    golfConstruct
    hi def link gg_h_print_inline_end_before_handler    golfConstruct
syn region gg_r_construct_end_after_handler start="^[[:space:]]*end-after-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_after_handler,gg_r_inline_end_after_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_after_handler,gg_r_inline_end_after_handler,gg_r_at
    syn match gg_h_construct_end_after_handler "^[[:space:]]*end-after-handler" contained containedin=gg_r_construct_end_after_handler
    hi def link gg_h_clause_end_after_handler    golfClause
    hi def link gg_h_clause_output_end_after_handler    golfClauseOutput
    hi def link gg_h_construct_end_after_handler    golfConstruct
    hi def link gg_h_print_inline_end_after_handler    golfConstruct
syn region gg_r_construct_before_handler start="^[[:space:]]*before-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_before_handler,gg_r_inline_before_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_before_handler,gg_r_inline_before_handler,gg_r_at
    syn match gg_h_construct_before_handler "^[[:space:]]*before-handler" contained containedin=gg_r_construct_before_handler
    hi def link gg_h_clause_before_handler    golfClause
    hi def link gg_h_clause_output_before_handler    golfClauseOutput
    hi def link gg_h_construct_before_handler    golfConstruct
    hi def link gg_h_print_inline_before_handler    golfConstruct
syn region gg_r_construct_after_handler start="^[[:space:]]*after-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_after_handler,gg_r_inline_after_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_after_handler,gg_r_inline_after_handler,gg_r_at
    syn match gg_h_construct_after_handler "^[[:space:]]*after-handler" contained containedin=gg_r_construct_after_handler
    hi def link gg_h_clause_after_handler    golfClause
    hi def link gg_h_clause_output_after_handler    golfClauseOutput
    hi def link gg_h_construct_after_handler    golfConstruct
    hi def link gg_h_print_inline_after_handler    golfConstruct
syn region gg_r_construct_end_handler start="^[[:space:]]*end-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_handler,gg_r_inline_end_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_handler,gg_r_inline_end_handler,gg_r_at
    syn match gg_h_construct_end_handler "^[[:space:]]*end-handler" contained containedin=gg_r_construct_end_handler
    hi def link gg_h_clause_end_handler    golfClause
    hi def link gg_h_clause_output_end_handler    golfClauseOutput
    hi def link gg_h_construct_end_handler    golfConstruct
    hi def link gg_h_print_inline_end_handler    golfConstruct
syn region gg_r_construct_set_param start="^[[:space:]]*set-param" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_param,gg_r_inline_set_param,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_param,gg_r_inline_set_param,gg_r_at
    syn match gg_h_construct_set_param "^[[:space:]]*set-param" contained containedin=gg_r_construct_set_param
    syn match gg_h_clause_set_param " , \@=" contained containedin=gg_r_construct_set_param
    syn match gg_h_clause_set_param " ,,\@=" contained containedin=gg_r_construct_set_param
    syn match gg_h_clause_set_param " ,$" contained containedin=gg_r_construct_set_param
    hi def link gg_h_clause_set_param    golfClause
    hi def link gg_h_clause_output_set_param    golfClauseOutput
    hi def link gg_h_construct_set_param    golfConstruct
    hi def link gg_h_print_inline_set_param    golfConstruct
syn region gg_r_construct_get_param start="^[[:space:]]*get-param" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_param,gg_r_inline_get_param,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_param,gg_r_inline_get_param,gg_r_at
    syn match gg_h_construct_get_param "^[[:space:]]*get-param" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " , \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " ,,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " ,$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " default-value \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " bool-array \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " bool-array,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " bool-array$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " number-array \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " number-array,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " number-array$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " string-array \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " string-array,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " string-array$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " bool \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " bool,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " bool$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " encrypt-decrypt \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " encrypt-decrypt,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " encrypt-decrypt$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " fifo \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " fifo,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " fifo$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " file \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " file,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " file$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " hash \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " hash,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " hash$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " lifo \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " lifo,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " lifo$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " list \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " list,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " list$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " message \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " message,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " message$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " number \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " number,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " number$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " service \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " service,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " service$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " split-string \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " split-string,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " split-string$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " string \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " string,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " string$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " tree \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " tree,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " tree$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " tree-cursor \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " tree-cursor,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " tree-cursor$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " type \@=" contained containedin=gg_r_construct_get_param
    hi def link gg_h_clause_get_param    golfClause
    hi def link gg_h_clause_output_get_param    golfClauseOutput
    hi def link gg_h_construct_get_param    golfConstruct
    hi def link gg_h_print_inline_get_param    golfConstruct
syn region gg_r_construct_get_cookie start="^[[:space:]]*get-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_cookie,gg_r_inline_get_cookie,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_cookie,gg_r_inline_get_cookie,gg_r_at
    syn match gg_h_construct_get_cookie "^[[:space:]]*get-cookie" contained containedin=gg_r_construct_get_cookie
    syn match gg_h_clause_get_cookie " , \@=" contained containedin=gg_r_construct_get_cookie
    syn match gg_h_clause_get_cookie " ,,\@=" contained containedin=gg_r_construct_get_cookie
    syn match gg_h_clause_get_cookie " ,$" contained containedin=gg_r_construct_get_cookie
    hi def link gg_h_clause_get_cookie    golfClause
    hi def link gg_h_clause_output_get_cookie    golfClauseOutput
    hi def link gg_h_construct_get_cookie    golfConstruct
    hi def link gg_h_print_inline_get_cookie    golfConstruct
syn region gg_r_construct_set_cookie start="^[[:space:]]*set-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_cookie,gg_r_inline_set_cookie,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_cookie,gg_r_inline_set_cookie,gg_r_at
    syn match gg_h_construct_set_cookie "^[[:space:]]*set-cookie" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " , \@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " ,,\@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " ,$" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " expires \@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " no-http-only \@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " no-http-only,\@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " no-http-only$" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " path \@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " same-site \@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " secure \@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " secure,\@=" contained containedin=gg_r_construct_set_cookie
    syn match gg_h_clause_set_cookie " secure$" contained containedin=gg_r_construct_set_cookie
    hi def link gg_h_clause_set_cookie    golfClause
    hi def link gg_h_clause_output_set_cookie    golfClauseOutput
    hi def link gg_h_construct_set_cookie    golfConstruct
    hi def link gg_h_print_inline_set_cookie    golfConstruct
syn region gg_r_construct_request_body start="^[[:space:]]*request-body" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_request_body,gg_r_inline_request_body,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_request_body,gg_r_inline_request_body,gg_r_at
    syn match gg_h_construct_request_body "^[[:space:]]*request-body" contained containedin=gg_r_construct_request_body
    hi def link gg_h_clause_request_body    golfClause
    hi def link gg_h_clause_output_request_body    golfClauseOutput
    hi def link gg_h_construct_request_body    golfConstruct
    hi def link gg_h_print_inline_request_body    golfConstruct
syn region gg_r_construct_delete_cookie start="^[[:space:]]*delete-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_cookie,gg_r_inline_delete_cookie,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_cookie,gg_r_inline_delete_cookie,gg_r_at
    syn match gg_h_construct_delete_cookie "^[[:space:]]*delete-cookie" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " path \@=" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " secure \@=" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " secure,\@=" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " secure$" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_output_delete_cookie " status \@=" contained containedin=gg_r_construct_delete_cookie
    hi def link gg_h_clause_delete_cookie    golfClause
    hi def link gg_h_clause_output_delete_cookie    golfClauseOutput
    hi def link gg_h_construct_delete_cookie    golfConstruct
    hi def link gg_h_print_inline_delete_cookie    golfConstruct
syn region gg_r_construct_copy_string start="^[[:space:]]*copy-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_copy_string,gg_r_inline_copy_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_copy_string,gg_r_inline_copy_string,gg_r_at
    syn match gg_h_construct_copy_string "^[[:space:]]*copy-string" contained containedin=gg_r_construct_copy_string
    syn match gg_h_clause_copy_string " length \@=" contained containedin=gg_r_construct_copy_string
    syn match gg_h_clause_copy_string " start-with \@=" contained containedin=gg_r_construct_copy_string
    syn match gg_h_clause_output_copy_string " to \@=" contained containedin=gg_r_construct_copy_string
    hi def link gg_h_clause_copy_string    golfClause
    hi def link gg_h_clause_output_copy_string    golfClauseOutput
    hi def link gg_h_construct_copy_string    golfConstruct
    hi def link gg_h_print_inline_copy_string    golfConstruct
syn region gg_r_construct_scan_string start="^[[:space:]]*scan-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_scan_string,gg_r_inline_scan_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_scan_string,gg_r_inline_scan_string,gg_r_at
    syn match gg_h_construct_scan_string "^[[:space:]]*scan-string" contained containedin=gg_r_construct_scan_string
    syn match gg_h_clause_scan_string " for \@=" contained containedin=gg_r_construct_scan_string
    syn match gg_h_clause_scan_string " length \@=" contained containedin=gg_r_construct_scan_string
    syn match gg_h_clause_scan_string " start-with \@=" contained containedin=gg_r_construct_scan_string
    syn match gg_h_clause_output_scan_string " status \@=" contained containedin=gg_r_construct_scan_string
    hi def link gg_h_clause_scan_string    golfClause
    hi def link gg_h_clause_output_scan_string    golfClauseOutput
    hi def link gg_h_construct_scan_string    golfConstruct
    hi def link gg_h_print_inline_scan_string    golfConstruct
syn region gg_r_construct_replace_string start="^[[:space:]]*replace-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_replace_string,gg_r_inline_replace_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_replace_string,gg_r_inline_replace_string,gg_r_at
    syn match gg_h_construct_replace_string "^[[:space:]]*replace-string" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " copy \@=" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " copy-end \@=" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " length \@=" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " start-with \@=" contained containedin=gg_r_construct_replace_string
    hi def link gg_h_clause_replace_string    golfClause
    hi def link gg_h_clause_output_replace_string    golfClauseOutput
    hi def link gg_h_construct_replace_string    golfConstruct
    hi def link gg_h_print_inline_replace_string    golfConstruct
syn region gg_r_construct_flush_output start="^[[:space:]]*flush-output" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_flush_output,gg_r_inline_flush_output,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_flush_output,gg_r_inline_flush_output,gg_r_at
    syn match gg_h_construct_flush_output "^[[:space:]]*flush-output" contained containedin=gg_r_construct_flush_output
    hi def link gg_h_clause_flush_output    golfClause
    hi def link gg_h_clause_output_flush_output    golfClauseOutput
    hi def link gg_h_construct_flush_output    golfConstruct
    hi def link gg_h_print_inline_flush_output    golfConstruct
syn region gg_r_construct_extended_mode start="^[[:space:]]*extended-mode" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_extended_mode,gg_r_inline_extended_mode,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_extended_mode,gg_r_inline_extended_mode,gg_r_at
    syn match gg_h_construct_extended_mode "^[[:space:]]*extended-mode" contained containedin=gg_r_construct_extended_mode
    hi def link gg_h_clause_extended_mode    golfClause
    hi def link gg_h_clause_output_extended_mode    golfClauseOutput
    hi def link gg_h_construct_extended_mode    golfConstruct
    hi def link gg_h_print_inline_extended_mode    golfConstruct
syn region gg_r_construct_number_string start="^[[:space:]]*number-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_number_string,gg_r_inline_number_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_number_string,gg_r_inline_number_string,gg_r_at
    syn match gg_h_construct_number_string "^[[:space:]]*number-string" contained containedin=gg_r_construct_number_string
    syn match gg_h_clause_number_string " base \@=" contained containedin=gg_r_construct_number_string
    syn match gg_h_print_inline_number_string " base \@=" contained containedin=gg_r_inline_number_string
    syn match gg_h_clause_output_number_string " status \@=" contained containedin=gg_r_construct_number_string
    syn match gg_h_print_inline_number_string " status \@=" contained containedin=gg_r_inline_number_string
    syn match gg_h_clause_output_number_string " to \@=" contained containedin=gg_r_construct_number_string
    syn match gg_h_print_inline_number_string " to \@=" contained containedin=gg_r_inline_number_string
    syn region gg_r_inline_number_string start="<<[[:space:]]*number-string \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_number_string '<<[[:space:]]*number-string \@=' contained containedin=gg_r_inline_number_string
    syn match gg_h_print_inline_number_string '>>' contained containedin=gg_r_inline_number_string
    hi def link gg_h_clause_number_string    golfClause
    hi def link gg_h_clause_output_number_string    golfClauseOutput
    hi def link gg_h_construct_number_string    golfConstruct
    hi def link gg_h_print_inline_number_string    golfConstruct
syn region gg_r_construct_string_number start="^[[:space:]]*string-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_string_number,gg_r_inline_string_number,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_string_number,gg_r_inline_string_number,gg_r_at
    syn match gg_h_construct_string_number "^[[:space:]]*string-number" contained containedin=gg_r_construct_string_number
    syn match gg_h_clause_string_number " base \@=" contained containedin=gg_r_construct_string_number
    syn match gg_h_clause_output_string_number " status \@=" contained containedin=gg_r_construct_string_number
    syn match gg_h_clause_output_string_number " to \@=" contained containedin=gg_r_construct_string_number
    hi def link gg_h_clause_string_number    golfClause
    hi def link gg_h_clause_output_string_number    golfClauseOutput
    hi def link gg_h_construct_string_number    golfConstruct
    hi def link gg_h_print_inline_string_number    golfConstruct
syn region gg_r_construct_abs_number start="^[[:space:]]*abs-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_abs_number,gg_r_inline_abs_number,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_abs_number,gg_r_inline_abs_number,gg_r_at
    syn match gg_h_construct_abs_number "^[[:space:]]*abs-number" contained containedin=gg_r_construct_abs_number
    syn match gg_h_clause_output_abs_number " to \@=" contained containedin=gg_r_construct_abs_number
    hi def link gg_h_clause_abs_number    golfClause
    hi def link gg_h_clause_output_abs_number    golfClauseOutput
    hi def link gg_h_construct_abs_number    golfConstruct
    hi def link gg_h_print_inline_abs_number    golfConstruct
syn region gg_r_construct_get_req start="^[[:space:]]*get-req" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_req,gg_r_inline_get_req,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_req,gg_r_inline_get_req,gg_r_at
    syn match gg_h_construct_get_req "^[[:space:]]*get-req" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " arg-count \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " arg-count,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " arg-count$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " arg-value \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " content-type \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " cookie \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " cookie-count \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " cookie-count,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " cookie-count$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " directory \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " directory,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " directory$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " errno \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " errno,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " errno$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " error \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " external-call \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " external-call,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " external-call$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " header \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " method \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " name \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " name,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " name$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " process-id \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " process-id,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " process-id$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " referring-url \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " referring-url,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " referring-url$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " source-file \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " source-file,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " source-file$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_output_get_req " to \@=" contained containedin=gg_r_construct_get_req
    hi def link gg_h_clause_get_req    golfClause
    hi def link gg_h_clause_output_get_req    golfClauseOutput
    hi def link gg_h_construct_get_req    golfConstruct
    hi def link gg_h_print_inline_get_req    golfConstruct
syn region gg_r_construct_set_app start="^[[:space:]]*set-app" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_app,gg_r_inline_set_app,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_app,gg_r_inline_set_app,gg_r_at
    syn match gg_h_construct_set_app "^[[:space:]]*set-app" contained containedin=gg_r_construct_set_app
    syn match gg_h_clause_set_app " stack-depth \@=" contained containedin=gg_r_construct_set_app
    hi def link gg_h_clause_set_app    golfClause
    hi def link gg_h_clause_output_set_app    golfClauseOutput
    hi def link gg_h_construct_set_app    golfConstruct
    hi def link gg_h_print_inline_set_app    golfConstruct
syn region gg_r_construct_get_app start="^[[:space:]]*get-app" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_app,gg_r_inline_get_app,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_app,gg_r_inline_get_app,gg_r_at
    syn match gg_h_construct_get_app "^[[:space:]]*get-app" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " application \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " db-vendor \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " directory \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " directory,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " directory$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " file-directory \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " file-directory,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " file-directory$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " is-service \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " is-service,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " is-service$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " name \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " name,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " name$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " path \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " socket-file \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " socket-file,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " socket-file$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " stack-depth \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_output_get_app " to \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " upload-size \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " upload-size,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " upload-size$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " user \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " user-directory \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " user-directory,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " user-directory$" contained containedin=gg_r_construct_get_app
    hi def link gg_h_clause_get_app    golfClause
    hi def link gg_h_clause_output_get_app    golfClauseOutput
    hi def link gg_h_construct_get_app    golfConstruct
    hi def link gg_h_print_inline_get_app    golfConstruct
syn region gg_r_construct_get_sys start="^[[:space:]]*get-sys" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_sys,gg_r_inline_get_sys,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_sys,gg_r_inline_get_sys,gg_r_at
    syn match gg_h_construct_get_sys "^[[:space:]]*get-sys" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " directory \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " directory,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " directory$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " environment \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-dir \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-dir,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-dir$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-name \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-name,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-name$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-user \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-user,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-user$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-version \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-version,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-version$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_output_get_sys " to \@=" contained containedin=gg_r_construct_get_sys
    hi def link gg_h_clause_get_sys    golfClause
    hi def link gg_h_clause_output_get_sys    golfClauseOutput
    hi def link gg_h_construct_get_sys    golfConstruct
    hi def link gg_h_print_inline_get_sys    golfConstruct
syn region gg_r_construct_match_regex start="^[[:space:]]*match-regex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_match_regex,gg_r_inline_match_regex,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_match_regex,gg_r_inline_match_regex,gg_r_at
    syn match gg_h_construct_match_regex "^[[:space:]]*match-regex" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " cache \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " cache,\@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " cache$" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " case-insensitive \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " case-insensitive,\@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " case-insensitive$" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " clear-cache \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " in \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " replace-with \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_output_match_regex " result \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " single-match \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " single-match,\@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " single-match$" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_output_match_regex " status \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " utf \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " utf,\@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " utf$" contained containedin=gg_r_construct_match_regex
    hi def link gg_h_clause_match_regex    golfClause
    hi def link gg_h_clause_output_match_regex    golfClauseOutput
    hi def link gg_h_construct_match_regex    golfConstruct
    hi def link gg_h_print_inline_match_regex    golfConstruct
syn region gg_r_construct_get_time start="^[[:space:]]*get-time" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_time,gg_r_inline_get_time,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_time,gg_r_inline_get_time,gg_r_at
    syn match gg_h_construct_get_time "^[[:space:]]*get-time" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " day \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " format \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " from-epoch \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " hour \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " minute \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " month \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " second \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " since-epoch \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " since-epoch,\@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " since-epoch$" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " timezone \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_output_get_time " to \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " year \@=" contained containedin=gg_r_construct_get_time
    hi def link gg_h_clause_get_time    golfClause
    hi def link gg_h_clause_output_get_time    golfClauseOutput
    hi def link gg_h_construct_get_time    golfConstruct
    hi def link gg_h_print_inline_get_time    golfConstruct
syn region gg_r_construct_uniq_file start="^[[:space:]]*uniq-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_uniq_file,gg_r_inline_uniq_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_uniq_file,gg_r_inline_uniq_file,gg_r_at
    syn match gg_h_construct_uniq_file "^[[:space:]]*uniq-file" contained containedin=gg_r_construct_uniq_file
    syn match gg_h_clause_uniq_file " temporary \@=" contained containedin=gg_r_construct_uniq_file
    syn match gg_h_clause_uniq_file " temporary,\@=" contained containedin=gg_r_construct_uniq_file
    syn match gg_h_clause_uniq_file " temporary$" contained containedin=gg_r_construct_uniq_file
    hi def link gg_h_clause_uniq_file    golfClause
    hi def link gg_h_clause_output_uniq_file    golfClauseOutput
    hi def link gg_h_construct_uniq_file    golfConstruct
    hi def link gg_h_print_inline_uniq_file    golfConstruct
syn region gg_r_construct_call_handler start="^[[:space:]]*call-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_handler,gg_r_inline_call_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_handler,gg_r_inline_call_handler,gg_r_at
    syn match gg_h_construct_call_handler "^[[:space:]]*call-handler" contained containedin=gg_r_construct_call_handler
    syn match gg_h_clause_output_call_handler " return-value \@=" contained containedin=gg_r_construct_call_handler
    syn match gg_h_print_inline_call_handler " return-value \@=" contained containedin=gg_r_inline_call_handler
    syn region gg_r_inline_call_handler start="<<[[:space:]]*call-handler \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_call_handler '<<[[:space:]]*call-handler \@=' contained containedin=gg_r_inline_call_handler
    syn match gg_h_print_inline_call_handler '>>' contained containedin=gg_r_inline_call_handler
    hi def link gg_h_clause_call_handler    golfClause
    hi def link gg_h_clause_output_call_handler    golfClauseOutput
    hi def link gg_h_construct_call_handler    golfConstruct
    hi def link gg_h_print_inline_call_handler    golfConstruct
syn region gg_r_construct_call_remote start="^[[:space:]]*call-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_remote,gg_r_inline_call_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_remote,gg_r_inline_call_remote,gg_r_at
    syn match gg_h_construct_call_remote "^[[:space:]]*call-remote" contained containedin=gg_r_construct_call_remote
    syn match gg_h_clause_output_call_remote " finished-okay \@=" contained containedin=gg_r_construct_call_remote
    syn match gg_h_clause_output_call_remote " started \@=" contained containedin=gg_r_construct_call_remote
    syn match gg_h_clause_output_call_remote " status \@=" contained containedin=gg_r_construct_call_remote
    hi def link gg_h_clause_call_remote    golfClause
    hi def link gg_h_clause_output_call_remote    golfClauseOutput
    hi def link gg_h_construct_call_remote    golfConstruct
    hi def link gg_h_print_inline_call_remote    golfConstruct
syn region gg_r_construct_call_extended start="^[[:space:]]*call-extended" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_extended,gg_r_inline_call_extended,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_extended,gg_r_inline_call_extended,gg_r_at
    syn match gg_h_construct_call_extended "^[[:space:]]*call-extended" contained containedin=gg_r_construct_call_extended
    syn region gg_r_inline_call_extended start="<<[[:space:]]*call-extended \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_call_extended '<<[[:space:]]*call-extended \@=' contained containedin=gg_r_inline_call_extended
    syn match gg_h_print_inline_call_extended '>>' contained containedin=gg_r_inline_call_extended
    hi def link gg_h_clause_call_extended    golfClause
    hi def link gg_h_clause_output_call_extended    golfClauseOutput
    hi def link gg_h_construct_call_extended    golfConstruct
    hi def link gg_h_print_inline_call_extended    golfConstruct
syn region gg_r_construct_read_remote start="^[[:space:]]*read-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_remote,gg_r_inline_read_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_remote,gg_r_inline_read_remote,gg_r_at
    syn match gg_h_construct_read_remote "^[[:space:]]*read-remote" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_read_remote " data \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_read_remote " error \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_read_remote " exit-status \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_read_remote " status \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_read_remote " status-text \@=" contained containedin=gg_r_construct_read_remote
    hi def link gg_h_clause_read_remote    golfClause
    hi def link gg_h_clause_output_read_remote    golfClauseOutput
    hi def link gg_h_construct_read_remote    golfConstruct
    hi def link gg_h_print_inline_read_remote    golfConstruct
syn region gg_r_construct_new_remote start="^[[:space:]]*new-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_remote,gg_r_inline_new_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_remote,gg_r_inline_new_remote,gg_r_at
    syn match gg_h_construct_new_remote "^[[:space:]]*new-remote" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " app-path \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " content \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " content-length \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " content-type \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " environment \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " local \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " location \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " method \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " request-body \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " request-path \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " timeout \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " url-params \@=" contained containedin=gg_r_construct_new_remote
    syn match gg_h_clause_new_remote " url-path \@=" contained containedin=gg_r_construct_new_remote
    hi def link gg_h_clause_new_remote    golfClause
    hi def link gg_h_clause_output_new_remote    golfClauseOutput
    hi def link gg_h_construct_new_remote    golfConstruct
    hi def link gg_h_print_inline_new_remote    golfConstruct
syn region gg_r_construct_run_remote start="^[[:space:]]*run-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_run_remote,gg_r_inline_run_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_run_remote,gg_r_inline_run_remote,gg_r_at
    syn match gg_h_construct_run_remote "^[[:space:]]*run-remote" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " app-path \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " content \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " content-length \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " content-type \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " data \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " environment \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " error \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " exit-status \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " finished-okay \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " local \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " location \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " method \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " request-body \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " request-path \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " started \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " status \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " status-text \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " timeout \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " url-params \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " url-path \@=" contained containedin=gg_r_construct_run_remote
    hi def link gg_h_clause_run_remote    golfClause
    hi def link gg_h_clause_output_run_remote    golfClauseOutput
    hi def link gg_h_construct_run_remote    golfConstruct
    hi def link gg_h_print_inline_run_remote    golfConstruct
syn region gg_r_construct_call_web start="^[[:space:]]*call-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_web,gg_r_inline_call_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_web,gg_r_inline_call_web,gg_r_at
    syn match gg_h_construct_call_web "^[[:space:]]*call-web" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " cert \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " content \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " content-length \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " content-type \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " cookie-jar \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " custom \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_output_call_web " error \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " fields \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " files \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " method \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " no-cert \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " no-cert,\@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " no-cert$" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " request-body \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " request-headers \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_output_call_web " response \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_output_call_web " response-code \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_output_call_web " response-headers \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_output_call_web " status \@=" contained containedin=gg_r_construct_call_web
    syn match gg_h_clause_call_web " timeout \@=" contained containedin=gg_r_construct_call_web
    hi def link gg_h_clause_call_web    golfClause
    hi def link gg_h_clause_output_call_web    golfClauseOutput
    hi def link gg_h_construct_call_web    golfConstruct
    hi def link gg_h_print_inline_call_web    golfConstruct
syn region gg_r_construct_delete_file start="^[[:space:]]*delete-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_file,gg_r_inline_delete_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_file,gg_r_inline_delete_file,gg_r_at
    syn match gg_h_construct_delete_file "^[[:space:]]*delete-file" contained containedin=gg_r_construct_delete_file
    syn match gg_h_clause_output_delete_file " status \@=" contained containedin=gg_r_construct_delete_file
    hi def link gg_h_clause_delete_file    golfClause
    hi def link gg_h_clause_output_delete_file    golfClauseOutput
    hi def link gg_h_construct_delete_file    golfConstruct
    hi def link gg_h_print_inline_delete_file    golfConstruct
syn region gg_r_construct_change_dir start="^[[:space:]]*change-dir" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_change_dir,gg_r_inline_change_dir,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_change_dir,gg_r_inline_change_dir,gg_r_at
    syn match gg_h_construct_change_dir "^[[:space:]]*change-dir" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_change_dir " home \@=" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_change_dir " home,\@=" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_change_dir " home$" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_change_dir " run-dir \@=" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_change_dir " run-dir,\@=" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_change_dir " run-dir$" contained containedin=gg_r_construct_change_dir
    syn match gg_h_clause_output_change_dir " status \@=" contained containedin=gg_r_construct_change_dir
    hi def link gg_h_clause_change_dir    golfClause
    hi def link gg_h_clause_output_change_dir    golfClauseOutput
    hi def link gg_h_construct_change_dir    golfConstruct
    hi def link gg_h_print_inline_change_dir    golfConstruct
syn region gg_r_construct_new_dir start="^[[:space:]]*new-dir" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_dir,gg_r_inline_new_dir,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_dir,gg_r_inline_new_dir,gg_r_at
    syn match gg_h_construct_new_dir "^[[:space:]]*new-dir" contained containedin=gg_r_construct_new_dir
    syn match gg_h_clause_new_dir " mode \@=" contained containedin=gg_r_construct_new_dir
    syn match gg_h_clause_output_new_dir " status \@=" contained containedin=gg_r_construct_new_dir
    hi def link gg_h_clause_new_dir    golfClause
    hi def link gg_h_clause_output_new_dir    golfClauseOutput
    hi def link gg_h_construct_new_dir    golfConstruct
    hi def link gg_h_print_inline_new_dir    golfConstruct
syn region gg_r_construct_change_mode start="^[[:space:]]*change-mode" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_change_mode,gg_r_inline_change_mode,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_change_mode,gg_r_inline_change_mode,gg_r_at
    syn match gg_h_construct_change_mode "^[[:space:]]*change-mode" contained containedin=gg_r_construct_change_mode
    syn match gg_h_clause_change_mode " mode \@=" contained containedin=gg_r_construct_change_mode
    syn match gg_h_clause_output_change_mode " status \@=" contained containedin=gg_r_construct_change_mode
    hi def link gg_h_clause_change_mode    golfClause
    hi def link gg_h_clause_output_change_mode    golfClauseOutput
    hi def link gg_h_construct_change_mode    golfConstruct
    hi def link gg_h_print_inline_change_mode    golfConstruct
syn region gg_r_construct_delete_dir start="^[[:space:]]*delete-dir" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_dir,gg_r_inline_delete_dir,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_dir,gg_r_inline_delete_dir,gg_r_at
    syn match gg_h_construct_delete_dir "^[[:space:]]*delete-dir" contained containedin=gg_r_construct_delete_dir
    syn match gg_h_clause_output_delete_dir " status \@=" contained containedin=gg_r_construct_delete_dir
    hi def link gg_h_clause_delete_dir    golfClause
    hi def link gg_h_clause_output_delete_dir    golfClauseOutput
    hi def link gg_h_construct_delete_dir    golfConstruct
    hi def link gg_h_print_inline_delete_dir    golfConstruct
syn region gg_r_construct_rename_file start="^[[:space:]]*rename-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rename_file,gg_r_inline_rename_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rename_file,gg_r_inline_rename_file,gg_r_at
    syn match gg_h_construct_rename_file "^[[:space:]]*rename-file" contained containedin=gg_r_construct_rename_file
    syn match gg_h_clause_output_rename_file " status \@=" contained containedin=gg_r_construct_rename_file
    syn match gg_h_clause_rename_file " to \@=" contained containedin=gg_r_construct_rename_file
    hi def link gg_h_clause_rename_file    golfClause
    hi def link gg_h_clause_output_rename_file    golfClauseOutput
    hi def link gg_h_construct_rename_file    golfConstruct
    hi def link gg_h_print_inline_rename_file    golfConstruct
syn region gg_r_construct_text_utf start="^[[:space:]]*text-utf" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_text_utf,gg_r_inline_text_utf,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_text_utf,gg_r_inline_text_utf,gg_r_at
    syn match gg_h_construct_text_utf "^[[:space:]]*text-utf" contained containedin=gg_r_construct_text_utf
    syn match gg_h_clause_output_text_utf " error-text \@=" contained containedin=gg_r_construct_text_utf
    syn match gg_h_clause_output_text_utf " status \@=" contained containedin=gg_r_construct_text_utf
    hi def link gg_h_clause_text_utf    golfClause
    hi def link gg_h_clause_output_text_utf    golfClauseOutput
    hi def link gg_h_construct_text_utf    golfConstruct
    hi def link gg_h_print_inline_text_utf    golfConstruct
syn region gg_r_construct_utf_text start="^[[:space:]]*utf-text" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_utf_text,gg_r_inline_utf_text,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_utf_text,gg_r_inline_utf_text,gg_r_at
    syn match gg_h_construct_utf_text "^[[:space:]]*utf-text" contained containedin=gg_r_construct_utf_text
    syn match gg_h_clause_output_utf_text " error-text \@=" contained containedin=gg_r_construct_utf_text
    syn match gg_h_clause_utf_text " length \@=" contained containedin=gg_r_construct_utf_text
    syn match gg_h_clause_output_utf_text " status \@=" contained containedin=gg_r_construct_utf_text
    syn match gg_h_clause_output_utf_text " to \@=" contained containedin=gg_r_construct_utf_text
    hi def link gg_h_clause_utf_text    golfClause
    hi def link gg_h_clause_output_utf_text    golfClauseOutput
    hi def link gg_h_construct_utf_text    golfConstruct
    hi def link gg_h_print_inline_utf_text    golfConstruct
syn region gg_r_construct_stat_file start="^[[:space:]]*stat-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_stat_file,gg_r_inline_stat_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_stat_file,gg_r_inline_stat_file,gg_r_at
    syn match gg_h_construct_stat_file "^[[:space:]]*stat-file" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_output_stat_file " mode \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_output_stat_file " name \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_output_stat_file " path \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_output_stat_file " size \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_output_stat_file " type \@=" contained containedin=gg_r_construct_stat_file
    hi def link gg_h_clause_stat_file    golfClause
    hi def link gg_h_clause_output_stat_file    golfClauseOutput
    hi def link gg_h_construct_stat_file    golfConstruct
    hi def link gg_h_print_inline_stat_file    golfConstruct
syn region gg_r_construct_decrypt_data start="^[[:space:]]*decrypt-data" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decrypt_data,gg_r_inline_decrypt_data,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decrypt_data,gg_r_inline_decrypt_data,gg_r_at
    syn match gg_h_construct_decrypt_data "^[[:space:]]*decrypt-data" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " binary \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " binary,\@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " binary$" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " cache \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " cache,\@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " cache$" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " cipher \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " clear-cache \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " digest \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " init-vector \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " input-length \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " iterations \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " password \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " salt \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_decrypt_data " salt-length \@=" contained containedin=gg_r_construct_decrypt_data
    syn match gg_h_clause_output_decrypt_data " to \@=" contained containedin=gg_r_construct_decrypt_data
    hi def link gg_h_clause_decrypt_data    golfClause
    hi def link gg_h_clause_output_decrypt_data    golfClauseOutput
    hi def link gg_h_construct_decrypt_data    golfConstruct
    hi def link gg_h_print_inline_decrypt_data    golfConstruct
syn region gg_r_construct_encrypt_data start="^[[:space:]]*encrypt-data" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encrypt_data,gg_r_inline_encrypt_data,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encrypt_data,gg_r_inline_encrypt_data,gg_r_at
    syn match gg_h_construct_encrypt_data "^[[:space:]]*encrypt-data" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " binary \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " binary,\@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " binary$" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " cache \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " cache,\@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " cache$" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " cipher \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " clear-cache \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " digest \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " init-vector \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " input-length \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " iterations \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " password \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " salt \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_encrypt_data " salt-length \@=" contained containedin=gg_r_construct_encrypt_data
    syn match gg_h_clause_output_encrypt_data " to \@=" contained containedin=gg_r_construct_encrypt_data
    hi def link gg_h_clause_encrypt_data    golfClause
    hi def link gg_h_clause_output_encrypt_data    golfClauseOutput
    hi def link gg_h_construct_encrypt_data    golfConstruct
    hi def link gg_h_print_inline_encrypt_data    golfConstruct
syn region gg_r_construct_trim_string start="^[[:space:]]*trim-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_trim_string,gg_r_inline_trim_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_trim_string,gg_r_inline_trim_string,gg_r_at
    syn match gg_h_construct_trim_string "^[[:space:]]*trim-string" contained containedin=gg_r_construct_trim_string
    hi def link gg_h_clause_trim_string    golfClause
    hi def link gg_h_clause_output_trim_string    golfClauseOutput
    hi def link gg_h_construct_trim_string    golfConstruct
    hi def link gg_h_print_inline_trim_string    golfConstruct
syn region gg_r_construct_encode_web start="^[[:space:]]*encode-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_web,gg_r_inline_encode_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_web,gg_r_inline_encode_web,gg_r_at
    syn match gg_h_construct_encode_web "^[[:space:]]*encode-web" contained containedin=gg_r_construct_encode_web
    syn match gg_h_clause_encode_web " input-length \@=" contained containedin=gg_r_construct_encode_web
    syn match gg_h_clause_output_encode_web " to \@=" contained containedin=gg_r_construct_encode_web
    hi def link gg_h_clause_encode_web    golfClause
    hi def link gg_h_clause_output_encode_web    golfClauseOutput
    hi def link gg_h_construct_encode_web    golfConstruct
    hi def link gg_h_print_inline_encode_web    golfConstruct
syn region gg_r_construct_encode_url start="^[[:space:]]*encode-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_url,gg_r_inline_encode_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_url,gg_r_inline_encode_url,gg_r_at
    syn match gg_h_construct_encode_url "^[[:space:]]*encode-url" contained containedin=gg_r_construct_encode_url
    syn match gg_h_clause_encode_url " input-length \@=" contained containedin=gg_r_construct_encode_url
    syn match gg_h_clause_output_encode_url " to \@=" contained containedin=gg_r_construct_encode_url
    hi def link gg_h_clause_encode_url    golfClause
    hi def link gg_h_clause_output_encode_url    golfClauseOutput
    hi def link gg_h_construct_encode_url    golfConstruct
    hi def link gg_h_print_inline_encode_url    golfConstruct
syn region gg_r_construct_decode_web start="^[[:space:]]*decode-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_web,gg_r_inline_decode_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_web,gg_r_inline_decode_web,gg_r_at
    syn match gg_h_construct_decode_web "^[[:space:]]*decode-web" contained containedin=gg_r_construct_decode_web
    syn match gg_h_clause_decode_web " input-length \@=" contained containedin=gg_r_construct_decode_web
    syn match gg_h_clause_decode_web " status \@=" contained containedin=gg_r_construct_decode_web
    hi def link gg_h_clause_decode_web    golfClause
    hi def link gg_h_clause_output_decode_web    golfClauseOutput
    hi def link gg_h_construct_decode_web    golfConstruct
    hi def link gg_h_print_inline_decode_web    golfConstruct
syn region gg_r_construct_decode_url start="^[[:space:]]*decode-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_url,gg_r_inline_decode_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_url,gg_r_inline_decode_url,gg_r_at
    syn match gg_h_construct_decode_url "^[[:space:]]*decode-url" contained containedin=gg_r_construct_decode_url
    syn match gg_h_clause_decode_url " input-length \@=" contained containedin=gg_r_construct_decode_url
    syn match gg_h_clause_decode_url " status \@=" contained containedin=gg_r_construct_decode_url
    hi def link gg_h_clause_decode_url    golfClause
    hi def link gg_h_clause_output_decode_url    golfClauseOutput
    hi def link gg_h_construct_decode_url    golfConstruct
    hi def link gg_h_print_inline_decode_url    golfConstruct
syn region gg_r_construct_decode_hex start="^[[:space:]]*decode-hex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_hex,gg_r_inline_decode_hex,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_hex,gg_r_inline_decode_hex,gg_r_at
    syn match gg_h_construct_decode_hex "^[[:space:]]*decode-hex" contained containedin=gg_r_construct_decode_hex
    syn match gg_h_clause_decode_hex " input-length \@=" contained containedin=gg_r_construct_decode_hex
    syn match gg_h_clause_output_decode_hex " to \@=" contained containedin=gg_r_construct_decode_hex
    hi def link gg_h_clause_decode_hex    golfClause
    hi def link gg_h_clause_output_decode_hex    golfClauseOutput
    hi def link gg_h_construct_decode_hex    golfConstruct
    hi def link gg_h_print_inline_decode_hex    golfConstruct
syn region gg_r_construct_encode_hex start="^[[:space:]]*encode-hex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_hex,gg_r_inline_encode_hex,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_hex,gg_r_inline_encode_hex,gg_r_at
    syn match gg_h_construct_encode_hex "^[[:space:]]*encode-hex" contained containedin=gg_r_construct_encode_hex
    syn match gg_h_clause_encode_hex " input-length \@=" contained containedin=gg_r_construct_encode_hex
    syn match gg_h_clause_encode_hex " prefix \@=" contained containedin=gg_r_construct_encode_hex
    syn match gg_h_clause_output_encode_hex " to \@=" contained containedin=gg_r_construct_encode_hex
    hi def link gg_h_clause_encode_hex    golfClause
    hi def link gg_h_clause_output_encode_hex    golfClauseOutput
    hi def link gg_h_construct_encode_hex    golfConstruct
    hi def link gg_h_print_inline_encode_hex    golfConstruct
syn region gg_r_construct_encode_base64 start="^[[:space:]]*encode-base64" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_base64,gg_r_inline_encode_base64,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_base64,gg_r_inline_encode_base64,gg_r_at
    syn match gg_h_construct_encode_base64 "^[[:space:]]*encode-base64" contained containedin=gg_r_construct_encode_base64
    syn match gg_h_clause_encode_base64 " input-length \@=" contained containedin=gg_r_construct_encode_base64
    syn match gg_h_clause_output_encode_base64 " to \@=" contained containedin=gg_r_construct_encode_base64
    hi def link gg_h_clause_encode_base64    golfClause
    hi def link gg_h_clause_output_encode_base64    golfClauseOutput
    hi def link gg_h_construct_encode_base64    golfConstruct
    hi def link gg_h_print_inline_encode_base64    golfConstruct
syn region gg_r_construct_decode_base64 start="^[[:space:]]*decode-base64" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_base64,gg_r_inline_decode_base64,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_base64,gg_r_inline_decode_base64,gg_r_at
    syn match gg_h_construct_decode_base64 "^[[:space:]]*decode-base64" contained containedin=gg_r_construct_decode_base64
    syn match gg_h_clause_decode_base64 " input-length \@=" contained containedin=gg_r_construct_decode_base64
    syn match gg_h_clause_output_decode_base64 " to \@=" contained containedin=gg_r_construct_decode_base64
    hi def link gg_h_clause_decode_base64    golfClause
    hi def link gg_h_clause_output_decode_base64    golfClauseOutput
    hi def link gg_h_construct_decode_base64    golfConstruct
    hi def link gg_h_print_inline_decode_base64    golfConstruct
syn region gg_r_construct_hash_string start="^[[:space:]]*hash-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_hash_string,gg_r_inline_hash_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_hash_string,gg_r_inline_hash_string,gg_r_at
    syn match gg_h_construct_hash_string "^[[:space:]]*hash-string" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " binary \@=" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " binary,\@=" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " binary$" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " digest \@=" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_output_hash_string " to \@=" contained containedin=gg_r_construct_hash_string
    hi def link gg_h_clause_hash_string    golfClause
    hi def link gg_h_clause_output_hash_string    golfClauseOutput
    hi def link gg_h_construct_hash_string    golfConstruct
    hi def link gg_h_print_inline_hash_string    golfConstruct
syn region gg_r_construct_hmac_string start="^[[:space:]]*hmac-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_hmac_string,gg_r_inline_hmac_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_hmac_string,gg_r_inline_hmac_string,gg_r_at
    syn match gg_h_construct_hmac_string "^[[:space:]]*hmac-string" contained containedin=gg_r_construct_hmac_string
    syn match gg_h_clause_hmac_string " binary \@=" contained containedin=gg_r_construct_hmac_string
    syn match gg_h_clause_hmac_string " binary,\@=" contained containedin=gg_r_construct_hmac_string
    syn match gg_h_clause_hmac_string " binary$" contained containedin=gg_r_construct_hmac_string
    syn match gg_h_clause_hmac_string " digest \@=" contained containedin=gg_r_construct_hmac_string
    syn match gg_h_clause_hmac_string " key \@=" contained containedin=gg_r_construct_hmac_string
    syn match gg_h_clause_output_hmac_string " to \@=" contained containedin=gg_r_construct_hmac_string
    hi def link gg_h_clause_hmac_string    golfClause
    hi def link gg_h_clause_output_hmac_string    golfClauseOutput
    hi def link gg_h_construct_hmac_string    golfConstruct
    hi def link gg_h_print_inline_hmac_string    golfConstruct
syn region gg_r_construct_derive_key start="^[[:space:]]*derive-key" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_derive_key,gg_r_inline_derive_key,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_derive_key,gg_r_inline_derive_key,gg_r_at
    syn match gg_h_construct_derive_key "^[[:space:]]*derive-key" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " binary \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " binary,\@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " binary$" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " digest \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " from \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " from-length \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " iterations \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " length \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " salt \@=" contained containedin=gg_r_construct_derive_key
    syn match gg_h_clause_derive_key " salt-length \@=" contained containedin=gg_r_construct_derive_key
    hi def link gg_h_clause_derive_key    golfClause
    hi def link gg_h_clause_output_derive_key    golfClauseOutput
    hi def link gg_h_construct_derive_key    golfConstruct
    hi def link gg_h_print_inline_derive_key    golfConstruct
syn region gg_r_construct_count_substring start="^[[:space:]]*count-substring" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_count_substring,gg_r_inline_count_substring,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_count_substring,gg_r_inline_count_substring,gg_r_at
    syn match gg_h_construct_count_substring "^[[:space:]]*count-substring" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " case-insensitive \@=" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " case-insensitive,\@=" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " case-insensitive$" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " in \@=" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_output_count_substring " to \@=" contained containedin=gg_r_construct_count_substring
    hi def link gg_h_clause_count_substring    golfClause
    hi def link gg_h_clause_output_count_substring    golfClauseOutput
    hi def link gg_h_construct_count_substring    golfConstruct
    hi def link gg_h_print_inline_count_substring    golfConstruct
syn region gg_r_construct_string_length start="^[[:space:]]*string-length" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_string_length,gg_r_inline_string_length,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_string_length,gg_r_inline_string_length,gg_r_at
    syn match gg_h_construct_string_length "^[[:space:]]*string-length" contained containedin=gg_r_construct_string_length
    syn match gg_h_clause_output_string_length " to \@=" contained containedin=gg_r_construct_string_length
    syn match gg_h_print_inline_string_length " to \@=" contained containedin=gg_r_inline_string_length
    syn region gg_r_inline_string_length start="<<[[:space:]]*string-length \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_string_length '<<[[:space:]]*string-length \@=' contained containedin=gg_r_inline_string_length
    syn match gg_h_print_inline_string_length '>>' contained containedin=gg_r_inline_string_length
    hi def link gg_h_clause_string_length    golfClause
    hi def link gg_h_clause_output_string_length    golfClauseOutput
    hi def link gg_h_construct_string_length    golfConstruct
    hi def link gg_h_print_inline_string_length    golfConstruct
syn region gg_r_construct_lower_string start="^[[:space:]]*lower-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_lower_string,gg_r_inline_lower_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_lower_string,gg_r_inline_lower_string,gg_r_at
    syn match gg_h_construct_lower_string "^[[:space:]]*lower-string" contained containedin=gg_r_construct_lower_string
    hi def link gg_h_clause_lower_string    golfClause
    hi def link gg_h_clause_output_lower_string    golfClauseOutput
    hi def link gg_h_construct_lower_string    golfConstruct
    hi def link gg_h_print_inline_lower_string    golfConstruct
syn region gg_r_construct_upper_string start="^[[:space:]]*upper-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_upper_string,gg_r_inline_upper_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_upper_string,gg_r_inline_upper_string,gg_r_at
    syn match gg_h_construct_upper_string "^[[:space:]]*upper-string" contained containedin=gg_r_construct_upper_string
    hi def link gg_h_clause_upper_string    golfClause
    hi def link gg_h_clause_output_upper_string    golfClauseOutput
    hi def link gg_h_construct_upper_string    golfConstruct
    hi def link gg_h_print_inline_upper_string    golfConstruct
syn region gg_r_construct_random_crypto start="^[[:space:]]*random-crypto" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_random_crypto,gg_r_inline_random_crypto,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_random_crypto,gg_r_inline_random_crypto,gg_r_at
    syn match gg_h_construct_random_crypto "^[[:space:]]*random-crypto" contained containedin=gg_r_construct_random_crypto
    syn match gg_h_clause_random_crypto " length \@=" contained containedin=gg_r_construct_random_crypto
    syn match gg_h_clause_output_random_crypto " to \@=" contained containedin=gg_r_construct_random_crypto
    hi def link gg_h_clause_random_crypto    golfClause
    hi def link gg_h_clause_output_random_crypto    golfClauseOutput
    hi def link gg_h_construct_random_crypto    golfConstruct
    hi def link gg_h_print_inline_random_crypto    golfConstruct
syn region gg_r_construct_random_string start="^[[:space:]]*random-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_random_string,gg_r_inline_random_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_random_string,gg_r_inline_random_string,gg_r_at
    syn match gg_h_construct_random_string "^[[:space:]]*random-string" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " binary \@=" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " binary,\@=" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " binary$" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " length \@=" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " number \@=" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " number,\@=" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_random_string " number$" contained containedin=gg_r_construct_random_string
    syn match gg_h_clause_output_random_string " to \@=" contained containedin=gg_r_construct_random_string
    hi def link gg_h_clause_random_string    golfClause
    hi def link gg_h_clause_output_random_string    golfClauseOutput
    hi def link gg_h_construct_random_string    golfConstruct
    hi def link gg_h_print_inline_random_string    golfConstruct
syn region gg_r_construct_unlock_file start="^[[:space:]]*unlock-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_unlock_file,gg_r_inline_unlock_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_unlock_file,gg_r_inline_unlock_file,gg_r_at
    syn match gg_h_construct_unlock_file "^[[:space:]]*unlock-file" contained containedin=gg_r_construct_unlock_file
    syn match gg_h_clause_unlock_file " id \@=" contained containedin=gg_r_construct_unlock_file
    hi def link gg_h_clause_unlock_file    golfClause
    hi def link gg_h_clause_output_unlock_file    golfClauseOutput
    hi def link gg_h_construct_unlock_file    golfConstruct
    hi def link gg_h_print_inline_unlock_file    golfConstruct
syn region gg_r_construct_lock_file start="^[[:space:]]*lock-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_lock_file,gg_r_inline_lock_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_lock_file,gg_r_inline_lock_file,gg_r_at
    syn match gg_h_construct_lock_file "^[[:space:]]*lock-file" contained containedin=gg_r_construct_lock_file
    syn match gg_h_clause_output_lock_file " id \@=" contained containedin=gg_r_construct_lock_file
    syn match gg_h_clause_output_lock_file " status \@=" contained containedin=gg_r_construct_lock_file
    hi def link gg_h_clause_lock_file    golfClause
    hi def link gg_h_clause_output_lock_file    golfClauseOutput
    hi def link gg_h_construct_lock_file    golfConstruct
    hi def link gg_h_print_inline_lock_file    golfConstruct
syn region gg_r_construct_send_file start="^[[:space:]]*send-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_send_file,gg_r_inline_send_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_send_file,gg_r_inline_send_file,gg_r_at
    syn match gg_h_construct_send_file "^[[:space:]]*send-file" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " cache-control \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " content-type \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " custom \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " download \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " download,\@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " download$" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " etag \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " etag,\@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " etag$" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " file-name \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " headers \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " no-cache \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " no-cache,\@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " no-cache$" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " status-id \@=" contained containedin=gg_r_construct_send_file
    syn match gg_h_clause_send_file " status-text \@=" contained containedin=gg_r_construct_send_file
    hi def link gg_h_clause_send_file    golfClause
    hi def link gg_h_clause_output_send_file    golfClauseOutput
    hi def link gg_h_construct_send_file    golfConstruct
    hi def link gg_h_print_inline_send_file    golfConstruct
syn region gg_r_construct_exec_program start="^[[:space:]]*exec-program" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_exec_program,gg_r_inline_exec_program,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_exec_program,gg_r_inline_exec_program,gg_r_at
    syn match gg_h_construct_exec_program "^[[:space:]]*exec-program" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_exec_program " args \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_output_exec_program " error \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_exec_program " error-file \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_exec_program " input \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_exec_program " input-file \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_exec_program " input-length \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_output_exec_program " output \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_exec_program " output-file \@=" contained containedin=gg_r_construct_exec_program
    syn match gg_h_clause_output_exec_program " status \@=" contained containedin=gg_r_construct_exec_program
    hi def link gg_h_clause_exec_program    golfClause
    hi def link gg_h_clause_output_exec_program    golfClauseOutput
    hi def link gg_h_construct_exec_program    golfConstruct
    hi def link gg_h_print_inline_exec_program    golfConstruct
syn region gg_r_construct_out_header start="^[[:space:]]*out-header" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_out_header,gg_r_inline_out_header,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_out_header,gg_r_inline_out_header,gg_r_at
    syn match gg_h_construct_out_header "^[[:space:]]*out-header" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " cache-control \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " content-type \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " custom \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " default \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " default,\@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " default$" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " download \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " download,\@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " download$" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " etag \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " etag,\@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " etag$" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " file-name \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " no-cache \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " no-cache,\@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " no-cache$" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " status-id \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " status-text \@=" contained containedin=gg_r_construct_out_header
    syn match gg_h_clause_out_header " use \@=" contained containedin=gg_r_construct_out_header
    hi def link gg_h_clause_out_header    golfClause
    hi def link gg_h_clause_output_out_header    golfClauseOutput
    hi def link gg_h_construct_out_header    golfConstruct
    hi def link gg_h_print_inline_out_header    golfConstruct
syn region gg_r_construct_silent_header start="^[[:space:]]*silent-header" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_silent_header,gg_r_inline_silent_header,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_silent_header,gg_r_inline_silent_header,gg_r_at
    syn match gg_h_construct_silent_header "^[[:space:]]*silent-header" contained containedin=gg_r_construct_silent_header
    hi def link gg_h_clause_silent_header    golfClause
    hi def link gg_h_clause_output_silent_header    golfClauseOutput
    hi def link gg_h_construct_silent_header    golfConstruct
    hi def link gg_h_print_inline_silent_header    golfConstruct
syn region gg_r_construct_exit_status start="^[[:space:]]*exit-status" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_exit_status,gg_r_inline_exit_status,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_exit_status,gg_r_inline_exit_status,gg_r_at
    syn match gg_h_construct_exit_status "^[[:space:]]*exit-status" contained containedin=gg_r_construct_exit_status
    hi def link gg_h_clause_exit_status    golfClause
    hi def link gg_h_clause_output_exit_status    golfClauseOutput
    hi def link gg_h_construct_exit_status    golfConstruct
    hi def link gg_h_print_inline_exit_status    golfConstruct
syn region gg_r_construct_report_error start="^[[:space:]]*report-error" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_report_error,gg_r_inline_report_error,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_report_error,gg_r_inline_report_error,gg_r_at
    syn match gg_h_construct_report_error "^[[:space:]]*report-error" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " , \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " ,,\@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " ,$" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to-error \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to-error,\@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to-error$" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " url-encode \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " url-encode,\@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " url-encode$" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " web-encode \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " web-encode,\@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " web-encode$" contained containedin=gg_r_construct_report_error
    hi def link gg_h_clause_report_error    golfClause
    hi def link gg_h_clause_output_report_error    golfClauseOutput
    hi def link gg_h_construct_report_error    golfConstruct
    hi def link gg_h_print_inline_report_error    golfConstruct
syn region gg_r_construct_print_format start="^[[:space:]]*print-format" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_print_format,gg_r_inline_print_format,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_print_format,gg_r_inline_print_format,gg_r_at
    syn match gg_h_construct_print_format "^[[:space:]]*print-format" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " , \@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " ,,\@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " ,$" contained containedin=gg_r_construct_print_format
    syn match gg_h_print_inline_print_format " ,\(>>\)\@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_print_inline_print_format " , \@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_clause_print_format " to \@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_print_inline_print_format " to \@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_clause_print_format " to-error \@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " to-error,\@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " to-error$" contained containedin=gg_r_construct_print_format
    syn match gg_h_print_inline_print_format " to-error\(>>\)\@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_print_inline_print_format " to-error \@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_clause_print_format " url-encode \@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " url-encode,\@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " url-encode$" contained containedin=gg_r_construct_print_format
    syn match gg_h_print_inline_print_format " url-encode\(>>\)\@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_print_inline_print_format " url-encode \@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_clause_print_format " web-encode \@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " web-encode,\@=" contained containedin=gg_r_construct_print_format
    syn match gg_h_clause_print_format " web-encode$" contained containedin=gg_r_construct_print_format
    syn match gg_h_print_inline_print_format " web-encode\(>>\)\@=" contained containedin=gg_r_inline_print_format
    syn match gg_h_print_inline_print_format " web-encode \@=" contained containedin=gg_r_inline_print_format
    syn region gg_r_inline_print_format start="<<[[:space:]]*print-format \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_print_format '<<[[:space:]]*print-format \@=' contained containedin=gg_r_inline_print_format
    syn match gg_h_print_inline_print_format '>>' contained containedin=gg_r_inline_print_format
    hi def link gg_h_clause_print_format    golfClause
    hi def link gg_h_clause_output_print_format    golfClauseOutput
    hi def link gg_h_construct_print_format    golfConstruct
    hi def link gg_h_print_inline_print_format    golfConstruct
syn region gg_r_construct_print_out start="^[[:space:]]*print-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_print_out,gg_r_inline_print_out,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_print_out,gg_r_inline_print_out,gg_r_at
    syn match gg_h_construct_print_out "^[[:space:]]*print-out" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " length \@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_print_inline_print_out " length \@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_clause_print_out " new-line \@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " new-line,\@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " new-line$" contained containedin=gg_r_construct_print_out
    syn match gg_h_print_inline_print_out " new-line\(>>\)\@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_print_inline_print_out " new-line \@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_clause_print_out " source-file \@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " source-file,\@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " source-file$" contained containedin=gg_r_construct_print_out
    syn match gg_h_print_inline_print_out " source-file\(>>\)\@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_print_inline_print_out " source-file \@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_clause_print_out " source-line \@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " source-line,\@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " source-line$" contained containedin=gg_r_construct_print_out
    syn match gg_h_print_inline_print_out " source-line\(>>\)\@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_print_inline_print_out " source-line \@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_clause_print_out " url-encode \@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " url-encode,\@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " url-encode$" contained containedin=gg_r_construct_print_out
    syn match gg_h_print_inline_print_out " url-encode\(>>\)\@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_print_inline_print_out " url-encode \@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_clause_print_out " web-encode \@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " web-encode,\@=" contained containedin=gg_r_construct_print_out
    syn match gg_h_clause_print_out " web-encode$" contained containedin=gg_r_construct_print_out
    syn match gg_h_print_inline_print_out " web-encode\(>>\)\@=" contained containedin=gg_r_inline_print_out
    syn match gg_h_print_inline_print_out " web-encode \@=" contained containedin=gg_r_inline_print_out
    syn region gg_r_inline_print_out start="<<[[:space:]]*print-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_print_out '<<[[:space:]]*print-out \@=' contained containedin=gg_r_inline_print_out
    syn match gg_h_print_inline_print_out '>>' contained containedin=gg_r_inline_print_out
    hi def link gg_h_clause_print_out    golfClause
    hi def link gg_h_clause_output_print_out    golfClauseOutput
    hi def link gg_h_construct_print_out    golfConstruct
    hi def link gg_h_print_inline_print_out    golfConstruct
syn region gg_r_construct_p_path start="^[[:space:]]*p-path" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_path,gg_r_inline_p_path,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_path,gg_r_inline_p_path,gg_r_at
    syn match gg_h_construct_p_path "^[[:space:]]*p-path" contained containedin=gg_r_construct_p_path
    syn match gg_h_clause_p_path " new-line \@=" contained containedin=gg_r_construct_p_path
    syn match gg_h_clause_p_path " new-line,\@=" contained containedin=gg_r_construct_p_path
    syn match gg_h_clause_p_path " new-line$" contained containedin=gg_r_construct_p_path
    syn match gg_h_print_inline_p_path " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_path
    syn match gg_h_print_inline_p_path " new-line \@=" contained containedin=gg_r_inline_p_path
    syn region gg_r_inline_p_path start="<<[[:space:]]*p-path \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_path '<<[[:space:]]*p-path \@=' contained containedin=gg_r_inline_p_path
    syn match gg_h_print_inline_p_path '>>' contained containedin=gg_r_inline_p_path
    hi def link gg_h_clause_p_path    golfClause
    hi def link gg_h_clause_output_p_path    golfClauseOutput
    hi def link gg_h_construct_p_path    golfConstruct
    hi def link gg_h_print_inline_p_path    golfConstruct
syn region gg_r_construct_print_path start="^[[:space:]]*print-path" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_print_path,gg_r_inline_print_path,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_print_path,gg_r_inline_print_path,gg_r_at
    syn match gg_h_construct_print_path "^[[:space:]]*print-path" contained containedin=gg_r_construct_print_path
    syn match gg_h_clause_print_path " new-line \@=" contained containedin=gg_r_construct_print_path
    syn match gg_h_clause_print_path " new-line,\@=" contained containedin=gg_r_construct_print_path
    syn match gg_h_clause_print_path " new-line$" contained containedin=gg_r_construct_print_path
    syn match gg_h_print_inline_print_path " new-line\(>>\)\@=" contained containedin=gg_r_inline_print_path
    syn match gg_h_print_inline_print_path " new-line \@=" contained containedin=gg_r_inline_print_path
    syn region gg_r_inline_print_path start="<<[[:space:]]*print-path \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_print_path '<<[[:space:]]*print-path \@=' contained containedin=gg_r_inline_print_path
    syn match gg_h_print_inline_print_path '>>' contained containedin=gg_r_inline_print_path
    hi def link gg_h_clause_print_path    golfClause
    hi def link gg_h_clause_output_print_path    golfClauseOutput
    hi def link gg_h_construct_print_path    golfConstruct
    hi def link gg_h_print_inline_print_path    golfConstruct
syn region gg_r_construct_pf_out start="^[[:space:]]*pf-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pf_out,gg_r_inline_pf_out,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pf_out,gg_r_inline_pf_out,gg_r_at
    syn match gg_h_construct_pf_out "^[[:space:]]*pf-out" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " , \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " ,,\@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " ,$" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " ,\(>>\)\@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out " , \@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_clause_pf_out " to \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " to \@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_clause_pf_out " to-error \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " to-error,\@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " to-error$" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " to-error\(>>\)\@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out " to-error \@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_clause_pf_out " url-encode \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " url-encode,\@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " url-encode$" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " url-encode\(>>\)\@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out " url-encode \@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_clause_pf_out " web-encode \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " web-encode,\@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " web-encode$" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " web-encode\(>>\)\@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out " web-encode \@=" contained containedin=gg_r_inline_pf_out
    syn region gg_r_inline_pf_out start="<<[[:space:]]*pf-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_pf_out '<<[[:space:]]*pf-out \@=' contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out '>>' contained containedin=gg_r_inline_pf_out
    hi def link gg_h_clause_pf_out    golfClause
    hi def link gg_h_clause_output_pf_out    golfClauseOutput
    hi def link gg_h_construct_pf_out    golfConstruct
    hi def link gg_h_print_inline_pf_out    golfConstruct
syn region gg_r_construct_pf_url start="^[[:space:]]*pf-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pf_url,gg_r_inline_pf_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pf_url,gg_r_inline_pf_url,gg_r_at
    syn match gg_h_construct_pf_url "^[[:space:]]*pf-url" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " , \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " ,,\@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " ,$" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " ,\(>>\)\@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url " , \@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_clause_pf_url " to \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " to \@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_clause_pf_url " to-error \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " to-error,\@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " to-error$" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " to-error\(>>\)\@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url " to-error \@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_clause_pf_url " url-encode \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " url-encode,\@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " url-encode$" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " url-encode\(>>\)\@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url " url-encode \@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_clause_pf_url " web-encode \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " web-encode,\@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " web-encode$" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " web-encode\(>>\)\@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url " web-encode \@=" contained containedin=gg_r_inline_pf_url
    syn region gg_r_inline_pf_url start="<<[[:space:]]*pf-url \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_pf_url '<<[[:space:]]*pf-url \@=' contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url '>>' contained containedin=gg_r_inline_pf_url
    hi def link gg_h_clause_pf_url    golfClause
    hi def link gg_h_clause_output_pf_url    golfClauseOutput
    hi def link gg_h_construct_pf_url    golfConstruct
    hi def link gg_h_print_inline_pf_url    golfConstruct
syn region gg_r_construct_pf_web start="^[[:space:]]*pf-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pf_web,gg_r_inline_pf_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pf_web,gg_r_inline_pf_web,gg_r_at
    syn match gg_h_construct_pf_web "^[[:space:]]*pf-web" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " , \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " ,,\@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " ,$" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " ,\(>>\)\@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web " , \@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_clause_pf_web " to \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " to \@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_clause_pf_web " to-error \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " to-error,\@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " to-error$" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " to-error\(>>\)\@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web " to-error \@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_clause_pf_web " url-encode \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " url-encode,\@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " url-encode$" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " url-encode\(>>\)\@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web " url-encode \@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_clause_pf_web " web-encode \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " web-encode,\@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " web-encode$" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " web-encode\(>>\)\@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web " web-encode \@=" contained containedin=gg_r_inline_pf_web
    syn region gg_r_inline_pf_web start="<<[[:space:]]*pf-web \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_pf_web '<<[[:space:]]*pf-web \@=' contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web '>>' contained containedin=gg_r_inline_pf_web
    hi def link gg_h_clause_pf_web    golfClause
    hi def link gg_h_clause_output_pf_web    golfClauseOutput
    hi def link gg_h_construct_pf_web    golfConstruct
    hi def link gg_h_print_inline_pf_web    golfConstruct
syn region gg_r_construct_p_web start="^[[:space:]]*p-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_web,gg_r_inline_p_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_web,gg_r_inline_p_web,gg_r_at
    syn match gg_h_construct_p_web "^[[:space:]]*p-web" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " length \@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_print_inline_p_web " length \@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_clause_p_web " new-line \@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " new-line,\@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " new-line$" contained containedin=gg_r_construct_p_web
    syn match gg_h_print_inline_p_web " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web " new-line \@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_clause_p_web " source-file \@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " source-file,\@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " source-file$" contained containedin=gg_r_construct_p_web
    syn match gg_h_print_inline_p_web " source-file\(>>\)\@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web " source-file \@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_clause_p_web " source-line \@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " source-line,\@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " source-line$" contained containedin=gg_r_construct_p_web
    syn match gg_h_print_inline_p_web " source-line\(>>\)\@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web " source-line \@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_clause_p_web " url-encode \@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " url-encode,\@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " url-encode$" contained containedin=gg_r_construct_p_web
    syn match gg_h_print_inline_p_web " url-encode\(>>\)\@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web " url-encode \@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_clause_p_web " web-encode \@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " web-encode,\@=" contained containedin=gg_r_construct_p_web
    syn match gg_h_clause_p_web " web-encode$" contained containedin=gg_r_construct_p_web
    syn match gg_h_print_inline_p_web " web-encode\(>>\)\@=" contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web " web-encode \@=" contained containedin=gg_r_inline_p_web
    syn region gg_r_inline_p_web start="<<[[:space:]]*p-web \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_web '<<[[:space:]]*p-web \@=' contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web '>>' contained containedin=gg_r_inline_p_web
    hi def link gg_h_clause_p_web    golfClause
    hi def link gg_h_clause_output_p_web    golfClauseOutput
    hi def link gg_h_construct_p_web    golfConstruct
    hi def link gg_h_print_inline_p_web    golfConstruct
syn region gg_r_construct_p_url start="^[[:space:]]*p-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_url,gg_r_inline_p_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_url,gg_r_inline_p_url,gg_r_at
    syn match gg_h_construct_p_url "^[[:space:]]*p-url" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " length \@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_print_inline_p_url " length \@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_clause_p_url " new-line \@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " new-line,\@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " new-line$" contained containedin=gg_r_construct_p_url
    syn match gg_h_print_inline_p_url " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url " new-line \@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_clause_p_url " source-file \@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " source-file,\@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " source-file$" contained containedin=gg_r_construct_p_url
    syn match gg_h_print_inline_p_url " source-file\(>>\)\@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url " source-file \@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_clause_p_url " source-line \@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " source-line,\@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " source-line$" contained containedin=gg_r_construct_p_url
    syn match gg_h_print_inline_p_url " source-line\(>>\)\@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url " source-line \@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_clause_p_url " url-encode \@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " url-encode,\@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " url-encode$" contained containedin=gg_r_construct_p_url
    syn match gg_h_print_inline_p_url " url-encode\(>>\)\@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url " url-encode \@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_clause_p_url " web-encode \@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " web-encode,\@=" contained containedin=gg_r_construct_p_url
    syn match gg_h_clause_p_url " web-encode$" contained containedin=gg_r_construct_p_url
    syn match gg_h_print_inline_p_url " web-encode\(>>\)\@=" contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url " web-encode \@=" contained containedin=gg_r_inline_p_url
    syn region gg_r_inline_p_url start="<<[[:space:]]*p-url \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_url '<<[[:space:]]*p-url \@=' contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url '>>' contained containedin=gg_r_inline_p_url
    hi def link gg_h_clause_p_url    golfClause
    hi def link gg_h_clause_output_p_url    golfClauseOutput
    hi def link gg_h_construct_p_url    golfConstruct
    hi def link gg_h_print_inline_p_url    golfConstruct
syn region gg_r_construct_p_num start="^[[:space:]]*p-num" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_num,gg_r_inline_p_num,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_num,gg_r_inline_p_num,gg_r_at
    syn match gg_h_construct_p_num "^[[:space:]]*p-num" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " length \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " length \@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_clause_p_num " new-line \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " new-line,\@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " new-line$" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num " new-line \@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_clause_p_num " source-file \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " source-file,\@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " source-file$" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " source-file\(>>\)\@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num " source-file \@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_clause_p_num " source-line \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " source-line,\@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " source-line$" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " source-line\(>>\)\@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num " source-line \@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_clause_p_num " url-encode \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " url-encode,\@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " url-encode$" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " url-encode\(>>\)\@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num " url-encode \@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_clause_p_num " web-encode \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " web-encode,\@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " web-encode$" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " web-encode\(>>\)\@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num " web-encode \@=" contained containedin=gg_r_inline_p_num
    syn region gg_r_inline_p_num start="<<[[:space:]]*p-num \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_num '<<[[:space:]]*p-num \@=' contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num '>>' contained containedin=gg_r_inline_p_num
    hi def link gg_h_clause_p_num    golfClause
    hi def link gg_h_clause_output_p_num    golfClauseOutput
    hi def link gg_h_construct_p_num    golfConstruct
    hi def link gg_h_print_inline_p_num    golfConstruct
syn region gg_r_construct_p_source_file start="^[[:space:]]*p-source-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_source_file,gg_r_inline_p_source_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_source_file,gg_r_inline_p_source_file,gg_r_at
    syn match gg_h_construct_p_source_file "^[[:space:]]*p-source-file" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " length \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " length \@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_clause_p_source_file " new-line \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " new-line,\@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " new-line$" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file " new-line \@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_clause_p_source_file " source-file \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " source-file,\@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " source-file$" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " source-file\(>>\)\@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file " source-file \@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_clause_p_source_file " source-line \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " source-line,\@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " source-line$" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " source-line\(>>\)\@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file " source-line \@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_clause_p_source_file " url-encode \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " url-encode,\@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " url-encode$" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " url-encode\(>>\)\@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file " url-encode \@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_clause_p_source_file " web-encode \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " web-encode,\@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " web-encode$" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " web-encode\(>>\)\@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file " web-encode \@=" contained containedin=gg_r_inline_p_source_file
    syn region gg_r_inline_p_source_file start="<<[[:space:]]*p-source-file" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_source_file '<<[[:space:]]*p-source-file' contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file '>>' contained containedin=gg_r_inline_p_source_file
    hi def link gg_h_clause_p_source_file    golfClause
    hi def link gg_h_clause_output_p_source_file    golfClauseOutput
    hi def link gg_h_construct_p_source_file    golfConstruct
    hi def link gg_h_print_inline_p_source_file    golfConstruct
syn region gg_r_construct_p_out start="^[[:space:]]*p-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_out,gg_r_inline_p_out,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_out,gg_r_inline_p_out,gg_r_at
    syn match gg_h_construct_p_out "^[[:space:]]*p-out" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " length \@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_print_inline_p_out " length \@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_clause_p_out " new-line \@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " new-line,\@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " new-line$" contained containedin=gg_r_construct_p_out
    syn match gg_h_print_inline_p_out " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out " new-line \@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_clause_p_out " source-file \@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " source-file,\@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " source-file$" contained containedin=gg_r_construct_p_out
    syn match gg_h_print_inline_p_out " source-file\(>>\)\@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out " source-file \@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_clause_p_out " source-line \@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " source-line,\@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " source-line$" contained containedin=gg_r_construct_p_out
    syn match gg_h_print_inline_p_out " source-line\(>>\)\@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out " source-line \@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_clause_p_out " url-encode \@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " url-encode,\@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " url-encode$" contained containedin=gg_r_construct_p_out
    syn match gg_h_print_inline_p_out " url-encode\(>>\)\@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out " url-encode \@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_clause_p_out " web-encode \@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " web-encode,\@=" contained containedin=gg_r_construct_p_out
    syn match gg_h_clause_p_out " web-encode$" contained containedin=gg_r_construct_p_out
    syn match gg_h_print_inline_p_out " web-encode\(>>\)\@=" contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out " web-encode \@=" contained containedin=gg_r_inline_p_out
    syn region gg_r_inline_p_out start="<<[[:space:]]*p-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_out '<<[[:space:]]*p-out \@=' contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out '>>' contained containedin=gg_r_inline_p_out
    hi def link gg_h_clause_p_out    golfClause
    hi def link gg_h_clause_output_p_out    golfClauseOutput
    hi def link gg_h_construct_p_out    golfConstruct
    hi def link gg_h_print_inline_p_out    golfConstruct
syn region gg_r_construct_p_source_line start="^[[:space:]]*p-source-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_source_line,gg_r_inline_p_source_line,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_source_line,gg_r_inline_p_source_line,gg_r_at
    syn match gg_h_construct_p_source_line "^[[:space:]]*p-source-line" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " length \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " length \@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_clause_p_source_line " new-line \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " new-line,\@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " new-line$" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line " new-line \@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_clause_p_source_line " source-file \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " source-file,\@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " source-file$" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " source-file\(>>\)\@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line " source-file \@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_clause_p_source_line " source-line \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " source-line,\@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " source-line$" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " source-line\(>>\)\@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line " source-line \@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_clause_p_source_line " url-encode \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " url-encode,\@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " url-encode$" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " url-encode\(>>\)\@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line " url-encode \@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_clause_p_source_line " web-encode \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " web-encode,\@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " web-encode$" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " web-encode\(>>\)\@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line " web-encode \@=" contained containedin=gg_r_inline_p_source_line
    syn region gg_r_inline_p_source_line start="<<[[:space:]]*p-source-line" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_source_line '<<[[:space:]]*p-source-line' contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line '>>' contained containedin=gg_r_inline_p_source_line
    hi def link gg_h_clause_p_source_line    golfClause
    hi def link gg_h_clause_output_p_source_line    golfClauseOutput
    hi def link gg_h_construct_p_source_line    golfConstruct
    hi def link gg_h_print_inline_p_source_line    golfConstruct
syn region gg_r_construct_run_query start="^[[:space:]]*run-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_run_query,gg_r_inline_run_query,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_run_query,gg_r_inline_run_query,gg_r_at
    syn match gg_h_construct_run_query "^[[:space:]]*run-query" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_output_run_query " affected-rows \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " @ \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " @,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " @$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " : \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " :,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " :$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_output_run_query " error \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_output_run_query " error-text \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " input \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " noencode \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " noencode,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " noencode$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " no-loop \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " no-loop,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " no-loop$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " on-error-continue \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " on-error-continue,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " on-error-continue$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " on-error-exit \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " on-error-exit,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " on-error-exit$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_output_run_query " output \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_output_run_query " row-count \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " url-encode \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " url-encode,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " url-encode$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " web-encode \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " web-encode,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " web-encode$" contained containedin=gg_r_construct_run_query
    hi def link gg_h_clause_run_query    golfClause
    hi def link gg_h_clause_output_run_query    golfClauseOutput
    hi def link gg_h_construct_run_query    golfConstruct
    hi def link gg_h_print_inline_run_query    golfConstruct
syn region gg_r_construct_run_prepared_query start="^[[:space:]]*run-prepared-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_run_prepared_query,gg_r_inline_run_prepared_query,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_run_prepared_query,gg_r_inline_run_prepared_query,gg_r_at
    syn match gg_h_construct_run_prepared_query "^[[:space:]]*run-prepared-query" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_output_run_prepared_query " affected-rows \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " @ \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " @,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " @$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " : \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " :,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " :$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_output_run_prepared_query " error \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_output_run_prepared_query " error-text \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " input \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " noencode \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " noencode,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " noencode$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " no-loop \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " no-loop,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " no-loop$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " on-error-continue \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " on-error-continue,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " on-error-continue$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " on-error-exit \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " on-error-exit,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " on-error-exit$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_output_run_prepared_query " output \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_output_run_prepared_query " row-count \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " url-encode \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " url-encode,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " url-encode$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " web-encode \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " web-encode,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " web-encode$" contained containedin=gg_r_construct_run_prepared_query
    hi def link gg_h_clause_run_prepared_query    golfClause
    hi def link gg_h_clause_output_run_prepared_query    golfClauseOutput
    hi def link gg_h_construct_run_prepared_query    golfConstruct
    hi def link gg_h_print_inline_run_prepared_query    golfConstruct
syn region gg_r_construct_current_row start="^[[:space:]]*current-row" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_current_row,gg_r_inline_current_row,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_current_row,gg_r_inline_current_row,gg_r_at
    syn match gg_h_construct_current_row "^[[:space:]]*current-row" contained containedin=gg_r_construct_current_row
    syn match gg_h_clause_current_row " to \@=" contained containedin=gg_r_construct_current_row
    syn match gg_h_print_inline_current_row " to \@=" contained containedin=gg_r_inline_current_row
    syn region gg_r_inline_current_row start="<<[[:space:]]*current-row" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_current_row '<<[[:space:]]*current-row' contained containedin=gg_r_inline_current_row
    syn match gg_h_print_inline_current_row '>>' contained containedin=gg_r_inline_current_row
    hi def link gg_h_clause_current_row    golfClause
    hi def link gg_h_clause_output_current_row    golfClauseOutput
    hi def link gg_h_construct_current_row    golfConstruct
    hi def link gg_h_print_inline_current_row    golfConstruct
syn region gg_r_construct_begin_transaction start="^[[:space:]]*begin-transaction" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_begin_transaction,gg_r_inline_begin_transaction,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_begin_transaction,gg_r_inline_begin_transaction,gg_r_at
    syn match gg_h_construct_begin_transaction "^[[:space:]]*begin-transaction" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " @ \@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " @,\@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " @$" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_output_begin_transaction " error \@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_output_begin_transaction " error-text \@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " on-error-continue \@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " on-error-continue,\@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " on-error-continue$" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " on-error-exit \@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " on-error-exit,\@=" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " on-error-exit$" contained containedin=gg_r_construct_begin_transaction
    syn match gg_h_clause_begin_transaction " options \@=" contained containedin=gg_r_construct_begin_transaction
    hi def link gg_h_clause_begin_transaction    golfClause
    hi def link gg_h_clause_output_begin_transaction    golfClauseOutput
    hi def link gg_h_construct_begin_transaction    golfConstruct
    hi def link gg_h_print_inline_begin_transaction    golfConstruct
syn region gg_r_construct_commit_transaction start="^[[:space:]]*commit-transaction" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_commit_transaction,gg_r_inline_commit_transaction,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_commit_transaction,gg_r_inline_commit_transaction,gg_r_at
    syn match gg_h_construct_commit_transaction "^[[:space:]]*commit-transaction" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " @ \@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " @,\@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " @$" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_output_commit_transaction " error \@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_output_commit_transaction " error-text \@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " on-error-continue \@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " on-error-continue,\@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " on-error-continue$" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " on-error-exit \@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " on-error-exit,\@=" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " on-error-exit$" contained containedin=gg_r_construct_commit_transaction
    syn match gg_h_clause_commit_transaction " options \@=" contained containedin=gg_r_construct_commit_transaction
    hi def link gg_h_clause_commit_transaction    golfClause
    hi def link gg_h_clause_output_commit_transaction    golfClauseOutput
    hi def link gg_h_construct_commit_transaction    golfConstruct
    hi def link gg_h_print_inline_commit_transaction    golfConstruct
syn region gg_r_construct_db_error start="^[[:space:]]*db-error" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_db_error,gg_r_inline_db_error,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_db_error,gg_r_inline_db_error,gg_r_at
    syn match gg_h_construct_db_error "^[[:space:]]*db-error" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " @ \@=" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " @,\@=" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " @$" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " continue \@=" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " continue,\@=" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " continue$" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " exit \@=" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " exit,\@=" contained containedin=gg_r_construct_db_error
    syn match gg_h_clause_db_error " exit$" contained containedin=gg_r_construct_db_error
    hi def link gg_h_clause_db_error    golfClause
    hi def link gg_h_clause_output_db_error    golfClauseOutput
    hi def link gg_h_construct_db_error    golfConstruct
    hi def link gg_h_print_inline_db_error    golfConstruct
syn region gg_r_construct_rollback_transaction start="^[[:space:]]*rollback-transaction" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rollback_transaction,gg_r_inline_rollback_transaction,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rollback_transaction,gg_r_inline_rollback_transaction,gg_r_at
    syn match gg_h_construct_rollback_transaction "^[[:space:]]*rollback-transaction" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " @ \@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " @,\@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " @$" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_output_rollback_transaction " error \@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_output_rollback_transaction " error-text \@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " on-error-continue \@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " on-error-continue,\@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " on-error-continue$" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " on-error-exit \@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " on-error-exit,\@=" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " on-error-exit$" contained containedin=gg_r_construct_rollback_transaction
    syn match gg_h_clause_rollback_transaction " options \@=" contained containedin=gg_r_construct_rollback_transaction
    hi def link gg_h_clause_rollback_transaction    golfClause
    hi def link gg_h_clause_output_rollback_transaction    golfClauseOutput
    hi def link gg_h_construct_rollback_transaction    golfConstruct
    hi def link gg_h_print_inline_rollback_transaction    golfConstruct
syn region gg_r_construct_end_loop start="^[[:space:]]*end-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_loop,gg_r_inline_end_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_loop,gg_r_inline_end_loop,gg_r_at
    syn match gg_h_construct_end_loop "^[[:space:]]*end-loop" contained containedin=gg_r_construct_end_loop
    hi def link gg_h_clause_end_loop    golfClause
    hi def link gg_h_clause_output_end_loop    golfClauseOutput
    hi def link gg_h_construct_end_loop    golfConstruct
    hi def link gg_h_print_inline_end_loop    golfConstruct
syn region gg_r_construct_break_loop start="^[[:space:]]*break-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_break_loop,gg_r_inline_break_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_break_loop,gg_r_inline_break_loop,gg_r_at
    syn match gg_h_construct_break_loop "^[[:space:]]*break-loop" contained containedin=gg_r_construct_break_loop
    hi def link gg_h_clause_break_loop    golfClause
    hi def link gg_h_clause_output_break_loop    golfClauseOutput
    hi def link gg_h_construct_break_loop    golfConstruct
    hi def link gg_h_print_inline_break_loop    golfConstruct
syn region gg_r_construct_continue_loop start="^[[:space:]]*continue-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_continue_loop,gg_r_inline_continue_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_continue_loop,gg_r_inline_continue_loop,gg_r_at
    syn match gg_h_construct_continue_loop "^[[:space:]]*continue-loop" contained containedin=gg_r_construct_continue_loop
    hi def link gg_h_clause_continue_loop    golfClause
    hi def link gg_h_clause_output_continue_loop    golfClauseOutput
    hi def link gg_h_construct_continue_loop    golfConstruct
    hi def link gg_h_print_inline_continue_loop    golfConstruct
syn region gg_r_construct_pause_program start="^[[:space:]]*pause-program" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pause_program,gg_r_inline_pause_program,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pause_program,gg_r_inline_pause_program,gg_r_at
    syn match gg_h_construct_pause_program "^[[:space:]]*pause-program" contained containedin=gg_r_construct_pause_program
    hi def link gg_h_clause_pause_program    golfClause
    hi def link gg_h_clause_output_pause_program    golfClauseOutput
    hi def link gg_h_construct_pause_program    golfConstruct
    hi def link gg_h_print_inline_pause_program    golfConstruct
syn region gg_r_construct_right_curly start="^[[:space:]]*}" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_right_curly,gg_r_inline_right_curly,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_right_curly,gg_r_inline_right_curly,gg_r_at
    syn match gg_h_construct_right_curly "^[[:space:]]*}" contained containedin=gg_r_construct_right_curly
    hi def link gg_h_clause_right_curly    golfClause
    hi def link gg_h_clause_output_right_curly    golfClauseOutput
    hi def link gg_h_construct_right_curly    golfConstruct
    hi def link gg_h_print_inline_right_curly    golfConstruct
syn region gg_r_construct_left_curly start="^[[:space:]]*{" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_left_curly,gg_r_inline_left_curly,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_left_curly,gg_r_inline_left_curly,gg_r_at
    syn match gg_h_construct_left_curly "^[[:space:]]*{" contained containedin=gg_r_construct_left_curly
    hi def link gg_h_clause_left_curly    golfClause
    hi def link gg_h_clause_output_left_curly    golfClauseOutput
    hi def link gg_h_construct_left_curly    golfConstruct
    hi def link gg_h_print_inline_left_curly    golfConstruct
syn region gg_r_construct_if_not_defined start="^[[:space:]]*if-not-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_if_not_defined,gg_r_inline_if_not_defined,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_if_not_defined,gg_r_inline_if_not_defined,gg_r_at
    syn match gg_h_construct_if_not_defined "^[[:space:]]*if-not-defined" contained containedin=gg_r_construct_if_not_defined
    syn match gg_h_clause_if_not_defined " equal \@=" contained containedin=gg_r_construct_if_not_defined
    syn match gg_h_clause_if_not_defined " greater-equal \@=" contained containedin=gg_r_construct_if_not_defined
    syn match gg_h_clause_if_not_defined " greater-than \@=" contained containedin=gg_r_construct_if_not_defined
    syn match gg_h_clause_if_not_defined " lesser-equal \@=" contained containedin=gg_r_construct_if_not_defined
    syn match gg_h_clause_if_not_defined " lesser-than \@=" contained containedin=gg_r_construct_if_not_defined
    syn match gg_h_clause_if_not_defined " not-equal \@=" contained containedin=gg_r_construct_if_not_defined
    hi def link gg_h_clause_if_not_defined    golfClause
    hi def link gg_h_clause_output_if_not_defined    golfClauseOutput
    hi def link gg_h_construct_if_not_defined    golfConstruct
    hi def link gg_h_print_inline_if_not_defined    golfConstruct
syn region gg_r_construct_if_defined start="^[[:space:]]*if-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_if_defined,gg_r_inline_if_defined,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_if_defined,gg_r_inline_if_defined,gg_r_at
    syn match gg_h_construct_if_defined "^[[:space:]]*if-defined" contained containedin=gg_r_construct_if_defined
    syn match gg_h_clause_if_defined " equal \@=" contained containedin=gg_r_construct_if_defined
    syn match gg_h_clause_if_defined " greater-equal \@=" contained containedin=gg_r_construct_if_defined
    syn match gg_h_clause_if_defined " greater-than \@=" contained containedin=gg_r_construct_if_defined
    syn match gg_h_clause_if_defined " lesser-equal \@=" contained containedin=gg_r_construct_if_defined
    syn match gg_h_clause_if_defined " lesser-than \@=" contained containedin=gg_r_construct_if_defined
    syn match gg_h_clause_if_defined " not-equal \@=" contained containedin=gg_r_construct_if_defined
    hi def link gg_h_clause_if_defined    golfClause
    hi def link gg_h_clause_output_if_defined    golfClauseOutput
    hi def link gg_h_construct_if_defined    golfConstruct
    hi def link gg_h_print_inline_if_defined    golfConstruct
syn region gg_r_construct_end_defined start="^[[:space:]]*end-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_defined,gg_r_inline_end_defined,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_defined,gg_r_inline_end_defined,gg_r_at
    syn match gg_h_construct_end_defined "^[[:space:]]*end-defined" contained containedin=gg_r_construct_end_defined
    hi def link gg_h_clause_end_defined    golfClause
    hi def link gg_h_clause_output_end_defined    golfClauseOutput
    hi def link gg_h_construct_end_defined    golfConstruct
    hi def link gg_h_print_inline_end_defined    golfConstruct
syn region gg_r_construct_start_loop start="^[[:space:]]*start-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_start_loop,gg_r_inline_start_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_start_loop,gg_r_inline_start_loop,gg_r_at
    syn match gg_h_construct_start_loop "^[[:space:]]*start-loop" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_start_loop " add \@=" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_start_loop " repeat \@=" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_start_loop " start-with \@=" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_output_start_loop " use \@=" contained containedin=gg_r_construct_start_loop
    hi def link gg_h_clause_start_loop    golfClause
    hi def link gg_h_clause_output_start_loop    golfClauseOutput
    hi def link gg_h_construct_start_loop    golfConstruct
    hi def link gg_h_print_inline_start_loop    golfConstruct
syn region gg_r_construct_end_query start="^[[:space:]]*end-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_query,gg_r_inline_end_query,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_query,gg_r_inline_end_query,gg_r_at
    syn match gg_h_construct_end_query "^[[:space:]]*end-query" contained containedin=gg_r_construct_end_query
    hi def link gg_h_clause_end_query    golfClause
    hi def link gg_h_clause_output_end_query    golfClauseOutput
    hi def link gg_h_construct_end_query    golfConstruct
    hi def link gg_h_print_inline_end_query    golfConstruct
