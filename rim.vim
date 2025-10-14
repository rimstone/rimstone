" SPDX-License-Identifier: Apache-2.0
" Copyright 2018-2025 Gliim LLC.
" Licensed under Apache License v2. See LICENSE file.
" On the web http://rimstone-lang.com/ - this file is part of RimStone framework.
" Language: RimStone
" Vim indent file
" Maintainer: Gliim LLC
so $VIMRUNTIME/syntax/c.vim
syntax sync minlines=10000
hi def link rimConstruct Statement
hi def link rimClause Identifier
hi def link rimClauseOutput Type
hi def link rim_h_other  Constant
hi def link Comment  SpecialKey
syn region rim_r_inline_dot start="<<[[:space:]]*[\.]" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_dot '<<[[:space:]]*[\.]' contained containedin=rim_r_inline_dot
    syn match rim_h_print_inline_dot '>>' contained containedin=rim_r_inline_dot
    hi def link rim_h_print_inline_dot    rimConstruct
syn region rim_r_excl start="^[[:space:]]*[!]" skip="\\[[:space:]]*$" end="$"  keepend
syn match rim_h_excl_begin '^[[:space:]]*[!]' contained containedin=rim_r_excl
hi def link rim_h_excl_begin    rimConstruct
syn match rim_h_dot '^[[:space:]]*\.'
syn region rim_r_at start="^[[:space:]]*[@]" skip="\\[[:space:]]*$" end="$"  keepend
syn match rim_h_at_begin '^[[:space:]]*[@]' contained containedin=rim_r_at
hi def link rim_h_at_begin    rimConstruct
hi def link rim_h_dot    rimConstruct
syn region rim_r_construct_end_read_line start="^[[:space:]]*end-read-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_read_line,rim_r_inline_end_read_line,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_read_line,rim_r_inline_end_read_line,rim_r_at
    syn match rim_h_construct_end_read_line "^[[:space:]]*end-read-line" contained containedin=rim_r_construct_end_read_line
    hi def link rim_h_clause_end_read_line    rimClause
    hi def link rim_h_clause_output_end_read_line    rimClauseOutput
    hi def link rim_h_construct_end_read_line    rimConstruct
    hi def link rim_h_print_inline_end_read_line    rimConstruct
syn region rim_r_construct_read_line start="^[[:space:]]*read-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_line,rim_r_inline_read_line,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_line,rim_r_inline_read_line,rim_r_at
    syn match rim_h_construct_read_line "^[[:space:]]*read-line" contained containedin=rim_r_construct_read_line
    syn match rim_h_clause_read_line " delimiter \@=" contained containedin=rim_r_construct_read_line
    syn match rim_h_clause_output_read_line " status \@=" contained containedin=rim_r_construct_read_line
    syn match rim_h_clause_output_read_line " to \@=" contained containedin=rim_r_construct_read_line
    hi def link rim_h_clause_read_line    rimClause
    hi def link rim_h_clause_output_read_line    rimClauseOutput
    hi def link rim_h_construct_read_line    rimConstruct
    hi def link rim_h_print_inline_read_line    rimConstruct
syn region rim_r_construct_double_left_par start="^[[:space:]]*((" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_double_left_par,rim_r_inline_double_left_par,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_double_left_par,rim_r_inline_double_left_par,rim_r_at
    syn match rim_h_construct_double_left_par "^[[:space:]]*((" contained containedin=rim_r_construct_double_left_par
    hi def link rim_h_clause_double_left_par    rimClause
    hi def link rim_h_clause_output_double_left_par    rimClauseOutput
    hi def link rim_h_construct_double_left_par    rimConstruct
    hi def link rim_h_print_inline_double_left_par    rimConstruct
syn region rim_r_construct_write_string start="^[[:space:]]*write-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_string,rim_r_inline_write_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_string,rim_r_inline_write_string,rim_r_at
    syn match rim_h_construct_write_string "^[[:space:]]*write-string" contained containedin=rim_r_construct_write_string
    hi def link rim_h_clause_write_string    rimClause
    hi def link rim_h_clause_output_write_string    rimClauseOutput
    hi def link rim_h_construct_write_string    rimConstruct
    hi def link rim_h_print_inline_write_string    rimConstruct
syn region rim_r_construct_double_right_par start="^[[:space:]]*))" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_double_right_par,rim_r_inline_double_right_par,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_double_right_par,rim_r_inline_double_right_par,rim_r_at
    syn match rim_h_construct_double_right_par "^[[:space:]]*))" contained containedin=rim_r_construct_double_right_par
    syn match rim_h_clause_double_right_par " notrim \@=" contained containedin=rim_r_construct_double_right_par
    syn match rim_h_clause_double_right_par " notrim,\@=" contained containedin=rim_r_construct_double_right_par
    syn match rim_h_clause_double_right_par " notrim$" contained containedin=rim_r_construct_double_right_par
    hi def link rim_h_clause_double_right_par    rimClause
    hi def link rim_h_clause_output_double_right_par    rimClauseOutput
    hi def link rim_h_construct_double_right_par    rimConstruct
    hi def link rim_h_print_inline_double_right_par    rimConstruct
syn region rim_r_construct_end_write_string start="^[[:space:]]*end-write-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_write_string,rim_r_inline_end_write_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_write_string,rim_r_inline_end_write_string,rim_r_at
    syn match rim_h_construct_end_write_string "^[[:space:]]*end-write-string" contained containedin=rim_r_construct_end_write_string
    syn match rim_h_clause_end_write_string " notrim \@=" contained containedin=rim_r_construct_end_write_string
    syn match rim_h_clause_end_write_string " notrim,\@=" contained containedin=rim_r_construct_end_write_string
    syn match rim_h_clause_end_write_string " notrim$" contained containedin=rim_r_construct_end_write_string
    hi def link rim_h_clause_end_write_string    rimClause
    hi def link rim_h_clause_output_end_write_string    rimClauseOutput
    hi def link rim_h_construct_end_write_string    rimConstruct
    hi def link rim_h_print_inline_end_write_string    rimConstruct
syn region rim_r_construct_open_file start="^[[:space:]]*open-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_open_file,rim_r_inline_open_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_open_file,rim_r_inline_open_file,rim_r_at
    syn match rim_h_construct_open_file "^[[:space:]]*open-file" contained containedin=rim_r_construct_open_file
    syn match rim_h_clause_output_open_file " file-id \@=" contained containedin=rim_r_construct_open_file
    syn match rim_h_clause_open_file " new-truncate \@=" contained containedin=rim_r_construct_open_file
    syn match rim_h_clause_open_file " new-truncate,\@=" contained containedin=rim_r_construct_open_file
    syn match rim_h_clause_open_file " new-truncate$" contained containedin=rim_r_construct_open_file
    syn match rim_h_clause_output_open_file " status \@=" contained containedin=rim_r_construct_open_file
    hi def link rim_h_clause_open_file    rimClause
    hi def link rim_h_clause_output_open_file    rimClauseOutput
    hi def link rim_h_construct_open_file    rimConstruct
    hi def link rim_h_print_inline_open_file    rimConstruct
syn region rim_r_construct_close_file start="^[[:space:]]*close-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_close_file,rim_r_inline_close_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_close_file,rim_r_inline_close_file,rim_r_at
    syn match rim_h_construct_close_file "^[[:space:]]*close-file" contained containedin=rim_r_construct_close_file
    syn match rim_h_clause_close_file " file-id \@=" contained containedin=rim_r_construct_close_file
    syn match rim_h_clause_output_close_file " status \@=" contained containedin=rim_r_construct_close_file
    hi def link rim_h_clause_close_file    rimClause
    hi def link rim_h_clause_output_close_file    rimClauseOutput
    hi def link rim_h_construct_close_file    rimConstruct
    hi def link rim_h_print_inline_close_file    rimConstruct
syn region rim_r_construct_file_position start="^[[:space:]]*file-position" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_file_position,rim_r_inline_file_position,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_file_position,rim_r_inline_file_position,rim_r_at
    syn match rim_h_construct_file_position "^[[:space:]]*file-position" contained containedin=rim_r_construct_file_position
    syn match rim_h_clause_file_position " file-id \@=" contained containedin=rim_r_construct_file_position
    syn match rim_h_clause_output_file_position " get \@=" contained containedin=rim_r_construct_file_position
    syn match rim_h_clause_file_position " set \@=" contained containedin=rim_r_construct_file_position
    syn match rim_h_clause_output_file_position " status \@=" contained containedin=rim_r_construct_file_position
    hi def link rim_h_clause_file_position    rimClause
    hi def link rim_h_clause_output_file_position    rimClauseOutput
    hi def link rim_h_construct_file_position    rimConstruct
    hi def link rim_h_print_inline_file_position    rimConstruct
syn region rim_r_construct_exit_handler start="^[[:space:]]*exit-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_exit_handler,rim_r_inline_exit_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_exit_handler,rim_r_inline_exit_handler,rim_r_at
    syn match rim_h_construct_exit_handler "^[[:space:]]*exit-handler" contained containedin=rim_r_construct_exit_handler
    hi def link rim_h_clause_exit_handler    rimClause
    hi def link rim_h_clause_output_exit_handler    rimClauseOutput
    hi def link rim_h_construct_exit_handler    rimConstruct
    hi def link rim_h_print_inline_exit_handler    rimConstruct
syn region rim_r_construct_return_handler start="^[[:space:]]*return-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_return_handler,rim_r_inline_return_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_return_handler,rim_r_inline_return_handler,rim_r_at
    syn match rim_h_construct_return_handler "^[[:space:]]*return-handler" contained containedin=rim_r_construct_return_handler
    hi def link rim_h_clause_return_handler    rimClause
    hi def link rim_h_clause_output_return_handler    rimClauseOutput
    hi def link rim_h_construct_return_handler    rimConstruct
    hi def link rim_h_print_inline_return_handler    rimConstruct
syn region rim_r_construct_quit_process start="^[[:space:]]*quit-process" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_quit_process,rim_r_inline_quit_process,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_quit_process,rim_r_inline_quit_process,rim_r_at
    syn match rim_h_construct_quit_process "^[[:space:]]*quit-process" contained containedin=rim_r_construct_quit_process
    hi def link rim_h_clause_quit_process    rimClause
    hi def link rim_h_clause_output_quit_process    rimClauseOutput
    hi def link rim_h_construct_quit_process    rimConstruct
    hi def link rim_h_print_inline_quit_process    rimConstruct
syn region rim_r_construct_finish_output start="^[[:space:]]*finish-output" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_finish_output,rim_r_inline_finish_output,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_finish_output,rim_r_inline_finish_output,rim_r_at
    syn match rim_h_construct_finish_output "^[[:space:]]*finish-output" contained containedin=rim_r_construct_finish_output
    hi def link rim_h_clause_finish_output    rimClause
    hi def link rim_h_clause_output_finish_output    rimClauseOutput
    hi def link rim_h_construct_finish_output    rimConstruct
    hi def link rim_h_print_inline_finish_output    rimConstruct
syn region rim_r_construct_copy_file start="^[[:space:]]*copy-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_copy_file,rim_r_inline_copy_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_copy_file,rim_r_inline_copy_file,rim_r_at
    syn match rim_h_construct_copy_file "^[[:space:]]*copy-file" contained containedin=rim_r_construct_copy_file
    syn match rim_h_clause_output_copy_file " status \@=" contained containedin=rim_r_construct_copy_file
    syn match rim_h_clause_copy_file " to \@=" contained containedin=rim_r_construct_copy_file
    hi def link rim_h_clause_copy_file    rimClause
    hi def link rim_h_clause_output_copy_file    rimClauseOutput
    hi def link rim_h_construct_copy_file    rimConstruct
    hi def link rim_h_print_inline_copy_file    rimConstruct
syn region rim_r_construct_end_do_once start="^[[:space:]]*end-do-once" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_do_once,rim_r_inline_end_do_once,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_do_once,rim_r_inline_end_do_once,rim_r_at
    syn match rim_h_construct_end_do_once "^[[:space:]]*end-do-once" contained containedin=rim_r_construct_end_do_once
    hi def link rim_h_clause_end_do_once    rimClause
    hi def link rim_h_clause_output_end_do_once    rimClauseOutput
    hi def link rim_h_construct_end_do_once    rimConstruct
    hi def link rim_h_print_inline_end_do_once    rimConstruct
syn region rim_r_construct_do_once start="^[[:space:]]*do-once" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_do_once,rim_r_inline_do_once,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_do_once,rim_r_inline_do_once,rim_r_at
    syn match rim_h_construct_do_once "^[[:space:]]*do-once" contained containedin=rim_r_construct_do_once
    hi def link rim_h_clause_do_once    rimClause
    hi def link rim_h_clause_output_do_once    rimClauseOutput
    hi def link rim_h_construct_do_once    rimConstruct
    hi def link rim_h_print_inline_do_once    rimConstruct
syn region rim_r_construct_use_cursor start="^[[:space:]]*use-cursor" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_use_cursor,rim_r_inline_use_cursor,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_use_cursor,rim_r_inline_use_cursor,rim_r_at
    syn match rim_h_construct_use_cursor "^[[:space:]]*use-cursor" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " current \@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " current,\@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " current$" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " get-greater \@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " get-greater,\@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " get-greater$" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " get-lesser \@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " get-lesser,\@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " get-lesser$" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_output_use_cursor " key \@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_output_use_cursor " status \@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_use_cursor " update-value \@=" contained containedin=rim_r_construct_use_cursor
    syn match rim_h_clause_output_use_cursor " value \@=" contained containedin=rim_r_construct_use_cursor
    hi def link rim_h_clause_use_cursor    rimClause
    hi def link rim_h_clause_output_use_cursor    rimClauseOutput
    hi def link rim_h_construct_use_cursor    rimConstruct
    hi def link rim_h_print_inline_use_cursor    rimConstruct
syn region rim_r_construct_delete_tree start="^[[:space:]]*delete-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_tree,rim_r_inline_delete_tree,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_tree,rim_r_inline_delete_tree,rim_r_at
    syn match rim_h_construct_delete_tree "^[[:space:]]*delete-tree" contained containedin=rim_r_construct_delete_tree
    syn match rim_h_clause_delete_tree " key \@=" contained containedin=rim_r_construct_delete_tree
    syn match rim_h_clause_output_delete_tree " status \@=" contained containedin=rim_r_construct_delete_tree
    syn match rim_h_clause_output_delete_tree " value \@=" contained containedin=rim_r_construct_delete_tree
    hi def link rim_h_clause_delete_tree    rimClause
    hi def link rim_h_clause_output_delete_tree    rimClauseOutput
    hi def link rim_h_construct_delete_tree    rimConstruct
    hi def link rim_h_print_inline_delete_tree    rimConstruct
syn region rim_r_construct_read_tree start="^[[:space:]]*read-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_tree,rim_r_inline_read_tree,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_tree,rim_r_inline_read_tree,rim_r_at
    syn match rim_h_construct_read_tree "^[[:space:]]*read-tree" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " equal \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " greater \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " greater-equal \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_output_read_tree " key \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " lesser \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " lesser-equal \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " max-key \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " max-key,\@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " max-key$" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " min-key \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " min-key,\@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " min-key$" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_output_read_tree " new-cursor \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_output_read_tree " status \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_read_tree " update-value \@=" contained containedin=rim_r_construct_read_tree
    syn match rim_h_clause_output_read_tree " value \@=" contained containedin=rim_r_construct_read_tree
    hi def link rim_h_clause_read_tree    rimClause
    hi def link rim_h_clause_output_read_tree    rimClauseOutput
    hi def link rim_h_construct_read_tree    rimConstruct
    hi def link rim_h_print_inline_read_tree    rimConstruct
syn region rim_r_construct_write_tree start="^[[:space:]]*write-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_tree,rim_r_inline_write_tree,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_tree,rim_r_inline_write_tree,rim_r_at
    syn match rim_h_construct_write_tree "^[[:space:]]*write-tree" contained containedin=rim_r_construct_write_tree
    syn match rim_h_clause_write_tree " key \@=" contained containedin=rim_r_construct_write_tree
    syn match rim_h_clause_output_write_tree " new-cursor \@=" contained containedin=rim_r_construct_write_tree
    syn match rim_h_clause_output_write_tree " status \@=" contained containedin=rim_r_construct_write_tree
    syn match rim_h_clause_write_tree " value \@=" contained containedin=rim_r_construct_write_tree
    hi def link rim_h_clause_write_tree    rimClause
    hi def link rim_h_clause_output_write_tree    rimClauseOutput
    hi def link rim_h_construct_write_tree    rimConstruct
    hi def link rim_h_print_inline_write_tree    rimConstruct
syn region rim_r_construct_purge_tree start="^[[:space:]]*purge-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_purge_tree,rim_r_inline_purge_tree,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_purge_tree,rim_r_inline_purge_tree,rim_r_at
    syn match rim_h_construct_purge_tree "^[[:space:]]*purge-tree" contained containedin=rim_r_construct_purge_tree
    hi def link rim_h_clause_purge_tree    rimClause
    hi def link rim_h_clause_output_purge_tree    rimClauseOutput
    hi def link rim_h_construct_purge_tree    rimConstruct
    hi def link rim_h_print_inline_purge_tree    rimConstruct
syn region rim_r_construct_get_tree start="^[[:space:]]*get-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_tree,rim_r_inline_get_tree,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_tree,rim_r_inline_get_tree,rim_r_at
    syn match rim_h_construct_get_tree "^[[:space:]]*get-tree" contained containedin=rim_r_construct_get_tree
    syn match rim_h_clause_output_get_tree " count \@=" contained containedin=rim_r_construct_get_tree
    syn match rim_h_clause_output_get_tree " hops \@=" contained containedin=rim_r_construct_get_tree
    hi def link rim_h_clause_get_tree    rimClause
    hi def link rim_h_clause_output_get_tree    rimClauseOutput
    hi def link rim_h_construct_get_tree    rimConstruct
    hi def link rim_h_print_inline_get_tree    rimConstruct
syn region rim_r_construct_get_list start="^[[:space:]]*get-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_list,rim_r_inline_get_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_list,rim_r_inline_get_list,rim_r_at
    syn match rim_h_construct_get_list "^[[:space:]]*get-list" contained containedin=rim_r_construct_get_list
    syn match rim_h_clause_output_get_list " count \@=" contained containedin=rim_r_construct_get_list
    hi def link rim_h_clause_get_list    rimClause
    hi def link rim_h_clause_output_get_list    rimClauseOutput
    hi def link rim_h_construct_get_list    rimConstruct
    hi def link rim_h_print_inline_get_list    rimConstruct
syn region rim_r_construct_get_lifo start="^[[:space:]]*get-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_lifo,rim_r_inline_get_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_lifo,rim_r_inline_get_lifo,rim_r_at
    syn match rim_h_construct_get_lifo "^[[:space:]]*get-lifo" contained containedin=rim_r_construct_get_lifo
    syn match rim_h_clause_output_get_lifo " count \@=" contained containedin=rim_r_construct_get_lifo
    hi def link rim_h_clause_get_lifo    rimClause
    hi def link rim_h_clause_output_get_lifo    rimClauseOutput
    hi def link rim_h_construct_get_lifo    rimConstruct
    hi def link rim_h_print_inline_get_lifo    rimConstruct
syn region rim_r_construct_get_upload start="^[[:space:]]*get-upload" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_upload,rim_r_inline_get_upload,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_upload,rim_r_inline_get_upload,rim_r_at
    syn match rim_h_construct_get_upload "^[[:space:]]*get-upload" contained containedin=rim_r_construct_get_upload
    syn match rim_h_clause_output_get_upload " client-file \@=" contained containedin=rim_r_construct_get_upload
    syn match rim_h_clause_output_get_upload " extension \@=" contained containedin=rim_r_construct_get_upload
    syn match rim_h_clause_output_get_upload " local-file \@=" contained containedin=rim_r_construct_get_upload
    syn match rim_h_clause_output_get_upload " size \@=" contained containedin=rim_r_construct_get_upload
    syn match rim_h_clause_get_upload " status \@=" contained containedin=rim_r_construct_get_upload
    hi def link rim_h_clause_get_upload    rimClause
    hi def link rim_h_clause_output_get_upload    rimClauseOutput
    hi def link rim_h_construct_get_upload    rimConstruct
    hi def link rim_h_print_inline_get_upload    rimConstruct
syn region rim_r_construct_new_tree start="^[[:space:]]*new-tree" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_tree,rim_r_inline_new_tree,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_tree,rim_r_inline_new_tree,rim_r_at
    syn match rim_h_construct_new_tree "^[[:space:]]*new-tree" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " key-as \@=" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " process-scope \@=" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " process-scope,\@=" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " process-scope$" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " unsorted \@=" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " unsorted,\@=" contained containedin=rim_r_construct_new_tree
    syn match rim_h_clause_new_tree " unsorted$" contained containedin=rim_r_construct_new_tree
    hi def link rim_h_clause_new_tree    rimClause
    hi def link rim_h_clause_output_new_tree    rimClauseOutput
    hi def link rim_h_construct_new_tree    rimConstruct
    hi def link rim_h_print_inline_new_tree    rimConstruct
syn region rim_r_construct_xml_doc start="^[[:space:]]*xml-doc" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_xml_doc,rim_r_inline_xml_doc,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_xml_doc,rim_r_inline_xml_doc,rim_r_at
    syn match rim_h_construct_xml_doc "^[[:space:]]*xml-doc" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_xml_doc " delete \@=" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_output_xml_doc " error-char \@=" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_output_xml_doc " error-line \@=" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_output_xml_doc " error-text \@=" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_xml_doc " length \@=" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_output_xml_doc " status \@=" contained containedin=rim_r_construct_xml_doc
    syn match rim_h_clause_output_xml_doc " to \@=" contained containedin=rim_r_construct_xml_doc
    hi def link rim_h_clause_xml_doc    rimClause
    hi def link rim_h_clause_output_xml_doc    rimClauseOutput
    hi def link rim_h_construct_xml_doc    rimConstruct
    hi def link rim_h_print_inline_xml_doc    rimConstruct
syn region rim_r_construct_json_doc start="^[[:space:]]*json-doc" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_json_doc,rim_r_inline_json_doc,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_json_doc,rim_r_inline_json_doc,rim_r_at
    syn match rim_h_construct_json_doc "^[[:space:]]*json-doc" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " delete \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_output_json_doc " error-char \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_output_json_doc " error-line \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_output_json_doc " error-position \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_output_json_doc " error-text \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " length \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " noencode \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " noencode,\@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " noencode$" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " no-enum \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " no-enum,\@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_json_doc " no-enum$" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_output_json_doc " status \@=" contained containedin=rim_r_construct_json_doc
    syn match rim_h_clause_output_json_doc " to \@=" contained containedin=rim_r_construct_json_doc
    hi def link rim_h_clause_json_doc    rimClause
    hi def link rim_h_clause_output_json_doc    rimClauseOutput
    hi def link rim_h_construct_json_doc    rimConstruct
    hi def link rim_h_print_inline_json_doc    rimConstruct
syn region rim_r_construct_read_xml start="^[[:space:]]*read-xml" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_xml,rim_r_inline_read_xml,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_xml,rim_r_inline_read_xml,rim_r_at
    syn match rim_h_construct_read_xml "^[[:space:]]*read-xml" contained containedin=rim_r_construct_read_xml
    syn match rim_h_clause_output_read_xml " key \@=" contained containedin=rim_r_construct_read_xml
    syn match rim_h_clause_read_xml " next \@=" contained containedin=rim_r_construct_read_xml
    syn match rim_h_clause_read_xml " next,\@=" contained containedin=rim_r_construct_read_xml
    syn match rim_h_clause_read_xml " next$" contained containedin=rim_r_construct_read_xml
    syn match rim_h_clause_output_read_xml " status \@=" contained containedin=rim_r_construct_read_xml
    syn match rim_h_clause_output_read_xml " value \@=" contained containedin=rim_r_construct_read_xml
    hi def link rim_h_clause_read_xml    rimClause
    hi def link rim_h_clause_output_read_xml    rimClauseOutput
    hi def link rim_h_construct_read_xml    rimConstruct
    hi def link rim_h_print_inline_read_xml    rimConstruct
syn region rim_r_construct_read_json start="^[[:space:]]*read-json" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_json,rim_r_inline_read_json,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_json,rim_r_inline_read_json,rim_r_at
    syn match rim_h_construct_read_json "^[[:space:]]*read-json" contained containedin=rim_r_construct_read_json
    syn match rim_h_clause_output_read_json " key \@=" contained containedin=rim_r_construct_read_json
    syn match rim_h_clause_read_json " next \@=" contained containedin=rim_r_construct_read_json
    syn match rim_h_clause_read_json " next,\@=" contained containedin=rim_r_construct_read_json
    syn match rim_h_clause_read_json " next$" contained containedin=rim_r_construct_read_json
    syn match rim_h_clause_output_read_json " type \@=" contained containedin=rim_r_construct_read_json
    syn match rim_h_clause_output_read_json " value \@=" contained containedin=rim_r_construct_read_json
    hi def link rim_h_clause_read_json    rimClause
    hi def link rim_h_clause_output_read_json    rimClauseOutput
    hi def link rim_h_construct_read_json    rimConstruct
    hi def link rim_h_print_inline_read_json    rimConstruct
syn region rim_r_construct_read_hash start="^[[:space:]]*read-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_hash,rim_r_inline_read_hash,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_hash,rim_r_inline_read_hash,rim_r_at
    syn match rim_h_construct_read_hash "^[[:space:]]*read-hash" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " begin \@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " begin,\@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " begin$" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " delete \@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " delete,\@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " delete$" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_output_read_hash " key \@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_output_read_hash " status \@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " traverse \@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " traverse,\@=" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_read_hash " traverse$" contained containedin=rim_r_construct_read_hash
    syn match rim_h_clause_output_read_hash " value \@=" contained containedin=rim_r_construct_read_hash
    hi def link rim_h_clause_read_hash    rimClause
    hi def link rim_h_clause_output_read_hash    rimClauseOutput
    hi def link rim_h_construct_read_hash    rimConstruct
    hi def link rim_h_print_inline_read_hash    rimConstruct
syn region rim_r_construct_write_hash start="^[[:space:]]*write-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_hash,rim_r_inline_write_hash,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_hash,rim_r_inline_write_hash,rim_r_at
    syn match rim_h_construct_write_hash "^[[:space:]]*write-hash" contained containedin=rim_r_construct_write_hash
    syn match rim_h_clause_write_hash " key \@=" contained containedin=rim_r_construct_write_hash
    syn match rim_h_clause_output_write_hash " old-value \@=" contained containedin=rim_r_construct_write_hash
    syn match rim_h_clause_output_write_hash " status \@=" contained containedin=rim_r_construct_write_hash
    syn match rim_h_clause_write_hash " value \@=" contained containedin=rim_r_construct_write_hash
    hi def link rim_h_clause_write_hash    rimClause
    hi def link rim_h_clause_output_write_hash    rimClauseOutput
    hi def link rim_h_construct_write_hash    rimConstruct
    hi def link rim_h_print_inline_write_hash    rimConstruct
syn region rim_r_construct_read_array start="^[[:space:]]*read-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_array,rim_r_inline_read_array,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_array,rim_r_inline_read_array,rim_r_at
    syn match rim_h_construct_read_array "^[[:space:]]*read-array" contained containedin=rim_r_construct_read_array
    syn match rim_h_clause_read_array " delete \@=" contained containedin=rim_r_construct_read_array
    syn match rim_h_clause_read_array " delete,\@=" contained containedin=rim_r_construct_read_array
    syn match rim_h_clause_read_array " delete$" contained containedin=rim_r_construct_read_array
    syn match rim_h_clause_read_array " key \@=" contained containedin=rim_r_construct_read_array
    syn match rim_h_clause_output_read_array " status \@=" contained containedin=rim_r_construct_read_array
    syn match rim_h_clause_output_read_array " value \@=" contained containedin=rim_r_construct_read_array
    hi def link rim_h_clause_read_array    rimClause
    hi def link rim_h_clause_output_read_array    rimClauseOutput
    hi def link rim_h_construct_read_array    rimConstruct
    hi def link rim_h_print_inline_read_array    rimConstruct
syn region rim_r_construct_purge_array start="^[[:space:]]*purge-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_purge_array,rim_r_inline_purge_array,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_purge_array,rim_r_inline_purge_array,rim_r_at
    syn match rim_h_construct_purge_array "^[[:space:]]*purge-array" contained containedin=rim_r_construct_purge_array
    hi def link rim_h_clause_purge_array    rimClause
    hi def link rim_h_clause_output_purge_array    rimClauseOutput
    hi def link rim_h_construct_purge_array    rimConstruct
    hi def link rim_h_print_inline_purge_array    rimConstruct
syn region rim_r_construct_write_array start="^[[:space:]]*write-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_array,rim_r_inline_write_array,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_array,rim_r_inline_write_array,rim_r_at
    syn match rim_h_construct_write_array "^[[:space:]]*write-array" contained containedin=rim_r_construct_write_array
    syn match rim_h_clause_write_array " key \@=" contained containedin=rim_r_construct_write_array
    syn match rim_h_clause_output_write_array " old-value \@=" contained containedin=rim_r_construct_write_array
    syn match rim_h_clause_write_array " value \@=" contained containedin=rim_r_construct_write_array
    hi def link rim_h_clause_write_array    rimClause
    hi def link rim_h_clause_output_write_array    rimClauseOutput
    hi def link rim_h_construct_write_array    rimConstruct
    hi def link rim_h_print_inline_write_array    rimConstruct
syn region rim_r_construct_new_hash start="^[[:space:]]*new-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_hash,rim_r_inline_new_hash,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_hash,rim_r_inline_new_hash,rim_r_at
    syn match rim_h_construct_new_hash "^[[:space:]]*new-hash" contained containedin=rim_r_construct_new_hash
    syn match rim_h_clause_new_hash " hash-size \@=" contained containedin=rim_r_construct_new_hash
    syn match rim_h_clause_new_hash " process-scope \@=" contained containedin=rim_r_construct_new_hash
    syn match rim_h_clause_new_hash " process-scope,\@=" contained containedin=rim_r_construct_new_hash
    syn match rim_h_clause_new_hash " process-scope$" contained containedin=rim_r_construct_new_hash
    hi def link rim_h_clause_new_hash    rimClause
    hi def link rim_h_clause_output_new_hash    rimClauseOutput
    hi def link rim_h_construct_new_hash    rimConstruct
    hi def link rim_h_print_inline_new_hash    rimConstruct
syn region rim_r_construct_new_array start="^[[:space:]]*new-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_array,rim_r_inline_new_array,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_array,rim_r_inline_new_array,rim_r_at
    syn match rim_h_construct_new_array "^[[:space:]]*new-array" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " max-size \@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " process-scope \@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " process-scope,\@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " process-scope$" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " bool \@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " bool,\@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " bool$" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " double \@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " double,\@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " double$" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " number \@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " number,\@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " number$" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " string \@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " string,\@=" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " string$" contained containedin=rim_r_construct_new_array
    syn match rim_h_clause_new_array " type \@=" contained containedin=rim_r_construct_new_array
    hi def link rim_h_clause_new_array    rimClause
    hi def link rim_h_clause_output_new_array    rimClauseOutput
    hi def link rim_h_construct_new_array    rimConstruct
    hi def link rim_h_print_inline_new_array    rimConstruct
syn region rim_r_construct_resize_hash start="^[[:space:]]*resize-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_resize_hash,rim_r_inline_resize_hash,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_resize_hash,rim_r_inline_resize_hash,rim_r_at
    syn match rim_h_construct_resize_hash "^[[:space:]]*resize-hash" contained containedin=rim_r_construct_resize_hash
    syn match rim_h_clause_resize_hash " hash-size \@=" contained containedin=rim_r_construct_resize_hash
    hi def link rim_h_clause_resize_hash    rimClause
    hi def link rim_h_clause_output_resize_hash    rimClauseOutput
    hi def link rim_h_construct_resize_hash    rimConstruct
    hi def link rim_h_print_inline_resize_hash    rimConstruct
syn region rim_r_construct_purge_hash start="^[[:space:]]*purge-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_purge_hash,rim_r_inline_purge_hash,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_purge_hash,rim_r_inline_purge_hash,rim_r_at
    syn match rim_h_construct_purge_hash "^[[:space:]]*purge-hash" contained containedin=rim_r_construct_purge_hash
    hi def link rim_h_clause_purge_hash    rimClause
    hi def link rim_h_clause_output_purge_hash    rimClauseOutput
    hi def link rim_h_construct_purge_hash    rimConstruct
    hi def link rim_h_print_inline_purge_hash    rimConstruct
syn region rim_r_construct_get_hash start="^[[:space:]]*get-hash" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_hash,rim_r_inline_get_hash,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_hash,rim_r_inline_get_hash,rim_r_at
    syn match rim_h_construct_get_hash "^[[:space:]]*get-hash" contained containedin=rim_r_construct_get_hash
    syn match rim_h_clause_output_get_hash " average-reads \@=" contained containedin=rim_r_construct_get_hash
    syn match rim_h_clause_output_get_hash " hash-size \@=" contained containedin=rim_r_construct_get_hash
    syn match rim_h_clause_output_get_hash " length \@=" contained containedin=rim_r_construct_get_hash
    hi def link rim_h_clause_get_hash    rimClause
    hi def link rim_h_clause_output_get_hash    rimClauseOutput
    hi def link rim_h_construct_get_hash    rimConstruct
    hi def link rim_h_print_inline_get_hash    rimConstruct
syn region rim_r_construct_read_file start="^[[:space:]]*read-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_file,rim_r_inline_read_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_file,rim_r_inline_read_file,rim_r_at
    syn match rim_h_construct_read_file "^[[:space:]]*read-file" contained containedin=rim_r_construct_read_file
    syn match rim_h_clause_output_read_file " end-of-file \@=" contained containedin=rim_r_construct_read_file
    syn match rim_h_clause_read_file " file-id \@=" contained containedin=rim_r_construct_read_file
    syn match rim_h_clause_read_file " length \@=" contained containedin=rim_r_construct_read_file
    syn match rim_h_clause_read_file " position \@=" contained containedin=rim_r_construct_read_file
    syn match rim_h_clause_output_read_file " status \@=" contained containedin=rim_r_construct_read_file
    syn match rim_h_clause_output_read_file " to \@=" contained containedin=rim_r_construct_read_file
    hi def link rim_h_clause_read_file    rimClause
    hi def link rim_h_clause_output_read_file    rimClauseOutput
    hi def link rim_h_construct_read_file    rimConstruct
    hi def link rim_h_print_inline_read_file    rimConstruct
syn region rim_r_construct_write_file start="^[[:space:]]*write-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_file,rim_r_inline_write_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_file,rim_r_inline_write_file,rim_r_at
    syn match rim_h_construct_write_file "^[[:space:]]*write-file" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " append \@=" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " append,\@=" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " append$" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " file-id \@=" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " from \@=" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " length \@=" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_write_file " position \@=" contained containedin=rim_r_construct_write_file
    syn match rim_h_clause_output_write_file " status \@=" contained containedin=rim_r_construct_write_file
    hi def link rim_h_clause_write_file    rimClause
    hi def link rim_h_clause_output_write_file    rimClauseOutput
    hi def link rim_h_construct_write_file    rimConstruct
    hi def link rim_h_print_inline_write_file    rimConstruct
syn region rim_r_construct_read_lifo start="^[[:space:]]*read-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_lifo,rim_r_inline_read_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_lifo,rim_r_inline_read_lifo,rim_r_at
    syn match rim_h_construct_read_lifo "^[[:space:]]*read-lifo" contained containedin=rim_r_construct_read_lifo
    syn match rim_h_clause_output_read_lifo " key \@=" contained containedin=rim_r_construct_read_lifo
    syn match rim_h_clause_output_read_lifo " status \@=" contained containedin=rim_r_construct_read_lifo
    syn match rim_h_clause_output_read_lifo " value \@=" contained containedin=rim_r_construct_read_lifo
    hi def link rim_h_clause_read_lifo    rimClause
    hi def link rim_h_clause_output_read_lifo    rimClauseOutput
    hi def link rim_h_construct_read_lifo    rimConstruct
    hi def link rim_h_print_inline_read_lifo    rimConstruct
syn region rim_r_construct_read_fifo start="^[[:space:]]*read-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_fifo,rim_r_inline_read_fifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_fifo,rim_r_inline_read_fifo,rim_r_at
    syn match rim_h_construct_read_fifo "^[[:space:]]*read-fifo" contained containedin=rim_r_construct_read_fifo
    syn match rim_h_clause_output_read_fifo " key \@=" contained containedin=rim_r_construct_read_fifo
    syn match rim_h_clause_output_read_fifo " status \@=" contained containedin=rim_r_construct_read_fifo
    syn match rim_h_clause_output_read_fifo " value \@=" contained containedin=rim_r_construct_read_fifo
    hi def link rim_h_clause_read_fifo    rimClause
    hi def link rim_h_clause_output_read_fifo    rimClauseOutput
    hi def link rim_h_construct_read_fifo    rimConstruct
    hi def link rim_h_print_inline_read_fifo    rimConstruct
syn region rim_r_construct_purge_lifo start="^[[:space:]]*purge-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_purge_lifo,rim_r_inline_purge_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_purge_lifo,rim_r_inline_purge_lifo,rim_r_at
    syn match rim_h_construct_purge_lifo "^[[:space:]]*purge-lifo" contained containedin=rim_r_construct_purge_lifo
    hi def link rim_h_clause_purge_lifo    rimClause
    hi def link rim_h_clause_output_purge_lifo    rimClauseOutput
    hi def link rim_h_construct_purge_lifo    rimConstruct
    hi def link rim_h_print_inline_purge_lifo    rimConstruct
syn region rim_r_construct_purge_fifo start="^[[:space:]]*purge-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_purge_fifo,rim_r_inline_purge_fifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_purge_fifo,rim_r_inline_purge_fifo,rim_r_at
    syn match rim_h_construct_purge_fifo "^[[:space:]]*purge-fifo" contained containedin=rim_r_construct_purge_fifo
    hi def link rim_h_clause_purge_fifo    rimClause
    hi def link rim_h_clause_output_purge_fifo    rimClauseOutput
    hi def link rim_h_construct_purge_fifo    rimConstruct
    hi def link rim_h_print_inline_purge_fifo    rimConstruct
syn region rim_r_construct_delete_lifo start="^[[:space:]]*delete-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_lifo,rim_r_inline_delete_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_lifo,rim_r_inline_delete_lifo,rim_r_at
    syn match rim_h_construct_delete_lifo "^[[:space:]]*delete-lifo" contained containedin=rim_r_construct_delete_lifo
    hi def link rim_h_clause_delete_lifo    rimClause
    hi def link rim_h_clause_output_delete_lifo    rimClauseOutput
    hi def link rim_h_construct_delete_lifo    rimConstruct
    hi def link rim_h_print_inline_delete_lifo    rimConstruct
syn region rim_r_construct_delete_fifo start="^[[:space:]]*delete-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_fifo,rim_r_inline_delete_fifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_fifo,rim_r_inline_delete_fifo,rim_r_at
    syn match rim_h_construct_delete_fifo "^[[:space:]]*delete-fifo" contained containedin=rim_r_construct_delete_fifo
    hi def link rim_h_clause_delete_fifo    rimClause
    hi def link rim_h_clause_output_delete_fifo    rimClauseOutput
    hi def link rim_h_construct_delete_fifo    rimConstruct
    hi def link rim_h_print_inline_delete_fifo    rimConstruct
syn region rim_r_construct_rewind_lifo start="^[[:space:]]*rewind-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_rewind_lifo,rim_r_inline_rewind_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_rewind_lifo,rim_r_inline_rewind_lifo,rim_r_at
    syn match rim_h_construct_rewind_lifo "^[[:space:]]*rewind-lifo" contained containedin=rim_r_construct_rewind_lifo
    hi def link rim_h_clause_rewind_lifo    rimClause
    hi def link rim_h_clause_output_rewind_lifo    rimClauseOutput
    hi def link rim_h_construct_rewind_lifo    rimConstruct
    hi def link rim_h_print_inline_rewind_lifo    rimConstruct
syn region rim_r_construct_rewind_fifo start="^[[:space:]]*rewind-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_rewind_fifo,rim_r_inline_rewind_fifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_rewind_fifo,rim_r_inline_rewind_fifo,rim_r_at
    syn match rim_h_construct_rewind_fifo "^[[:space:]]*rewind-fifo" contained containedin=rim_r_construct_rewind_fifo
    hi def link rim_h_clause_rewind_fifo    rimClause
    hi def link rim_h_clause_output_rewind_fifo    rimClauseOutput
    hi def link rim_h_construct_rewind_fifo    rimConstruct
    hi def link rim_h_print_inline_rewind_fifo    rimConstruct
syn region rim_r_construct_write_lifo start="^[[:space:]]*write-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_lifo,rim_r_inline_write_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_lifo,rim_r_inline_write_lifo,rim_r_at
    syn match rim_h_construct_write_lifo "^[[:space:]]*write-lifo" contained containedin=rim_r_construct_write_lifo
    syn match rim_h_clause_write_lifo " key \@=" contained containedin=rim_r_construct_write_lifo
    syn match rim_h_clause_write_lifo " value \@=" contained containedin=rim_r_construct_write_lifo
    hi def link rim_h_clause_write_lifo    rimClause
    hi def link rim_h_clause_output_write_lifo    rimClauseOutput
    hi def link rim_h_construct_write_lifo    rimConstruct
    hi def link rim_h_print_inline_write_lifo    rimConstruct
syn region rim_r_construct_write_fifo start="^[[:space:]]*write-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_fifo,rim_r_inline_write_fifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_fifo,rim_r_inline_write_fifo,rim_r_at
    syn match rim_h_construct_write_fifo "^[[:space:]]*write-fifo" contained containedin=rim_r_construct_write_fifo
    syn match rim_h_clause_write_fifo " key \@=" contained containedin=rim_r_construct_write_fifo
    syn match rim_h_clause_write_fifo " value \@=" contained containedin=rim_r_construct_write_fifo
    hi def link rim_h_clause_write_fifo    rimClause
    hi def link rim_h_clause_output_write_fifo    rimClauseOutput
    hi def link rim_h_construct_write_fifo    rimConstruct
    hi def link rim_h_print_inline_write_fifo    rimConstruct
syn region rim_r_construct_new_lifo start="^[[:space:]]*new-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_lifo,rim_r_inline_new_lifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_lifo,rim_r_inline_new_lifo,rim_r_at
    syn match rim_h_construct_new_lifo "^[[:space:]]*new-lifo" contained containedin=rim_r_construct_new_lifo
    hi def link rim_h_clause_new_lifo    rimClause
    hi def link rim_h_clause_output_new_lifo    rimClauseOutput
    hi def link rim_h_construct_new_lifo    rimConstruct
    hi def link rim_h_print_inline_new_lifo    rimConstruct
syn region rim_r_construct_new_fifo start="^[[:space:]]*new-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_fifo,rim_r_inline_new_fifo,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_fifo,rim_r_inline_new_fifo,rim_r_at
    syn match rim_h_construct_new_fifo "^[[:space:]]*new-fifo" contained containedin=rim_r_construct_new_fifo
    hi def link rim_h_clause_new_fifo    rimClause
    hi def link rim_h_clause_output_new_fifo    rimClauseOutput
    hi def link rim_h_construct_new_fifo    rimConstruct
    hi def link rim_h_print_inline_new_fifo    rimConstruct
syn region rim_r_construct_delete_list start="^[[:space:]]*delete-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_list,rim_r_inline_delete_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_list,rim_r_inline_delete_list,rim_r_at
    syn match rim_h_construct_delete_list "^[[:space:]]*delete-list" contained containedin=rim_r_construct_delete_list
    syn match rim_h_clause_output_delete_list " status \@=" contained containedin=rim_r_construct_delete_list
    hi def link rim_h_clause_delete_list    rimClause
    hi def link rim_h_clause_output_delete_list    rimClauseOutput
    hi def link rim_h_construct_delete_list    rimConstruct
    hi def link rim_h_print_inline_delete_list    rimConstruct
syn region rim_r_construct_purge_list start="^[[:space:]]*purge-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_purge_list,rim_r_inline_purge_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_purge_list,rim_r_inline_purge_list,rim_r_at
    syn match rim_h_construct_purge_list "^[[:space:]]*purge-list" contained containedin=rim_r_construct_purge_list
    hi def link rim_h_clause_purge_list    rimClause
    hi def link rim_h_clause_output_purge_list    rimClauseOutput
    hi def link rim_h_construct_purge_list    rimConstruct
    hi def link rim_h_print_inline_purge_list    rimConstruct
syn region rim_r_construct_position_list start="^[[:space:]]*position-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_position_list,rim_r_inline_position_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_position_list,rim_r_inline_position_list,rim_r_at
    syn match rim_h_construct_position_list "^[[:space:]]*position-list" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " end \@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " end,\@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " end$" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " first \@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " first,\@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " first$" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " last \@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " last,\@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " last$" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " next \@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " next,\@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " next$" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " previous \@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " previous,\@=" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_position_list " previous$" contained containedin=rim_r_construct_position_list
    syn match rim_h_clause_output_position_list " status \@=" contained containedin=rim_r_construct_position_list
    hi def link rim_h_clause_position_list    rimClause
    hi def link rim_h_clause_output_position_list    rimClauseOutput
    hi def link rim_h_construct_position_list    rimConstruct
    hi def link rim_h_print_inline_position_list    rimConstruct
syn region rim_r_construct_read_list start="^[[:space:]]*read-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_list,rim_r_inline_read_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_list,rim_r_inline_read_list,rim_r_at
    syn match rim_h_construct_read_list "^[[:space:]]*read-list" contained containedin=rim_r_construct_read_list
    syn match rim_h_clause_output_read_list " key \@=" contained containedin=rim_r_construct_read_list
    syn match rim_h_clause_output_read_list " status \@=" contained containedin=rim_r_construct_read_list
    syn match rim_h_clause_read_list " update-key \@=" contained containedin=rim_r_construct_read_list
    syn match rim_h_clause_read_list " update-value \@=" contained containedin=rim_r_construct_read_list
    syn match rim_h_clause_output_read_list " value \@=" contained containedin=rim_r_construct_read_list
    hi def link rim_h_clause_read_list    rimClause
    hi def link rim_h_clause_output_read_list    rimClauseOutput
    hi def link rim_h_construct_read_list    rimConstruct
    hi def link rim_h_print_inline_read_list    rimConstruct
syn region rim_r_construct_write_list start="^[[:space:]]*write-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_list,rim_r_inline_write_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_list,rim_r_inline_write_list,rim_r_at
    syn match rim_h_construct_write_list "^[[:space:]]*write-list" contained containedin=rim_r_construct_write_list
    syn match rim_h_clause_write_list " append \@=" contained containedin=rim_r_construct_write_list
    syn match rim_h_clause_write_list " append,\@=" contained containedin=rim_r_construct_write_list
    syn match rim_h_clause_write_list " append$" contained containedin=rim_r_construct_write_list
    syn match rim_h_clause_write_list " key \@=" contained containedin=rim_r_construct_write_list
    syn match rim_h_clause_write_list " value \@=" contained containedin=rim_r_construct_write_list
    hi def link rim_h_clause_write_list    rimClause
    hi def link rim_h_clause_output_write_list    rimClauseOutput
    hi def link rim_h_construct_write_list    rimConstruct
    hi def link rim_h_print_inline_write_list    rimConstruct
syn region rim_r_construct_new_list start="^[[:space:]]*new-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_list,rim_r_inline_new_list,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_list,rim_r_inline_new_list,rim_r_at
    syn match rim_h_construct_new_list "^[[:space:]]*new-list" contained containedin=rim_r_construct_new_list
    syn match rim_h_clause_new_list " process-scope \@=" contained containedin=rim_r_construct_new_list
    syn match rim_h_clause_new_list " process-scope,\@=" contained containedin=rim_r_construct_new_list
    syn match rim_h_clause_new_list " process-scope$" contained containedin=rim_r_construct_new_list
    hi def link rim_h_clause_new_list    rimClause
    hi def link rim_h_clause_output_new_list    rimClauseOutput
    hi def link rim_h_construct_new_list    rimConstruct
    hi def link rim_h_print_inline_new_list    rimConstruct
syn region rim_r_construct_unused_var start="^[[:space:]]*unused-var" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_unused_var,rim_r_inline_unused_var,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_unused_var,rim_r_inline_unused_var,rim_r_at
    syn match rim_h_construct_unused_var "^[[:space:]]*unused-var" contained containedin=rim_r_construct_unused_var
    syn match rim_h_clause_unused_var " , \@=" contained containedin=rim_r_construct_unused_var
    syn match rim_h_clause_unused_var " ,,\@=" contained containedin=rim_r_construct_unused_var
    syn match rim_h_clause_unused_var " ,$" contained containedin=rim_r_construct_unused_var
    hi def link rim_h_clause_unused_var    rimClause
    hi def link rim_h_clause_output_unused_var    rimClauseOutput
    hi def link rim_h_construct_unused_var    rimConstruct
    hi def link rim_h_print_inline_unused_var    rimConstruct
syn region rim_r_construct_split_string start="^[[:space:]]*split-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_split_string,rim_r_inline_split_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_split_string,rim_r_inline_split_string,rim_r_at
    syn match rim_h_construct_split_string "^[[:space:]]*split-string" contained containedin=rim_r_construct_split_string
    syn match rim_h_clause_output_split_string " count \@=" contained containedin=rim_r_construct_split_string
    syn match rim_h_clause_split_string " delete \@=" contained containedin=rim_r_construct_split_string
    syn match rim_h_clause_output_split_string " to \@=" contained containedin=rim_r_construct_split_string
    syn match rim_h_clause_split_string " with \@=" contained containedin=rim_r_construct_split_string
    hi def link rim_h_clause_split_string    rimClause
    hi def link rim_h_clause_output_split_string    rimClauseOutput
    hi def link rim_h_construct_split_string    rimConstruct
    hi def link rim_h_print_inline_split_string    rimConstruct
syn region rim_r_construct_read_split start="^[[:space:]]*read-split" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_split,rim_r_inline_read_split,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_split,rim_r_inline_read_split,rim_r_at
    syn match rim_h_construct_read_split "^[[:space:]]*read-split" contained containedin=rim_r_construct_read_split
    syn match rim_h_clause_read_split " from \@=" contained containedin=rim_r_construct_read_split
    syn match rim_h_clause_output_read_split " status \@=" contained containedin=rim_r_construct_read_split
    syn match rim_h_clause_output_read_split " to \@=" contained containedin=rim_r_construct_read_split
    hi def link rim_h_clause_read_split    rimClause
    hi def link rim_h_clause_output_read_split    rimClauseOutput
    hi def link rim_h_construct_read_split    rimConstruct
    hi def link rim_h_print_inline_read_split    rimConstruct
syn region rim_r_construct_new_message start="^[[:space:]]*new-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_message,rim_r_inline_new_message,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_message,rim_r_inline_new_message,rim_r_at
    syn match rim_h_construct_new_message "^[[:space:]]*new-message" contained containedin=rim_r_construct_new_message
    syn match rim_h_clause_new_message " from \@=" contained containedin=rim_r_construct_new_message
    hi def link rim_h_clause_new_message    rimClause
    hi def link rim_h_clause_output_new_message    rimClauseOutput
    hi def link rim_h_construct_new_message    rimConstruct
    hi def link rim_h_print_inline_new_message    rimConstruct
syn region rim_r_construct_get_message start="^[[:space:]]*get-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_message,rim_r_inline_get_message,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_message,rim_r_inline_get_message,rim_r_at
    syn match rim_h_construct_get_message "^[[:space:]]*get-message" contained containedin=rim_r_construct_get_message
    syn match rim_h_clause_output_get_message " to \@=" contained containedin=rim_r_construct_get_message
    hi def link rim_h_clause_get_message    rimClause
    hi def link rim_h_clause_output_get_message    rimClauseOutput
    hi def link rim_h_construct_get_message    rimConstruct
    hi def link rim_h_print_inline_get_message    rimConstruct
syn region rim_r_construct_read_message start="^[[:space:]]*read-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_message,rim_r_inline_read_message,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_message,rim_r_inline_read_message,rim_r_at
    syn match rim_h_construct_read_message "^[[:space:]]*read-message" contained containedin=rim_r_construct_read_message
    syn match rim_h_clause_output_read_message " key \@=" contained containedin=rim_r_construct_read_message
    syn match rim_h_clause_output_read_message " status \@=" contained containedin=rim_r_construct_read_message
    syn match rim_h_clause_output_read_message " value \@=" contained containedin=rim_r_construct_read_message
    hi def link rim_h_clause_read_message    rimClause
    hi def link rim_h_clause_output_read_message    rimClauseOutput
    hi def link rim_h_construct_read_message    rimConstruct
    hi def link rim_h_print_inline_read_message    rimConstruct
syn region rim_r_construct_write_message start="^[[:space:]]*write-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_write_message,rim_r_inline_write_message,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_write_message,rim_r_inline_write_message,rim_r_at
    syn match rim_h_construct_write_message "^[[:space:]]*write-message" contained containedin=rim_r_construct_write_message
    syn match rim_h_clause_write_message " key \@=" contained containedin=rim_r_construct_write_message
    syn match rim_h_clause_write_message " value \@=" contained containedin=rim_r_construct_write_message
    hi def link rim_h_clause_write_message    rimClause
    hi def link rim_h_clause_output_write_message    rimClauseOutput
    hi def link rim_h_construct_write_message    rimConstruct
    hi def link rim_h_print_inline_write_message    rimConstruct
syn region rim_r_construct_delete_string start="^[[:space:]]*delete-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_string,rim_r_inline_delete_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_string,rim_r_inline_delete_string,rim_r_at
    syn match rim_h_construct_delete_string "^[[:space:]]*delete-string" contained containedin=rim_r_construct_delete_string
    hi def link rim_h_clause_delete_string    rimClause
    hi def link rim_h_clause_output_delete_string    rimClauseOutput
    hi def link rim_h_construct_delete_string    rimConstruct
    hi def link rim_h_print_inline_delete_string    rimConstruct
syn region rim_r_construct_end_if start="^[[:space:]]*end-if" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_if,rim_r_inline_end_if,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_if,rim_r_inline_end_if,rim_r_at
    syn match rim_h_construct_end_if "^[[:space:]]*end-if" contained containedin=rim_r_construct_end_if
    hi def link rim_h_clause_end_if    rimClause
    hi def link rim_h_clause_output_end_if    rimClauseOutput
    hi def link rim_h_construct_end_if    rimConstruct
    hi def link rim_h_print_inline_end_if    rimConstruct
syn region rim_r_construct_else_if start="^[[:space:]]*else-if" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_else_if,rim_r_inline_else_if,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_else_if,rim_r_inline_else_if,rim_r_at
    syn match rim_h_construct_else_if "^[[:space:]]*else-if" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " and \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " case-insensitive \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " case-insensitive,\@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " case-insensitive$" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " contain \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " equal \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " error-margin \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " every \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " greater-equal \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " greater-than \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " length \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " lesser-equal \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " lesser-than \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " not-contain \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " not-equal \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " not-every \@=" contained containedin=rim_r_construct_else_if
    syn match rim_h_clause_else_if " or \@=" contained containedin=rim_r_construct_else_if
    hi def link rim_h_clause_else_if    rimClause
    hi def link rim_h_clause_output_else_if    rimClauseOutput
    hi def link rim_h_construct_else_if    rimConstruct
    hi def link rim_h_print_inline_else_if    rimConstruct
syn region rim_r_construct_if_true start="^[[:space:]]*if-true" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_if_true,rim_r_inline_if_true,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_if_true,rim_r_inline_if_true,rim_r_at
    syn match rim_h_construct_if_true "^[[:space:]]*if-true" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " and \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " case-insensitive \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " case-insensitive,\@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " case-insensitive$" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " contain \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " equal \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " error-margin \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " every \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " greater-equal \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " greater-than \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " length \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " lesser-equal \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " lesser-than \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " not-contain \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " not-equal \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " not-every \@=" contained containedin=rim_r_construct_if_true
    syn match rim_h_clause_if_true " or \@=" contained containedin=rim_r_construct_if_true
    hi def link rim_h_clause_if_true    rimClause
    hi def link rim_h_clause_output_if_true    rimClauseOutput
    hi def link rim_h_construct_if_true    rimConstruct
    hi def link rim_h_print_inline_if_true    rimConstruct
syn region rim_r_construct_set_string start="^[[:space:]]*set-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_string,rim_r_inline_set_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_string,rim_r_inline_set_string,rim_r_at
    syn match rim_h_construct_set_string "^[[:space:]]*set-string" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " length \@=" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " process-scope \@=" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " process-scope,\@=" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " process-scope$" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " unquoted \@=" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " unquoted,\@=" contained containedin=rim_r_construct_set_string
    syn match rim_h_clause_set_string " unquoted$" contained containedin=rim_r_construct_set_string
    hi def link rim_h_clause_set_string    rimClause
    hi def link rim_h_clause_output_set_string    rimClauseOutput
    hi def link rim_h_construct_set_string    rimConstruct
    hi def link rim_h_print_inline_set_string    rimConstruct
syn region rim_r_construct_new_string start="^[[:space:]]*new-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_string,rim_r_inline_new_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_string,rim_r_inline_new_string,rim_r_at
    syn match rim_h_construct_new_string "^[[:space:]]*new-string" contained containedin=rim_r_construct_new_string
    syn match rim_h_clause_new_string " length \@=" contained containedin=rim_r_construct_new_string
    hi def link rim_h_clause_new_string    rimClause
    hi def link rim_h_clause_output_new_string    rimClauseOutput
    hi def link rim_h_construct_new_string    rimConstruct
    hi def link rim_h_print_inline_new_string    rimConstruct
syn region rim_r_construct_set_double start="^[[:space:]]*set-double" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_double,rim_r_inline_set_double,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_double,rim_r_inline_set_double,rim_r_at
    syn match rim_h_construct_set_double "^[[:space:]]*set-double" contained containedin=rim_r_construct_set_double
    syn match rim_h_clause_set_double " process-scope \@=" contained containedin=rim_r_construct_set_double
    syn match rim_h_clause_set_double " process-scope,\@=" contained containedin=rim_r_construct_set_double
    syn match rim_h_clause_set_double " process-scope$" contained containedin=rim_r_construct_set_double
    hi def link rim_h_clause_set_double    rimClause
    hi def link rim_h_clause_output_set_double    rimClauseOutput
    hi def link rim_h_construct_set_double    rimConstruct
    hi def link rim_h_print_inline_set_double    rimConstruct
syn region rim_r_construct_set_number start="^[[:space:]]*set-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_number,rim_r_inline_set_number,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_number,rim_r_inline_set_number,rim_r_at
    syn match rim_h_construct_set_number "^[[:space:]]*set-number" contained containedin=rim_r_construct_set_number
    syn match rim_h_clause_set_number " process-scope \@=" contained containedin=rim_r_construct_set_number
    syn match rim_h_clause_set_number " process-scope,\@=" contained containedin=rim_r_construct_set_number
    syn match rim_h_clause_set_number " process-scope$" contained containedin=rim_r_construct_set_number
    hi def link rim_h_clause_set_number    rimClause
    hi def link rim_h_clause_output_set_number    rimClauseOutput
    hi def link rim_h_construct_set_number    rimConstruct
    hi def link rim_h_print_inline_set_number    rimConstruct
syn region rim_r_construct_set_bool start="^[[:space:]]*set-bool" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_bool,rim_r_inline_set_bool,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_bool,rim_r_inline_set_bool,rim_r_at
    syn match rim_h_construct_set_bool "^[[:space:]]*set-bool" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " and \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " && \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " &&,\@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " &&$" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " case-insensitive \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " case-insensitive,\@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " case-insensitive$" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " contain \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " equal \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " every \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " greater-equal \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " greater-than \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " length \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " lesser-equal \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " lesser-than \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " ! \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " !,\@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " !$" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " not-contain \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " not-equal \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " not-every \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " or \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " || \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " ||,\@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " ||$" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " process-scope \@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " process-scope,\@=" contained containedin=rim_r_construct_set_bool
    syn match rim_h_clause_set_bool " process-scope$" contained containedin=rim_r_construct_set_bool
    hi def link rim_h_clause_set_bool    rimClause
    hi def link rim_h_clause_output_set_bool    rimClauseOutput
    hi def link rim_h_construct_set_bool    rimConstruct
    hi def link rim_h_print_inline_set_bool    rimConstruct
syn region rim_r_construct___ start="^[[:space:]]*%%" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct___,rim_r_inline___,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct___,rim_r_inline___,rim_r_at
    syn match rim_h_construct___ "^[[:space:]]*%%" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " , \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " ,,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " ,$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " default-value \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " get-param \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " private \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " private,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " private$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " public \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " public,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " public$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " bool-array \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " bool-array,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " bool-array$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " double-array \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " double-array,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " double-array$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " number-array \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " number-array,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " number-array$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " string-array \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " string-array,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " string-array$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " bool \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " bool,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " bool$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " double \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " double,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " double$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " encrypt-decrypt \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " encrypt-decrypt,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " encrypt-decrypt$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " fifo \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " fifo,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " fifo$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " file \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " file,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " file$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " hash \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " hash,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " hash$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " lifo \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " lifo,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " lifo$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " list \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " list,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " list$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " message \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " message,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " message$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " number \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " number,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " number$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " service \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " service,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " service$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " split-string \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " split-string,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " split-string$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " string \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " string,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " string$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " tree \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " tree,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " tree$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " tree-cursor \@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " tree-cursor,\@=" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " tree-cursor$" contained containedin=rim_r_construct___
    syn match rim_h_clause___ " type \@=" contained containedin=rim_r_construct___
    hi def link rim_h_clause___    rimClause
    hi def link rim_h_clause_output___    rimClauseOutput
    hi def link rim_h_construct___    rimConstruct
    hi def link rim_h_print_inline___    rimConstruct
syn region rim_r_construct_begin_handler start="^[[:space:]]*begin-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_begin_handler,rim_r_inline_begin_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_begin_handler,rim_r_inline_begin_handler,rim_r_at
    syn match rim_h_construct_begin_handler "^[[:space:]]*begin-handler" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " , \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " ,,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " ,$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " default-value \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " get-param \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " private \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " private,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " private$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " public \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " public,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " public$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " bool-array \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " bool-array,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " bool-array$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " double-array \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " double-array,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " double-array$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " number-array \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " number-array,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " number-array$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " string-array \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " string-array,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " string-array$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " bool \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " bool,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " bool$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " double \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " double,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " double$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " encrypt-decrypt \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " encrypt-decrypt,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " encrypt-decrypt$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " fifo \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " fifo,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " fifo$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " file \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " file,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " file$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " hash \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " hash,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " hash$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " lifo \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " lifo,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " lifo$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " list \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " list,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " list$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " message \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " message,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " message$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " number \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " number,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " number$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " service \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " service,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " service$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " split-string \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " split-string,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " split-string$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " string \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " string,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " string$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " tree \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " tree,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " tree$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " tree-cursor \@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " tree-cursor,\@=" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " tree-cursor$" contained containedin=rim_r_construct_begin_handler
    syn match rim_h_clause_begin_handler " type \@=" contained containedin=rim_r_construct_begin_handler
    hi def link rim_h_clause_begin_handler    rimClause
    hi def link rim_h_clause_output_begin_handler    rimClauseOutput
    hi def link rim_h_construct_begin_handler    rimConstruct
    hi def link rim_h_print_inline_begin_handler    rimConstruct
syn region rim_r_construct_end_before_handler start="^[[:space:]]*end-before-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_before_handler,rim_r_inline_end_before_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_before_handler,rim_r_inline_end_before_handler,rim_r_at
    syn match rim_h_construct_end_before_handler "^[[:space:]]*end-before-handler" contained containedin=rim_r_construct_end_before_handler
    hi def link rim_h_clause_end_before_handler    rimClause
    hi def link rim_h_clause_output_end_before_handler    rimClauseOutput
    hi def link rim_h_construct_end_before_handler    rimConstruct
    hi def link rim_h_print_inline_end_before_handler    rimConstruct
syn region rim_r_construct_end_after_handler start="^[[:space:]]*end-after-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_after_handler,rim_r_inline_end_after_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_after_handler,rim_r_inline_end_after_handler,rim_r_at
    syn match rim_h_construct_end_after_handler "^[[:space:]]*end-after-handler" contained containedin=rim_r_construct_end_after_handler
    hi def link rim_h_clause_end_after_handler    rimClause
    hi def link rim_h_clause_output_end_after_handler    rimClauseOutput
    hi def link rim_h_construct_end_after_handler    rimConstruct
    hi def link rim_h_print_inline_end_after_handler    rimConstruct
syn region rim_r_construct_before_handler start="^[[:space:]]*before-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_before_handler,rim_r_inline_before_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_before_handler,rim_r_inline_before_handler,rim_r_at
    syn match rim_h_construct_before_handler "^[[:space:]]*before-handler" contained containedin=rim_r_construct_before_handler
    hi def link rim_h_clause_before_handler    rimClause
    hi def link rim_h_clause_output_before_handler    rimClauseOutput
    hi def link rim_h_construct_before_handler    rimConstruct
    hi def link rim_h_print_inline_before_handler    rimConstruct
syn region rim_r_construct_after_handler start="^[[:space:]]*after-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_after_handler,rim_r_inline_after_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_after_handler,rim_r_inline_after_handler,rim_r_at
    syn match rim_h_construct_after_handler "^[[:space:]]*after-handler" contained containedin=rim_r_construct_after_handler
    hi def link rim_h_clause_after_handler    rimClause
    hi def link rim_h_clause_output_after_handler    rimClauseOutput
    hi def link rim_h_construct_after_handler    rimConstruct
    hi def link rim_h_print_inline_after_handler    rimConstruct
syn region rim_r_construct_end_handler start="^[[:space:]]*end-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_handler,rim_r_inline_end_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_handler,rim_r_inline_end_handler,rim_r_at
    syn match rim_h_construct_end_handler "^[[:space:]]*end-handler" contained containedin=rim_r_construct_end_handler
    hi def link rim_h_clause_end_handler    rimClause
    hi def link rim_h_clause_output_end_handler    rimClauseOutput
    hi def link rim_h_construct_end_handler    rimConstruct
    hi def link rim_h_print_inline_end_handler    rimConstruct
syn region rim_r_construct_set_param start="^[[:space:]]*set-param" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_param,rim_r_inline_set_param,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_param,rim_r_inline_set_param,rim_r_at
    syn match rim_h_construct_set_param "^[[:space:]]*set-param" contained containedin=rim_r_construct_set_param
    hi def link rim_h_clause_set_param    rimClause
    hi def link rim_h_clause_output_set_param    rimClauseOutput
    hi def link rim_h_construct_set_param    rimConstruct
    hi def link rim_h_print_inline_set_param    rimConstruct
syn region rim_r_construct_get_param start="^[[:space:]]*get-param" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_param,rim_r_inline_get_param,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_param,rim_r_inline_get_param,rim_r_at
    syn match rim_h_construct_get_param "^[[:space:]]*get-param" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " , \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " ,,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " ,$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " default-value \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " bool-array \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " bool-array,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " bool-array$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " double-array \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " double-array,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " double-array$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " number-array \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " number-array,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " number-array$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " string-array \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " string-array,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " string-array$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " bool \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " bool,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " bool$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " double \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " double,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " double$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " encrypt-decrypt \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " encrypt-decrypt,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " encrypt-decrypt$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " fifo \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " fifo,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " fifo$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " file \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " file,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " file$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " hash \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " hash,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " hash$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " lifo \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " lifo,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " lifo$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " list \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " list,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " list$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " message \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " message,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " message$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " number \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " number,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " number$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " service \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " service,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " service$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " split-string \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " split-string,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " split-string$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " string \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " string,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " string$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " tree \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " tree,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " tree$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " tree-cursor \@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " tree-cursor,\@=" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " tree-cursor$" contained containedin=rim_r_construct_get_param
    syn match rim_h_clause_get_param " type \@=" contained containedin=rim_r_construct_get_param
    hi def link rim_h_clause_get_param    rimClause
    hi def link rim_h_clause_output_get_param    rimClauseOutput
    hi def link rim_h_construct_get_param    rimConstruct
    hi def link rim_h_print_inline_get_param    rimConstruct
syn region rim_r_construct_get_cookie start="^[[:space:]]*get-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_cookie,rim_r_inline_get_cookie,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_cookie,rim_r_inline_get_cookie,rim_r_at
    syn match rim_h_construct_get_cookie "^[[:space:]]*get-cookie" contained containedin=rim_r_construct_get_cookie
    syn match rim_h_clause_get_cookie " , \@=" contained containedin=rim_r_construct_get_cookie
    syn match rim_h_clause_get_cookie " ,,\@=" contained containedin=rim_r_construct_get_cookie
    syn match rim_h_clause_get_cookie " ,$" contained containedin=rim_r_construct_get_cookie
    hi def link rim_h_clause_get_cookie    rimClause
    hi def link rim_h_clause_output_get_cookie    rimClauseOutput
    hi def link rim_h_construct_get_cookie    rimConstruct
    hi def link rim_h_print_inline_get_cookie    rimConstruct
syn region rim_r_construct_set_cookie start="^[[:space:]]*set-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_cookie,rim_r_inline_set_cookie,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_cookie,rim_r_inline_set_cookie,rim_r_at
    syn match rim_h_construct_set_cookie "^[[:space:]]*set-cookie" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " , \@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " ,,\@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " ,$" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " expires \@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " no-http-only \@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " no-http-only,\@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " no-http-only$" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " path \@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " same-site \@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " secure \@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " secure,\@=" contained containedin=rim_r_construct_set_cookie
    syn match rim_h_clause_set_cookie " secure$" contained containedin=rim_r_construct_set_cookie
    hi def link rim_h_clause_set_cookie    rimClause
    hi def link rim_h_clause_output_set_cookie    rimClauseOutput
    hi def link rim_h_construct_set_cookie    rimConstruct
    hi def link rim_h_print_inline_set_cookie    rimConstruct
syn region rim_r_construct_request_body start="^[[:space:]]*request-body" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_request_body,rim_r_inline_request_body,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_request_body,rim_r_inline_request_body,rim_r_at
    syn match rim_h_construct_request_body "^[[:space:]]*request-body" contained containedin=rim_r_construct_request_body
    hi def link rim_h_clause_request_body    rimClause
    hi def link rim_h_clause_output_request_body    rimClauseOutput
    hi def link rim_h_construct_request_body    rimConstruct
    hi def link rim_h_print_inline_request_body    rimConstruct
syn region rim_r_construct_delete_cookie start="^[[:space:]]*delete-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_cookie,rim_r_inline_delete_cookie,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_cookie,rim_r_inline_delete_cookie,rim_r_at
    syn match rim_h_construct_delete_cookie "^[[:space:]]*delete-cookie" contained containedin=rim_r_construct_delete_cookie
    syn match rim_h_clause_delete_cookie " path \@=" contained containedin=rim_r_construct_delete_cookie
    syn match rim_h_clause_delete_cookie " secure \@=" contained containedin=rim_r_construct_delete_cookie
    syn match rim_h_clause_delete_cookie " secure,\@=" contained containedin=rim_r_construct_delete_cookie
    syn match rim_h_clause_delete_cookie " secure$" contained containedin=rim_r_construct_delete_cookie
    syn match rim_h_clause_output_delete_cookie " status \@=" contained containedin=rim_r_construct_delete_cookie
    hi def link rim_h_clause_delete_cookie    rimClause
    hi def link rim_h_clause_output_delete_cookie    rimClauseOutput
    hi def link rim_h_construct_delete_cookie    rimConstruct
    hi def link rim_h_print_inline_delete_cookie    rimConstruct
syn region rim_r_construct_copy_string start="^[[:space:]]*copy-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_copy_string,rim_r_inline_copy_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_copy_string,rim_r_inline_copy_string,rim_r_at
    syn match rim_h_construct_copy_string "^[[:space:]]*copy-string" contained containedin=rim_r_construct_copy_string
    syn match rim_h_clause_copy_string " length \@=" contained containedin=rim_r_construct_copy_string
    syn match rim_h_clause_copy_string " start-with \@=" contained containedin=rim_r_construct_copy_string
    syn match rim_h_clause_output_copy_string " to \@=" contained containedin=rim_r_construct_copy_string
    hi def link rim_h_clause_copy_string    rimClause
    hi def link rim_h_clause_output_copy_string    rimClauseOutput
    hi def link rim_h_construct_copy_string    rimConstruct
    hi def link rim_h_print_inline_copy_string    rimConstruct
syn region rim_r_construct_scan_string start="^[[:space:]]*scan-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_scan_string,rim_r_inline_scan_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_scan_string,rim_r_inline_scan_string,rim_r_at
    syn match rim_h_construct_scan_string "^[[:space:]]*scan-string" contained containedin=rim_r_construct_scan_string
    syn match rim_h_clause_scan_string " for \@=" contained containedin=rim_r_construct_scan_string
    syn match rim_h_clause_scan_string " length \@=" contained containedin=rim_r_construct_scan_string
    syn match rim_h_clause_scan_string " start-with \@=" contained containedin=rim_r_construct_scan_string
    syn match rim_h_clause_output_scan_string " status \@=" contained containedin=rim_r_construct_scan_string
    hi def link rim_h_clause_scan_string    rimClause
    hi def link rim_h_clause_output_scan_string    rimClauseOutput
    hi def link rim_h_construct_scan_string    rimConstruct
    hi def link rim_h_print_inline_scan_string    rimConstruct
syn region rim_r_construct_replace_string start="^[[:space:]]*replace-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_replace_string,rim_r_inline_replace_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_replace_string,rim_r_inline_replace_string,rim_r_at
    syn match rim_h_construct_replace_string "^[[:space:]]*replace-string" contained containedin=rim_r_construct_replace_string
    syn match rim_h_clause_replace_string " copy \@=" contained containedin=rim_r_construct_replace_string
    syn match rim_h_clause_replace_string " copy-end \@=" contained containedin=rim_r_construct_replace_string
    syn match rim_h_clause_replace_string " length \@=" contained containedin=rim_r_construct_replace_string
    syn match rim_h_clause_replace_string " start-with \@=" contained containedin=rim_r_construct_replace_string
    hi def link rim_h_clause_replace_string    rimClause
    hi def link rim_h_clause_output_replace_string    rimClauseOutput
    hi def link rim_h_construct_replace_string    rimConstruct
    hi def link rim_h_print_inline_replace_string    rimConstruct
syn region rim_r_construct_flush_output start="^[[:space:]]*flush-output" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_flush_output,rim_r_inline_flush_output,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_flush_output,rim_r_inline_flush_output,rim_r_at
    syn match rim_h_construct_flush_output "^[[:space:]]*flush-output" contained containedin=rim_r_construct_flush_output
    hi def link rim_h_clause_flush_output    rimClause
    hi def link rim_h_clause_output_flush_output    rimClauseOutput
    hi def link rim_h_construct_flush_output    rimConstruct
    hi def link rim_h_print_inline_flush_output    rimConstruct
syn region rim_r_construct_extended_mode start="^[[:space:]]*extended-mode" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_extended_mode,rim_r_inline_extended_mode,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_extended_mode,rim_r_inline_extended_mode,rim_r_at
    syn match rim_h_construct_extended_mode "^[[:space:]]*extended-mode" contained containedin=rim_r_construct_extended_mode
    hi def link rim_h_clause_extended_mode    rimClause
    hi def link rim_h_clause_output_extended_mode    rimClauseOutput
    hi def link rim_h_construct_extended_mode    rimConstruct
    hi def link rim_h_print_inline_extended_mode    rimConstruct
syn region rim_r_construct_number_string start="^[[:space:]]*number-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_number_string,rim_r_inline_number_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_number_string,rim_r_inline_number_string,rim_r_at
    syn match rim_h_construct_number_string "^[[:space:]]*number-string" contained containedin=rim_r_construct_number_string
    syn match rim_h_clause_number_string " base \@=" contained containedin=rim_r_construct_number_string
    syn match rim_h_print_inline_number_string " base \@=" contained containedin=rim_r_inline_number_string
    syn match rim_h_clause_output_number_string " status \@=" contained containedin=rim_r_construct_number_string
    syn match rim_h_print_inline_number_string " status \@=" contained containedin=rim_r_inline_number_string
    syn match rim_h_clause_output_number_string " to \@=" contained containedin=rim_r_construct_number_string
    syn match rim_h_print_inline_number_string " to \@=" contained containedin=rim_r_inline_number_string
    syn region rim_r_inline_number_string start="<<[[:space:]]*number-string \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_number_string '<<[[:space:]]*number-string \@=' contained containedin=rim_r_inline_number_string
    syn match rim_h_print_inline_number_string '>>' contained containedin=rim_r_inline_number_string
    hi def link rim_h_clause_number_string    rimClause
    hi def link rim_h_clause_output_number_string    rimClauseOutput
    hi def link rim_h_construct_number_string    rimConstruct
    hi def link rim_h_print_inline_number_string    rimConstruct
syn region rim_r_construct_double_string start="^[[:space:]]*double-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_double_string,rim_r_inline_double_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_double_string,rim_r_inline_double_string,rim_r_at
    syn match rim_h_construct_double_string "^[[:space:]]*double-string" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-compact \@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-compact,\@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-compact$" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-floating \@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-floating,\@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-floating$" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-scientific \@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-scientific,\@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " format-scientific$" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " precision \@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_output_double_string " to \@=" contained containedin=rim_r_construct_double_string
    syn match rim_h_clause_double_string " width \@=" contained containedin=rim_r_construct_double_string
    hi def link rim_h_clause_double_string    rimClause
    hi def link rim_h_clause_output_double_string    rimClauseOutput
    hi def link rim_h_construct_double_string    rimConstruct
    hi def link rim_h_print_inline_double_string    rimConstruct
syn region rim_r_construct_string_number start="^[[:space:]]*string-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_string_number,rim_r_inline_string_number,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_string_number,rim_r_inline_string_number,rim_r_at
    syn match rim_h_construct_string_number "^[[:space:]]*string-number" contained containedin=rim_r_construct_string_number
    syn match rim_h_clause_string_number " base \@=" contained containedin=rim_r_construct_string_number
    syn match rim_h_clause_output_string_number " status \@=" contained containedin=rim_r_construct_string_number
    syn match rim_h_clause_output_string_number " to \@=" contained containedin=rim_r_construct_string_number
    hi def link rim_h_clause_string_number    rimClause
    hi def link rim_h_clause_output_string_number    rimClauseOutput
    hi def link rim_h_construct_string_number    rimConstruct
    hi def link rim_h_print_inline_string_number    rimConstruct
syn region rim_r_construct_string_double start="^[[:space:]]*string-double" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_string_double,rim_r_inline_string_double,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_string_double,rim_r_inline_string_double,rim_r_at
    syn match rim_h_construct_string_double "^[[:space:]]*string-double" contained containedin=rim_r_construct_string_double
    syn match rim_h_clause_output_string_double " status \@=" contained containedin=rim_r_construct_string_double
    syn match rim_h_clause_output_string_double " to \@=" contained containedin=rim_r_construct_string_double
    hi def link rim_h_clause_string_double    rimClause
    hi def link rim_h_clause_output_string_double    rimClauseOutput
    hi def link rim_h_construct_string_double    rimConstruct
    hi def link rim_h_print_inline_string_double    rimConstruct
syn region rim_r_construct_abs_number start="^[[:space:]]*abs-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_abs_number,rim_r_inline_abs_number,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_abs_number,rim_r_inline_abs_number,rim_r_at
    syn match rim_h_construct_abs_number "^[[:space:]]*abs-number" contained containedin=rim_r_construct_abs_number
    syn match rim_h_clause_output_abs_number " to \@=" contained containedin=rim_r_construct_abs_number
    hi def link rim_h_clause_abs_number    rimClause
    hi def link rim_h_clause_output_abs_number    rimClauseOutput
    hi def link rim_h_construct_abs_number    rimConstruct
    hi def link rim_h_print_inline_abs_number    rimConstruct
syn region rim_r_construct_abs_double start="^[[:space:]]*abs-double" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_abs_double,rim_r_inline_abs_double,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_abs_double,rim_r_inline_abs_double,rim_r_at
    syn match rim_h_construct_abs_double "^[[:space:]]*abs-double" contained containedin=rim_r_construct_abs_double
    syn match rim_h_clause_output_abs_double " to \@=" contained containedin=rim_r_construct_abs_double
    hi def link rim_h_clause_abs_double    rimClause
    hi def link rim_h_clause_output_abs_double    rimClauseOutput
    hi def link rim_h_construct_abs_double    rimConstruct
    hi def link rim_h_print_inline_abs_double    rimConstruct
syn region rim_r_construct_get_req start="^[[:space:]]*get-req" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_req,rim_r_inline_get_req,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_req,rim_r_inline_get_req,rim_r_at
    syn match rim_h_construct_get_req "^[[:space:]]*get-req" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " arg-count \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " arg-count,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " arg-count$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " arg-value \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " content-type \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " cookie \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " cookie-count \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " cookie-count,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " cookie-count$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " directory \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " directory,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " directory$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " errno \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " errno,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " errno$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " error \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " external-call \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " external-call,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " external-call$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " header \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " method \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " name \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " name,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " name$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " process-id \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " process-id,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " process-id$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " referring-url \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " referring-url,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " referring-url$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " source-file \@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " source-file,\@=" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_get_req " source-file$" contained containedin=rim_r_construct_get_req
    syn match rim_h_clause_output_get_req " to \@=" contained containedin=rim_r_construct_get_req
    hi def link rim_h_clause_get_req    rimClause
    hi def link rim_h_clause_output_get_req    rimClauseOutput
    hi def link rim_h_construct_get_req    rimConstruct
    hi def link rim_h_print_inline_get_req    rimConstruct
syn region rim_r_construct_set_app start="^[[:space:]]*set-app" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_set_app,rim_r_inline_set_app,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_set_app,rim_r_inline_set_app,rim_r_at
    syn match rim_h_construct_set_app "^[[:space:]]*set-app" contained containedin=rim_r_construct_set_app
    syn match rim_h_clause_set_app " stack-depth \@=" contained containedin=rim_r_construct_set_app
    hi def link rim_h_clause_set_app    rimClause
    hi def link rim_h_clause_output_set_app    rimClauseOutput
    hi def link rim_h_construct_set_app    rimConstruct
    hi def link rim_h_print_inline_set_app    rimConstruct
syn region rim_r_construct_get_app start="^[[:space:]]*get-app" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_app,rim_r_inline_get_app,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_app,rim_r_inline_get_app,rim_r_at
    syn match rim_h_construct_get_app "^[[:space:]]*get-app" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " application \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " db-vendor \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " directory \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " directory,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " directory$" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " file-directory \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " file-directory,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " file-directory$" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " is-service \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " is-service,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " is-service$" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " name \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " name,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " name$" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " path \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " socket-file \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " socket-file,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " socket-file$" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " stack-depth \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_output_get_app " to \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " upload-size \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " upload-size,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " upload-size$" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " user \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " user-directory \@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " user-directory,\@=" contained containedin=rim_r_construct_get_app
    syn match rim_h_clause_get_app " user-directory$" contained containedin=rim_r_construct_get_app
    hi def link rim_h_clause_get_app    rimClause
    hi def link rim_h_clause_output_get_app    rimClauseOutput
    hi def link rim_h_construct_get_app    rimConstruct
    hi def link rim_h_print_inline_get_app    rimConstruct
syn region rim_r_construct_get_sys start="^[[:space:]]*get-sys" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_sys,rim_r_inline_get_sys,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_sys,rim_r_inline_get_sys,rim_r_at
    syn match rim_h_construct_get_sys "^[[:space:]]*get-sys" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " directory \@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " directory,\@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " directory$" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " environment \@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-dir \@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-dir,\@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-dir$" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-name \@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-name,\@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-name$" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-user \@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-user,\@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-user$" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-version \@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-version,\@=" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_get_sys " os-version$" contained containedin=rim_r_construct_get_sys
    syn match rim_h_clause_output_get_sys " to \@=" contained containedin=rim_r_construct_get_sys
    hi def link rim_h_clause_get_sys    rimClause
    hi def link rim_h_clause_output_get_sys    rimClauseOutput
    hi def link rim_h_construct_get_sys    rimConstruct
    hi def link rim_h_print_inline_get_sys    rimConstruct
syn region rim_r_construct_match_regex start="^[[:space:]]*match-regex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_match_regex,rim_r_inline_match_regex,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_match_regex,rim_r_inline_match_regex,rim_r_at
    syn match rim_h_construct_match_regex "^[[:space:]]*match-regex" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " cache \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " cache,\@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " cache$" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " case-insensitive \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " case-insensitive,\@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " case-insensitive$" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " clear-cache \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " in \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " replace-with \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_output_match_regex " result \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " single-match \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " single-match,\@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " single-match$" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_output_match_regex " status \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " utf \@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " utf,\@=" contained containedin=rim_r_construct_match_regex
    syn match rim_h_clause_match_regex " utf$" contained containedin=rim_r_construct_match_regex
    hi def link rim_h_clause_match_regex    rimClause
    hi def link rim_h_clause_output_match_regex    rimClauseOutput
    hi def link rim_h_construct_match_regex    rimConstruct
    hi def link rim_h_print_inline_match_regex    rimConstruct
syn region rim_r_construct_get_time start="^[[:space:]]*get-time" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_get_time,rim_r_inline_get_time,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_get_time,rim_r_inline_get_time,rim_r_at
    syn match rim_h_construct_get_time "^[[:space:]]*get-time" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " day \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " format \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " from-epoch \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " hour \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " minute \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " month \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " second \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " since-epoch \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " since-epoch,\@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " since-epoch$" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " timezone \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_output_get_time " to \@=" contained containedin=rim_r_construct_get_time
    syn match rim_h_clause_get_time " year \@=" contained containedin=rim_r_construct_get_time
    hi def link rim_h_clause_get_time    rimClause
    hi def link rim_h_clause_output_get_time    rimClauseOutput
    hi def link rim_h_construct_get_time    rimConstruct
    hi def link rim_h_print_inline_get_time    rimConstruct
syn region rim_r_construct_uniq_file start="^[[:space:]]*uniq-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_uniq_file,rim_r_inline_uniq_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_uniq_file,rim_r_inline_uniq_file,rim_r_at
    syn match rim_h_construct_uniq_file "^[[:space:]]*uniq-file" contained containedin=rim_r_construct_uniq_file
    syn match rim_h_clause_uniq_file " temporary \@=" contained containedin=rim_r_construct_uniq_file
    syn match rim_h_clause_uniq_file " temporary,\@=" contained containedin=rim_r_construct_uniq_file
    syn match rim_h_clause_uniq_file " temporary$" contained containedin=rim_r_construct_uniq_file
    hi def link rim_h_clause_uniq_file    rimClause
    hi def link rim_h_clause_output_uniq_file    rimClauseOutput
    hi def link rim_h_construct_uniq_file    rimConstruct
    hi def link rim_h_print_inline_uniq_file    rimConstruct
syn region rim_r_construct_call_handler start="^[[:space:]]*call-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_call_handler,rim_r_inline_call_handler,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_call_handler,rim_r_inline_call_handler,rim_r_at
    syn match rim_h_construct_call_handler "^[[:space:]]*call-handler" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " , \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " ,,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " ,$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " ,\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " , \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " default-value \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " default-value \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " get-param \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " get-param \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_output_call_handler " return-code \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " return-code \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " return-value \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " return-value \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " set-param \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " set-param \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " bool-array \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " bool-array,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " bool-array$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " bool-array\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " bool-array \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " double-array \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " double-array,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " double-array$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " double-array\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " double-array \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " number-array \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " number-array,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " number-array$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " number-array\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " number-array \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " string-array \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " string-array,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " string-array$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " string-array\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " string-array \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " bool \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " bool,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " bool$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " bool\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " bool \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " double \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " double,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " double$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " double\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " double \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " encrypt-decrypt \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " encrypt-decrypt,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " encrypt-decrypt$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " encrypt-decrypt\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " encrypt-decrypt \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " fifo \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " fifo,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " fifo$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " fifo\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " fifo \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " file \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " file,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " file$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " file\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " file \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " hash \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " hash,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " hash$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " hash\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " hash \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " lifo \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " lifo,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " lifo$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " lifo\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " lifo \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " list \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " list,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " list$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " list\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " list \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " message \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " message,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " message$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " message\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " message \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " number \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " number,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " number$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " number\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " number \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " service \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " service,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " service$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " service\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " service \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " split-string \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " split-string,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " split-string$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " split-string\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " split-string \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " string \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " string,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " string$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " string\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " string \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " tree \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " tree,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " tree$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " tree\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " tree \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " tree-cursor \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " tree-cursor,\@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_clause_call_handler " tree-cursor$" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " tree-cursor\(>>\)\@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler " tree-cursor \@=" contained containedin=rim_r_inline_call_handler
    syn match rim_h_clause_call_handler " type \@=" contained containedin=rim_r_construct_call_handler
    syn match rim_h_print_inline_call_handler " type \@=" contained containedin=rim_r_inline_call_handler
    syn region rim_r_inline_call_handler start="<<[[:space:]]*call-handler \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_call_handler '<<[[:space:]]*call-handler \@=' contained containedin=rim_r_inline_call_handler
    syn match rim_h_print_inline_call_handler '>>' contained containedin=rim_r_inline_call_handler
    hi def link rim_h_clause_call_handler    rimClause
    hi def link rim_h_clause_output_call_handler    rimClauseOutput
    hi def link rim_h_construct_call_handler    rimConstruct
    hi def link rim_h_print_inline_call_handler    rimConstruct
syn region rim_r_construct_call_remote start="^[[:space:]]*call-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_call_remote,rim_r_inline_call_remote,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_call_remote,rim_r_inline_call_remote,rim_r_at
    syn match rim_h_construct_call_remote "^[[:space:]]*call-remote" contained containedin=rim_r_construct_call_remote
    syn match rim_h_clause_output_call_remote " finished-okay \@=" contained containedin=rim_r_construct_call_remote
    syn match rim_h_clause_output_call_remote " started \@=" contained containedin=rim_r_construct_call_remote
    syn match rim_h_clause_output_call_remote " status \@=" contained containedin=rim_r_construct_call_remote
    hi def link rim_h_clause_call_remote    rimClause
    hi def link rim_h_clause_output_call_remote    rimClauseOutput
    hi def link rim_h_construct_call_remote    rimConstruct
    hi def link rim_h_print_inline_call_remote    rimConstruct
syn region rim_r_construct_call_extended start="^[[:space:]]*call-extended" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_call_extended,rim_r_inline_call_extended,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_call_extended,rim_r_inline_call_extended,rim_r_at
    syn match rim_h_construct_call_extended "^[[:space:]]*call-extended" contained containedin=rim_r_construct_call_extended
    syn region rim_r_inline_call_extended start="<<[[:space:]]*call-extended \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_call_extended '<<[[:space:]]*call-extended \@=' contained containedin=rim_r_inline_call_extended
    syn match rim_h_print_inline_call_extended '>>' contained containedin=rim_r_inline_call_extended
    hi def link rim_h_clause_call_extended    rimClause
    hi def link rim_h_clause_output_call_extended    rimClauseOutput
    hi def link rim_h_construct_call_extended    rimConstruct
    hi def link rim_h_print_inline_call_extended    rimConstruct
syn region rim_r_construct_read_remote start="^[[:space:]]*read-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_read_remote,rim_r_inline_read_remote,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_read_remote,rim_r_inline_read_remote,rim_r_at
    syn match rim_h_construct_read_remote "^[[:space:]]*read-remote" contained containedin=rim_r_construct_read_remote
    syn match rim_h_clause_read_remote " data \@=" contained containedin=rim_r_construct_read_remote
    syn match rim_h_clause_read_remote " error \@=" contained containedin=rim_r_construct_read_remote
    syn match rim_h_clause_read_remote " exit-status \@=" contained containedin=rim_r_construct_read_remote
    syn match rim_h_clause_read_remote " status \@=" contained containedin=rim_r_construct_read_remote
    syn match rim_h_clause_read_remote " status-text \@=" contained containedin=rim_r_construct_read_remote
    hi def link rim_h_clause_read_remote    rimClause
    hi def link rim_h_clause_output_read_remote    rimClauseOutput
    hi def link rim_h_construct_read_remote    rimConstruct
    hi def link rim_h_print_inline_read_remote    rimConstruct
syn region rim_r_construct_new_remote start="^[[:space:]]*new-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_remote,rim_r_inline_new_remote,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_remote,rim_r_inline_new_remote,rim_r_at
    syn match rim_h_construct_new_remote "^[[:space:]]*new-remote" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " app-path \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " content \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " content-length \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " content-type \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " environment \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " local \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " location \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " method \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " request-body \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " request-path \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " timeout \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " url-params \@=" contained containedin=rim_r_construct_new_remote
    syn match rim_h_clause_new_remote " url-path \@=" contained containedin=rim_r_construct_new_remote
    hi def link rim_h_clause_new_remote    rimClause
    hi def link rim_h_clause_output_new_remote    rimClauseOutput
    hi def link rim_h_construct_new_remote    rimConstruct
    hi def link rim_h_print_inline_new_remote    rimConstruct
syn region rim_r_construct_run_remote start="^[[:space:]]*run-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_run_remote,rim_r_inline_run_remote,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_run_remote,rim_r_inline_run_remote,rim_r_at
    syn match rim_h_construct_run_remote "^[[:space:]]*run-remote" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " app-path \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " content \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " content-length \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " content-type \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " data \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " environment \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " error \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " exit-status \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " finished-okay \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " local \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " location \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " method \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " request-body \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " request-path \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " started \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " status \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " status-text \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " timeout \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " url-params \@=" contained containedin=rim_r_construct_run_remote
    syn match rim_h_clause_run_remote " url-path \@=" contained containedin=rim_r_construct_run_remote
    hi def link rim_h_clause_run_remote    rimClause
    hi def link rim_h_clause_output_run_remote    rimClauseOutput
    hi def link rim_h_construct_run_remote    rimConstruct
    hi def link rim_h_print_inline_run_remote    rimConstruct
syn region rim_r_construct_call_web start="^[[:space:]]*call-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_call_web,rim_r_inline_call_web,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_call_web,rim_r_inline_call_web,rim_r_at
    syn match rim_h_construct_call_web "^[[:space:]]*call-web" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " cert \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " content \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " content-length \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " content-type \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " cookie-jar \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " custom \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_output_call_web " error \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " fields \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " files \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " method \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " no-cert \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " no-cert,\@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " no-cert$" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " request-body \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " request-headers \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_output_call_web " response \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_output_call_web " response-code \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_output_call_web " response-headers \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_output_call_web " status \@=" contained containedin=rim_r_construct_call_web
    syn match rim_h_clause_call_web " timeout \@=" contained containedin=rim_r_construct_call_web
    hi def link rim_h_clause_call_web    rimClause
    hi def link rim_h_clause_output_call_web    rimClauseOutput
    hi def link rim_h_construct_call_web    rimConstruct
    hi def link rim_h_print_inline_call_web    rimConstruct
syn region rim_r_construct_delete_file start="^[[:space:]]*delete-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_file,rim_r_inline_delete_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_file,rim_r_inline_delete_file,rim_r_at
    syn match rim_h_construct_delete_file "^[[:space:]]*delete-file" contained containedin=rim_r_construct_delete_file
    syn match rim_h_clause_output_delete_file " status \@=" contained containedin=rim_r_construct_delete_file
    hi def link rim_h_clause_delete_file    rimClause
    hi def link rim_h_clause_output_delete_file    rimClauseOutput
    hi def link rim_h_construct_delete_file    rimConstruct
    hi def link rim_h_print_inline_delete_file    rimConstruct
syn region rim_r_construct_change_dir start="^[[:space:]]*change-dir" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_change_dir,rim_r_inline_change_dir,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_change_dir,rim_r_inline_change_dir,rim_r_at
    syn match rim_h_construct_change_dir "^[[:space:]]*change-dir" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_change_dir " home \@=" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_change_dir " home,\@=" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_change_dir " home$" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_change_dir " run-dir \@=" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_change_dir " run-dir,\@=" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_change_dir " run-dir$" contained containedin=rim_r_construct_change_dir
    syn match rim_h_clause_output_change_dir " status \@=" contained containedin=rim_r_construct_change_dir
    hi def link rim_h_clause_change_dir    rimClause
    hi def link rim_h_clause_output_change_dir    rimClauseOutput
    hi def link rim_h_construct_change_dir    rimConstruct
    hi def link rim_h_print_inline_change_dir    rimConstruct
syn region rim_r_construct_new_dir start="^[[:space:]]*new-dir" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_new_dir,rim_r_inline_new_dir,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_new_dir,rim_r_inline_new_dir,rim_r_at
    syn match rim_h_construct_new_dir "^[[:space:]]*new-dir" contained containedin=rim_r_construct_new_dir
    syn match rim_h_clause_new_dir " mode \@=" contained containedin=rim_r_construct_new_dir
    syn match rim_h_clause_output_new_dir " status \@=" contained containedin=rim_r_construct_new_dir
    hi def link rim_h_clause_new_dir    rimClause
    hi def link rim_h_clause_output_new_dir    rimClauseOutput
    hi def link rim_h_construct_new_dir    rimConstruct
    hi def link rim_h_print_inline_new_dir    rimConstruct
syn region rim_r_construct_change_mode start="^[[:space:]]*change-mode" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_change_mode,rim_r_inline_change_mode,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_change_mode,rim_r_inline_change_mode,rim_r_at
    syn match rim_h_construct_change_mode "^[[:space:]]*change-mode" contained containedin=rim_r_construct_change_mode
    syn match rim_h_clause_change_mode " mode \@=" contained containedin=rim_r_construct_change_mode
    syn match rim_h_clause_output_change_mode " status \@=" contained containedin=rim_r_construct_change_mode
    hi def link rim_h_clause_change_mode    rimClause
    hi def link rim_h_clause_output_change_mode    rimClauseOutput
    hi def link rim_h_construct_change_mode    rimConstruct
    hi def link rim_h_print_inline_change_mode    rimConstruct
syn region rim_r_construct_delete_dir start="^[[:space:]]*delete-dir" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_delete_dir,rim_r_inline_delete_dir,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_delete_dir,rim_r_inline_delete_dir,rim_r_at
    syn match rim_h_construct_delete_dir "^[[:space:]]*delete-dir" contained containedin=rim_r_construct_delete_dir
    syn match rim_h_clause_output_delete_dir " status \@=" contained containedin=rim_r_construct_delete_dir
    hi def link rim_h_clause_delete_dir    rimClause
    hi def link rim_h_clause_output_delete_dir    rimClauseOutput
    hi def link rim_h_construct_delete_dir    rimConstruct
    hi def link rim_h_print_inline_delete_dir    rimConstruct
syn region rim_r_construct_rename_file start="^[[:space:]]*rename-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_rename_file,rim_r_inline_rename_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_rename_file,rim_r_inline_rename_file,rim_r_at
    syn match rim_h_construct_rename_file "^[[:space:]]*rename-file" contained containedin=rim_r_construct_rename_file
    syn match rim_h_clause_output_rename_file " status \@=" contained containedin=rim_r_construct_rename_file
    syn match rim_h_clause_rename_file " to \@=" contained containedin=rim_r_construct_rename_file
    hi def link rim_h_clause_rename_file    rimClause
    hi def link rim_h_clause_output_rename_file    rimClauseOutput
    hi def link rim_h_construct_rename_file    rimConstruct
    hi def link rim_h_print_inline_rename_file    rimConstruct
syn region rim_r_construct_text_utf start="^[[:space:]]*text-utf" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_text_utf,rim_r_inline_text_utf,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_text_utf,rim_r_inline_text_utf,rim_r_at
    syn match rim_h_construct_text_utf "^[[:space:]]*text-utf" contained containedin=rim_r_construct_text_utf
    syn match rim_h_clause_output_text_utf " error-text \@=" contained containedin=rim_r_construct_text_utf
    syn match rim_h_clause_output_text_utf " status \@=" contained containedin=rim_r_construct_text_utf
    hi def link rim_h_clause_text_utf    rimClause
    hi def link rim_h_clause_output_text_utf    rimClauseOutput
    hi def link rim_h_construct_text_utf    rimConstruct
    hi def link rim_h_print_inline_text_utf    rimConstruct
syn region rim_r_construct_utf_text start="^[[:space:]]*utf-text" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_utf_text,rim_r_inline_utf_text,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_utf_text,rim_r_inline_utf_text,rim_r_at
    syn match rim_h_construct_utf_text "^[[:space:]]*utf-text" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_output_utf_text " error-text \@=" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_utf_text " for-json \@=" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_utf_text " for-json,\@=" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_utf_text " for-json$" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_utf_text " length \@=" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_output_utf_text " status \@=" contained containedin=rim_r_construct_utf_text
    syn match rim_h_clause_output_utf_text " to \@=" contained containedin=rim_r_construct_utf_text
    hi def link rim_h_clause_utf_text    rimClause
    hi def link rim_h_clause_output_utf_text    rimClauseOutput
    hi def link rim_h_construct_utf_text    rimConstruct
    hi def link rim_h_print_inline_utf_text    rimConstruct
syn region rim_r_construct_stat_file start="^[[:space:]]*stat-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_stat_file,rim_r_inline_stat_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_stat_file,rim_r_inline_stat_file,rim_r_at
    syn match rim_h_construct_stat_file "^[[:space:]]*stat-file" contained containedin=rim_r_construct_stat_file
    syn match rim_h_clause_output_stat_file " mode \@=" contained containedin=rim_r_construct_stat_file
    syn match rim_h_clause_output_stat_file " name \@=" contained containedin=rim_r_construct_stat_file
    syn match rim_h_clause_output_stat_file " path \@=" contained containedin=rim_r_construct_stat_file
    syn match rim_h_clause_output_stat_file " size \@=" contained containedin=rim_r_construct_stat_file
    syn match rim_h_clause_output_stat_file " type \@=" contained containedin=rim_r_construct_stat_file
    hi def link rim_h_clause_stat_file    rimClause
    hi def link rim_h_clause_output_stat_file    rimClauseOutput
    hi def link rim_h_construct_stat_file    rimConstruct
    hi def link rim_h_print_inline_stat_file    rimConstruct
syn region rim_r_construct_decrypt_data start="^[[:space:]]*decrypt-data" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_decrypt_data,rim_r_inline_decrypt_data,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_decrypt_data,rim_r_inline_decrypt_data,rim_r_at
    syn match rim_h_construct_decrypt_data "^[[:space:]]*decrypt-data" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " binary \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " binary,\@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " binary$" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " cache \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " cache,\@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " cache$" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " cipher \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " clear-cache \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " digest \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " init-vector \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " input-length \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " iterations \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " password \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " salt \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_decrypt_data " salt-length \@=" contained containedin=rim_r_construct_decrypt_data
    syn match rim_h_clause_output_decrypt_data " to \@=" contained containedin=rim_r_construct_decrypt_data
    hi def link rim_h_clause_decrypt_data    rimClause
    hi def link rim_h_clause_output_decrypt_data    rimClauseOutput
    hi def link rim_h_construct_decrypt_data    rimConstruct
    hi def link rim_h_print_inline_decrypt_data    rimConstruct
syn region rim_r_construct_encrypt_data start="^[[:space:]]*encrypt-data" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_encrypt_data,rim_r_inline_encrypt_data,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_encrypt_data,rim_r_inline_encrypt_data,rim_r_at
    syn match rim_h_construct_encrypt_data "^[[:space:]]*encrypt-data" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " binary \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " binary,\@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " binary$" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " cache \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " cache,\@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " cache$" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " cipher \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " clear-cache \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " digest \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " init-vector \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " input-length \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " iterations \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " password \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " salt \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_encrypt_data " salt-length \@=" contained containedin=rim_r_construct_encrypt_data
    syn match rim_h_clause_output_encrypt_data " to \@=" contained containedin=rim_r_construct_encrypt_data
    hi def link rim_h_clause_encrypt_data    rimClause
    hi def link rim_h_clause_output_encrypt_data    rimClauseOutput
    hi def link rim_h_construct_encrypt_data    rimConstruct
    hi def link rim_h_print_inline_encrypt_data    rimConstruct
syn region rim_r_construct_trim_string start="^[[:space:]]*trim-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_trim_string,rim_r_inline_trim_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_trim_string,rim_r_inline_trim_string,rim_r_at
    syn match rim_h_construct_trim_string "^[[:space:]]*trim-string" contained containedin=rim_r_construct_trim_string
    hi def link rim_h_clause_trim_string    rimClause
    hi def link rim_h_clause_output_trim_string    rimClauseOutput
    hi def link rim_h_construct_trim_string    rimConstruct
    hi def link rim_h_print_inline_trim_string    rimConstruct
syn region rim_r_construct_encode_web start="^[[:space:]]*encode-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_encode_web,rim_r_inline_encode_web,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_encode_web,rim_r_inline_encode_web,rim_r_at
    syn match rim_h_construct_encode_web "^[[:space:]]*encode-web" contained containedin=rim_r_construct_encode_web
    syn match rim_h_clause_encode_web " input-length \@=" contained containedin=rim_r_construct_encode_web
    syn match rim_h_clause_output_encode_web " to \@=" contained containedin=rim_r_construct_encode_web
    hi def link rim_h_clause_encode_web    rimClause
    hi def link rim_h_clause_output_encode_web    rimClauseOutput
    hi def link rim_h_construct_encode_web    rimConstruct
    hi def link rim_h_print_inline_encode_web    rimConstruct
syn region rim_r_construct_encode_url start="^[[:space:]]*encode-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_encode_url,rim_r_inline_encode_url,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_encode_url,rim_r_inline_encode_url,rim_r_at
    syn match rim_h_construct_encode_url "^[[:space:]]*encode-url" contained containedin=rim_r_construct_encode_url
    syn match rim_h_clause_encode_url " input-length \@=" contained containedin=rim_r_construct_encode_url
    syn match rim_h_clause_output_encode_url " to \@=" contained containedin=rim_r_construct_encode_url
    hi def link rim_h_clause_encode_url    rimClause
    hi def link rim_h_clause_output_encode_url    rimClauseOutput
    hi def link rim_h_construct_encode_url    rimConstruct
    hi def link rim_h_print_inline_encode_url    rimConstruct
syn region rim_r_construct_decode_web start="^[[:space:]]*decode-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_decode_web,rim_r_inline_decode_web,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_decode_web,rim_r_inline_decode_web,rim_r_at
    syn match rim_h_construct_decode_web "^[[:space:]]*decode-web" contained containedin=rim_r_construct_decode_web
    syn match rim_h_clause_decode_web " input-length \@=" contained containedin=rim_r_construct_decode_web
    syn match rim_h_clause_decode_web " status \@=" contained containedin=rim_r_construct_decode_web
    hi def link rim_h_clause_decode_web    rimClause
    hi def link rim_h_clause_output_decode_web    rimClauseOutput
    hi def link rim_h_construct_decode_web    rimConstruct
    hi def link rim_h_print_inline_decode_web    rimConstruct
syn region rim_r_construct_decode_url start="^[[:space:]]*decode-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_decode_url,rim_r_inline_decode_url,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_decode_url,rim_r_inline_decode_url,rim_r_at
    syn match rim_h_construct_decode_url "^[[:space:]]*decode-url" contained containedin=rim_r_construct_decode_url
    syn match rim_h_clause_decode_url " input-length \@=" contained containedin=rim_r_construct_decode_url
    syn match rim_h_clause_decode_url " status \@=" contained containedin=rim_r_construct_decode_url
    hi def link rim_h_clause_decode_url    rimClause
    hi def link rim_h_clause_output_decode_url    rimClauseOutput
    hi def link rim_h_construct_decode_url    rimConstruct
    hi def link rim_h_print_inline_decode_url    rimConstruct
syn region rim_r_construct_decode_hex start="^[[:space:]]*decode-hex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_decode_hex,rim_r_inline_decode_hex,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_decode_hex,rim_r_inline_decode_hex,rim_r_at
    syn match rim_h_construct_decode_hex "^[[:space:]]*decode-hex" contained containedin=rim_r_construct_decode_hex
    syn match rim_h_clause_decode_hex " input-length \@=" contained containedin=rim_r_construct_decode_hex
    syn match rim_h_clause_output_decode_hex " to \@=" contained containedin=rim_r_construct_decode_hex
    hi def link rim_h_clause_decode_hex    rimClause
    hi def link rim_h_clause_output_decode_hex    rimClauseOutput
    hi def link rim_h_construct_decode_hex    rimConstruct
    hi def link rim_h_print_inline_decode_hex    rimConstruct
syn region rim_r_construct_encode_hex start="^[[:space:]]*encode-hex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_encode_hex,rim_r_inline_encode_hex,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_encode_hex,rim_r_inline_encode_hex,rim_r_at
    syn match rim_h_construct_encode_hex "^[[:space:]]*encode-hex" contained containedin=rim_r_construct_encode_hex
    syn match rim_h_clause_encode_hex " input-length \@=" contained containedin=rim_r_construct_encode_hex
    syn match rim_h_clause_encode_hex " prefix \@=" contained containedin=rim_r_construct_encode_hex
    syn match rim_h_clause_output_encode_hex " to \@=" contained containedin=rim_r_construct_encode_hex
    hi def link rim_h_clause_encode_hex    rimClause
    hi def link rim_h_clause_output_encode_hex    rimClauseOutput
    hi def link rim_h_construct_encode_hex    rimConstruct
    hi def link rim_h_print_inline_encode_hex    rimConstruct
syn region rim_r_construct_encode_base64 start="^[[:space:]]*encode-base64" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_encode_base64,rim_r_inline_encode_base64,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_encode_base64,rim_r_inline_encode_base64,rim_r_at
    syn match rim_h_construct_encode_base64 "^[[:space:]]*encode-base64" contained containedin=rim_r_construct_encode_base64
    syn match rim_h_clause_encode_base64 " input-length \@=" contained containedin=rim_r_construct_encode_base64
    syn match rim_h_clause_output_encode_base64 " to \@=" contained containedin=rim_r_construct_encode_base64
    hi def link rim_h_clause_encode_base64    rimClause
    hi def link rim_h_clause_output_encode_base64    rimClauseOutput
    hi def link rim_h_construct_encode_base64    rimConstruct
    hi def link rim_h_print_inline_encode_base64    rimConstruct
syn region rim_r_construct_decode_base64 start="^[[:space:]]*decode-base64" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_decode_base64,rim_r_inline_decode_base64,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_decode_base64,rim_r_inline_decode_base64,rim_r_at
    syn match rim_h_construct_decode_base64 "^[[:space:]]*decode-base64" contained containedin=rim_r_construct_decode_base64
    syn match rim_h_clause_decode_base64 " input-length \@=" contained containedin=rim_r_construct_decode_base64
    syn match rim_h_clause_output_decode_base64 " to \@=" contained containedin=rim_r_construct_decode_base64
    hi def link rim_h_clause_decode_base64    rimClause
    hi def link rim_h_clause_output_decode_base64    rimClauseOutput
    hi def link rim_h_construct_decode_base64    rimConstruct
    hi def link rim_h_print_inline_decode_base64    rimConstruct
syn region rim_r_construct_hash_string start="^[[:space:]]*hash-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_hash_string,rim_r_inline_hash_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_hash_string,rim_r_inline_hash_string,rim_r_at
    syn match rim_h_construct_hash_string "^[[:space:]]*hash-string" contained containedin=rim_r_construct_hash_string
    syn match rim_h_clause_hash_string " binary \@=" contained containedin=rim_r_construct_hash_string
    syn match rim_h_clause_hash_string " binary,\@=" contained containedin=rim_r_construct_hash_string
    syn match rim_h_clause_hash_string " binary$" contained containedin=rim_r_construct_hash_string
    syn match rim_h_clause_hash_string " digest \@=" contained containedin=rim_r_construct_hash_string
    syn match rim_h_clause_output_hash_string " to \@=" contained containedin=rim_r_construct_hash_string
    hi def link rim_h_clause_hash_string    rimClause
    hi def link rim_h_clause_output_hash_string    rimClauseOutput
    hi def link rim_h_construct_hash_string    rimConstruct
    hi def link rim_h_print_inline_hash_string    rimConstruct
syn region rim_r_construct_hmac_string start="^[[:space:]]*hmac-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_hmac_string,rim_r_inline_hmac_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_hmac_string,rim_r_inline_hmac_string,rim_r_at
    syn match rim_h_construct_hmac_string "^[[:space:]]*hmac-string" contained containedin=rim_r_construct_hmac_string
    syn match rim_h_clause_hmac_string " binary \@=" contained containedin=rim_r_construct_hmac_string
    syn match rim_h_clause_hmac_string " binary,\@=" contained containedin=rim_r_construct_hmac_string
    syn match rim_h_clause_hmac_string " binary$" contained containedin=rim_r_construct_hmac_string
    syn match rim_h_clause_hmac_string " digest \@=" contained containedin=rim_r_construct_hmac_string
    syn match rim_h_clause_hmac_string " key \@=" contained containedin=rim_r_construct_hmac_string
    syn match rim_h_clause_output_hmac_string " to \@=" contained containedin=rim_r_construct_hmac_string
    hi def link rim_h_clause_hmac_string    rimClause
    hi def link rim_h_clause_output_hmac_string    rimClauseOutput
    hi def link rim_h_construct_hmac_string    rimConstruct
    hi def link rim_h_print_inline_hmac_string    rimConstruct
syn region rim_r_construct_derive_key start="^[[:space:]]*derive-key" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_derive_key,rim_r_inline_derive_key,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_derive_key,rim_r_inline_derive_key,rim_r_at
    syn match rim_h_construct_derive_key "^[[:space:]]*derive-key" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " binary \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " binary,\@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " binary$" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " digest \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " from \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " from-length \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " iterations \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " length \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " salt \@=" contained containedin=rim_r_construct_derive_key
    syn match rim_h_clause_derive_key " salt-length \@=" contained containedin=rim_r_construct_derive_key
    hi def link rim_h_clause_derive_key    rimClause
    hi def link rim_h_clause_output_derive_key    rimClauseOutput
    hi def link rim_h_construct_derive_key    rimConstruct
    hi def link rim_h_print_inline_derive_key    rimConstruct
syn region rim_r_construct_count_substring start="^[[:space:]]*count-substring" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_count_substring,rim_r_inline_count_substring,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_count_substring,rim_r_inline_count_substring,rim_r_at
    syn match rim_h_construct_count_substring "^[[:space:]]*count-substring" contained containedin=rim_r_construct_count_substring
    syn match rim_h_clause_count_substring " case-insensitive \@=" contained containedin=rim_r_construct_count_substring
    syn match rim_h_clause_count_substring " case-insensitive,\@=" contained containedin=rim_r_construct_count_substring
    syn match rim_h_clause_count_substring " case-insensitive$" contained containedin=rim_r_construct_count_substring
    syn match rim_h_clause_count_substring " in \@=" contained containedin=rim_r_construct_count_substring
    syn match rim_h_clause_output_count_substring " to \@=" contained containedin=rim_r_construct_count_substring
    hi def link rim_h_clause_count_substring    rimClause
    hi def link rim_h_clause_output_count_substring    rimClauseOutput
    hi def link rim_h_construct_count_substring    rimConstruct
    hi def link rim_h_print_inline_count_substring    rimConstruct
syn region rim_r_construct_string_length start="^[[:space:]]*string-length" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_string_length,rim_r_inline_string_length,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_string_length,rim_r_inline_string_length,rim_r_at
    syn match rim_h_construct_string_length "^[[:space:]]*string-length" contained containedin=rim_r_construct_string_length
    syn match rim_h_clause_output_string_length " to \@=" contained containedin=rim_r_construct_string_length
    syn match rim_h_print_inline_string_length " to \@=" contained containedin=rim_r_inline_string_length
    syn region rim_r_inline_string_length start="<<[[:space:]]*string-length \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_string_length '<<[[:space:]]*string-length \@=' contained containedin=rim_r_inline_string_length
    syn match rim_h_print_inline_string_length '>>' contained containedin=rim_r_inline_string_length
    hi def link rim_h_clause_string_length    rimClause
    hi def link rim_h_clause_output_string_length    rimClauseOutput
    hi def link rim_h_construct_string_length    rimConstruct
    hi def link rim_h_print_inline_string_length    rimConstruct
syn region rim_r_construct_lower_string start="^[[:space:]]*lower-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_lower_string,rim_r_inline_lower_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_lower_string,rim_r_inline_lower_string,rim_r_at
    syn match rim_h_construct_lower_string "^[[:space:]]*lower-string" contained containedin=rim_r_construct_lower_string
    hi def link rim_h_clause_lower_string    rimClause
    hi def link rim_h_clause_output_lower_string    rimClauseOutput
    hi def link rim_h_construct_lower_string    rimConstruct
    hi def link rim_h_print_inline_lower_string    rimConstruct
syn region rim_r_construct_upper_string start="^[[:space:]]*upper-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_upper_string,rim_r_inline_upper_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_upper_string,rim_r_inline_upper_string,rim_r_at
    syn match rim_h_construct_upper_string "^[[:space:]]*upper-string" contained containedin=rim_r_construct_upper_string
    hi def link rim_h_clause_upper_string    rimClause
    hi def link rim_h_clause_output_upper_string    rimClauseOutput
    hi def link rim_h_construct_upper_string    rimConstruct
    hi def link rim_h_print_inline_upper_string    rimConstruct
syn region rim_r_construct_random_crypto start="^[[:space:]]*random-crypto" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_random_crypto,rim_r_inline_random_crypto,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_random_crypto,rim_r_inline_random_crypto,rim_r_at
    syn match rim_h_construct_random_crypto "^[[:space:]]*random-crypto" contained containedin=rim_r_construct_random_crypto
    syn match rim_h_clause_random_crypto " length \@=" contained containedin=rim_r_construct_random_crypto
    syn match rim_h_clause_output_random_crypto " to \@=" contained containedin=rim_r_construct_random_crypto
    hi def link rim_h_clause_random_crypto    rimClause
    hi def link rim_h_clause_output_random_crypto    rimClauseOutput
    hi def link rim_h_construct_random_crypto    rimConstruct
    hi def link rim_h_print_inline_random_crypto    rimConstruct
syn region rim_r_construct_random_string start="^[[:space:]]*random-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_random_string,rim_r_inline_random_string,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_random_string,rim_r_inline_random_string,rim_r_at
    syn match rim_h_construct_random_string "^[[:space:]]*random-string" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " binary \@=" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " binary,\@=" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " binary$" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " length \@=" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " number \@=" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " number,\@=" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_random_string " number$" contained containedin=rim_r_construct_random_string
    syn match rim_h_clause_output_random_string " to \@=" contained containedin=rim_r_construct_random_string
    hi def link rim_h_clause_random_string    rimClause
    hi def link rim_h_clause_output_random_string    rimClauseOutput
    hi def link rim_h_construct_random_string    rimConstruct
    hi def link rim_h_print_inline_random_string    rimConstruct
syn region rim_r_construct_unlock_file start="^[[:space:]]*unlock-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_unlock_file,rim_r_inline_unlock_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_unlock_file,rim_r_inline_unlock_file,rim_r_at
    syn match rim_h_construct_unlock_file "^[[:space:]]*unlock-file" contained containedin=rim_r_construct_unlock_file
    syn match rim_h_clause_unlock_file " id \@=" contained containedin=rim_r_construct_unlock_file
    hi def link rim_h_clause_unlock_file    rimClause
    hi def link rim_h_clause_output_unlock_file    rimClauseOutput
    hi def link rim_h_construct_unlock_file    rimConstruct
    hi def link rim_h_print_inline_unlock_file    rimConstruct
syn region rim_r_construct_lock_file start="^[[:space:]]*lock-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_lock_file,rim_r_inline_lock_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_lock_file,rim_r_inline_lock_file,rim_r_at
    syn match rim_h_construct_lock_file "^[[:space:]]*lock-file" contained containedin=rim_r_construct_lock_file
    syn match rim_h_clause_output_lock_file " id \@=" contained containedin=rim_r_construct_lock_file
    syn match rim_h_clause_output_lock_file " status \@=" contained containedin=rim_r_construct_lock_file
    hi def link rim_h_clause_lock_file    rimClause
    hi def link rim_h_clause_output_lock_file    rimClauseOutput
    hi def link rim_h_construct_lock_file    rimConstruct
    hi def link rim_h_print_inline_lock_file    rimConstruct
syn region rim_r_construct_send_file start="^[[:space:]]*send-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_send_file,rim_r_inline_send_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_send_file,rim_r_inline_send_file,rim_r_at
    syn match rim_h_construct_send_file "^[[:space:]]*send-file" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " cache-control \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " content-type \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " custom \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " download \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " download,\@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " download$" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " etag \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " etag,\@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " etag$" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " file-name \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " headers \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " no-cache \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " no-cache,\@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " no-cache$" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " status-id \@=" contained containedin=rim_r_construct_send_file
    syn match rim_h_clause_send_file " status-text \@=" contained containedin=rim_r_construct_send_file
    hi def link rim_h_clause_send_file    rimClause
    hi def link rim_h_clause_output_send_file    rimClauseOutput
    hi def link rim_h_construct_send_file    rimConstruct
    hi def link rim_h_print_inline_send_file    rimConstruct
syn region rim_r_construct_exec_program start="^[[:space:]]*exec-program" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_exec_program,rim_r_inline_exec_program,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_exec_program,rim_r_inline_exec_program,rim_r_at
    syn match rim_h_construct_exec_program "^[[:space:]]*exec-program" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_exec_program " args \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_output_exec_program " error \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_exec_program " error-file \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_exec_program " input \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_exec_program " input-file \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_exec_program " input-length \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_output_exec_program " output \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_exec_program " output-file \@=" contained containedin=rim_r_construct_exec_program
    syn match rim_h_clause_output_exec_program " status \@=" contained containedin=rim_r_construct_exec_program
    hi def link rim_h_clause_exec_program    rimClause
    hi def link rim_h_clause_output_exec_program    rimClauseOutput
    hi def link rim_h_construct_exec_program    rimConstruct
    hi def link rim_h_print_inline_exec_program    rimConstruct
syn region rim_r_construct_out_header start="^[[:space:]]*out-header" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_out_header,rim_r_inline_out_header,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_out_header,rim_r_inline_out_header,rim_r_at
    syn match rim_h_construct_out_header "^[[:space:]]*out-header" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " cache-control \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " content-type \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " custom \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " default \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " default,\@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " default$" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " download \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " download,\@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " download$" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " etag \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " etag,\@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " etag$" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " file-name \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " no-cache \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " no-cache,\@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " no-cache$" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " status-id \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " status-text \@=" contained containedin=rim_r_construct_out_header
    syn match rim_h_clause_out_header " use \@=" contained containedin=rim_r_construct_out_header
    hi def link rim_h_clause_out_header    rimClause
    hi def link rim_h_clause_output_out_header    rimClauseOutput
    hi def link rim_h_construct_out_header    rimConstruct
    hi def link rim_h_print_inline_out_header    rimConstruct
syn region rim_r_construct_silent_header start="^[[:space:]]*silent-header" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_silent_header,rim_r_inline_silent_header,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_silent_header,rim_r_inline_silent_header,rim_r_at
    syn match rim_h_construct_silent_header "^[[:space:]]*silent-header" contained containedin=rim_r_construct_silent_header
    hi def link rim_h_clause_silent_header    rimClause
    hi def link rim_h_clause_output_silent_header    rimClauseOutput
    hi def link rim_h_construct_silent_header    rimConstruct
    hi def link rim_h_print_inline_silent_header    rimConstruct
syn region rim_r_construct_exit_status start="^[[:space:]]*exit-status" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_exit_status,rim_r_inline_exit_status,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_exit_status,rim_r_inline_exit_status,rim_r_at
    syn match rim_h_construct_exit_status "^[[:space:]]*exit-status" contained containedin=rim_r_construct_exit_status
    hi def link rim_h_clause_exit_status    rimClause
    hi def link rim_h_clause_output_exit_status    rimClauseOutput
    hi def link rim_h_construct_exit_status    rimConstruct
    hi def link rim_h_print_inline_exit_status    rimConstruct
syn region rim_r_construct_report_error start="^[[:space:]]*report-error" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_report_error,rim_r_inline_report_error,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_report_error,rim_r_inline_report_error,rim_r_at
    syn match rim_h_construct_report_error "^[[:space:]]*report-error" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " , \@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " ,,\@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " ,$" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " to \@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " to-error \@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " to-error,\@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " to-error$" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " url-encode \@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " url-encode,\@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " url-encode$" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " web-encode \@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " web-encode,\@=" contained containedin=rim_r_construct_report_error
    syn match rim_h_clause_report_error " web-encode$" contained containedin=rim_r_construct_report_error
    hi def link rim_h_clause_report_error    rimClause
    hi def link rim_h_clause_output_report_error    rimClauseOutput
    hi def link rim_h_construct_report_error    rimConstruct
    hi def link rim_h_print_inline_report_error    rimConstruct
syn region rim_r_construct_print_format start="^[[:space:]]*print-format" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_print_format,rim_r_inline_print_format,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_print_format,rim_r_inline_print_format,rim_r_at
    syn match rim_h_construct_print_format "^[[:space:]]*print-format" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " , \@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " ,,\@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " ,$" contained containedin=rim_r_construct_print_format
    syn match rim_h_print_inline_print_format " ,\(>>\)\@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_print_inline_print_format " , \@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_clause_print_format " to \@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_print_inline_print_format " to \@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_clause_print_format " to-error \@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " to-error,\@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " to-error$" contained containedin=rim_r_construct_print_format
    syn match rim_h_print_inline_print_format " to-error\(>>\)\@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_print_inline_print_format " to-error \@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_clause_print_format " url-encode \@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " url-encode,\@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " url-encode$" contained containedin=rim_r_construct_print_format
    syn match rim_h_print_inline_print_format " url-encode\(>>\)\@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_print_inline_print_format " url-encode \@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_clause_print_format " web-encode \@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " web-encode,\@=" contained containedin=rim_r_construct_print_format
    syn match rim_h_clause_print_format " web-encode$" contained containedin=rim_r_construct_print_format
    syn match rim_h_print_inline_print_format " web-encode\(>>\)\@=" contained containedin=rim_r_inline_print_format
    syn match rim_h_print_inline_print_format " web-encode \@=" contained containedin=rim_r_inline_print_format
    syn region rim_r_inline_print_format start="<<[[:space:]]*print-format \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_print_format '<<[[:space:]]*print-format \@=' contained containedin=rim_r_inline_print_format
    syn match rim_h_print_inline_print_format '>>' contained containedin=rim_r_inline_print_format
    hi def link rim_h_clause_print_format    rimClause
    hi def link rim_h_clause_output_print_format    rimClauseOutput
    hi def link rim_h_construct_print_format    rimConstruct
    hi def link rim_h_print_inline_print_format    rimConstruct
syn region rim_r_construct_print_out start="^[[:space:]]*print-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_print_out,rim_r_inline_print_out,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_print_out,rim_r_inline_print_out,rim_r_at
    syn match rim_h_construct_print_out "^[[:space:]]*print-out" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " , \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " ,,\@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " ,$" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " ,\(>>\)\@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out " , \@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_clause_print_out " length \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " length \@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_clause_print_out " new-line \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " new-line,\@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " new-line$" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " new-line\(>>\)\@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out " new-line \@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_clause_print_out " source-file \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " source-file,\@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " source-file$" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " source-file\(>>\)\@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out " source-file \@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_clause_print_out " source-line \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " source-line,\@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " source-line$" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " source-line\(>>\)\@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out " source-line \@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_clause_print_out " url-encode \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " url-encode,\@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " url-encode$" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " url-encode\(>>\)\@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out " url-encode \@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_clause_print_out " web-encode \@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " web-encode,\@=" contained containedin=rim_r_construct_print_out
    syn match rim_h_clause_print_out " web-encode$" contained containedin=rim_r_construct_print_out
    syn match rim_h_print_inline_print_out " web-encode\(>>\)\@=" contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out " web-encode \@=" contained containedin=rim_r_inline_print_out
    syn region rim_r_inline_print_out start="<<[[:space:]]*print-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_print_out '<<[[:space:]]*print-out \@=' contained containedin=rim_r_inline_print_out
    syn match rim_h_print_inline_print_out '>>' contained containedin=rim_r_inline_print_out
    hi def link rim_h_clause_print_out    rimClause
    hi def link rim_h_clause_output_print_out    rimClauseOutput
    hi def link rim_h_construct_print_out    rimConstruct
    hi def link rim_h_print_inline_print_out    rimConstruct
syn region rim_r_construct_p_path start="^[[:space:]]*p-path" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_path,rim_r_inline_p_path,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_path,rim_r_inline_p_path,rim_r_at
    syn match rim_h_construct_p_path "^[[:space:]]*p-path" contained containedin=rim_r_construct_p_path
    syn match rim_h_clause_p_path " new-line \@=" contained containedin=rim_r_construct_p_path
    syn match rim_h_clause_p_path " new-line,\@=" contained containedin=rim_r_construct_p_path
    syn match rim_h_clause_p_path " new-line$" contained containedin=rim_r_construct_p_path
    syn match rim_h_print_inline_p_path " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_path
    syn match rim_h_print_inline_p_path " new-line \@=" contained containedin=rim_r_inline_p_path
    syn region rim_r_inline_p_path start="<<[[:space:]]*p-path \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_path '<<[[:space:]]*p-path \@=' contained containedin=rim_r_inline_p_path
    syn match rim_h_print_inline_p_path '>>' contained containedin=rim_r_inline_p_path
    hi def link rim_h_clause_p_path    rimClause
    hi def link rim_h_clause_output_p_path    rimClauseOutput
    hi def link rim_h_construct_p_path    rimConstruct
    hi def link rim_h_print_inline_p_path    rimConstruct
syn region rim_r_construct_print_path start="^[[:space:]]*print-path" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_print_path,rim_r_inline_print_path,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_print_path,rim_r_inline_print_path,rim_r_at
    syn match rim_h_construct_print_path "^[[:space:]]*print-path" contained containedin=rim_r_construct_print_path
    syn match rim_h_clause_print_path " new-line \@=" contained containedin=rim_r_construct_print_path
    syn match rim_h_clause_print_path " new-line,\@=" contained containedin=rim_r_construct_print_path
    syn match rim_h_clause_print_path " new-line$" contained containedin=rim_r_construct_print_path
    syn match rim_h_print_inline_print_path " new-line\(>>\)\@=" contained containedin=rim_r_inline_print_path
    syn match rim_h_print_inline_print_path " new-line \@=" contained containedin=rim_r_inline_print_path
    syn region rim_r_inline_print_path start="<<[[:space:]]*print-path \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_print_path '<<[[:space:]]*print-path \@=' contained containedin=rim_r_inline_print_path
    syn match rim_h_print_inline_print_path '>>' contained containedin=rim_r_inline_print_path
    hi def link rim_h_clause_print_path    rimClause
    hi def link rim_h_clause_output_print_path    rimClauseOutput
    hi def link rim_h_construct_print_path    rimConstruct
    hi def link rim_h_print_inline_print_path    rimConstruct
syn region rim_r_construct_pf_out start="^[[:space:]]*pf-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_pf_out,rim_r_inline_pf_out,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_pf_out,rim_r_inline_pf_out,rim_r_at
    syn match rim_h_construct_pf_out "^[[:space:]]*pf-out" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " , \@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " ,,\@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " ,$" contained containedin=rim_r_construct_pf_out
    syn match rim_h_print_inline_pf_out " ,\(>>\)\@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_print_inline_pf_out " , \@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_clause_pf_out " to \@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_print_inline_pf_out " to \@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_clause_pf_out " to-error \@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " to-error,\@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " to-error$" contained containedin=rim_r_construct_pf_out
    syn match rim_h_print_inline_pf_out " to-error\(>>\)\@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_print_inline_pf_out " to-error \@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_clause_pf_out " url-encode \@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " url-encode,\@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " url-encode$" contained containedin=rim_r_construct_pf_out
    syn match rim_h_print_inline_pf_out " url-encode\(>>\)\@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_print_inline_pf_out " url-encode \@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_clause_pf_out " web-encode \@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " web-encode,\@=" contained containedin=rim_r_construct_pf_out
    syn match rim_h_clause_pf_out " web-encode$" contained containedin=rim_r_construct_pf_out
    syn match rim_h_print_inline_pf_out " web-encode\(>>\)\@=" contained containedin=rim_r_inline_pf_out
    syn match rim_h_print_inline_pf_out " web-encode \@=" contained containedin=rim_r_inline_pf_out
    syn region rim_r_inline_pf_out start="<<[[:space:]]*pf-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_pf_out '<<[[:space:]]*pf-out \@=' contained containedin=rim_r_inline_pf_out
    syn match rim_h_print_inline_pf_out '>>' contained containedin=rim_r_inline_pf_out
    hi def link rim_h_clause_pf_out    rimClause
    hi def link rim_h_clause_output_pf_out    rimClauseOutput
    hi def link rim_h_construct_pf_out    rimConstruct
    hi def link rim_h_print_inline_pf_out    rimConstruct
syn region rim_r_construct_pf_url start="^[[:space:]]*pf-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_pf_url,rim_r_inline_pf_url,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_pf_url,rim_r_inline_pf_url,rim_r_at
    syn match rim_h_construct_pf_url "^[[:space:]]*pf-url" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " , \@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " ,,\@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " ,$" contained containedin=rim_r_construct_pf_url
    syn match rim_h_print_inline_pf_url " ,\(>>\)\@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_print_inline_pf_url " , \@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_clause_pf_url " to \@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_print_inline_pf_url " to \@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_clause_pf_url " to-error \@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " to-error,\@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " to-error$" contained containedin=rim_r_construct_pf_url
    syn match rim_h_print_inline_pf_url " to-error\(>>\)\@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_print_inline_pf_url " to-error \@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_clause_pf_url " url-encode \@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " url-encode,\@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " url-encode$" contained containedin=rim_r_construct_pf_url
    syn match rim_h_print_inline_pf_url " url-encode\(>>\)\@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_print_inline_pf_url " url-encode \@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_clause_pf_url " web-encode \@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " web-encode,\@=" contained containedin=rim_r_construct_pf_url
    syn match rim_h_clause_pf_url " web-encode$" contained containedin=rim_r_construct_pf_url
    syn match rim_h_print_inline_pf_url " web-encode\(>>\)\@=" contained containedin=rim_r_inline_pf_url
    syn match rim_h_print_inline_pf_url " web-encode \@=" contained containedin=rim_r_inline_pf_url
    syn region rim_r_inline_pf_url start="<<[[:space:]]*pf-url \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_pf_url '<<[[:space:]]*pf-url \@=' contained containedin=rim_r_inline_pf_url
    syn match rim_h_print_inline_pf_url '>>' contained containedin=rim_r_inline_pf_url
    hi def link rim_h_clause_pf_url    rimClause
    hi def link rim_h_clause_output_pf_url    rimClauseOutput
    hi def link rim_h_construct_pf_url    rimConstruct
    hi def link rim_h_print_inline_pf_url    rimConstruct
syn region rim_r_construct_pf_web start="^[[:space:]]*pf-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_pf_web,rim_r_inline_pf_web,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_pf_web,rim_r_inline_pf_web,rim_r_at
    syn match rim_h_construct_pf_web "^[[:space:]]*pf-web" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " , \@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " ,,\@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " ,$" contained containedin=rim_r_construct_pf_web
    syn match rim_h_print_inline_pf_web " ,\(>>\)\@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_print_inline_pf_web " , \@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_clause_pf_web " to \@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_print_inline_pf_web " to \@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_clause_pf_web " to-error \@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " to-error,\@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " to-error$" contained containedin=rim_r_construct_pf_web
    syn match rim_h_print_inline_pf_web " to-error\(>>\)\@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_print_inline_pf_web " to-error \@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_clause_pf_web " url-encode \@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " url-encode,\@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " url-encode$" contained containedin=rim_r_construct_pf_web
    syn match rim_h_print_inline_pf_web " url-encode\(>>\)\@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_print_inline_pf_web " url-encode \@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_clause_pf_web " web-encode \@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " web-encode,\@=" contained containedin=rim_r_construct_pf_web
    syn match rim_h_clause_pf_web " web-encode$" contained containedin=rim_r_construct_pf_web
    syn match rim_h_print_inline_pf_web " web-encode\(>>\)\@=" contained containedin=rim_r_inline_pf_web
    syn match rim_h_print_inline_pf_web " web-encode \@=" contained containedin=rim_r_inline_pf_web
    syn region rim_r_inline_pf_web start="<<[[:space:]]*pf-web \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_pf_web '<<[[:space:]]*pf-web \@=' contained containedin=rim_r_inline_pf_web
    syn match rim_h_print_inline_pf_web '>>' contained containedin=rim_r_inline_pf_web
    hi def link rim_h_clause_pf_web    rimClause
    hi def link rim_h_clause_output_pf_web    rimClauseOutput
    hi def link rim_h_construct_pf_web    rimConstruct
    hi def link rim_h_print_inline_pf_web    rimConstruct
syn region rim_r_construct_p_web start="^[[:space:]]*p-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_web,rim_r_inline_p_web,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_web,rim_r_inline_p_web,rim_r_at
    syn match rim_h_construct_p_web "^[[:space:]]*p-web" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " length \@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_print_inline_p_web " length \@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_clause_p_web " new-line \@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " new-line,\@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " new-line$" contained containedin=rim_r_construct_p_web
    syn match rim_h_print_inline_p_web " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_print_inline_p_web " new-line \@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_clause_p_web " source-file \@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " source-file,\@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " source-file$" contained containedin=rim_r_construct_p_web
    syn match rim_h_print_inline_p_web " source-file\(>>\)\@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_print_inline_p_web " source-file \@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_clause_p_web " source-line \@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " source-line,\@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " source-line$" contained containedin=rim_r_construct_p_web
    syn match rim_h_print_inline_p_web " source-line\(>>\)\@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_print_inline_p_web " source-line \@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_clause_p_web " url-encode \@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " url-encode,\@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " url-encode$" contained containedin=rim_r_construct_p_web
    syn match rim_h_print_inline_p_web " url-encode\(>>\)\@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_print_inline_p_web " url-encode \@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_clause_p_web " web-encode \@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " web-encode,\@=" contained containedin=rim_r_construct_p_web
    syn match rim_h_clause_p_web " web-encode$" contained containedin=rim_r_construct_p_web
    syn match rim_h_print_inline_p_web " web-encode\(>>\)\@=" contained containedin=rim_r_inline_p_web
    syn match rim_h_print_inline_p_web " web-encode \@=" contained containedin=rim_r_inline_p_web
    syn region rim_r_inline_p_web start="<<[[:space:]]*p-web \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_web '<<[[:space:]]*p-web \@=' contained containedin=rim_r_inline_p_web
    syn match rim_h_print_inline_p_web '>>' contained containedin=rim_r_inline_p_web
    hi def link rim_h_clause_p_web    rimClause
    hi def link rim_h_clause_output_p_web    rimClauseOutput
    hi def link rim_h_construct_p_web    rimConstruct
    hi def link rim_h_print_inline_p_web    rimConstruct
syn region rim_r_construct_p_url start="^[[:space:]]*p-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_url,rim_r_inline_p_url,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_url,rim_r_inline_p_url,rim_r_at
    syn match rim_h_construct_p_url "^[[:space:]]*p-url" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " length \@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_print_inline_p_url " length \@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_clause_p_url " new-line \@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " new-line,\@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " new-line$" contained containedin=rim_r_construct_p_url
    syn match rim_h_print_inline_p_url " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_print_inline_p_url " new-line \@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_clause_p_url " source-file \@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " source-file,\@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " source-file$" contained containedin=rim_r_construct_p_url
    syn match rim_h_print_inline_p_url " source-file\(>>\)\@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_print_inline_p_url " source-file \@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_clause_p_url " source-line \@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " source-line,\@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " source-line$" contained containedin=rim_r_construct_p_url
    syn match rim_h_print_inline_p_url " source-line\(>>\)\@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_print_inline_p_url " source-line \@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_clause_p_url " url-encode \@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " url-encode,\@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " url-encode$" contained containedin=rim_r_construct_p_url
    syn match rim_h_print_inline_p_url " url-encode\(>>\)\@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_print_inline_p_url " url-encode \@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_clause_p_url " web-encode \@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " web-encode,\@=" contained containedin=rim_r_construct_p_url
    syn match rim_h_clause_p_url " web-encode$" contained containedin=rim_r_construct_p_url
    syn match rim_h_print_inline_p_url " web-encode\(>>\)\@=" contained containedin=rim_r_inline_p_url
    syn match rim_h_print_inline_p_url " web-encode \@=" contained containedin=rim_r_inline_p_url
    syn region rim_r_inline_p_url start="<<[[:space:]]*p-url \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_url '<<[[:space:]]*p-url \@=' contained containedin=rim_r_inline_p_url
    syn match rim_h_print_inline_p_url '>>' contained containedin=rim_r_inline_p_url
    hi def link rim_h_clause_p_url    rimClause
    hi def link rim_h_clause_output_p_url    rimClauseOutput
    hi def link rim_h_construct_p_url    rimConstruct
    hi def link rim_h_print_inline_p_url    rimConstruct
syn region rim_r_construct_p_num start="^[[:space:]]*p-num" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_num,rim_r_inline_p_num,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_num,rim_r_inline_p_num,rim_r_at
    syn match rim_h_construct_p_num "^[[:space:]]*p-num" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " length \@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_print_inline_p_num " length \@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_clause_p_num " new-line \@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " new-line,\@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " new-line$" contained containedin=rim_r_construct_p_num
    syn match rim_h_print_inline_p_num " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_print_inline_p_num " new-line \@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_clause_p_num " source-file \@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " source-file,\@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " source-file$" contained containedin=rim_r_construct_p_num
    syn match rim_h_print_inline_p_num " source-file\(>>\)\@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_print_inline_p_num " source-file \@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_clause_p_num " source-line \@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " source-line,\@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " source-line$" contained containedin=rim_r_construct_p_num
    syn match rim_h_print_inline_p_num " source-line\(>>\)\@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_print_inline_p_num " source-line \@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_clause_p_num " url-encode \@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " url-encode,\@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " url-encode$" contained containedin=rim_r_construct_p_num
    syn match rim_h_print_inline_p_num " url-encode\(>>\)\@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_print_inline_p_num " url-encode \@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_clause_p_num " web-encode \@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " web-encode,\@=" contained containedin=rim_r_construct_p_num
    syn match rim_h_clause_p_num " web-encode$" contained containedin=rim_r_construct_p_num
    syn match rim_h_print_inline_p_num " web-encode\(>>\)\@=" contained containedin=rim_r_inline_p_num
    syn match rim_h_print_inline_p_num " web-encode \@=" contained containedin=rim_r_inline_p_num
    syn region rim_r_inline_p_num start="<<[[:space:]]*p-num \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_num '<<[[:space:]]*p-num \@=' contained containedin=rim_r_inline_p_num
    syn match rim_h_print_inline_p_num '>>' contained containedin=rim_r_inline_p_num
    hi def link rim_h_clause_p_num    rimClause
    hi def link rim_h_clause_output_p_num    rimClauseOutput
    hi def link rim_h_construct_p_num    rimConstruct
    hi def link rim_h_print_inline_p_num    rimConstruct
syn region rim_r_construct_p_source_file start="^[[:space:]]*p-source-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_source_file,rim_r_inline_p_source_file,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_source_file,rim_r_inline_p_source_file,rim_r_at
    syn match rim_h_construct_p_source_file "^[[:space:]]*p-source-file" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " length \@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_print_inline_p_source_file " length \@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_clause_p_source_file " new-line \@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " new-line,\@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " new-line$" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_print_inline_p_source_file " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_print_inline_p_source_file " new-line \@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_clause_p_source_file " source-file \@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " source-file,\@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " source-file$" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_print_inline_p_source_file " source-file\(>>\)\@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_print_inline_p_source_file " source-file \@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_clause_p_source_file " source-line \@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " source-line,\@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " source-line$" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_print_inline_p_source_file " source-line\(>>\)\@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_print_inline_p_source_file " source-line \@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_clause_p_source_file " url-encode \@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " url-encode,\@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " url-encode$" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_print_inline_p_source_file " url-encode\(>>\)\@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_print_inline_p_source_file " url-encode \@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_clause_p_source_file " web-encode \@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " web-encode,\@=" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_clause_p_source_file " web-encode$" contained containedin=rim_r_construct_p_source_file
    syn match rim_h_print_inline_p_source_file " web-encode\(>>\)\@=" contained containedin=rim_r_inline_p_source_file
    syn match rim_h_print_inline_p_source_file " web-encode \@=" contained containedin=rim_r_inline_p_source_file
    syn region rim_r_inline_p_source_file start="<<[[:space:]]*p-source-file" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_source_file '<<[[:space:]]*p-source-file' contained containedin=rim_r_inline_p_source_file
    syn match rim_h_print_inline_p_source_file '>>' contained containedin=rim_r_inline_p_source_file
    hi def link rim_h_clause_p_source_file    rimClause
    hi def link rim_h_clause_output_p_source_file    rimClauseOutput
    hi def link rim_h_construct_p_source_file    rimConstruct
    hi def link rim_h_print_inline_p_source_file    rimConstruct
syn region rim_r_construct_p_out start="^[[:space:]]*p-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_out,rim_r_inline_p_out,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_out,rim_r_inline_p_out,rim_r_at
    syn match rim_h_construct_p_out "^[[:space:]]*p-out" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " length \@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_print_inline_p_out " length \@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_clause_p_out " new-line \@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " new-line,\@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " new-line$" contained containedin=rim_r_construct_p_out
    syn match rim_h_print_inline_p_out " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_print_inline_p_out " new-line \@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_clause_p_out " source-file \@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " source-file,\@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " source-file$" contained containedin=rim_r_construct_p_out
    syn match rim_h_print_inline_p_out " source-file\(>>\)\@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_print_inline_p_out " source-file \@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_clause_p_out " source-line \@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " source-line,\@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " source-line$" contained containedin=rim_r_construct_p_out
    syn match rim_h_print_inline_p_out " source-line\(>>\)\@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_print_inline_p_out " source-line \@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_clause_p_out " url-encode \@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " url-encode,\@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " url-encode$" contained containedin=rim_r_construct_p_out
    syn match rim_h_print_inline_p_out " url-encode\(>>\)\@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_print_inline_p_out " url-encode \@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_clause_p_out " web-encode \@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " web-encode,\@=" contained containedin=rim_r_construct_p_out
    syn match rim_h_clause_p_out " web-encode$" contained containedin=rim_r_construct_p_out
    syn match rim_h_print_inline_p_out " web-encode\(>>\)\@=" contained containedin=rim_r_inline_p_out
    syn match rim_h_print_inline_p_out " web-encode \@=" contained containedin=rim_r_inline_p_out
    syn region rim_r_inline_p_out start="<<[[:space:]]*p-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_out '<<[[:space:]]*p-out \@=' contained containedin=rim_r_inline_p_out
    syn match rim_h_print_inline_p_out '>>' contained containedin=rim_r_inline_p_out
    hi def link rim_h_clause_p_out    rimClause
    hi def link rim_h_clause_output_p_out    rimClauseOutput
    hi def link rim_h_construct_p_out    rimConstruct
    hi def link rim_h_print_inline_p_out    rimConstruct
syn region rim_r_construct_p_source_line start="^[[:space:]]*p-source-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_p_source_line,rim_r_inline_p_source_line,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_p_source_line,rim_r_inline_p_source_line,rim_r_at
    syn match rim_h_construct_p_source_line "^[[:space:]]*p-source-line" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " length \@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_print_inline_p_source_line " length \@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_clause_p_source_line " new-line \@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " new-line,\@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " new-line$" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_print_inline_p_source_line " new-line\(>>\)\@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_print_inline_p_source_line " new-line \@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_clause_p_source_line " source-file \@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " source-file,\@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " source-file$" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_print_inline_p_source_line " source-file\(>>\)\@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_print_inline_p_source_line " source-file \@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_clause_p_source_line " source-line \@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " source-line,\@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " source-line$" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_print_inline_p_source_line " source-line\(>>\)\@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_print_inline_p_source_line " source-line \@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_clause_p_source_line " url-encode \@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " url-encode,\@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " url-encode$" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_print_inline_p_source_line " url-encode\(>>\)\@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_print_inline_p_source_line " url-encode \@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_clause_p_source_line " web-encode \@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " web-encode,\@=" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_clause_p_source_line " web-encode$" contained containedin=rim_r_construct_p_source_line
    syn match rim_h_print_inline_p_source_line " web-encode\(>>\)\@=" contained containedin=rim_r_inline_p_source_line
    syn match rim_h_print_inline_p_source_line " web-encode \@=" contained containedin=rim_r_inline_p_source_line
    syn region rim_r_inline_p_source_line start="<<[[:space:]]*p-source-line" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_p_source_line '<<[[:space:]]*p-source-line' contained containedin=rim_r_inline_p_source_line
    syn match rim_h_print_inline_p_source_line '>>' contained containedin=rim_r_inline_p_source_line
    hi def link rim_h_clause_p_source_line    rimClause
    hi def link rim_h_clause_output_p_source_line    rimClauseOutput
    hi def link rim_h_construct_p_source_line    rimConstruct
    hi def link rim_h_print_inline_p_source_line    rimConstruct
syn region rim_r_construct_run_query start="^[[:space:]]*run-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_run_query,rim_r_inline_run_query,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_run_query,rim_r_inline_run_query,rim_r_at
    syn match rim_h_construct_run_query "^[[:space:]]*run-query" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_output_run_query " affected-rows \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " @ \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " @,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " @$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " : \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " :,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " :$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_output_run_query " error \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_output_run_query " error-text \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " input \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " noencode \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " noencode,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " noencode$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " no-loop \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " no-loop,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " no-loop$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " on-error-continue \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " on-error-continue,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " on-error-continue$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " on-error-exit \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " on-error-exit,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " on-error-exit$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_output_run_query " output \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_output_run_query " row-count \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " url-encode \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " url-encode,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " url-encode$" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " web-encode \@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " web-encode,\@=" contained containedin=rim_r_construct_run_query
    syn match rim_h_clause_run_query " web-encode$" contained containedin=rim_r_construct_run_query
    hi def link rim_h_clause_run_query    rimClause
    hi def link rim_h_clause_output_run_query    rimClauseOutput
    hi def link rim_h_construct_run_query    rimConstruct
    hi def link rim_h_print_inline_run_query    rimConstruct
syn region rim_r_construct_run_prepared_query start="^[[:space:]]*run-prepared-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_run_prepared_query,rim_r_inline_run_prepared_query,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_run_prepared_query,rim_r_inline_run_prepared_query,rim_r_at
    syn match rim_h_construct_run_prepared_query "^[[:space:]]*run-prepared-query" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_output_run_prepared_query " affected-rows \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " @ \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " @,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " @$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " : \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " :,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " :$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_output_run_prepared_query " error \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_output_run_prepared_query " error-text \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " input \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " noencode \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " noencode,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " noencode$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " no-loop \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " no-loop,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " no-loop$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " on-error-continue \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " on-error-continue,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " on-error-continue$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " on-error-exit \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " on-error-exit,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " on-error-exit$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_output_run_prepared_query " output \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_output_run_prepared_query " row-count \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " url-encode \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " url-encode,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " url-encode$" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " web-encode \@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " web-encode,\@=" contained containedin=rim_r_construct_run_prepared_query
    syn match rim_h_clause_run_prepared_query " web-encode$" contained containedin=rim_r_construct_run_prepared_query
    hi def link rim_h_clause_run_prepared_query    rimClause
    hi def link rim_h_clause_output_run_prepared_query    rimClauseOutput
    hi def link rim_h_construct_run_prepared_query    rimConstruct
    hi def link rim_h_print_inline_run_prepared_query    rimConstruct
syn region rim_r_construct_current_row start="^[[:space:]]*current-row" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_current_row,rim_r_inline_current_row,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_current_row,rim_r_inline_current_row,rim_r_at
    syn match rim_h_construct_current_row "^[[:space:]]*current-row" contained containedin=rim_r_construct_current_row
    syn match rim_h_clause_current_row " to \@=" contained containedin=rim_r_construct_current_row
    syn match rim_h_print_inline_current_row " to \@=" contained containedin=rim_r_inline_current_row
    syn region rim_r_inline_current_row start="<<[[:space:]]*current-row" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=rim_r_at keepend
    syn match rim_h_print_inline_current_row '<<[[:space:]]*current-row' contained containedin=rim_r_inline_current_row
    syn match rim_h_print_inline_current_row '>>' contained containedin=rim_r_inline_current_row
    hi def link rim_h_clause_current_row    rimClause
    hi def link rim_h_clause_output_current_row    rimClauseOutput
    hi def link rim_h_construct_current_row    rimConstruct
    hi def link rim_h_print_inline_current_row    rimConstruct
syn region rim_r_construct_begin_transaction start="^[[:space:]]*begin-transaction" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_begin_transaction,rim_r_inline_begin_transaction,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_begin_transaction,rim_r_inline_begin_transaction,rim_r_at
    syn match rim_h_construct_begin_transaction "^[[:space:]]*begin-transaction" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " @ \@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " @,\@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " @$" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_output_begin_transaction " error \@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_output_begin_transaction " error-text \@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " on-error-continue \@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " on-error-continue,\@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " on-error-continue$" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " on-error-exit \@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " on-error-exit,\@=" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " on-error-exit$" contained containedin=rim_r_construct_begin_transaction
    syn match rim_h_clause_begin_transaction " options \@=" contained containedin=rim_r_construct_begin_transaction
    hi def link rim_h_clause_begin_transaction    rimClause
    hi def link rim_h_clause_output_begin_transaction    rimClauseOutput
    hi def link rim_h_construct_begin_transaction    rimConstruct
    hi def link rim_h_print_inline_begin_transaction    rimConstruct
syn region rim_r_construct_commit_transaction start="^[[:space:]]*commit-transaction" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_commit_transaction,rim_r_inline_commit_transaction,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_commit_transaction,rim_r_inline_commit_transaction,rim_r_at
    syn match rim_h_construct_commit_transaction "^[[:space:]]*commit-transaction" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " @ \@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " @,\@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " @$" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_output_commit_transaction " error \@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_output_commit_transaction " error-text \@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " on-error-continue \@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " on-error-continue,\@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " on-error-continue$" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " on-error-exit \@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " on-error-exit,\@=" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " on-error-exit$" contained containedin=rim_r_construct_commit_transaction
    syn match rim_h_clause_commit_transaction " options \@=" contained containedin=rim_r_construct_commit_transaction
    hi def link rim_h_clause_commit_transaction    rimClause
    hi def link rim_h_clause_output_commit_transaction    rimClauseOutput
    hi def link rim_h_construct_commit_transaction    rimConstruct
    hi def link rim_h_print_inline_commit_transaction    rimConstruct
syn region rim_r_construct_db_error start="^[[:space:]]*db-error" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_db_error,rim_r_inline_db_error,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_db_error,rim_r_inline_db_error,rim_r_at
    syn match rim_h_construct_db_error "^[[:space:]]*db-error" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " @ \@=" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " @,\@=" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " @$" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " continue \@=" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " continue,\@=" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " continue$" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " exit \@=" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " exit,\@=" contained containedin=rim_r_construct_db_error
    syn match rim_h_clause_db_error " exit$" contained containedin=rim_r_construct_db_error
    hi def link rim_h_clause_db_error    rimClause
    hi def link rim_h_clause_output_db_error    rimClauseOutput
    hi def link rim_h_construct_db_error    rimConstruct
    hi def link rim_h_print_inline_db_error    rimConstruct
syn region rim_r_construct_rollback_transaction start="^[[:space:]]*rollback-transaction" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_rollback_transaction,rim_r_inline_rollback_transaction,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_rollback_transaction,rim_r_inline_rollback_transaction,rim_r_at
    syn match rim_h_construct_rollback_transaction "^[[:space:]]*rollback-transaction" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " @ \@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " @,\@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " @$" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_output_rollback_transaction " error \@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_output_rollback_transaction " error-text \@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " on-error-continue \@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " on-error-continue,\@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " on-error-continue$" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " on-error-exit \@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " on-error-exit,\@=" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " on-error-exit$" contained containedin=rim_r_construct_rollback_transaction
    syn match rim_h_clause_rollback_transaction " options \@=" contained containedin=rim_r_construct_rollback_transaction
    hi def link rim_h_clause_rollback_transaction    rimClause
    hi def link rim_h_clause_output_rollback_transaction    rimClauseOutput
    hi def link rim_h_construct_rollback_transaction    rimConstruct
    hi def link rim_h_print_inline_rollback_transaction    rimConstruct
syn region rim_r_construct_end_loop start="^[[:space:]]*end-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_loop,rim_r_inline_end_loop,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_loop,rim_r_inline_end_loop,rim_r_at
    syn match rim_h_construct_end_loop "^[[:space:]]*end-loop" contained containedin=rim_r_construct_end_loop
    hi def link rim_h_clause_end_loop    rimClause
    hi def link rim_h_clause_output_end_loop    rimClauseOutput
    hi def link rim_h_construct_end_loop    rimConstruct
    hi def link rim_h_print_inline_end_loop    rimConstruct
syn region rim_r_construct_break_loop start="^[[:space:]]*break-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_break_loop,rim_r_inline_break_loop,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_break_loop,rim_r_inline_break_loop,rim_r_at
    syn match rim_h_construct_break_loop "^[[:space:]]*break-loop" contained containedin=rim_r_construct_break_loop
    hi def link rim_h_clause_break_loop    rimClause
    hi def link rim_h_clause_output_break_loop    rimClauseOutput
    hi def link rim_h_construct_break_loop    rimConstruct
    hi def link rim_h_print_inline_break_loop    rimConstruct
syn region rim_r_construct_continue_loop start="^[[:space:]]*continue-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_continue_loop,rim_r_inline_continue_loop,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_continue_loop,rim_r_inline_continue_loop,rim_r_at
    syn match rim_h_construct_continue_loop "^[[:space:]]*continue-loop" contained containedin=rim_r_construct_continue_loop
    hi def link rim_h_clause_continue_loop    rimClause
    hi def link rim_h_clause_output_continue_loop    rimClauseOutput
    hi def link rim_h_construct_continue_loop    rimConstruct
    hi def link rim_h_print_inline_continue_loop    rimConstruct
syn region rim_r_construct_pause_program start="^[[:space:]]*pause-program" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_pause_program,rim_r_inline_pause_program,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_pause_program,rim_r_inline_pause_program,rim_r_at
    syn match rim_h_construct_pause_program "^[[:space:]]*pause-program" contained containedin=rim_r_construct_pause_program
    hi def link rim_h_clause_pause_program    rimClause
    hi def link rim_h_clause_output_pause_program    rimClauseOutput
    hi def link rim_h_construct_pause_program    rimConstruct
    hi def link rim_h_print_inline_pause_program    rimConstruct
syn region rim_r_construct_right_curly start="^[[:space:]]*}" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_right_curly,rim_r_inline_right_curly,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_right_curly,rim_r_inline_right_curly,rim_r_at
    syn match rim_h_construct_right_curly "^[[:space:]]*}" contained containedin=rim_r_construct_right_curly
    hi def link rim_h_clause_right_curly    rimClause
    hi def link rim_h_clause_output_right_curly    rimClauseOutput
    hi def link rim_h_construct_right_curly    rimConstruct
    hi def link rim_h_print_inline_right_curly    rimConstruct
syn region rim_r_construct_left_curly start="^[[:space:]]*{" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_left_curly,rim_r_inline_left_curly,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_left_curly,rim_r_inline_left_curly,rim_r_at
    syn match rim_h_construct_left_curly "^[[:space:]]*{" contained containedin=rim_r_construct_left_curly
    hi def link rim_h_clause_left_curly    rimClause
    hi def link rim_h_clause_output_left_curly    rimClauseOutput
    hi def link rim_h_construct_left_curly    rimConstruct
    hi def link rim_h_print_inline_left_curly    rimConstruct
syn region rim_r_construct_if_not_defined start="^[[:space:]]*if-not-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_if_not_defined,rim_r_inline_if_not_defined,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_if_not_defined,rim_r_inline_if_not_defined,rim_r_at
    syn match rim_h_construct_if_not_defined "^[[:space:]]*if-not-defined" contained containedin=rim_r_construct_if_not_defined
    syn match rim_h_clause_if_not_defined " equal \@=" contained containedin=rim_r_construct_if_not_defined
    syn match rim_h_clause_if_not_defined " greater-equal \@=" contained containedin=rim_r_construct_if_not_defined
    syn match rim_h_clause_if_not_defined " greater-than \@=" contained containedin=rim_r_construct_if_not_defined
    syn match rim_h_clause_if_not_defined " lesser-equal \@=" contained containedin=rim_r_construct_if_not_defined
    syn match rim_h_clause_if_not_defined " lesser-than \@=" contained containedin=rim_r_construct_if_not_defined
    syn match rim_h_clause_if_not_defined " not-equal \@=" contained containedin=rim_r_construct_if_not_defined
    hi def link rim_h_clause_if_not_defined    rimClause
    hi def link rim_h_clause_output_if_not_defined    rimClauseOutput
    hi def link rim_h_construct_if_not_defined    rimConstruct
    hi def link rim_h_print_inline_if_not_defined    rimConstruct
syn region rim_r_construct_if_defined start="^[[:space:]]*if-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_if_defined,rim_r_inline_if_defined,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_if_defined,rim_r_inline_if_defined,rim_r_at
    syn match rim_h_construct_if_defined "^[[:space:]]*if-defined" contained containedin=rim_r_construct_if_defined
    syn match rim_h_clause_if_defined " equal \@=" contained containedin=rim_r_construct_if_defined
    syn match rim_h_clause_if_defined " greater-equal \@=" contained containedin=rim_r_construct_if_defined
    syn match rim_h_clause_if_defined " greater-than \@=" contained containedin=rim_r_construct_if_defined
    syn match rim_h_clause_if_defined " lesser-equal \@=" contained containedin=rim_r_construct_if_defined
    syn match rim_h_clause_if_defined " lesser-than \@=" contained containedin=rim_r_construct_if_defined
    syn match rim_h_clause_if_defined " not-equal \@=" contained containedin=rim_r_construct_if_defined
    hi def link rim_h_clause_if_defined    rimClause
    hi def link rim_h_clause_output_if_defined    rimClauseOutput
    hi def link rim_h_construct_if_defined    rimConstruct
    hi def link rim_h_print_inline_if_defined    rimConstruct
syn region rim_r_construct_end_defined start="^[[:space:]]*end-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_defined,rim_r_inline_end_defined,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_defined,rim_r_inline_end_defined,rim_r_at
    syn match rim_h_construct_end_defined "^[[:space:]]*end-defined" contained containedin=rim_r_construct_end_defined
    hi def link rim_h_clause_end_defined    rimClause
    hi def link rim_h_clause_output_end_defined    rimClauseOutput
    hi def link rim_h_construct_end_defined    rimConstruct
    hi def link rim_h_print_inline_end_defined    rimConstruct
syn region rim_r_construct_start_loop start="^[[:space:]]*start-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_start_loop,rim_r_inline_start_loop,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_start_loop,rim_r_inline_start_loop,rim_r_at
    syn match rim_h_construct_start_loop "^[[:space:]]*start-loop" contained containedin=rim_r_construct_start_loop
    syn match rim_h_clause_start_loop " add \@=" contained containedin=rim_r_construct_start_loop
    syn match rim_h_clause_start_loop " repeat \@=" contained containedin=rim_r_construct_start_loop
    syn match rim_h_clause_start_loop " start-with \@=" contained containedin=rim_r_construct_start_loop
    syn match rim_h_clause_output_start_loop " use \@=" contained containedin=rim_r_construct_start_loop
    hi def link rim_h_clause_start_loop    rimClause
    hi def link rim_h_clause_output_start_loop    rimClauseOutput
    hi def link rim_h_construct_start_loop    rimConstruct
    hi def link rim_h_print_inline_start_loop    rimConstruct
syn region rim_r_construct_end_query start="^[[:space:]]*end-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match rim_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=rim_r_construct_end_query,rim_r_inline_end_query,rim_r_at
    syn match rim_h_other '[0-9]\+' contained containedin=rim_r_construct_end_query,rim_r_inline_end_query,rim_r_at
    syn match rim_h_construct_end_query "^[[:space:]]*end-query" contained containedin=rim_r_construct_end_query
    hi def link rim_h_clause_end_query    rimClause
    hi def link rim_h_clause_output_end_query    rimClauseOutput
    hi def link rim_h_construct_end_query    rimConstruct
    hi def link rim_h_print_inline_end_query    rimConstruct
