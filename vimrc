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

autocmd BufWritePost $MYVIMRC source $MYVIMRC


" add file notes .c and .h files
autocmd BufNewFile *.[ch],*.cpp exec ":call SetTitleHead()"
func! SetTitleHead()
    call append(0,"/*")
    call append(1," * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc")
    call append(2," *")
    call append(3," * File: ".expand("%:t"))
    call append(4," * This file is xxx(need to be modified)")
    call append(5," *")
    call append(6," *")
    call append(7," * @Author splin")
    call append(8," * @Date ".strftime("%Y-%m-%d"))
    call append(9," *")
    call append(10," */")
endfunc
map <F4> :call SetTitleHead()<CR>


set nocompatible
set wildmenu

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
syntax on

"set smarttab
filetype indent on
set shiftwidth=4 
set tabstop=4 
set expandtab

set autowrite

"set report=0

set t_Co=256

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
Plugin 'suan/vim-instant-markdown'
"Plugin 'lilydjwg/fcitx.vim'
call vundle#end()
filetype plugin indent on


let tagbar_left=1
let tagbar_width=32
let g:tagbar_compact=1
nnoremap <silent> <F7> :TagbarToggle<CR>


let NERDTreeAutoCenter=1
let NERDTreeHightCursorline=1
let NERDTreeWinPos='right'
let NERDTreeWinSize=32 
let NERDTreeShowHidden=1
let NERDTreeMinimalUI=1
let NERDTreeAutoDeleteBuffer=1
nnoremap <silent> <F8> :NERDTreeToggle<CR> 


"map <C-Tab> :MBEbn<CR>
"map <C-S-TAB> :MBEbp<CR>
map <Leader>t :MBEFocus<CR>
nnoremap <silent> <F6> :MBEToggle<CR>
