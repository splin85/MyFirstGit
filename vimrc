"splin's .vimrc
"2017-12-02 



let mapleader=';'

" define quick keys
vnoremap <Leader>y "+y
nmap <Leader>p "+p
nmap <Leader>q :q<CR>
nmap <Leader>w :w<CR>
nmap <Leader>WQ :wa<CR>:q<CR>
nmap <Leader>Q :qa!<CR>
nnoremap nw <C-W><C-W>
nnoremap lw <C-W>l
nnoremap hw <C-W>h

vnoremap / y/<c-r>"<cr>

autocmd BufWritePost $MYVIMRC source $MYVIMRC


" add file notes .c and .h files
autocmd BufNewFile *.[ch],*.cpp,*.sh exec ":call SetFileHead()"
func! SetTitleHead_cpp()
    call append(0,"/**")
    call append(1," * Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc")
    call append(2," *")
    call append(3," * file: ".expand("%:t"))
    call append(4," * This file is xxx(need to be modified)")
    call append(5," *")
    call append(6," * @author splin")
    call append(7," * @date ".strftime("%Y/%m/%d"))
    call append(8," */")
endfunc

func! SetTitleHead_sh()
    call append(0,"#!/bin/bash")
    call append(1,"# Copyright (C) 2018 Bei Jing Fu Hua Yu Qi Info Tech, Inc")
    call append(2,"#")
    call append(3,"# file: ".expand("%:t"))
    call append(4,"# This is script which")
    call append(5,"#")
    call append(6,"# @author splin")
    call append(7,"# @date ".strftime("%Y/%m/%d"))
    call append(8,"#")
endfunc

func! SetFileHead()
    if &filetype == 'sh'
        call SetTitleHead_sh()
    else
        call SetTitleHead_cpp()
    endif
endfunc

map <F4> :call SetFileHead()<CR>

func! CallCompile()
    exec 'w'
    call make
endfunc


set fileformat=unix

set nocompatible
set wildmenu
set vb t_vb=
set number
"set laststatus=2
set ruler
"set cursorline
"set cursorcolumn

set nowrap

set showcmd

set nobackup
set noswapfile

set showmatch
set hlsearch
set incsearch
set ignorecase

set ruler

set autoindent
set cindent
set smartindent

syntax enable
set t_Co=256
"let g:solarized_termcolors=256  
"set background=dark 
colorscheme torte
syntax on

"set smarttab
filetype indent on
set shiftwidth=4 
set ts=4 expandtab

set autowrite

"set report=0


"filetype off
filetype plugin on
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
Plugin 'VundleVim/Vundle.vim'
Plugin 'altercation/vim-colors-solarized'
Plugin 'tomasr/molokai'
Plugin 'vim-scripts/phd'
Plugin 'octol/vim-cpp-enhanced-highlight'
Plugin 'nathanaelkane/vim-indent-guides'
Plugin 'derekwyatt/vim-fswitch'
Plugin 'kshenoy/vim-signature'
Plugin 'vim-scripts/BOOKMARKS--Mark-and-Highlight-Full-Lines'
Plugin 'majutsushi/tagbar'
Plugin 'vim-scripts/indexer.tar.gz'
Plugin 'vim-scripts/DfrankUtil'
Plugin 'vim-scripts/vimprj'
Plugin 'dyng/ctrlsf.vim'
Plugin 'terryma/vim-multiple-cursors'
Plugin 'scrooloose/nerdcommenter'
Plugin 'vim-scripts/DrawIt'
Plugin 'SirVer/ultisnips'
"Plugin 'Valloric/YouCompleteMe'
Plugin 'derekwyatt/vim-protodef'
Plugin 'scrooloose/nerdtree'
Plugin 'fholgado/minibufexpl.vim'
Plugin 'gcmt/wildfire.vim'
Plugin 'sjl/gundo.vim'
"Plugin 'Lokaltog/vim-easymotion'
"Plugin 'godlygeek/tabular'
"Plugin 'plasticboy/vim-markdown'
"Plugin 'suan/vim-instant-markdown'
Plugin 'chriskempson/vim-tomorrow-theme'
"Plugin 'lilydjwg/fcitx.vim'
"Plugin 'xolox/vim-misc'
"Plugin 'xolox/vim-easytags'
call vundle#end()
filetype plugin indent on


let tagbar_left=1
let tagbar_width=50
let g:tagbar_compact=1
"let g:tagbar_autopreview=1
let g:tagbar_sort=0
nnoremap <silent> <F7> :TagbarToggle<CR>


let NERDTreeAutoCenter=1
let NERDTreeHightCursorline=1
let NERDTreeWinPos='right'
let NERDTreeWinSize=40 
let NERDTreeShowHidden=1
let NERDTreeMinimalUI=1
let NERDTreeAutoDeleteBuffer=1
nnoremap <silent> <F8> :NERDTreeToggle<CR> 


"map <C-Tab> :MBEbn<CR>
"map <C-S-TAB> :MBEbp<CR>
map <Leader>t :MBEFocus<CR>
nnoremap <silent> <F6> :MBEToggle<CR>
