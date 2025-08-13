" SPDX-License-Identifier: Apache-2.0
" Copyright 2018-2025 Gliim LLC.
" Licensed under Apache License v2. See LICENSE file.
" On the web http://golf-lang.com/ - this file is part of Golf framework.
" Language: Golf
" Vim indent file
" Maintainer: Gliim LLC

if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal iskeyword+=-
setlocal indentexpr=GetGolfIndent(v:lnum)
setlocal indentkeys=o,O,e,0},=end-if,=end-query,=end-handler,=end-loop,=end-read-line,=end-do-once,=end-defined,=end-after-handler,=end-before-handler,=end-write-string,0)),0%%,=else-if,!^F

function! GetGolfIndent(lnum)
    let l = a:lnum
    let p_line_num = prevnonblank(l - 1)
    let c_line = getline(l)

    if p_line_num == 0
        return 0
    endif

    let p_line = getline(p_line_num)
    let p_indent = indent(p_line_num)


    if p_line =~ '^\s*if-true.*$' || p_line =~ '^\s*start-loop.*$' || p_line =~ '^\s*run-query.*$' || p_line =~ '^\s*run-prepared-query.*$' || p_line =~ '^\s*read-line.*$' || p_line =~ '^\s*{.*$' || p_line =~ '^\s*do-once.*$' || p_line =~ '^\s*if-defined.*$' || p_line =~ '^\s*after-handler.*$' || p_line =~ '^\s*before-handler.*$' || p_line =~ '^\s*begin-handler.*$' || p_line =~ '^\s*%%\s\+/[^/].*$' || p_line =~ '^\s*write-string.*$' || p_line =~ '^\s*((.*$' || p_line =~ '^\s*else-if.*$'
        let p_indent += &shiftwidth
    endif

    if c_line =~ '^\s*end-if.*$' || c_line =~ '^\s*end-loop.*$' || c_line =~ '^\s*end-query.*$' || c_line =~ '^\s*end-handler.*$' || c_line =~ '^\s*end-read-line.*$' || c_line =~ '^\s*}.*$' || c_line =~ '^\s*end-do-once.*$' || c_line =~ '^\s*end-defined.*$' || c_line =~ '^\s*end-after-handler.*$' || c_line =~ '^\s*end-before-handler.*$' || c_line =~ '^\s*end-write-string.*$' || c_line =~ '^\s*)).*$' || c_line =~ '^\s*else-if.*$' || c_line =~ '^\s*%%\s*$'
        let p_indent -= &shiftwidth
    endif
    :echom "Prev  indent " . p_indent

    return p_indent
endfunction

