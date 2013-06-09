set nocompatible

if has('win32')
	source $VIMRUNTIME/mswin.vim
	behave mswin
endif

set enc=utf-8
set fencs=cp932,euc-jp,utf-8
"language messages ja_JP.UTF-8
language messages C

set tabstop=4
set shiftwidth=4
set iminsert=0
set imsearch=0
set nowrap

set incsearch
set ignorecase

set ruler
set showcmd
set laststatus=2

set list
set listchars=tab:>-,eol:$

"set noswapfile
set nobackup

set cryptmethod=blowfish

"Mintty
if $TERM == 'xterm-256color'
	let &t_Co=256
	let &t_AF="\e[38;5;%dm"
	let &t_AB="\e[48;5;%dm"
endif

if has('gui_running')
	set ambiwidth=double
	if has('win32')
		set guifont=Inconsolata:h12:cANSI
		set guifontwide=M+_2m_medium:h10:cSHIFTJIS
	elseif has('unix')
		set guifont=Inconsolata\ Medium\ 12
		set guifontwide=IPAGothic\ Regular\ 12
	endif
endif

if &t_Co > 2 || has("gui_running")
	syntax on
	colorscheme zenburn
	set hlsearch
endif

"SKK
let g:skk_jisyo = '~/.skk/skk-jisyo'
let g:skk_large_jisyo = '~/.skk/SKK-JISYO.L'
let g:skk_egg_like_newline = 1
let g:skk_auto_save_jisyo = 1
