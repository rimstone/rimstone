" Language: Gliimly
" Vim syntax file
" Maintainer: Gliim LLC
" Latest Revision: 2024-November-08
so $VIMRUNTIME/syntax/c.vim
syntax sync minlines=10000
hi def link gliimConstruct Statement
hi def link gliimClause Identifier
hi def link gliimClauseOutput Type
hi def link gg_h_other  Constant
hi def link Comment  SpecialKey
syn region gg_r_inline_dot start="<<[[:space:]]*[\.]" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_dot '<<[[:space:]]*[\.]' contained containedin=gg_r_inline_dot
    syn match gg_h_print_inline_dot '>>' contained containedin=gg_r_inline_dot
    hi def link gg_h_print_inline_dot    gliimConstruct
syn region gg_r_excl start="^[[:space:]]*[!]" skip="\\[[:space:]]*$" end="$"  keepend
syn match gg_h_excl_begin '^[[:space:]]*[!]' contained containedin=gg_r_excl
hi def link gg_h_excl_begin    gliimConstruct
syn match gg_h_dot '^[[:space:]]*\.'
syn region gg_r_at start="^[[:space:]]*[@]" skip="\\[[:space:]]*$" end="$"  keepend
syn match gg_h_at_begin '^[[:space:]]*[@]' contained containedin=gg_r_at
hi def link gg_h_at_begin    gliimConstruct
hi def link gg_h_dot    gliimConstruct
syn region gg_r_construct_end_read_line start="^[[:space:]]*end-read-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_read_line,gg_r_inline_end_read_line,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_read_line,gg_r_inline_end_read_line,gg_r_at
    syn match gg_h_construct_end_read_line "^[[:space:]]*end-read-line" contained containedin=gg_r_construct_end_read_line
    hi def link gg_h_clause_end_read_line    gliimClause
    hi def link gg_h_clause_output_end_read_line    gliimClauseOutput
    hi def link gg_h_construct_end_read_line    gliimConstruct
    hi def link gg_h_print_inline_end_read_line    gliimConstruct
syn region gg_r_construct_read_line start="^[[:space:]]*read-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_line,gg_r_inline_read_line,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_line,gg_r_inline_read_line,gg_r_at
    syn match gg_h_construct_read_line "^[[:space:]]*read-line" contained containedin=gg_r_construct_read_line
    syn match gg_h_clause_read_line " delimiter \@=" contained containedin=gg_r_construct_read_line
    syn match gg_h_clause_output_read_line " status \@=" contained containedin=gg_r_construct_read_line
    syn match gg_h_clause_output_read_line " to \@=" contained containedin=gg_r_construct_read_line
    hi def link gg_h_clause_read_line    gliimClause
    hi def link gg_h_clause_output_read_line    gliimClauseOutput
    hi def link gg_h_construct_read_line    gliimConstruct
    hi def link gg_h_print_inline_read_line    gliimConstruct
syn region gg_r_construct_double_left_par start="^[[:space:]]*((" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_double_left_par,gg_r_inline_double_left_par,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_double_left_par,gg_r_inline_double_left_par,gg_r_at
    syn match gg_h_construct_double_left_par "^[[:space:]]*((" contained containedin=gg_r_construct_double_left_par
    hi def link gg_h_clause_double_left_par    gliimClause
    hi def link gg_h_clause_output_double_left_par    gliimClauseOutput
    hi def link gg_h_construct_double_left_par    gliimConstruct
    hi def link gg_h_print_inline_double_left_par    gliimConstruct
syn region gg_r_construct_write_string start="^[[:space:]]*write-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_string,gg_r_inline_write_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_string,gg_r_inline_write_string,gg_r_at
    syn match gg_h_construct_write_string "^[[:space:]]*write-string" contained containedin=gg_r_construct_write_string
    hi def link gg_h_clause_write_string    gliimClause
    hi def link gg_h_clause_output_write_string    gliimClauseOutput
    hi def link gg_h_construct_write_string    gliimConstruct
    hi def link gg_h_print_inline_write_string    gliimConstruct
syn region gg_r_construct_double_right_par start="^[[:space:]]*))" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_double_right_par,gg_r_inline_double_right_par,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_double_right_par,gg_r_inline_double_right_par,gg_r_at
    syn match gg_h_construct_double_right_par "^[[:space:]]*))" contained containedin=gg_r_construct_double_right_par
    syn match gg_h_clause_double_right_par " notrim \@=" contained containedin=gg_r_construct_double_right_par
    syn match gg_h_clause_double_right_par " notrim,\@=" contained containedin=gg_r_construct_double_right_par
    syn match gg_h_clause_double_right_par " notrim$" contained containedin=gg_r_construct_double_right_par
    hi def link gg_h_clause_double_right_par    gliimClause
    hi def link gg_h_clause_output_double_right_par    gliimClauseOutput
    hi def link gg_h_construct_double_right_par    gliimConstruct
    hi def link gg_h_print_inline_double_right_par    gliimConstruct
syn region gg_r_construct_end_write_string start="^[[:space:]]*end-write-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_write_string,gg_r_inline_end_write_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_write_string,gg_r_inline_end_write_string,gg_r_at
    syn match gg_h_construct_end_write_string "^[[:space:]]*end-write-string" contained containedin=gg_r_construct_end_write_string
    syn match gg_h_clause_end_write_string " notrim \@=" contained containedin=gg_r_construct_end_write_string
    syn match gg_h_clause_end_write_string " notrim,\@=" contained containedin=gg_r_construct_end_write_string
    syn match gg_h_clause_end_write_string " notrim$" contained containedin=gg_r_construct_end_write_string
    hi def link gg_h_clause_end_write_string    gliimClause
    hi def link gg_h_clause_output_end_write_string    gliimClauseOutput
    hi def link gg_h_construct_end_write_string    gliimConstruct
    hi def link gg_h_print_inline_end_write_string    gliimConstruct
syn region gg_r_construct_open_file start="^[[:space:]]*open-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_open_file,gg_r_inline_open_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_open_file,gg_r_inline_open_file,gg_r_at
    syn match gg_h_construct_open_file "^[[:space:]]*open-file" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_output_open_file " file-id \@=" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_open_file " new-truncate \@=" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_open_file " new-truncate,\@=" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_open_file " new-truncate$" contained containedin=gg_r_construct_open_file
    syn match gg_h_clause_output_open_file " status \@=" contained containedin=gg_r_construct_open_file
    hi def link gg_h_clause_open_file    gliimClause
    hi def link gg_h_clause_output_open_file    gliimClauseOutput
    hi def link gg_h_construct_open_file    gliimConstruct
    hi def link gg_h_print_inline_open_file    gliimConstruct
syn region gg_r_construct_close_file start="^[[:space:]]*close-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_close_file,gg_r_inline_close_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_close_file,gg_r_inline_close_file,gg_r_at
    syn match gg_h_construct_close_file "^[[:space:]]*close-file" contained containedin=gg_r_construct_close_file
    syn match gg_h_clause_close_file " file-id \@=" contained containedin=gg_r_construct_close_file
    syn match gg_h_clause_output_close_file " status \@=" contained containedin=gg_r_construct_close_file
    hi def link gg_h_clause_close_file    gliimClause
    hi def link gg_h_clause_output_close_file    gliimClauseOutput
    hi def link gg_h_construct_close_file    gliimConstruct
    hi def link gg_h_print_inline_close_file    gliimConstruct
syn region gg_r_construct_file_position start="^[[:space:]]*file-position" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_file_position,gg_r_inline_file_position,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_file_position,gg_r_inline_file_position,gg_r_at
    syn match gg_h_construct_file_position "^[[:space:]]*file-position" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_file_position " file-id \@=" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_output_file_position " get \@=" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_file_position " set \@=" contained containedin=gg_r_construct_file_position
    syn match gg_h_clause_output_file_position " status \@=" contained containedin=gg_r_construct_file_position
    hi def link gg_h_clause_file_position    gliimClause
    hi def link gg_h_clause_output_file_position    gliimClauseOutput
    hi def link gg_h_construct_file_position    gliimConstruct
    hi def link gg_h_print_inline_file_position    gliimConstruct
syn region gg_r_construct_exit_handler start="^[[:space:]]*exit-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_exit_handler,gg_r_inline_exit_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_exit_handler,gg_r_inline_exit_handler,gg_r_at
    syn match gg_h_construct_exit_handler "^[[:space:]]*exit-handler" contained containedin=gg_r_construct_exit_handler
    hi def link gg_h_clause_exit_handler    gliimClause
    hi def link gg_h_clause_output_exit_handler    gliimClauseOutput
    hi def link gg_h_construct_exit_handler    gliimConstruct
    hi def link gg_h_print_inline_exit_handler    gliimConstruct
syn region gg_r_construct_finish_output start="^[[:space:]]*finish-output" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_finish_output,gg_r_inline_finish_output,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_finish_output,gg_r_inline_finish_output,gg_r_at
    syn match gg_h_construct_finish_output "^[[:space:]]*finish-output" contained containedin=gg_r_construct_finish_output
    hi def link gg_h_clause_finish_output    gliimClause
    hi def link gg_h_clause_output_finish_output    gliimClauseOutput
    hi def link gg_h_construct_finish_output    gliimConstruct
    hi def link gg_h_print_inline_finish_output    gliimConstruct
syn region gg_r_construct_copy_file start="^[[:space:]]*copy-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_copy_file,gg_r_inline_copy_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_copy_file,gg_r_inline_copy_file,gg_r_at
    syn match gg_h_construct_copy_file "^[[:space:]]*copy-file" contained containedin=gg_r_construct_copy_file
    syn match gg_h_clause_output_copy_file " status \@=" contained containedin=gg_r_construct_copy_file
    syn match gg_h_clause_copy_file " to \@=" contained containedin=gg_r_construct_copy_file
    hi def link gg_h_clause_copy_file    gliimClause
    hi def link gg_h_clause_output_copy_file    gliimClauseOutput
    hi def link gg_h_construct_copy_file    gliimConstruct
    hi def link gg_h_print_inline_copy_file    gliimConstruct
syn region gg_r_construct_end_do_once start="^[[:space:]]*end-do-once" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_do_once,gg_r_inline_end_do_once,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_do_once,gg_r_inline_end_do_once,gg_r_at
    syn match gg_h_construct_end_do_once "^[[:space:]]*end-do-once" contained containedin=gg_r_construct_end_do_once
    hi def link gg_h_clause_end_do_once    gliimClause
    hi def link gg_h_clause_output_end_do_once    gliimClauseOutput
    hi def link gg_h_construct_end_do_once    gliimConstruct
    hi def link gg_h_print_inline_end_do_once    gliimConstruct
syn region gg_r_construct_do_once start="^[[:space:]]*do-once" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_do_once,gg_r_inline_do_once,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_do_once,gg_r_inline_do_once,gg_r_at
    syn match gg_h_construct_do_once "^[[:space:]]*do-once" contained containedin=gg_r_construct_do_once
    hi def link gg_h_clause_do_once    gliimClause
    hi def link gg_h_clause_output_do_once    gliimClauseOutput
    hi def link gg_h_construct_do_once    gliimConstruct
    hi def link gg_h_print_inline_do_once    gliimConstruct
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
    hi def link gg_h_clause_use_cursor    gliimClause
    hi def link gg_h_clause_output_use_cursor    gliimClauseOutput
    hi def link gg_h_construct_use_cursor    gliimConstruct
    hi def link gg_h_print_inline_use_cursor    gliimConstruct
syn region gg_r_construct_delete_index start="^[[:space:]]*delete-index" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_index,gg_r_inline_delete_index,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_index,gg_r_inline_delete_index,gg_r_at
    syn match gg_h_construct_delete_index "^[[:space:]]*delete-index" contained containedin=gg_r_construct_delete_index
    syn match gg_h_clause_delete_index " key \@=" contained containedin=gg_r_construct_delete_index
    syn match gg_h_clause_output_delete_index " status \@=" contained containedin=gg_r_construct_delete_index
    syn match gg_h_clause_output_delete_index " value \@=" contained containedin=gg_r_construct_delete_index
    hi def link gg_h_clause_delete_index    gliimClause
    hi def link gg_h_clause_output_delete_index    gliimClauseOutput
    hi def link gg_h_construct_delete_index    gliimConstruct
    hi def link gg_h_print_inline_delete_index    gliimConstruct
syn region gg_r_construct_read_index start="^[[:space:]]*read-index" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_index,gg_r_inline_read_index,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_index,gg_r_inline_read_index,gg_r_at
    syn match gg_h_construct_read_index "^[[:space:]]*read-index" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " equal \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " greater \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " greater-equal \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_output_read_index " key \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " lesser \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " lesser-equal \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " max-key \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " max-key,\@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " max-key$" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " min-key \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " min-key,\@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " min-key$" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_output_read_index " new-cursor \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_output_read_index " status \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_read_index " update-value \@=" contained containedin=gg_r_construct_read_index
    syn match gg_h_clause_output_read_index " value \@=" contained containedin=gg_r_construct_read_index
    hi def link gg_h_clause_read_index    gliimClause
    hi def link gg_h_clause_output_read_index    gliimClauseOutput
    hi def link gg_h_construct_read_index    gliimConstruct
    hi def link gg_h_print_inline_read_index    gliimConstruct
syn region gg_r_construct_write_index start="^[[:space:]]*write-index" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_index,gg_r_inline_write_index,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_index,gg_r_inline_write_index,gg_r_at
    syn match gg_h_construct_write_index "^[[:space:]]*write-index" contained containedin=gg_r_construct_write_index
    syn match gg_h_clause_write_index " key \@=" contained containedin=gg_r_construct_write_index
    syn match gg_h_clause_output_write_index " new-cursor \@=" contained containedin=gg_r_construct_write_index
    syn match gg_h_clause_output_write_index " status \@=" contained containedin=gg_r_construct_write_index
    syn match gg_h_clause_write_index " value \@=" contained containedin=gg_r_construct_write_index
    hi def link gg_h_clause_write_index    gliimClause
    hi def link gg_h_clause_output_write_index    gliimClauseOutput
    hi def link gg_h_construct_write_index    gliimConstruct
    hi def link gg_h_print_inline_write_index    gliimConstruct
syn region gg_r_construct_purge_index start="^[[:space:]]*purge-index" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_index,gg_r_inline_purge_index,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_index,gg_r_inline_purge_index,gg_r_at
    syn match gg_h_construct_purge_index "^[[:space:]]*purge-index" contained containedin=gg_r_construct_purge_index
    hi def link gg_h_clause_purge_index    gliimClause
    hi def link gg_h_clause_output_purge_index    gliimClauseOutput
    hi def link gg_h_construct_purge_index    gliimConstruct
    hi def link gg_h_print_inline_purge_index    gliimConstruct
syn region gg_r_construct_get_index start="^[[:space:]]*get-index" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_index,gg_r_inline_get_index,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_index,gg_r_inline_get_index,gg_r_at
    syn match gg_h_construct_get_index "^[[:space:]]*get-index" contained containedin=gg_r_construct_get_index
    syn match gg_h_clause_output_get_index " count \@=" contained containedin=gg_r_construct_get_index
    syn match gg_h_clause_output_get_index " hops \@=" contained containedin=gg_r_construct_get_index
    hi def link gg_h_clause_get_index    gliimClause
    hi def link gg_h_clause_output_get_index    gliimClauseOutput
    hi def link gg_h_construct_get_index    gliimConstruct
    hi def link gg_h_print_inline_get_index    gliimConstruct
syn region gg_r_construct_get_list start="^[[:space:]]*get-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_list,gg_r_inline_get_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_list,gg_r_inline_get_list,gg_r_at
    syn match gg_h_construct_get_list "^[[:space:]]*get-list" contained containedin=gg_r_construct_get_list
    syn match gg_h_clause_output_get_list " count \@=" contained containedin=gg_r_construct_get_list
    hi def link gg_h_clause_get_list    gliimClause
    hi def link gg_h_clause_output_get_list    gliimClauseOutput
    hi def link gg_h_construct_get_list    gliimConstruct
    hi def link gg_h_print_inline_get_list    gliimConstruct
syn region gg_r_construct_new_index start="^[[:space:]]*new-index" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_index,gg_r_inline_new_index,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_index,gg_r_inline_new_index,gg_r_at
    syn match gg_h_construct_new_index "^[[:space:]]*new-index" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " key-as \@=" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " process-scope \@=" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " process-scope,\@=" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " process-scope$" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " unsorted \@=" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " unsorted,\@=" contained containedin=gg_r_construct_new_index
    syn match gg_h_clause_new_index " unsorted$" contained containedin=gg_r_construct_new_index
    hi def link gg_h_clause_new_index    gliimClause
    hi def link gg_h_clause_output_new_index    gliimClauseOutput
    hi def link gg_h_construct_new_index    gliimConstruct
    hi def link gg_h_print_inline_new_index    gliimConstruct
syn region gg_r_construct_json_doc start="^[[:space:]]*json-doc" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_json_doc,gg_r_inline_json_doc,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_json_doc,gg_r_inline_json_doc,gg_r_at
    syn match gg_h_construct_json_doc "^[[:space:]]*json-doc" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " delete \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " error-position \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " error-text \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " length \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " noencode \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " noencode,\@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_json_doc " noencode$" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " status \@=" contained containedin=gg_r_construct_json_doc
    syn match gg_h_clause_output_json_doc " to \@=" contained containedin=gg_r_construct_json_doc
    hi def link gg_h_clause_json_doc    gliimClause
    hi def link gg_h_clause_output_json_doc    gliimClauseOutput
    hi def link gg_h_construct_json_doc    gliimConstruct
    hi def link gg_h_print_inline_json_doc    gliimConstruct
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
    hi def link gg_h_clause_read_json    gliimClause
    hi def link gg_h_clause_output_read_json    gliimClauseOutput
    hi def link gg_h_construct_read_json    gliimConstruct
    hi def link gg_h_print_inline_read_json    gliimConstruct
syn region gg_r_construct_read_array start="^[[:space:]]*read-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_array,gg_r_inline_read_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_array,gg_r_inline_read_array,gg_r_at
    syn match gg_h_construct_read_array "^[[:space:]]*read-array" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " begin \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " begin,\@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " begin$" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " delete \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " delete,\@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " delete$" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_output_read_array " key \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_output_read_array " status \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " traverse \@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " traverse,\@=" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_read_array " traverse$" contained containedin=gg_r_construct_read_array
    syn match gg_h_clause_output_read_array " value \@=" contained containedin=gg_r_construct_read_array
    hi def link gg_h_clause_read_array    gliimClause
    hi def link gg_h_clause_output_read_array    gliimClauseOutput
    hi def link gg_h_construct_read_array    gliimConstruct
    hi def link gg_h_print_inline_read_array    gliimConstruct
syn region gg_r_construct_write_array start="^[[:space:]]*write-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_array,gg_r_inline_write_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_array,gg_r_inline_write_array,gg_r_at
    syn match gg_h_construct_write_array "^[[:space:]]*write-array" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_write_array " key \@=" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_output_write_array " old-value \@=" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_output_write_array " status \@=" contained containedin=gg_r_construct_write_array
    syn match gg_h_clause_write_array " value \@=" contained containedin=gg_r_construct_write_array
    hi def link gg_h_clause_write_array    gliimClause
    hi def link gg_h_clause_output_write_array    gliimClauseOutput
    hi def link gg_h_construct_write_array    gliimConstruct
    hi def link gg_h_print_inline_write_array    gliimConstruct
syn region gg_r_construct_new_array start="^[[:space:]]*new-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_array,gg_r_inline_new_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_array,gg_r_inline_new_array,gg_r_at
    syn match gg_h_construct_new_array "^[[:space:]]*new-array" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " hash-size \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " process-scope \@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " process-scope,\@=" contained containedin=gg_r_construct_new_array
    syn match gg_h_clause_new_array " process-scope$" contained containedin=gg_r_construct_new_array
    hi def link gg_h_clause_new_array    gliimClause
    hi def link gg_h_clause_output_new_array    gliimClauseOutput
    hi def link gg_h_construct_new_array    gliimConstruct
    hi def link gg_h_print_inline_new_array    gliimConstruct
syn region gg_r_construct_resize_array start="^[[:space:]]*resize-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_resize_array,gg_r_inline_resize_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_resize_array,gg_r_inline_resize_array,gg_r_at
    syn match gg_h_construct_resize_array "^[[:space:]]*resize-array" contained containedin=gg_r_construct_resize_array
    syn match gg_h_clause_resize_array " hash-size \@=" contained containedin=gg_r_construct_resize_array
    hi def link gg_h_clause_resize_array    gliimClause
    hi def link gg_h_clause_output_resize_array    gliimClauseOutput
    hi def link gg_h_construct_resize_array    gliimConstruct
    hi def link gg_h_print_inline_resize_array    gliimConstruct
syn region gg_r_construct_purge_array start="^[[:space:]]*purge-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_array,gg_r_inline_purge_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_array,gg_r_inline_purge_array,gg_r_at
    syn match gg_h_construct_purge_array "^[[:space:]]*purge-array" contained containedin=gg_r_construct_purge_array
    hi def link gg_h_clause_purge_array    gliimClause
    hi def link gg_h_clause_output_purge_array    gliimClauseOutput
    hi def link gg_h_construct_purge_array    gliimConstruct
    hi def link gg_h_print_inline_purge_array    gliimConstruct
syn region gg_r_construct_get_array start="^[[:space:]]*get-array" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_array,gg_r_inline_get_array,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_array,gg_r_inline_get_array,gg_r_at
    syn match gg_h_construct_get_array "^[[:space:]]*get-array" contained containedin=gg_r_construct_get_array
    syn match gg_h_clause_output_get_array " average-reads \@=" contained containedin=gg_r_construct_get_array
    syn match gg_h_clause_output_get_array " hash-size \@=" contained containedin=gg_r_construct_get_array
    syn match gg_h_clause_output_get_array " length \@=" contained containedin=gg_r_construct_get_array
    hi def link gg_h_clause_get_array    gliimClause
    hi def link gg_h_clause_output_get_array    gliimClauseOutput
    hi def link gg_h_construct_get_array    gliimConstruct
    hi def link gg_h_print_inline_get_array    gliimConstruct
syn region gg_r_construct_read_file start="^[[:space:]]*read-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_file,gg_r_inline_read_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_file,gg_r_inline_read_file,gg_r_at
    syn match gg_h_construct_read_file "^[[:space:]]*read-file" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_read_file " file-id \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_read_file " length \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_read_file " position \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_output_read_file " status \@=" contained containedin=gg_r_construct_read_file
    syn match gg_h_clause_output_read_file " to \@=" contained containedin=gg_r_construct_read_file
    hi def link gg_h_clause_read_file    gliimClause
    hi def link gg_h_clause_output_read_file    gliimClauseOutput
    hi def link gg_h_construct_read_file    gliimConstruct
    hi def link gg_h_print_inline_read_file    gliimConstruct
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
    hi def link gg_h_clause_write_file    gliimClause
    hi def link gg_h_clause_output_write_file    gliimClauseOutput
    hi def link gg_h_construct_write_file    gliimConstruct
    hi def link gg_h_print_inline_write_file    gliimConstruct
syn region gg_r_construct_read_lifo start="^[[:space:]]*read-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_lifo,gg_r_inline_read_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_lifo,gg_r_inline_read_lifo,gg_r_at
    syn match gg_h_construct_read_lifo "^[[:space:]]*read-lifo" contained containedin=gg_r_construct_read_lifo
    syn match gg_h_clause_output_read_lifo " key \@=" contained containedin=gg_r_construct_read_lifo
    syn match gg_h_clause_output_read_lifo " status \@=" contained containedin=gg_r_construct_read_lifo
    syn match gg_h_clause_output_read_lifo " value \@=" contained containedin=gg_r_construct_read_lifo
    hi def link gg_h_clause_read_lifo    gliimClause
    hi def link gg_h_clause_output_read_lifo    gliimClauseOutput
    hi def link gg_h_construct_read_lifo    gliimConstruct
    hi def link gg_h_print_inline_read_lifo    gliimConstruct
syn region gg_r_construct_read_fifo start="^[[:space:]]*read-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_fifo,gg_r_inline_read_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_fifo,gg_r_inline_read_fifo,gg_r_at
    syn match gg_h_construct_read_fifo "^[[:space:]]*read-fifo" contained containedin=gg_r_construct_read_fifo
    syn match gg_h_clause_output_read_fifo " key \@=" contained containedin=gg_r_construct_read_fifo
    syn match gg_h_clause_output_read_fifo " status \@=" contained containedin=gg_r_construct_read_fifo
    syn match gg_h_clause_output_read_fifo " value \@=" contained containedin=gg_r_construct_read_fifo
    hi def link gg_h_clause_read_fifo    gliimClause
    hi def link gg_h_clause_output_read_fifo    gliimClauseOutput
    hi def link gg_h_construct_read_fifo    gliimConstruct
    hi def link gg_h_print_inline_read_fifo    gliimConstruct
syn region gg_r_construct_purge_lifo start="^[[:space:]]*purge-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_lifo,gg_r_inline_purge_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_lifo,gg_r_inline_purge_lifo,gg_r_at
    syn match gg_h_construct_purge_lifo "^[[:space:]]*purge-lifo" contained containedin=gg_r_construct_purge_lifo
    hi def link gg_h_clause_purge_lifo    gliimClause
    hi def link gg_h_clause_output_purge_lifo    gliimClauseOutput
    hi def link gg_h_construct_purge_lifo    gliimConstruct
    hi def link gg_h_print_inline_purge_lifo    gliimConstruct
syn region gg_r_construct_purge_fifo start="^[[:space:]]*purge-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_fifo,gg_r_inline_purge_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_fifo,gg_r_inline_purge_fifo,gg_r_at
    syn match gg_h_construct_purge_fifo "^[[:space:]]*purge-fifo" contained containedin=gg_r_construct_purge_fifo
    hi def link gg_h_clause_purge_fifo    gliimClause
    hi def link gg_h_clause_output_purge_fifo    gliimClauseOutput
    hi def link gg_h_construct_purge_fifo    gliimConstruct
    hi def link gg_h_print_inline_purge_fifo    gliimConstruct
syn region gg_r_construct_delete_lifo start="^[[:space:]]*delete-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_lifo,gg_r_inline_delete_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_lifo,gg_r_inline_delete_lifo,gg_r_at
    syn match gg_h_construct_delete_lifo "^[[:space:]]*delete-lifo" contained containedin=gg_r_construct_delete_lifo
    hi def link gg_h_clause_delete_lifo    gliimClause
    hi def link gg_h_clause_output_delete_lifo    gliimClauseOutput
    hi def link gg_h_construct_delete_lifo    gliimConstruct
    hi def link gg_h_print_inline_delete_lifo    gliimConstruct
syn region gg_r_construct_delete_fifo start="^[[:space:]]*delete-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_fifo,gg_r_inline_delete_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_fifo,gg_r_inline_delete_fifo,gg_r_at
    syn match gg_h_construct_delete_fifo "^[[:space:]]*delete-fifo" contained containedin=gg_r_construct_delete_fifo
    hi def link gg_h_clause_delete_fifo    gliimClause
    hi def link gg_h_clause_output_delete_fifo    gliimClauseOutput
    hi def link gg_h_construct_delete_fifo    gliimConstruct
    hi def link gg_h_print_inline_delete_fifo    gliimConstruct
syn region gg_r_construct_rewind_lifo start="^[[:space:]]*rewind-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rewind_lifo,gg_r_inline_rewind_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rewind_lifo,gg_r_inline_rewind_lifo,gg_r_at
    syn match gg_h_construct_rewind_lifo "^[[:space:]]*rewind-lifo" contained containedin=gg_r_construct_rewind_lifo
    hi def link gg_h_clause_rewind_lifo    gliimClause
    hi def link gg_h_clause_output_rewind_lifo    gliimClauseOutput
    hi def link gg_h_construct_rewind_lifo    gliimConstruct
    hi def link gg_h_print_inline_rewind_lifo    gliimConstruct
syn region gg_r_construct_rewind_fifo start="^[[:space:]]*rewind-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rewind_fifo,gg_r_inline_rewind_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rewind_fifo,gg_r_inline_rewind_fifo,gg_r_at
    syn match gg_h_construct_rewind_fifo "^[[:space:]]*rewind-fifo" contained containedin=gg_r_construct_rewind_fifo
    hi def link gg_h_clause_rewind_fifo    gliimClause
    hi def link gg_h_clause_output_rewind_fifo    gliimClauseOutput
    hi def link gg_h_construct_rewind_fifo    gliimConstruct
    hi def link gg_h_print_inline_rewind_fifo    gliimConstruct
syn region gg_r_construct_write_lifo start="^[[:space:]]*write-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_lifo,gg_r_inline_write_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_lifo,gg_r_inline_write_lifo,gg_r_at
    syn match gg_h_construct_write_lifo "^[[:space:]]*write-lifo" contained containedin=gg_r_construct_write_lifo
    syn match gg_h_clause_write_lifo " key \@=" contained containedin=gg_r_construct_write_lifo
    syn match gg_h_clause_write_lifo " value \@=" contained containedin=gg_r_construct_write_lifo
    hi def link gg_h_clause_write_lifo    gliimClause
    hi def link gg_h_clause_output_write_lifo    gliimClauseOutput
    hi def link gg_h_construct_write_lifo    gliimConstruct
    hi def link gg_h_print_inline_write_lifo    gliimConstruct
syn region gg_r_construct_write_fifo start="^[[:space:]]*write-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_fifo,gg_r_inline_write_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_fifo,gg_r_inline_write_fifo,gg_r_at
    syn match gg_h_construct_write_fifo "^[[:space:]]*write-fifo" contained containedin=gg_r_construct_write_fifo
    syn match gg_h_clause_write_fifo " key \@=" contained containedin=gg_r_construct_write_fifo
    syn match gg_h_clause_write_fifo " value \@=" contained containedin=gg_r_construct_write_fifo
    hi def link gg_h_clause_write_fifo    gliimClause
    hi def link gg_h_clause_output_write_fifo    gliimClauseOutput
    hi def link gg_h_construct_write_fifo    gliimConstruct
    hi def link gg_h_print_inline_write_fifo    gliimConstruct
syn region gg_r_construct_new_lifo start="^[[:space:]]*new-lifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_lifo,gg_r_inline_new_lifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_lifo,gg_r_inline_new_lifo,gg_r_at
    syn match gg_h_construct_new_lifo "^[[:space:]]*new-lifo" contained containedin=gg_r_construct_new_lifo
    hi def link gg_h_clause_new_lifo    gliimClause
    hi def link gg_h_clause_output_new_lifo    gliimClauseOutput
    hi def link gg_h_construct_new_lifo    gliimConstruct
    hi def link gg_h_print_inline_new_lifo    gliimConstruct
syn region gg_r_construct_new_fifo start="^[[:space:]]*new-fifo" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_fifo,gg_r_inline_new_fifo,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_fifo,gg_r_inline_new_fifo,gg_r_at
    syn match gg_h_construct_new_fifo "^[[:space:]]*new-fifo" contained containedin=gg_r_construct_new_fifo
    hi def link gg_h_clause_new_fifo    gliimClause
    hi def link gg_h_clause_output_new_fifo    gliimClauseOutput
    hi def link gg_h_construct_new_fifo    gliimConstruct
    hi def link gg_h_print_inline_new_fifo    gliimConstruct
syn region gg_r_construct_delete_list start="^[[:space:]]*delete-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_list,gg_r_inline_delete_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_list,gg_r_inline_delete_list,gg_r_at
    syn match gg_h_construct_delete_list "^[[:space:]]*delete-list" contained containedin=gg_r_construct_delete_list
    syn match gg_h_clause_output_delete_list " status \@=" contained containedin=gg_r_construct_delete_list
    hi def link gg_h_clause_delete_list    gliimClause
    hi def link gg_h_clause_output_delete_list    gliimClauseOutput
    hi def link gg_h_construct_delete_list    gliimConstruct
    hi def link gg_h_print_inline_delete_list    gliimConstruct
syn region gg_r_construct_purge_list start="^[[:space:]]*purge-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_purge_list,gg_r_inline_purge_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_purge_list,gg_r_inline_purge_list,gg_r_at
    syn match gg_h_construct_purge_list "^[[:space:]]*purge-list" contained containedin=gg_r_construct_purge_list
    hi def link gg_h_clause_purge_list    gliimClause
    hi def link gg_h_clause_output_purge_list    gliimClauseOutput
    hi def link gg_h_construct_purge_list    gliimConstruct
    hi def link gg_h_print_inline_purge_list    gliimConstruct
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
    hi def link gg_h_clause_position_list    gliimClause
    hi def link gg_h_clause_output_position_list    gliimClauseOutput
    hi def link gg_h_construct_position_list    gliimConstruct
    hi def link gg_h_print_inline_position_list    gliimConstruct
syn region gg_r_construct_read_list start="^[[:space:]]*read-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_list,gg_r_inline_read_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_list,gg_r_inline_read_list,gg_r_at
    syn match gg_h_construct_read_list "^[[:space:]]*read-list" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_output_read_list " key \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_output_read_list " status \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_read_list " update-key \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_read_list " update-value \@=" contained containedin=gg_r_construct_read_list
    syn match gg_h_clause_output_read_list " value \@=" contained containedin=gg_r_construct_read_list
    hi def link gg_h_clause_read_list    gliimClause
    hi def link gg_h_clause_output_read_list    gliimClauseOutput
    hi def link gg_h_construct_read_list    gliimConstruct
    hi def link gg_h_print_inline_read_list    gliimConstruct
syn region gg_r_construct_write_list start="^[[:space:]]*write-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_list,gg_r_inline_write_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_list,gg_r_inline_write_list,gg_r_at
    syn match gg_h_construct_write_list "^[[:space:]]*write-list" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " append \@=" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " append,\@=" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " append$" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " key \@=" contained containedin=gg_r_construct_write_list
    syn match gg_h_clause_write_list " value \@=" contained containedin=gg_r_construct_write_list
    hi def link gg_h_clause_write_list    gliimClause
    hi def link gg_h_clause_output_write_list    gliimClauseOutput
    hi def link gg_h_construct_write_list    gliimConstruct
    hi def link gg_h_print_inline_write_list    gliimConstruct
syn region gg_r_construct_new_list start="^[[:space:]]*new-list" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_list,gg_r_inline_new_list,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_list,gg_r_inline_new_list,gg_r_at
    syn match gg_h_construct_new_list "^[[:space:]]*new-list" contained containedin=gg_r_construct_new_list
    syn match gg_h_clause_new_list " process-scope \@=" contained containedin=gg_r_construct_new_list
    syn match gg_h_clause_new_list " process-scope,\@=" contained containedin=gg_r_construct_new_list
    syn match gg_h_clause_new_list " process-scope$" contained containedin=gg_r_construct_new_list
    hi def link gg_h_clause_new_list    gliimClause
    hi def link gg_h_clause_output_new_list    gliimClauseOutput
    hi def link gg_h_construct_new_list    gliimConstruct
    hi def link gg_h_print_inline_new_list    gliimConstruct
syn region gg_r_construct_unused_var start="^[[:space:]]*unused-var" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_unused_var,gg_r_inline_unused_var,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_unused_var,gg_r_inline_unused_var,gg_r_at
    syn match gg_h_construct_unused_var "^[[:space:]]*unused-var" contained containedin=gg_r_construct_unused_var
    hi def link gg_h_clause_unused_var    gliimClause
    hi def link gg_h_clause_output_unused_var    gliimClauseOutput
    hi def link gg_h_construct_unused_var    gliimConstruct
    hi def link gg_h_print_inline_unused_var    gliimConstruct
syn region gg_r_construct_split_string start="^[[:space:]]*split-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_split_string,gg_r_inline_split_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_split_string,gg_r_inline_split_string,gg_r_at
    syn match gg_h_construct_split_string "^[[:space:]]*split-string" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_output_split_string " count \@=" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_split_string " delete \@=" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_output_split_string " to \@=" contained containedin=gg_r_construct_split_string
    syn match gg_h_clause_split_string " with \@=" contained containedin=gg_r_construct_split_string
    hi def link gg_h_clause_split_string    gliimClause
    hi def link gg_h_clause_output_split_string    gliimClauseOutput
    hi def link gg_h_construct_split_string    gliimConstruct
    hi def link gg_h_print_inline_split_string    gliimConstruct
syn region gg_r_construct_read_split start="^[[:space:]]*read-split" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_split,gg_r_inline_read_split,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_split,gg_r_inline_read_split,gg_r_at
    syn match gg_h_construct_read_split "^[[:space:]]*read-split" contained containedin=gg_r_construct_read_split
    syn match gg_h_clause_read_split " from \@=" contained containedin=gg_r_construct_read_split
    syn match gg_h_clause_output_read_split " status \@=" contained containedin=gg_r_construct_read_split
    syn match gg_h_clause_output_read_split " to \@=" contained containedin=gg_r_construct_read_split
    hi def link gg_h_clause_read_split    gliimClause
    hi def link gg_h_clause_output_read_split    gliimClauseOutput
    hi def link gg_h_construct_read_split    gliimConstruct
    hi def link gg_h_print_inline_read_split    gliimConstruct
syn region gg_r_construct_new_message start="^[[:space:]]*new-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_new_message,gg_r_inline_new_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_new_message,gg_r_inline_new_message,gg_r_at
    syn match gg_h_construct_new_message "^[[:space:]]*new-message" contained containedin=gg_r_construct_new_message
    syn match gg_h_clause_new_message " from \@=" contained containedin=gg_r_construct_new_message
    hi def link gg_h_clause_new_message    gliimClause
    hi def link gg_h_clause_output_new_message    gliimClauseOutput
    hi def link gg_h_construct_new_message    gliimConstruct
    hi def link gg_h_print_inline_new_message    gliimConstruct
syn region gg_r_construct_get_message start="^[[:space:]]*get-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_message,gg_r_inline_get_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_message,gg_r_inline_get_message,gg_r_at
    syn match gg_h_construct_get_message "^[[:space:]]*get-message" contained containedin=gg_r_construct_get_message
    syn match gg_h_clause_output_get_message " to \@=" contained containedin=gg_r_construct_get_message
    hi def link gg_h_clause_get_message    gliimClause
    hi def link gg_h_clause_output_get_message    gliimClauseOutput
    hi def link gg_h_construct_get_message    gliimConstruct
    hi def link gg_h_print_inline_get_message    gliimConstruct
syn region gg_r_construct_read_message start="^[[:space:]]*read-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_message,gg_r_inline_read_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_message,gg_r_inline_read_message,gg_r_at
    syn match gg_h_construct_read_message "^[[:space:]]*read-message" contained containedin=gg_r_construct_read_message
    syn match gg_h_clause_output_read_message " key \@=" contained containedin=gg_r_construct_read_message
    syn match gg_h_clause_output_read_message " status \@=" contained containedin=gg_r_construct_read_message
    syn match gg_h_clause_output_read_message " value \@=" contained containedin=gg_r_construct_read_message
    hi def link gg_h_clause_read_message    gliimClause
    hi def link gg_h_clause_output_read_message    gliimClauseOutput
    hi def link gg_h_construct_read_message    gliimConstruct
    hi def link gg_h_print_inline_read_message    gliimConstruct
syn region gg_r_construct_write_message start="^[[:space:]]*write-message" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_write_message,gg_r_inline_write_message,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_write_message,gg_r_inline_write_message,gg_r_at
    syn match gg_h_construct_write_message "^[[:space:]]*write-message" contained containedin=gg_r_construct_write_message
    syn match gg_h_clause_write_message " key \@=" contained containedin=gg_r_construct_write_message
    syn match gg_h_clause_write_message " value \@=" contained containedin=gg_r_construct_write_message
    hi def link gg_h_clause_write_message    gliimClause
    hi def link gg_h_clause_output_write_message    gliimClauseOutput
    hi def link gg_h_construct_write_message    gliimConstruct
    hi def link gg_h_print_inline_write_message    gliimConstruct
syn region gg_r_construct_delete_string start="^[[:space:]]*delete-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_string,gg_r_inline_delete_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_string,gg_r_inline_delete_string,gg_r_at
    syn match gg_h_construct_delete_string "^[[:space:]]*delete-string" contained containedin=gg_r_construct_delete_string
    hi def link gg_h_clause_delete_string    gliimClause
    hi def link gg_h_clause_output_delete_string    gliimClauseOutput
    hi def link gg_h_construct_delete_string    gliimConstruct
    hi def link gg_h_print_inline_delete_string    gliimConstruct
syn region gg_r_construct_end_if start="^[[:space:]]*end-if" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_if,gg_r_inline_end_if,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_if,gg_r_inline_end_if,gg_r_at
    syn match gg_h_construct_end_if "^[[:space:]]*end-if" contained containedin=gg_r_construct_end_if
    hi def link gg_h_clause_end_if    gliimClause
    hi def link gg_h_clause_output_end_if    gliimClauseOutput
    hi def link gg_h_construct_end_if    gliimConstruct
    hi def link gg_h_print_inline_end_if    gliimConstruct
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
    hi def link gg_h_clause_else_if    gliimClause
    hi def link gg_h_clause_output_else_if    gliimClauseOutput
    hi def link gg_h_construct_else_if    gliimConstruct
    hi def link gg_h_print_inline_else_if    gliimConstruct
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
    hi def link gg_h_clause_if_true    gliimClause
    hi def link gg_h_clause_output_if_true    gliimClauseOutput
    hi def link gg_h_construct_if_true    gliimConstruct
    hi def link gg_h_print_inline_if_true    gliimConstruct
syn region gg_r_construct_set_string start="^[[:space:]]*set-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_string,gg_r_inline_set_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_string,gg_r_inline_set_string,gg_r_at
    syn match gg_h_construct_set_string "^[[:space:]]*set-string" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " process-scope \@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " process-scope,\@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " process-scope$" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " unquoted \@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " unquoted,\@=" contained containedin=gg_r_construct_set_string
    syn match gg_h_clause_set_string " unquoted$" contained containedin=gg_r_construct_set_string
    hi def link gg_h_clause_set_string    gliimClause
    hi def link gg_h_clause_output_set_string    gliimClauseOutput
    hi def link gg_h_construct_set_string    gliimConstruct
    hi def link gg_h_print_inline_set_string    gliimConstruct
syn region gg_r_construct_set_number start="^[[:space:]]*set-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_number,gg_r_inline_set_number,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_number,gg_r_inline_set_number,gg_r_at
    syn match gg_h_construct_set_number "^[[:space:]]*set-number" contained containedin=gg_r_construct_set_number
    syn match gg_h_clause_set_number " process-scope \@=" contained containedin=gg_r_construct_set_number
    syn match gg_h_clause_set_number " process-scope,\@=" contained containedin=gg_r_construct_set_number
    syn match gg_h_clause_set_number " process-scope$" contained containedin=gg_r_construct_set_number
    hi def link gg_h_clause_set_number    gliimClause
    hi def link gg_h_clause_output_set_number    gliimClauseOutput
    hi def link gg_h_construct_set_number    gliimConstruct
    hi def link gg_h_print_inline_set_number    gliimConstruct
syn region gg_r_construct_set_bool start="^[[:space:]]*set-bool" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_bool,gg_r_inline_set_bool,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_bool,gg_r_inline_set_bool,gg_r_at
    syn match gg_h_construct_set_bool "^[[:space:]]*set-bool" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " process-scope \@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " process-scope,\@=" contained containedin=gg_r_construct_set_bool
    syn match gg_h_clause_set_bool " process-scope$" contained containedin=gg_r_construct_set_bool
    hi def link gg_h_clause_set_bool    gliimClause
    hi def link gg_h_clause_output_set_bool    gliimClauseOutput
    hi def link gg_h_construct_set_bool    gliimConstruct
    hi def link gg_h_print_inline_set_bool    gliimConstruct
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
    hi def link gg_h_clause___    gliimClause
    hi def link gg_h_clause_output___    gliimClauseOutput
    hi def link gg_h_construct___    gliimConstruct
    hi def link gg_h_print_inline___    gliimConstruct
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
    hi def link gg_h_clause_begin_handler    gliimClause
    hi def link gg_h_clause_output_begin_handler    gliimClauseOutput
    hi def link gg_h_construct_begin_handler    gliimConstruct
    hi def link gg_h_print_inline_begin_handler    gliimConstruct
syn region gg_r_construct_end_before_handler start="^[[:space:]]*end-before-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_before_handler,gg_r_inline_end_before_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_before_handler,gg_r_inline_end_before_handler,gg_r_at
    syn match gg_h_construct_end_before_handler "^[[:space:]]*end-before-handler" contained containedin=gg_r_construct_end_before_handler
    hi def link gg_h_clause_end_before_handler    gliimClause
    hi def link gg_h_clause_output_end_before_handler    gliimClauseOutput
    hi def link gg_h_construct_end_before_handler    gliimConstruct
    hi def link gg_h_print_inline_end_before_handler    gliimConstruct
syn region gg_r_construct_end_after_handler start="^[[:space:]]*end-after-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_after_handler,gg_r_inline_end_after_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_after_handler,gg_r_inline_end_after_handler,gg_r_at
    syn match gg_h_construct_end_after_handler "^[[:space:]]*end-after-handler" contained containedin=gg_r_construct_end_after_handler
    hi def link gg_h_clause_end_after_handler    gliimClause
    hi def link gg_h_clause_output_end_after_handler    gliimClauseOutput
    hi def link gg_h_construct_end_after_handler    gliimConstruct
    hi def link gg_h_print_inline_end_after_handler    gliimConstruct
syn region gg_r_construct_before_handler start="^[[:space:]]*before-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_before_handler,gg_r_inline_before_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_before_handler,gg_r_inline_before_handler,gg_r_at
    syn match gg_h_construct_before_handler "^[[:space:]]*before-handler" contained containedin=gg_r_construct_before_handler
    hi def link gg_h_clause_before_handler    gliimClause
    hi def link gg_h_clause_output_before_handler    gliimClauseOutput
    hi def link gg_h_construct_before_handler    gliimConstruct
    hi def link gg_h_print_inline_before_handler    gliimConstruct
syn region gg_r_construct_after_handler start="^[[:space:]]*after-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_after_handler,gg_r_inline_after_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_after_handler,gg_r_inline_after_handler,gg_r_at
    syn match gg_h_construct_after_handler "^[[:space:]]*after-handler" contained containedin=gg_r_construct_after_handler
    hi def link gg_h_clause_after_handler    gliimClause
    hi def link gg_h_clause_output_after_handler    gliimClauseOutput
    hi def link gg_h_construct_after_handler    gliimConstruct
    hi def link gg_h_print_inline_after_handler    gliimConstruct
syn region gg_r_construct_end_handler start="^[[:space:]]*end-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_handler,gg_r_inline_end_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_handler,gg_r_inline_end_handler,gg_r_at
    syn match gg_h_construct_end_handler "^[[:space:]]*end-handler" contained containedin=gg_r_construct_end_handler
    hi def link gg_h_clause_end_handler    gliimClause
    hi def link gg_h_clause_output_end_handler    gliimClauseOutput
    hi def link gg_h_construct_end_handler    gliimConstruct
    hi def link gg_h_print_inline_end_handler    gliimConstruct
syn region gg_r_construct_set_param start="^[[:space:]]*set-param" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_set_param,gg_r_inline_set_param,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_set_param,gg_r_inline_set_param,gg_r_at
    syn match gg_h_construct_set_param "^[[:space:]]*set-param" contained containedin=gg_r_construct_set_param
    syn match gg_h_clause_set_param " , \@=" contained containedin=gg_r_construct_set_param
    syn match gg_h_clause_set_param " ,,\@=" contained containedin=gg_r_construct_set_param
    syn match gg_h_clause_set_param " ,$" contained containedin=gg_r_construct_set_param
    hi def link gg_h_clause_set_param    gliimClause
    hi def link gg_h_clause_output_set_param    gliimClauseOutput
    hi def link gg_h_construct_set_param    gliimConstruct
    hi def link gg_h_print_inline_set_param    gliimConstruct
syn region gg_r_construct_get_param start="^[[:space:]]*get-param" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_param,gg_r_inline_get_param,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_param,gg_r_inline_get_param,gg_r_at
    syn match gg_h_construct_get_param "^[[:space:]]*get-param" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " , \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " ,,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " ,$" contained containedin=gg_r_construct_get_param
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
    syn match gg_h_clause_get_param " array \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " array,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " array$" contained containedin=gg_r_construct_get_param
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
    syn match gg_h_clause_get_param " index \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " index,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " index$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " index-cursor \@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " index-cursor,\@=" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " index-cursor$" contained containedin=gg_r_construct_get_param
    syn match gg_h_clause_get_param " type \@=" contained containedin=gg_r_construct_get_param
    hi def link gg_h_clause_get_param    gliimClause
    hi def link gg_h_clause_output_get_param    gliimClauseOutput
    hi def link gg_h_construct_get_param    gliimConstruct
    hi def link gg_h_print_inline_get_param    gliimConstruct
syn region gg_r_construct_get_cookie start="^[[:space:]]*get-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_cookie,gg_r_inline_get_cookie,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_cookie,gg_r_inline_get_cookie,gg_r_at
    syn match gg_h_construct_get_cookie "^[[:space:]]*get-cookie" contained containedin=gg_r_construct_get_cookie
    syn match gg_h_clause_get_cookie " , \@=" contained containedin=gg_r_construct_get_cookie
    syn match gg_h_clause_get_cookie " ,,\@=" contained containedin=gg_r_construct_get_cookie
    syn match gg_h_clause_get_cookie " ,$" contained containedin=gg_r_construct_get_cookie
    hi def link gg_h_clause_get_cookie    gliimClause
    hi def link gg_h_clause_output_get_cookie    gliimClauseOutput
    hi def link gg_h_construct_get_cookie    gliimConstruct
    hi def link gg_h_print_inline_get_cookie    gliimConstruct
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
    hi def link gg_h_clause_set_cookie    gliimClause
    hi def link gg_h_clause_output_set_cookie    gliimClauseOutput
    hi def link gg_h_construct_set_cookie    gliimConstruct
    hi def link gg_h_print_inline_set_cookie    gliimConstruct
syn region gg_r_construct_request_body start="^[[:space:]]*request-body" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_request_body,gg_r_inline_request_body,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_request_body,gg_r_inline_request_body,gg_r_at
    syn match gg_h_construct_request_body "^[[:space:]]*request-body" contained containedin=gg_r_construct_request_body
    hi def link gg_h_clause_request_body    gliimClause
    hi def link gg_h_clause_output_request_body    gliimClauseOutput
    hi def link gg_h_construct_request_body    gliimConstruct
    hi def link gg_h_print_inline_request_body    gliimConstruct
syn region gg_r_construct_delete_cookie start="^[[:space:]]*delete-cookie" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_cookie,gg_r_inline_delete_cookie,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_cookie,gg_r_inline_delete_cookie,gg_r_at
    syn match gg_h_construct_delete_cookie "^[[:space:]]*delete-cookie" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " path \@=" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " secure \@=" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " secure,\@=" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_delete_cookie " secure$" contained containedin=gg_r_construct_delete_cookie
    syn match gg_h_clause_output_delete_cookie " status \@=" contained containedin=gg_r_construct_delete_cookie
    hi def link gg_h_clause_delete_cookie    gliimClause
    hi def link gg_h_clause_output_delete_cookie    gliimClauseOutput
    hi def link gg_h_construct_delete_cookie    gliimConstruct
    hi def link gg_h_print_inline_delete_cookie    gliimConstruct
syn region gg_r_construct_copy_string start="^[[:space:]]*copy-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_copy_string,gg_r_inline_copy_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_copy_string,gg_r_inline_copy_string,gg_r_at
    syn match gg_h_construct_copy_string "^[[:space:]]*copy-string" contained containedin=gg_r_construct_copy_string
    syn match gg_h_clause_copy_string " length \@=" contained containedin=gg_r_construct_copy_string
    syn match gg_h_clause_copy_string " start-with \@=" contained containedin=gg_r_construct_copy_string
    syn match gg_h_clause_output_copy_string " to \@=" contained containedin=gg_r_construct_copy_string
    hi def link gg_h_clause_copy_string    gliimClause
    hi def link gg_h_clause_output_copy_string    gliimClauseOutput
    hi def link gg_h_construct_copy_string    gliimConstruct
    hi def link gg_h_print_inline_copy_string    gliimConstruct
syn region gg_r_construct_replace_string start="^[[:space:]]*replace-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_replace_string,gg_r_inline_replace_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_replace_string,gg_r_inline_replace_string,gg_r_at
    syn match gg_h_construct_replace_string "^[[:space:]]*replace-string" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " copy \@=" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " copy-end \@=" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " length \@=" contained containedin=gg_r_construct_replace_string
    syn match gg_h_clause_replace_string " start-with \@=" contained containedin=gg_r_construct_replace_string
    hi def link gg_h_clause_replace_string    gliimClause
    hi def link gg_h_clause_output_replace_string    gliimClauseOutput
    hi def link gg_h_construct_replace_string    gliimConstruct
    hi def link gg_h_print_inline_replace_string    gliimConstruct
syn region gg_r_construct_pf_url start="^[[:space:]]*pf-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pf_url,gg_r_inline_pf_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pf_url,gg_r_inline_pf_url,gg_r_at
    syn match gg_h_construct_pf_url "^[[:space:]]*pf-url" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " , \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " ,,\@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " ,$" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " ,\(>>\)\@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url " , \@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_clause_output_pf_url " to \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " to \@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_clause_pf_url " to-error \@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " to-error,\@=" contained containedin=gg_r_construct_pf_url
    syn match gg_h_clause_pf_url " to-error$" contained containedin=gg_r_construct_pf_url
    syn match gg_h_print_inline_pf_url " to-error\(>>\)\@=" contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url " to-error \@=" contained containedin=gg_r_inline_pf_url
    syn region gg_r_inline_pf_url start="<<[[:space:]]*pf-url \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_pf_url '<<[[:space:]]*pf-url \@=' contained containedin=gg_r_inline_pf_url
    syn match gg_h_print_inline_pf_url '>>' contained containedin=gg_r_inline_pf_url
    hi def link gg_h_clause_pf_url    gliimClause
    hi def link gg_h_clause_output_pf_url    gliimClauseOutput
    hi def link gg_h_construct_pf_url    gliimConstruct
    hi def link gg_h_print_inline_pf_url    gliimConstruct
syn region gg_r_construct_trace_run start="^[[:space:]]*trace-run" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_trace_run,gg_r_inline_trace_run,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_trace_run,gg_r_inline_trace_run,gg_r_at
    syn match gg_h_construct_trace_run "^[[:space:]]*trace-run" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_trace_run " , \@=" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_trace_run " ,,\@=" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_trace_run " ,$" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_output_trace_run " to \@=" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_trace_run " to-error \@=" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_trace_run " to-error,\@=" contained containedin=gg_r_construct_trace_run
    syn match gg_h_clause_trace_run " to-error$" contained containedin=gg_r_construct_trace_run
    hi def link gg_h_clause_trace_run    gliimClause
    hi def link gg_h_clause_output_trace_run    gliimClauseOutput
    hi def link gg_h_construct_trace_run    gliimConstruct
    hi def link gg_h_print_inline_trace_run    gliimConstruct
syn region gg_r_construct_report_error start="^[[:space:]]*report-error" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_report_error,gg_r_inline_report_error,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_report_error,gg_r_inline_report_error,gg_r_at
    syn match gg_h_construct_report_error "^[[:space:]]*report-error" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " , \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " ,,\@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " ,$" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_output_report_error " to \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to-error \@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to-error,\@=" contained containedin=gg_r_construct_report_error
    syn match gg_h_clause_report_error " to-error$" contained containedin=gg_r_construct_report_error
    hi def link gg_h_clause_report_error    gliimClause
    hi def link gg_h_clause_output_report_error    gliimClauseOutput
    hi def link gg_h_construct_report_error    gliimConstruct
    hi def link gg_h_print_inline_report_error    gliimConstruct
syn region gg_r_construct_pf_web start="^[[:space:]]*pf-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pf_web,gg_r_inline_pf_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pf_web,gg_r_inline_pf_web,gg_r_at
    syn match gg_h_construct_pf_web "^[[:space:]]*pf-web" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " , \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " ,,\@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " ,$" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " ,\(>>\)\@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web " , \@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_clause_output_pf_web " to \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " to \@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_clause_pf_web " to-error \@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " to-error,\@=" contained containedin=gg_r_construct_pf_web
    syn match gg_h_clause_pf_web " to-error$" contained containedin=gg_r_construct_pf_web
    syn match gg_h_print_inline_pf_web " to-error\(>>\)\@=" contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web " to-error \@=" contained containedin=gg_r_inline_pf_web
    syn region gg_r_inline_pf_web start="<<[[:space:]]*pf-web \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_pf_web '<<[[:space:]]*pf-web \@=' contained containedin=gg_r_inline_pf_web
    syn match gg_h_print_inline_pf_web '>>' contained containedin=gg_r_inline_pf_web
    hi def link gg_h_clause_pf_web    gliimClause
    hi def link gg_h_clause_output_pf_web    gliimClauseOutput
    hi def link gg_h_construct_pf_web    gliimConstruct
    hi def link gg_h_print_inline_pf_web    gliimConstruct
syn region gg_r_construct_pf_out start="^[[:space:]]*pf-out" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pf_out,gg_r_inline_pf_out,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pf_out,gg_r_inline_pf_out,gg_r_at
    syn match gg_h_construct_pf_out "^[[:space:]]*pf-out" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " , \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " ,,\@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " ,$" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " ,\(>>\)\@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out " , \@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_clause_output_pf_out " to \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " to \@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_clause_pf_out " to-error \@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " to-error,\@=" contained containedin=gg_r_construct_pf_out
    syn match gg_h_clause_pf_out " to-error$" contained containedin=gg_r_construct_pf_out
    syn match gg_h_print_inline_pf_out " to-error\(>>\)\@=" contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out " to-error \@=" contained containedin=gg_r_inline_pf_out
    syn region gg_r_inline_pf_out start="<<[[:space:]]*pf-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_pf_out '<<[[:space:]]*pf-out \@=' contained containedin=gg_r_inline_pf_out
    syn match gg_h_print_inline_pf_out '>>' contained containedin=gg_r_inline_pf_out
    hi def link gg_h_clause_pf_out    gliimClause
    hi def link gg_h_clause_output_pf_out    gliimClauseOutput
    hi def link gg_h_construct_pf_out    gliimConstruct
    hi def link gg_h_print_inline_pf_out    gliimConstruct
syn region gg_r_construct_flush_output start="^[[:space:]]*flush-output" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_flush_output,gg_r_inline_flush_output,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_flush_output,gg_r_inline_flush_output,gg_r_at
    syn match gg_h_construct_flush_output "^[[:space:]]*flush-output" contained containedin=gg_r_construct_flush_output
    hi def link gg_h_clause_flush_output    gliimClause
    hi def link gg_h_clause_output_flush_output    gliimClauseOutput
    hi def link gg_h_construct_flush_output    gliimConstruct
    hi def link gg_h_print_inline_flush_output    gliimConstruct
syn region gg_r_construct_internal_mode start="^[[:space:]]*internal-mode" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_internal_mode,gg_r_inline_internal_mode,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_internal_mode,gg_r_inline_internal_mode,gg_r_at
    syn match gg_h_construct_internal_mode "^[[:space:]]*internal-mode" contained containedin=gg_r_construct_internal_mode
    hi def link gg_h_clause_internal_mode    gliimClause
    hi def link gg_h_clause_output_internal_mode    gliimClauseOutput
    hi def link gg_h_construct_internal_mode    gliimConstruct
    hi def link gg_h_print_inline_internal_mode    gliimConstruct
syn region gg_r_construct_extended_mode start="^[[:space:]]*extended-mode" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_extended_mode,gg_r_inline_extended_mode,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_extended_mode,gg_r_inline_extended_mode,gg_r_at
    syn match gg_h_construct_extended_mode "^[[:space:]]*extended-mode" contained containedin=gg_r_construct_extended_mode
    hi def link gg_h_clause_extended_mode    gliimClause
    hi def link gg_h_clause_output_extended_mode    gliimClauseOutput
    hi def link gg_h_construct_extended_mode    gliimConstruct
    hi def link gg_h_print_inline_extended_mode    gliimConstruct
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
    hi def link gg_h_clause_number_string    gliimClause
    hi def link gg_h_clause_output_number_string    gliimClauseOutput
    hi def link gg_h_construct_number_string    gliimConstruct
    hi def link gg_h_print_inline_number_string    gliimConstruct
syn region gg_r_construct_string_number start="^[[:space:]]*string-number" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_string_number,gg_r_inline_string_number,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_string_number,gg_r_inline_string_number,gg_r_at
    syn match gg_h_construct_string_number "^[[:space:]]*string-number" contained containedin=gg_r_construct_string_number
    syn match gg_h_clause_string_number " base \@=" contained containedin=gg_r_construct_string_number
    syn match gg_h_clause_output_string_number " status \@=" contained containedin=gg_r_construct_string_number
    syn match gg_h_clause_output_string_number " to \@=" contained containedin=gg_r_construct_string_number
    hi def link gg_h_clause_string_number    gliimClause
    hi def link gg_h_clause_output_string_number    gliimClauseOutput
    hi def link gg_h_construct_string_number    gliimConstruct
    hi def link gg_h_print_inline_string_number    gliimConstruct
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
    syn match gg_h_clause_get_req " errno \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " errno,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " errno$" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " error \@=" contained containedin=gg_r_construct_get_req
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
    syn match gg_h_clause_output_get_req " to \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " trace-file \@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " trace-file,\@=" contained containedin=gg_r_construct_get_req
    syn match gg_h_clause_get_req " trace-file$" contained containedin=gg_r_construct_get_req
    hi def link gg_h_clause_get_req    gliimClause
    hi def link gg_h_clause_output_get_req    gliimClauseOutput
    hi def link gg_h_construct_get_req    gliimConstruct
    hi def link gg_h_print_inline_get_req    gliimConstruct
syn region gg_r_construct_get_app start="^[[:space:]]*get-app" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_app,gg_r_inline_get_app,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_app,gg_r_inline_get_app,gg_r_at
    syn match gg_h_construct_get_app "^[[:space:]]*get-app" contained containedin=gg_r_construct_get_app
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
    syn match gg_h_clause_output_get_app " to \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " trace-directory \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " trace-directory,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " trace-directory$" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " upload-size \@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " upload-size,\@=" contained containedin=gg_r_construct_get_app
    syn match gg_h_clause_get_app " upload-size$" contained containedin=gg_r_construct_get_app
    hi def link gg_h_clause_get_app    gliimClause
    hi def link gg_h_clause_output_get_app    gliimClauseOutput
    hi def link gg_h_construct_get_app    gliimConstruct
    hi def link gg_h_print_inline_get_app    gliimConstruct
syn region gg_r_construct_get_sys start="^[[:space:]]*get-sys" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_sys,gg_r_inline_get_sys,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_sys,gg_r_inline_get_sys,gg_r_at
    syn match gg_h_construct_get_sys "^[[:space:]]*get-sys" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " directory \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " directory,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " directory$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " environment \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-name \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-name,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-name$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-version \@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-version,\@=" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_get_sys " os-version$" contained containedin=gg_r_construct_get_sys
    syn match gg_h_clause_output_get_sys " to \@=" contained containedin=gg_r_construct_get_sys
    hi def link gg_h_clause_get_sys    gliimClause
    hi def link gg_h_clause_output_get_sys    gliimClauseOutput
    hi def link gg_h_construct_get_sys    gliimConstruct
    hi def link gg_h_print_inline_get_sys    gliimConstruct
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
    syn match gg_h_clause_match_regex " utf8 \@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " utf8,\@=" contained containedin=gg_r_construct_match_regex
    syn match gg_h_clause_match_regex " utf8$" contained containedin=gg_r_construct_match_regex
    hi def link gg_h_clause_match_regex    gliimClause
    hi def link gg_h_clause_output_match_regex    gliimClauseOutput
    hi def link gg_h_construct_match_regex    gliimConstruct
    hi def link gg_h_print_inline_match_regex    gliimConstruct
syn region gg_r_construct_get_time start="^[[:space:]]*get-time" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_get_time,gg_r_inline_get_time,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_get_time,gg_r_inline_get_time,gg_r_at
    syn match gg_h_construct_get_time "^[[:space:]]*get-time" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " day \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " format \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " hour \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " minute \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " month \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " second \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " timezone \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_output_get_time " to \@=" contained containedin=gg_r_construct_get_time
    syn match gg_h_clause_get_time " year \@=" contained containedin=gg_r_construct_get_time
    hi def link gg_h_clause_get_time    gliimClause
    hi def link gg_h_clause_output_get_time    gliimClauseOutput
    hi def link gg_h_construct_get_time    gliimConstruct
    hi def link gg_h_print_inline_get_time    gliimConstruct
syn region gg_r_construct_uniq_file start="^[[:space:]]*uniq-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_uniq_file,gg_r_inline_uniq_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_uniq_file,gg_r_inline_uniq_file,gg_r_at
    syn match gg_h_construct_uniq_file "^[[:space:]]*uniq-file" contained containedin=gg_r_construct_uniq_file
    syn match gg_h_clause_uniq_file " temporary \@=" contained containedin=gg_r_construct_uniq_file
    syn match gg_h_clause_uniq_file " temporary,\@=" contained containedin=gg_r_construct_uniq_file
    syn match gg_h_clause_uniq_file " temporary$" contained containedin=gg_r_construct_uniq_file
    hi def link gg_h_clause_uniq_file    gliimClause
    hi def link gg_h_clause_output_uniq_file    gliimClauseOutput
    hi def link gg_h_construct_uniq_file    gliimConstruct
    hi def link gg_h_print_inline_uniq_file    gliimConstruct
syn region gg_r_construct_call_handler start="^[[:space:]]*call-handler" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_handler,gg_r_inline_call_handler,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_handler,gg_r_inline_call_handler,gg_r_at
    syn match gg_h_construct_call_handler "^[[:space:]]*call-handler" contained containedin=gg_r_construct_call_handler
    syn region gg_r_inline_call_handler start="<<[[:space:]]*call-handler \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_call_handler '<<[[:space:]]*call-handler \@=' contained containedin=gg_r_inline_call_handler
    syn match gg_h_print_inline_call_handler '>>' contained containedin=gg_r_inline_call_handler
    hi def link gg_h_clause_call_handler    gliimClause
    hi def link gg_h_clause_output_call_handler    gliimClauseOutput
    hi def link gg_h_construct_call_handler    gliimConstruct
    hi def link gg_h_print_inline_call_handler    gliimConstruct
syn region gg_r_construct_call_remote start="^[[:space:]]*call-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_remote,gg_r_inline_call_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_remote,gg_r_inline_call_remote,gg_r_at
    syn match gg_h_construct_call_remote "^[[:space:]]*call-remote" contained containedin=gg_r_construct_call_remote
    syn match gg_h_clause_output_call_remote " finished-okay \@=" contained containedin=gg_r_construct_call_remote
    syn match gg_h_clause_output_call_remote " started \@=" contained containedin=gg_r_construct_call_remote
    syn match gg_h_clause_output_call_remote " status \@=" contained containedin=gg_r_construct_call_remote
    hi def link gg_h_clause_call_remote    gliimClause
    hi def link gg_h_clause_output_call_remote    gliimClauseOutput
    hi def link gg_h_construct_call_remote    gliimConstruct
    hi def link gg_h_print_inline_call_remote    gliimConstruct
syn region gg_r_construct_call_extended start="^[[:space:]]*call-extended" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_call_extended,gg_r_inline_call_extended,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_call_extended,gg_r_inline_call_extended,gg_r_at
    syn match gg_h_construct_call_extended "^[[:space:]]*call-extended" contained containedin=gg_r_construct_call_extended
    syn region gg_r_inline_call_extended start="<<[[:space:]]*call-extended \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_call_extended '<<[[:space:]]*call-extended \@=' contained containedin=gg_r_inline_call_extended
    syn match gg_h_print_inline_call_extended '>>' contained containedin=gg_r_inline_call_extended
    hi def link gg_h_clause_call_extended    gliimClause
    hi def link gg_h_clause_output_call_extended    gliimClauseOutput
    hi def link gg_h_construct_call_extended    gliimConstruct
    hi def link gg_h_print_inline_call_extended    gliimConstruct
syn region gg_r_construct_read_remote start="^[[:space:]]*read-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_read_remote,gg_r_inline_read_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_read_remote,gg_r_inline_read_remote,gg_r_at
    syn match gg_h_construct_read_remote "^[[:space:]]*read-remote" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_output_read_remote " data \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_output_read_remote " error \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_output_read_remote " handler-status \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_output_read_remote " status \@=" contained containedin=gg_r_construct_read_remote
    syn match gg_h_clause_output_read_remote " status-text \@=" contained containedin=gg_r_construct_read_remote
    hi def link gg_h_clause_read_remote    gliimClause
    hi def link gg_h_clause_output_read_remote    gliimClauseOutput
    hi def link gg_h_construct_read_remote    gliimConstruct
    hi def link gg_h_print_inline_read_remote    gliimConstruct
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
    hi def link gg_h_clause_new_remote    gliimClause
    hi def link gg_h_clause_output_new_remote    gliimClauseOutput
    hi def link gg_h_construct_new_remote    gliimConstruct
    hi def link gg_h_print_inline_new_remote    gliimConstruct
syn region gg_r_construct_run_remote start="^[[:space:]]*run-remote" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_run_remote,gg_r_inline_run_remote,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_run_remote,gg_r_inline_run_remote,gg_r_at
    syn match gg_h_construct_run_remote "^[[:space:]]*run-remote" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " app-path \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " content \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " content-length \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " content-type \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " data \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " environment \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " error \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " finished-okay \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " handler-status \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " local \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " location \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " method \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " request-body \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " request-path \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " started \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " status \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_output_run_remote " status-text \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " timeout \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " url-params \@=" contained containedin=gg_r_construct_run_remote
    syn match gg_h_clause_run_remote " url-path \@=" contained containedin=gg_r_construct_run_remote
    hi def link gg_h_clause_run_remote    gliimClause
    hi def link gg_h_clause_output_run_remote    gliimClauseOutput
    hi def link gg_h_construct_run_remote    gliimConstruct
    hi def link gg_h_print_inline_run_remote    gliimConstruct
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
    hi def link gg_h_clause_call_web    gliimClause
    hi def link gg_h_clause_output_call_web    gliimClauseOutput
    hi def link gg_h_construct_call_web    gliimConstruct
    hi def link gg_h_print_inline_call_web    gliimConstruct
syn region gg_r_construct_delete_file start="^[[:space:]]*delete-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_delete_file,gg_r_inline_delete_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_delete_file,gg_r_inline_delete_file,gg_r_at
    syn match gg_h_construct_delete_file "^[[:space:]]*delete-file" contained containedin=gg_r_construct_delete_file
    syn match gg_h_clause_output_delete_file " status \@=" contained containedin=gg_r_construct_delete_file
    hi def link gg_h_clause_delete_file    gliimClause
    hi def link gg_h_clause_output_delete_file    gliimClauseOutput
    hi def link gg_h_construct_delete_file    gliimConstruct
    hi def link gg_h_print_inline_delete_file    gliimConstruct
syn region gg_r_construct_rename_file start="^[[:space:]]*rename-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_rename_file,gg_r_inline_rename_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_rename_file,gg_r_inline_rename_file,gg_r_at
    syn match gg_h_construct_rename_file "^[[:space:]]*rename-file" contained containedin=gg_r_construct_rename_file
    syn match gg_h_clause_output_rename_file " status \@=" contained containedin=gg_r_construct_rename_file
    syn match gg_h_clause_rename_file " to \@=" contained containedin=gg_r_construct_rename_file
    hi def link gg_h_clause_rename_file    gliimClause
    hi def link gg_h_clause_output_rename_file    gliimClauseOutput
    hi def link gg_h_construct_rename_file    gliimConstruct
    hi def link gg_h_print_inline_rename_file    gliimConstruct
syn region gg_r_construct_text_utf8 start="^[[:space:]]*text-utf8" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_text_utf8,gg_r_inline_text_utf8,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_text_utf8,gg_r_inline_text_utf8,gg_r_at
    syn match gg_h_construct_text_utf8 "^[[:space:]]*text-utf8" contained containedin=gg_r_construct_text_utf8
    syn match gg_h_clause_output_text_utf8 " error-text \@=" contained containedin=gg_r_construct_text_utf8
    syn match gg_h_clause_output_text_utf8 " status \@=" contained containedin=gg_r_construct_text_utf8
    hi def link gg_h_clause_text_utf8    gliimClause
    hi def link gg_h_clause_output_text_utf8    gliimClauseOutput
    hi def link gg_h_construct_text_utf8    gliimConstruct
    hi def link gg_h_print_inline_text_utf8    gliimConstruct
syn region gg_r_construct_utf8_text start="^[[:space:]]*utf8-text" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_utf8_text,gg_r_inline_utf8_text,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_utf8_text,gg_r_inline_utf8_text,gg_r_at
    syn match gg_h_construct_utf8_text "^[[:space:]]*utf8-text" contained containedin=gg_r_construct_utf8_text
    syn match gg_h_clause_output_utf8_text " error-text \@=" contained containedin=gg_r_construct_utf8_text
    syn match gg_h_clause_utf8_text " length \@=" contained containedin=gg_r_construct_utf8_text
    syn match gg_h_clause_output_utf8_text " status \@=" contained containedin=gg_r_construct_utf8_text
    syn match gg_h_clause_output_utf8_text " to \@=" contained containedin=gg_r_construct_utf8_text
    hi def link gg_h_clause_utf8_text    gliimClause
    hi def link gg_h_clause_output_utf8_text    gliimClauseOutput
    hi def link gg_h_construct_utf8_text    gliimConstruct
    hi def link gg_h_print_inline_utf8_text    gliimConstruct
syn region gg_r_construct_stat_file start="^[[:space:]]*stat-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_stat_file,gg_r_inline_stat_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_stat_file,gg_r_inline_stat_file,gg_r_at
    syn match gg_h_construct_stat_file "^[[:space:]]*stat-file" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " name \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " name,\@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " name$" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " path \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " path,\@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " path$" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " size \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " size,\@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " size$" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_output_stat_file " to \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " type \@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " type,\@=" contained containedin=gg_r_construct_stat_file
    syn match gg_h_clause_stat_file " type$" contained containedin=gg_r_construct_stat_file
    hi def link gg_h_clause_stat_file    gliimClause
    hi def link gg_h_clause_output_stat_file    gliimClauseOutput
    hi def link gg_h_construct_stat_file    gliimConstruct
    hi def link gg_h_print_inline_stat_file    gliimConstruct
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
    hi def link gg_h_clause_decrypt_data    gliimClause
    hi def link gg_h_clause_output_decrypt_data    gliimClauseOutput
    hi def link gg_h_construct_decrypt_data    gliimConstruct
    hi def link gg_h_print_inline_decrypt_data    gliimConstruct
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
    hi def link gg_h_clause_encrypt_data    gliimClause
    hi def link gg_h_clause_output_encrypt_data    gliimClauseOutput
    hi def link gg_h_construct_encrypt_data    gliimConstruct
    hi def link gg_h_print_inline_encrypt_data    gliimConstruct
syn region gg_r_construct_trim_string start="^[[:space:]]*trim-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_trim_string,gg_r_inline_trim_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_trim_string,gg_r_inline_trim_string,gg_r_at
    syn match gg_h_construct_trim_string "^[[:space:]]*trim-string" contained containedin=gg_r_construct_trim_string
    hi def link gg_h_clause_trim_string    gliimClause
    hi def link gg_h_clause_output_trim_string    gliimClauseOutput
    hi def link gg_h_construct_trim_string    gliimConstruct
    hi def link gg_h_print_inline_trim_string    gliimConstruct
syn region gg_r_construct_encode_web start="^[[:space:]]*encode-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_web,gg_r_inline_encode_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_web,gg_r_inline_encode_web,gg_r_at
    syn match gg_h_construct_encode_web "^[[:space:]]*encode-web" contained containedin=gg_r_construct_encode_web
    syn match gg_h_clause_encode_web " input-length \@=" contained containedin=gg_r_construct_encode_web
    syn match gg_h_clause_output_encode_web " to \@=" contained containedin=gg_r_construct_encode_web
    hi def link gg_h_clause_encode_web    gliimClause
    hi def link gg_h_clause_output_encode_web    gliimClauseOutput
    hi def link gg_h_construct_encode_web    gliimConstruct
    hi def link gg_h_print_inline_encode_web    gliimConstruct
syn region gg_r_construct_encode_url start="^[[:space:]]*encode-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_url,gg_r_inline_encode_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_url,gg_r_inline_encode_url,gg_r_at
    syn match gg_h_construct_encode_url "^[[:space:]]*encode-url" contained containedin=gg_r_construct_encode_url
    syn match gg_h_clause_encode_url " input-length \@=" contained containedin=gg_r_construct_encode_url
    syn match gg_h_clause_output_encode_url " to \@=" contained containedin=gg_r_construct_encode_url
    hi def link gg_h_clause_encode_url    gliimClause
    hi def link gg_h_clause_output_encode_url    gliimClauseOutput
    hi def link gg_h_construct_encode_url    gliimConstruct
    hi def link gg_h_print_inline_encode_url    gliimConstruct
syn region gg_r_construct_decode_web start="^[[:space:]]*decode-web" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_web,gg_r_inline_decode_web,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_web,gg_r_inline_decode_web,gg_r_at
    syn match gg_h_construct_decode_web "^[[:space:]]*decode-web" contained containedin=gg_r_construct_decode_web
    syn match gg_h_clause_decode_web " input-length \@=" contained containedin=gg_r_construct_decode_web
    syn match gg_h_clause_decode_web " status \@=" contained containedin=gg_r_construct_decode_web
    hi def link gg_h_clause_decode_web    gliimClause
    hi def link gg_h_clause_output_decode_web    gliimClauseOutput
    hi def link gg_h_construct_decode_web    gliimConstruct
    hi def link gg_h_print_inline_decode_web    gliimConstruct
syn region gg_r_construct_decode_url start="^[[:space:]]*decode-url" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_url,gg_r_inline_decode_url,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_url,gg_r_inline_decode_url,gg_r_at
    syn match gg_h_construct_decode_url "^[[:space:]]*decode-url" contained containedin=gg_r_construct_decode_url
    syn match gg_h_clause_decode_url " input-length \@=" contained containedin=gg_r_construct_decode_url
    syn match gg_h_clause_decode_url " status \@=" contained containedin=gg_r_construct_decode_url
    hi def link gg_h_clause_decode_url    gliimClause
    hi def link gg_h_clause_output_decode_url    gliimClauseOutput
    hi def link gg_h_construct_decode_url    gliimConstruct
    hi def link gg_h_print_inline_decode_url    gliimConstruct
syn region gg_r_construct_decode_hex start="^[[:space:]]*decode-hex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_hex,gg_r_inline_decode_hex,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_hex,gg_r_inline_decode_hex,gg_r_at
    syn match gg_h_construct_decode_hex "^[[:space:]]*decode-hex" contained containedin=gg_r_construct_decode_hex
    syn match gg_h_clause_decode_hex " input-length \@=" contained containedin=gg_r_construct_decode_hex
    syn match gg_h_clause_output_decode_hex " to \@=" contained containedin=gg_r_construct_decode_hex
    hi def link gg_h_clause_decode_hex    gliimClause
    hi def link gg_h_clause_output_decode_hex    gliimClauseOutput
    hi def link gg_h_construct_decode_hex    gliimConstruct
    hi def link gg_h_print_inline_decode_hex    gliimConstruct
syn region gg_r_construct_encode_hex start="^[[:space:]]*encode-hex" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_hex,gg_r_inline_encode_hex,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_hex,gg_r_inline_encode_hex,gg_r_at
    syn match gg_h_construct_encode_hex "^[[:space:]]*encode-hex" contained containedin=gg_r_construct_encode_hex
    syn match gg_h_clause_encode_hex " input-length \@=" contained containedin=gg_r_construct_encode_hex
    syn match gg_h_clause_encode_hex " prefix \@=" contained containedin=gg_r_construct_encode_hex
    syn match gg_h_clause_output_encode_hex " to \@=" contained containedin=gg_r_construct_encode_hex
    hi def link gg_h_clause_encode_hex    gliimClause
    hi def link gg_h_clause_output_encode_hex    gliimClauseOutput
    hi def link gg_h_construct_encode_hex    gliimConstruct
    hi def link gg_h_print_inline_encode_hex    gliimConstruct
syn region gg_r_construct_encode_base64 start="^[[:space:]]*encode-base64" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_encode_base64,gg_r_inline_encode_base64,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_encode_base64,gg_r_inline_encode_base64,gg_r_at
    syn match gg_h_construct_encode_base64 "^[[:space:]]*encode-base64" contained containedin=gg_r_construct_encode_base64
    syn match gg_h_clause_encode_base64 " input-length \@=" contained containedin=gg_r_construct_encode_base64
    syn match gg_h_clause_output_encode_base64 " to \@=" contained containedin=gg_r_construct_encode_base64
    hi def link gg_h_clause_encode_base64    gliimClause
    hi def link gg_h_clause_output_encode_base64    gliimClauseOutput
    hi def link gg_h_construct_encode_base64    gliimConstruct
    hi def link gg_h_print_inline_encode_base64    gliimConstruct
syn region gg_r_construct_decode_base64 start="^[[:space:]]*decode-base64" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_decode_base64,gg_r_inline_decode_base64,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_decode_base64,gg_r_inline_decode_base64,gg_r_at
    syn match gg_h_construct_decode_base64 "^[[:space:]]*decode-base64" contained containedin=gg_r_construct_decode_base64
    syn match gg_h_clause_decode_base64 " input-length \@=" contained containedin=gg_r_construct_decode_base64
    syn match gg_h_clause_output_decode_base64 " to \@=" contained containedin=gg_r_construct_decode_base64
    hi def link gg_h_clause_decode_base64    gliimClause
    hi def link gg_h_clause_output_decode_base64    gliimClauseOutput
    hi def link gg_h_construct_decode_base64    gliimConstruct
    hi def link gg_h_print_inline_decode_base64    gliimConstruct
syn region gg_r_construct_hash_string start="^[[:space:]]*hash-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_hash_string,gg_r_inline_hash_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_hash_string,gg_r_inline_hash_string,gg_r_at
    syn match gg_h_construct_hash_string "^[[:space:]]*hash-string" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " binary \@=" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " binary,\@=" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " binary$" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_hash_string " digest \@=" contained containedin=gg_r_construct_hash_string
    syn match gg_h_clause_output_hash_string " to \@=" contained containedin=gg_r_construct_hash_string
    hi def link gg_h_clause_hash_string    gliimClause
    hi def link gg_h_clause_output_hash_string    gliimClauseOutput
    hi def link gg_h_construct_hash_string    gliimConstruct
    hi def link gg_h_print_inline_hash_string    gliimConstruct
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
    hi def link gg_h_clause_hmac_string    gliimClause
    hi def link gg_h_clause_output_hmac_string    gliimClauseOutput
    hi def link gg_h_construct_hmac_string    gliimConstruct
    hi def link gg_h_print_inline_hmac_string    gliimConstruct
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
    hi def link gg_h_clause_derive_key    gliimClause
    hi def link gg_h_clause_output_derive_key    gliimClauseOutput
    hi def link gg_h_construct_derive_key    gliimConstruct
    hi def link gg_h_print_inline_derive_key    gliimConstruct
syn region gg_r_construct_count_substring start="^[[:space:]]*count-substring" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_count_substring,gg_r_inline_count_substring,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_count_substring,gg_r_inline_count_substring,gg_r_at
    syn match gg_h_construct_count_substring "^[[:space:]]*count-substring" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " case-insensitive \@=" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " case-insensitive,\@=" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " case-insensitive$" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_count_substring " in \@=" contained containedin=gg_r_construct_count_substring
    syn match gg_h_clause_output_count_substring " to \@=" contained containedin=gg_r_construct_count_substring
    hi def link gg_h_clause_count_substring    gliimClause
    hi def link gg_h_clause_output_count_substring    gliimClauseOutput
    hi def link gg_h_construct_count_substring    gliimConstruct
    hi def link gg_h_print_inline_count_substring    gliimConstruct
syn region gg_r_construct_string_length start="^[[:space:]]*string-length" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_string_length,gg_r_inline_string_length,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_string_length,gg_r_inline_string_length,gg_r_at
    syn match gg_h_construct_string_length "^[[:space:]]*string-length" contained containedin=gg_r_construct_string_length
    syn match gg_h_clause_output_string_length " to \@=" contained containedin=gg_r_construct_string_length
    syn match gg_h_print_inline_string_length " to \@=" contained containedin=gg_r_inline_string_length
    syn region gg_r_inline_string_length start="<<[[:space:]]*string-length \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_string_length '<<[[:space:]]*string-length \@=' contained containedin=gg_r_inline_string_length
    syn match gg_h_print_inline_string_length '>>' contained containedin=gg_r_inline_string_length
    hi def link gg_h_clause_string_length    gliimClause
    hi def link gg_h_clause_output_string_length    gliimClauseOutput
    hi def link gg_h_construct_string_length    gliimConstruct
    hi def link gg_h_print_inline_string_length    gliimConstruct
syn region gg_r_construct_lower_string start="^[[:space:]]*lower-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_lower_string,gg_r_inline_lower_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_lower_string,gg_r_inline_lower_string,gg_r_at
    syn match gg_h_construct_lower_string "^[[:space:]]*lower-string" contained containedin=gg_r_construct_lower_string
    hi def link gg_h_clause_lower_string    gliimClause
    hi def link gg_h_clause_output_lower_string    gliimClauseOutput
    hi def link gg_h_construct_lower_string    gliimConstruct
    hi def link gg_h_print_inline_lower_string    gliimConstruct
syn region gg_r_construct_upper_string start="^[[:space:]]*upper-string" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_upper_string,gg_r_inline_upper_string,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_upper_string,gg_r_inline_upper_string,gg_r_at
    syn match gg_h_construct_upper_string "^[[:space:]]*upper-string" contained containedin=gg_r_construct_upper_string
    hi def link gg_h_clause_upper_string    gliimClause
    hi def link gg_h_clause_output_upper_string    gliimClauseOutput
    hi def link gg_h_construct_upper_string    gliimConstruct
    hi def link gg_h_print_inline_upper_string    gliimConstruct
syn region gg_r_construct_random_crypto start="^[[:space:]]*random-crypto" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_random_crypto,gg_r_inline_random_crypto,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_random_crypto,gg_r_inline_random_crypto,gg_r_at
    syn match gg_h_construct_random_crypto "^[[:space:]]*random-crypto" contained containedin=gg_r_construct_random_crypto
    syn match gg_h_clause_random_crypto " length \@=" contained containedin=gg_r_construct_random_crypto
    syn match gg_h_clause_output_random_crypto " to \@=" contained containedin=gg_r_construct_random_crypto
    hi def link gg_h_clause_random_crypto    gliimClause
    hi def link gg_h_clause_output_random_crypto    gliimClauseOutput
    hi def link gg_h_construct_random_crypto    gliimConstruct
    hi def link gg_h_print_inline_random_crypto    gliimConstruct
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
    hi def link gg_h_clause_random_string    gliimClause
    hi def link gg_h_clause_output_random_string    gliimClauseOutput
    hi def link gg_h_construct_random_string    gliimConstruct
    hi def link gg_h_print_inline_random_string    gliimConstruct
syn region gg_r_construct_unlock_file start="^[[:space:]]*unlock-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_unlock_file,gg_r_inline_unlock_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_unlock_file,gg_r_inline_unlock_file,gg_r_at
    syn match gg_h_construct_unlock_file "^[[:space:]]*unlock-file" contained containedin=gg_r_construct_unlock_file
    syn match gg_h_clause_unlock_file " id \@=" contained containedin=gg_r_construct_unlock_file
    hi def link gg_h_clause_unlock_file    gliimClause
    hi def link gg_h_clause_output_unlock_file    gliimClauseOutput
    hi def link gg_h_construct_unlock_file    gliimConstruct
    hi def link gg_h_print_inline_unlock_file    gliimConstruct
syn region gg_r_construct_lock_file start="^[[:space:]]*lock-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_lock_file,gg_r_inline_lock_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_lock_file,gg_r_inline_lock_file,gg_r_at
    syn match gg_h_construct_lock_file "^[[:space:]]*lock-file" contained containedin=gg_r_construct_lock_file
    syn match gg_h_clause_output_lock_file " id \@=" contained containedin=gg_r_construct_lock_file
    syn match gg_h_clause_output_lock_file " status \@=" contained containedin=gg_r_construct_lock_file
    hi def link gg_h_clause_lock_file    gliimClause
    hi def link gg_h_clause_output_lock_file    gliimClauseOutput
    hi def link gg_h_construct_lock_file    gliimConstruct
    hi def link gg_h_print_inline_lock_file    gliimConstruct
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
    hi def link gg_h_clause_send_file    gliimClause
    hi def link gg_h_clause_output_send_file    gliimClauseOutput
    hi def link gg_h_construct_send_file    gliimConstruct
    hi def link gg_h_print_inline_send_file    gliimConstruct
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
    hi def link gg_h_clause_exec_program    gliimClause
    hi def link gg_h_clause_output_exec_program    gliimClauseOutput
    hi def link gg_h_construct_exec_program    gliimConstruct
    hi def link gg_h_print_inline_exec_program    gliimConstruct
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
    hi def link gg_h_clause_out_header    gliimClause
    hi def link gg_h_clause_output_out_header    gliimClauseOutput
    hi def link gg_h_construct_out_header    gliimConstruct
    hi def link gg_h_print_inline_out_header    gliimConstruct
syn region gg_r_construct_silent_header start="^[[:space:]]*silent-header" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_silent_header,gg_r_inline_silent_header,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_silent_header,gg_r_inline_silent_header,gg_r_at
    syn match gg_h_construct_silent_header "^[[:space:]]*silent-header" contained containedin=gg_r_construct_silent_header
    hi def link gg_h_clause_silent_header    gliimClause
    hi def link gg_h_clause_output_silent_header    gliimClauseOutput
    hi def link gg_h_construct_silent_header    gliimConstruct
    hi def link gg_h_print_inline_silent_header    gliimConstruct
syn region gg_r_construct_handler_status start="^[[:space:]]*handler-status" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_handler_status,gg_r_inline_handler_status,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_handler_status,gg_r_inline_handler_status,gg_r_at
    syn match gg_h_construct_handler_status "^[[:space:]]*handler-status" contained containedin=gg_r_construct_handler_status
    hi def link gg_h_clause_handler_status    gliimClause
    hi def link gg_h_clause_output_handler_status    gliimClauseOutput
    hi def link gg_h_construct_handler_status    gliimConstruct
    hi def link gg_h_print_inline_handler_status    gliimConstruct
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
    syn region gg_r_inline_p_web start="<<[[:space:]]*p-web \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_web '<<[[:space:]]*p-web \@=' contained containedin=gg_r_inline_p_web
    syn match gg_h_print_inline_p_web '>>' contained containedin=gg_r_inline_p_web
    hi def link gg_h_clause_p_web    gliimClause
    hi def link gg_h_clause_output_p_web    gliimClauseOutput
    hi def link gg_h_construct_p_web    gliimConstruct
    hi def link gg_h_print_inline_p_web    gliimConstruct
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
    syn region gg_r_inline_p_url start="<<[[:space:]]*p-url \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_url '<<[[:space:]]*p-url \@=' contained containedin=gg_r_inline_p_url
    syn match gg_h_print_inline_p_url '>>' contained containedin=gg_r_inline_p_url
    hi def link gg_h_clause_p_url    gliimClause
    hi def link gg_h_clause_output_p_url    gliimClauseOutput
    hi def link gg_h_construct_p_url    gliimConstruct
    hi def link gg_h_print_inline_p_url    gliimConstruct
syn region gg_r_construct_p_num start="^[[:space:]]*p-num" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_num,gg_r_inline_p_num,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_num,gg_r_inline_p_num,gg_r_at
    syn match gg_h_construct_p_num "^[[:space:]]*p-num" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " new-line \@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " new-line,\@=" contained containedin=gg_r_construct_p_num
    syn match gg_h_clause_p_num " new-line$" contained containedin=gg_r_construct_p_num
    syn match gg_h_print_inline_p_num " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num " new-line \@=" contained containedin=gg_r_inline_p_num
    syn region gg_r_inline_p_num start="<<[[:space:]]*p-num \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_num '<<[[:space:]]*p-num \@=' contained containedin=gg_r_inline_p_num
    syn match gg_h_print_inline_p_num '>>' contained containedin=gg_r_inline_p_num
    hi def link gg_h_clause_p_num    gliimClause
    hi def link gg_h_clause_output_p_num    gliimClauseOutput
    hi def link gg_h_construct_p_num    gliimConstruct
    hi def link gg_h_print_inline_p_num    gliimConstruct
syn region gg_r_construct_p_source_file start="^[[:space:]]*p-source-file" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_source_file,gg_r_inline_p_source_file,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_source_file,gg_r_inline_p_source_file,gg_r_at
    syn match gg_h_construct_p_source_file "^[[:space:]]*p-source-file" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " new-line \@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " new-line,\@=" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_clause_p_source_file " new-line$" contained containedin=gg_r_construct_p_source_file
    syn match gg_h_print_inline_p_source_file " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file " new-line \@=" contained containedin=gg_r_inline_p_source_file
    syn region gg_r_inline_p_source_file start="<<[[:space:]]*p-source-file" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_source_file '<<[[:space:]]*p-source-file' contained containedin=gg_r_inline_p_source_file
    syn match gg_h_print_inline_p_source_file '>>' contained containedin=gg_r_inline_p_source_file
    hi def link gg_h_clause_p_source_file    gliimClause
    hi def link gg_h_clause_output_p_source_file    gliimClauseOutput
    hi def link gg_h_construct_p_source_file    gliimConstruct
    hi def link gg_h_print_inline_p_source_file    gliimConstruct
syn region gg_r_construct_p_source_line start="^[[:space:]]*p-source-line" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_p_source_line,gg_r_inline_p_source_line,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_p_source_line,gg_r_inline_p_source_line,gg_r_at
    syn match gg_h_construct_p_source_line "^[[:space:]]*p-source-line" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " new-line \@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " new-line,\@=" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_clause_p_source_line " new-line$" contained containedin=gg_r_construct_p_source_line
    syn match gg_h_print_inline_p_source_line " new-line\(>>\)\@=" contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line " new-line \@=" contained containedin=gg_r_inline_p_source_line
    syn region gg_r_inline_p_source_line start="<<[[:space:]]*p-source-line" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_source_line '<<[[:space:]]*p-source-line' contained containedin=gg_r_inline_p_source_line
    syn match gg_h_print_inline_p_source_line '>>' contained containedin=gg_r_inline_p_source_line
    hi def link gg_h_clause_p_source_line    gliimClause
    hi def link gg_h_clause_output_p_source_line    gliimClauseOutput
    hi def link gg_h_construct_p_source_line    gliimConstruct
    hi def link gg_h_print_inline_p_source_line    gliimConstruct
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
    hi def link gg_h_clause_p_path    gliimClause
    hi def link gg_h_clause_output_p_path    gliimClauseOutput
    hi def link gg_h_construct_p_path    gliimConstruct
    hi def link gg_h_print_inline_p_path    gliimConstruct
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
    syn region gg_r_inline_p_out start="<<[[:space:]]*p-out \@=" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_p_out '<<[[:space:]]*p-out \@=' contained containedin=gg_r_inline_p_out
    syn match gg_h_print_inline_p_out '>>' contained containedin=gg_r_inline_p_out
    hi def link gg_h_clause_p_out    gliimClause
    hi def link gg_h_clause_output_p_out    gliimClauseOutput
    hi def link gg_h_construct_p_out    gliimConstruct
    hi def link gg_h_print_inline_p_out    gliimConstruct
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
    syn match gg_h_clause_run_query " urlencode \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " urlencode,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " urlencode$" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " webencode \@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " webencode,\@=" contained containedin=gg_r_construct_run_query
    syn match gg_h_clause_run_query " webencode$" contained containedin=gg_r_construct_run_query
    hi def link gg_h_clause_run_query    gliimClause
    hi def link gg_h_clause_output_run_query    gliimClauseOutput
    hi def link gg_h_construct_run_query    gliimConstruct
    hi def link gg_h_print_inline_run_query    gliimConstruct
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
    syn match gg_h_clause_run_prepared_query " urlencode \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " urlencode,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " urlencode$" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " webencode \@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " webencode,\@=" contained containedin=gg_r_construct_run_prepared_query
    syn match gg_h_clause_run_prepared_query " webencode$" contained containedin=gg_r_construct_run_prepared_query
    hi def link gg_h_clause_run_prepared_query    gliimClause
    hi def link gg_h_clause_output_run_prepared_query    gliimClauseOutput
    hi def link gg_h_construct_run_prepared_query    gliimConstruct
    hi def link gg_h_print_inline_run_prepared_query    gliimConstruct
syn region gg_r_construct_current_row start="^[[:space:]]*current-row" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_current_row,gg_r_inline_current_row,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_current_row,gg_r_inline_current_row,gg_r_at
    syn match gg_h_construct_current_row "^[[:space:]]*current-row" contained containedin=gg_r_construct_current_row
    syn match gg_h_clause_current_row " to \@=" contained containedin=gg_r_construct_current_row
    syn match gg_h_print_inline_current_row " to \@=" contained containedin=gg_r_inline_current_row
    syn region gg_r_inline_current_row start="<<[[:space:]]*current-row" skip="\\[[:space:]]*$" end=">>" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat contained containedin=gg_r_at keepend
    syn match gg_h_print_inline_current_row '<<[[:space:]]*current-row' contained containedin=gg_r_inline_current_row
    syn match gg_h_print_inline_current_row '>>' contained containedin=gg_r_inline_current_row
    hi def link gg_h_clause_current_row    gliimClause
    hi def link gg_h_clause_output_current_row    gliimClauseOutput
    hi def link gg_h_construct_current_row    gliimConstruct
    hi def link gg_h_print_inline_current_row    gliimConstruct
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
    hi def link gg_h_clause_begin_transaction    gliimClause
    hi def link gg_h_clause_output_begin_transaction    gliimClauseOutput
    hi def link gg_h_construct_begin_transaction    gliimConstruct
    hi def link gg_h_print_inline_begin_transaction    gliimConstruct
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
    hi def link gg_h_clause_commit_transaction    gliimClause
    hi def link gg_h_clause_output_commit_transaction    gliimClauseOutput
    hi def link gg_h_construct_commit_transaction    gliimConstruct
    hi def link gg_h_print_inline_commit_transaction    gliimConstruct
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
    hi def link gg_h_clause_db_error    gliimClause
    hi def link gg_h_clause_output_db_error    gliimClauseOutput
    hi def link gg_h_construct_db_error    gliimConstruct
    hi def link gg_h_print_inline_db_error    gliimConstruct
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
    hi def link gg_h_clause_rollback_transaction    gliimClause
    hi def link gg_h_clause_output_rollback_transaction    gliimClauseOutput
    hi def link gg_h_construct_rollback_transaction    gliimConstruct
    hi def link gg_h_print_inline_rollback_transaction    gliimConstruct
syn region gg_r_construct_end_loop start="^[[:space:]]*end-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_loop,gg_r_inline_end_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_loop,gg_r_inline_end_loop,gg_r_at
    syn match gg_h_construct_end_loop "^[[:space:]]*end-loop" contained containedin=gg_r_construct_end_loop
    hi def link gg_h_clause_end_loop    gliimClause
    hi def link gg_h_clause_output_end_loop    gliimClauseOutput
    hi def link gg_h_construct_end_loop    gliimConstruct
    hi def link gg_h_print_inline_end_loop    gliimConstruct
syn region gg_r_construct_break_loop start="^[[:space:]]*break-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_break_loop,gg_r_inline_break_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_break_loop,gg_r_inline_break_loop,gg_r_at
    syn match gg_h_construct_break_loop "^[[:space:]]*break-loop" contained containedin=gg_r_construct_break_loop
    hi def link gg_h_clause_break_loop    gliimClause
    hi def link gg_h_clause_output_break_loop    gliimClauseOutput
    hi def link gg_h_construct_break_loop    gliimConstruct
    hi def link gg_h_print_inline_break_loop    gliimConstruct
syn region gg_r_construct_continue_loop start="^[[:space:]]*continue-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_continue_loop,gg_r_inline_continue_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_continue_loop,gg_r_inline_continue_loop,gg_r_at
    syn match gg_h_construct_continue_loop "^[[:space:]]*continue-loop" contained containedin=gg_r_construct_continue_loop
    hi def link gg_h_clause_continue_loop    gliimClause
    hi def link gg_h_clause_output_continue_loop    gliimClauseOutput
    hi def link gg_h_construct_continue_loop    gliimConstruct
    hi def link gg_h_print_inline_continue_loop    gliimConstruct
syn region gg_r_construct_pause_program start="^[[:space:]]*pause-program" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_pause_program,gg_r_inline_pause_program,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_pause_program,gg_r_inline_pause_program,gg_r_at
    syn match gg_h_construct_pause_program "^[[:space:]]*pause-program" contained containedin=gg_r_construct_pause_program
    hi def link gg_h_clause_pause_program    gliimClause
    hi def link gg_h_clause_output_pause_program    gliimClauseOutput
    hi def link gg_h_construct_pause_program    gliimConstruct
    hi def link gg_h_print_inline_pause_program    gliimConstruct
syn region gg_r_construct_right_curly start="^[[:space:]]*}" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_right_curly,gg_r_inline_right_curly,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_right_curly,gg_r_inline_right_curly,gg_r_at
    syn match gg_h_construct_right_curly "^[[:space:]]*}" contained containedin=gg_r_construct_right_curly
    hi def link gg_h_clause_right_curly    gliimClause
    hi def link gg_h_clause_output_right_curly    gliimClauseOutput
    hi def link gg_h_construct_right_curly    gliimConstruct
    hi def link gg_h_print_inline_right_curly    gliimConstruct
syn region gg_r_construct_left_curly start="^[[:space:]]*{" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_left_curly,gg_r_inline_left_curly,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_left_curly,gg_r_inline_left_curly,gg_r_at
    syn match gg_h_construct_left_curly "^[[:space:]]*{" contained containedin=gg_r_construct_left_curly
    hi def link gg_h_clause_left_curly    gliimClause
    hi def link gg_h_clause_output_left_curly    gliimClauseOutput
    hi def link gg_h_construct_left_curly    gliimConstruct
    hi def link gg_h_print_inline_left_curly    gliimConstruct
syn region gg_r_construct_if_not_defined start="^[[:space:]]*if-not-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_if_not_defined,gg_r_inline_if_not_defined,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_if_not_defined,gg_r_inline_if_not_defined,gg_r_at
    syn match gg_h_construct_if_not_defined "^[[:space:]]*if-not-defined" contained containedin=gg_r_construct_if_not_defined
    hi def link gg_h_clause_if_not_defined    gliimClause
    hi def link gg_h_clause_output_if_not_defined    gliimClauseOutput
    hi def link gg_h_construct_if_not_defined    gliimConstruct
    hi def link gg_h_print_inline_if_not_defined    gliimConstruct
syn region gg_r_construct_if_defined start="^[[:space:]]*if-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_if_defined,gg_r_inline_if_defined,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_if_defined,gg_r_inline_if_defined,gg_r_at
    syn match gg_h_construct_if_defined "^[[:space:]]*if-defined" contained containedin=gg_r_construct_if_defined
    hi def link gg_h_clause_if_defined    gliimClause
    hi def link gg_h_clause_output_if_defined    gliimClauseOutput
    hi def link gg_h_construct_if_defined    gliimConstruct
    hi def link gg_h_print_inline_if_defined    gliimConstruct
syn region gg_r_construct_end_defined start="^[[:space:]]*end-defined" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_defined,gg_r_inline_end_defined,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_defined,gg_r_inline_end_defined,gg_r_at
    syn match gg_h_construct_end_defined "^[[:space:]]*end-defined" contained containedin=gg_r_construct_end_defined
    hi def link gg_h_clause_end_defined    gliimClause
    hi def link gg_h_clause_output_end_defined    gliimClauseOutput
    hi def link gg_h_construct_end_defined    gliimConstruct
    hi def link gg_h_print_inline_end_defined    gliimConstruct
syn region gg_r_construct_start_loop start="^[[:space:]]*start-loop" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_start_loop,gg_r_inline_start_loop,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_start_loop,gg_r_inline_start_loop,gg_r_at
    syn match gg_h_construct_start_loop "^[[:space:]]*start-loop" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_start_loop " add \@=" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_start_loop " repeat \@=" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_start_loop " start-with \@=" contained containedin=gg_r_construct_start_loop
    syn match gg_h_clause_output_start_loop " use \@=" contained containedin=gg_r_construct_start_loop
    hi def link gg_h_clause_start_loop    gliimClause
    hi def link gg_h_clause_output_start_loop    gliimClauseOutput
    hi def link gg_h_construct_start_loop    gliimConstruct
    hi def link gg_h_print_inline_start_loop    gliimConstruct
syn region gg_r_construct_end_query start="^[[:space:]]*end-query" skip="\\[[:space:]]*$" end="$" contains=cString,cNumbers,cOperator,cType,cConstant,cFormat,cComment,cCommentL keepend
    syn match gg_h_other_var '[_a-zA-Z][_a-zA-Z0-9]\+' contained containedin=gg_r_construct_end_query,gg_r_inline_end_query,gg_r_at
    syn match gg_h_other '[0-9]\+' contained containedin=gg_r_construct_end_query,gg_r_inline_end_query,gg_r_at
    syn match gg_h_construct_end_query "^[[:space:]]*end-query" contained containedin=gg_r_construct_end_query
    hi def link gg_h_clause_end_query    gliimClause
    hi def link gg_h_clause_output_end_query    gliimClauseOutput
    hi def link gg_h_construct_end_query    gliimConstruct
    hi def link gg_h_print_inline_end_query    gliimConstruct
